#include <boost/foreach.hpp>

#include <stdio.h>
#include <iostream>
#include <utility>

#include "Contours.h"
#include "Normalizer.h"
#include "Thresholder.h"

#include "ShooterFloodFinder.h"

using namespace boost;
using namespace cv;
using namespace std;

static const int OFFSET=18;

static Point pointFromIndex(int i, int offset) {
    int sx = (i & 1) ? offset : -offset;
    int sy = (i & 2) ? offset : -offset;
    return Point(sx, sy);
}

IFinder::FinderResult ShooterFloodFinder::find(const subjugator::ImageSource::Image &img) {
	Mat hsv;
	cvtColor(img.image, hsv, CV_BGR2HSV);

	Mat hsv_split[3];
	split(hsv, hsv_split);

	Mat dbg = Mat::zeros(hsv.rows, hsv.cols, CV_8UC1);

	boost::optional<QuadPointResults> quad_point = trackQuadPoint(hsv_split);
	if (quad_point) {
		int range = config.get<int>(objectPath[0] + "_range");
		Point pt = quad_point->dirs[objectPath[0]];
            
//		Mat h_no_border = hsv_split[0](Rect(1, 1, hsv_split[0].cols-2, hsv_split[0].rows-2));
		Mat h_no_border = img.image(Rect(1, 1, img.image.cols-2, img.image.rows-2));
                Normalizer::normRGB(h_no_border);
		rectangle(dbg, quad_point->point, quad_point->point+600*pt, Scalar(255));
		floodFill(h_no_border, dbg, quad_point->point + OFFSET*pt, Scalar(), NULL,
                          Scalar(range, range, range), Scalar(range, range, range),
                          /*FLOODFILL_FIXED_RANGE | */FLOODFILL_MASK_ONLY | 4);

		dbg = (dbg != 0);
	}

	dilate(dbg,dbg,Mat::ones(5,5,CV_8UC1));
        if (objectPath[0] != "red")
            dbg &= hsv_split[1] > 48;
	erode(dbg,dbg,Mat::ones(5,5,CV_8UC1));

	// call to specific member function here
	Contours contours(dbg, 50, 7000000,1500000, img.camera_model);

	// Draw result
	Mat res = img.image.clone();
	contours.drawResult(res, CV_RGB(255, 255, 255));

	vector<property_tree::ptree> resultVector;
	if(objectPath[1] == "box") {
		// Prepare results
		if(contours.boxes.size() && contours.shapes.size()) {
			property_tree::ptree fResult;
			fResult.put_child("center", Point_to_ptree(contours.boxes[0].centroid, img));
			fResult.put("scale", contours.boxes[0].area);
			fResult.put("angle", contours.boxes[0].orientationError);
			resultVector.push_back(fResult);
		}
	} else if(objectPath[1] == "small") {
		Contours::InnerContour bestShape;
		bool foundSomething = false;
		BOOST_FOREACH(const Contours::InnerContour &shape, contours.shapes)
			if(shape.circularity > 0.5 && shape.area < shape.outer_area*2/3 && (!foundSomething || shape.area > bestShape.area)) {
				foundSomething = true;
				bestShape = shape;
			}
		if(foundSomething) {
			Contours::InnerContour bestShape2;
			{
				bool foundSomething2 = false;
				BOOST_FOREACH(const Contours::InnerContour &shape, contours.shapes)
					if(shape.circularity > 0.5 && shape.area < shape.outer_area*2/3 && (!foundSomething2 || shape.area > bestShape2.area) && shape.area < bestShape.area) {
						foundSomething2 = true;
						bestShape2 = shape;
					}
				if(!foundSomething2)
					bestShape2 = bestShape;
			}
			if(bestShape2.area < 0.1*bestShape.area) // if second largest is an order of magnitude smaller
				bestShape2 = bestShape; // use largest

			circle(res, bestShape2.centroid, 10, CV_RGB(255, 255, 0), 2);

			property_tree::ptree fResult;
			fResult.put_child("center", Point_to_ptree(bestShape2.centroid, img));
			//fResult.put("angle", contours.boxes[0].orientationError);
			fResult.put("scale", bestShape2.area);
			resultVector.push_back(fResult);
		}
	} else { assert(objectPath[1] == "large");
		if(contours.shapes.size()) {
			Contours::InnerContour shape = contours.findLargestShape();
			property_tree::ptree fResult;
			fResult.put_child("center", Point_to_ptree(shape.centroid, img));
			//fResult.put("angle", contours.boxes[0].orientationError);
			fResult.put("scale", shape.area);
			resultVector.push_back(fResult);
		}
	}

	if (quad_point) {
		circle(res, quad_point->point + Point(OFFSET, OFFSET), 2, Scalar(0, 255, 0), 3);
		circle(res, quad_point->point + Point(OFFSET, -OFFSET), 2, Scalar(0, 255, 0), 3);
		circle(res, quad_point->point + Point(-OFFSET, OFFSET), 2, Scalar(0, 255, 0), 3);
		circle(res, quad_point->point + Point(-OFFSET, -OFFSET), 2, Scalar(0, 255, 0), 3);

                //dbg = quad_point->scores;
	}

	return FinderResult(resultVector, res, dbg);
}

boost::optional<ShooterFloodFinder::QuadPointResults> ShooterFloodFinder::trackQuadPoint(
	const cv::Mat (&hsv_split)[3]) 
{
	Mat scores = Mat::zeros(hsv_split[0].rows, hsv_split[0].cols, CV_8UC1);
	for (int r=OFFSET; r < scores.rows-OFFSET; r++) {
		for (int c=OFFSET; c < scores.cols-OFFSET; c++) {
			// Gather hue and saturation samples
			std::pair<Vec<uchar, 4>, Vec<uchar, 4> > result =
				sample_point(hsv_split, r, c, OFFSET);
			const Vec<uchar, 4> &hues = result.first;
			const Vec<uchar, 4> &sats = result.second;

			// Compute the minimum difference between all pairs of hues
			int mindiff=9999;
			for (int i=0; i<4; i++) {
				for (int j=0; j<i; j++) {
					int diff = std::abs(hues[i] - hues[j]);
					if (diff > 90)
						diff = 180 - diff;
					mindiff = std::min(mindiff, diff);
				}
			}

			// Score is minimum difference times sum of saturation
			int score = mindiff * (sats[0]+sats[1]+sats[2]+sats[3]);

			scores.at<uchar>(r, c) = std::min(std::max(score/100, 0), 255);
		}
	}

	// Gets rid of almost all noise, because only the actual point will
	// appear as a mostly solid square of size OFFSET
//	erode(scores, scores, Mat::ones(OFFSET+1,OFFSET+1,CV_8UC1));
	erode(scores, scores, Mat::ones(OFFSET/2-1,OFFSET/2-1,CV_8UC1));

	Point maxpoint;
	double maxscore;
	minMaxLoc(scores, NULL, &maxscore, NULL, &maxpoint);

	if (maxscore > 25) {
		QuadPointResults results;
		results.point = maxpoint;
		results.score = maxscore;
		std::pair<Vec<uchar, 4>, Vec<uchar, 4> > tmp = sample_point(hsv_split,
									    maxpoint.y, maxpoint.x,
									    OFFSET);
		results.hues = tmp.first;
		results.sats = tmp.second;
                results.scores = scores;

		std::vector<std::pair<uint, int> > vec;
		for (int i=0; i<4; i++) {
		    int hue = results.hues[i];
		    if (hue > 140) // red (hopefully)
			hue = 0;
		    vec.push_back(make_pair(hue, i));
		}

		std::sort(vec.begin(), vec.end());

		for (int i=0; i<4; i++) {
		    std::cout << vec[i].first << ' ' << pointFromIndex(vec[i].second, 1) << ' ';
		}

		std::cout << std::endl;

		results.dirs["red"] = pointFromIndex(vec[0].second, 1);
		results.dirs["yellow"] = pointFromIndex(vec[1].second, 1);
		results.dirs["green"] = pointFromIndex(vec[2].second, 1);
		results.dirs["blue"] = pointFromIndex(vec[3].second, 1);
		
		return results;
	} else {
		return boost::none;
	}
}

std::pair<Vec<uchar, 4>, Vec<uchar, 4> > ShooterFloodFinder::sample_point(const Mat (&hsv_split)[3],
									  int r, int c, int offset) {
	Vec<uchar, 4> hues;
	Vec<uchar, 4> sats;
	for (int i=0; i<4; i++) {
		cv::Point pt = pointFromIndex(i, offset);
		hues[i] = hsv_split[0].at<uchar>(r+pt.y, c+pt.x);
		sats[i] = hsv_split[1].at<uchar>(r+pt.y, c+pt.x);
	}
	return make_pair(hues, sats);
}

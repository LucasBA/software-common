#ifndef CONTOURS_H
#define CONTOURS_H

#include <vector>

#include <opencv/cv.h>
#include <image_geometry/pinhole_camera_model.h>

class Contours
{
	public:
		struct InnerContour {
			cv::Point centroid;
			float area;
			float perimeter;
			float radius;
			float circularity; // ranges from 0 for a line to 1 for a perfect circle
			float outer_area;
			std::vector<std::vector<cv::Point> > contour;
		};

		struct OuterBox {
			float perimeter;
			cv::Point centroid;
			cv::Point2f direction;
			float area;
			float angle;
			cv::Point orientation;
			std::vector<cv::Point> corners;
			std::vector<std::vector<cv::Point> > contour;
			double orientationError;
			bool touches_edge;
			std::vector<InnerContour> shapes; // contours within this box
			double aspect_ratio;
		};
		std::vector<InnerContour> shapes; // all inner contours
		std::vector<OuterBox> boxes; // output holder

		Contours(const cv::Mat &img, float minContour, float maxContour, float maxPerimeter, image_geometry::PinholeCameraModel camera_model);
		void drawResult(cv::Mat &img, const cv::Scalar &color);
		double angle(cv::Point pt1, cv::Point pt2, cv::Point pt0);
		InnerContour findLargestShape();
		InnerContour findSmallestShape();
		cv::Point calcCentroidOfAllBoxes();
		float calcAngleOfAllBoxes();
		cv::Point2f calcDirectionOfAllBoxes();

	private:
		std::vector<std::vector<cv::Point> > contours;
		std::vector<cv::Vec4i> hierarchy; // hierarchy holder for the contour tree
};

#endif

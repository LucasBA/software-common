#ifndef THRESHOLDER_H
#define THRESHOLDER_H

#include <opencv/cv.h>

class Thresholder {
	public:
		Thresholder(const cv::Mat &img);
		cv::Mat orange();
		cv::Mat red();
		cv::Mat shooterRed();
		cv::Mat green();
		cv::Mat yellow();
		cv::Mat blue();
		cv::Mat black();
		cv::Mat black2();
		cv::Mat forrest(cv::Vec3b bg, cv::Vec3b fg, double radius=30, double inclusion=0.5);
		cv::Mat simpleRGB(cv::Vec3b bg, cv::Vec3b fg, int window=11, int cutoff=3);
		cv::Mat simpleHSV(uchar hue, uchar range, uchar sat_C);
	private:
	    cv::Mat img;
		std::vector<cv::Mat> channelsRGB;
		std::vector<cv::Mat> channelsLAB;
		std::vector<cv::Mat> channelsHSV;
};

#endif

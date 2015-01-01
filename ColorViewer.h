#ifndef COLOR_VIEWER_H
#define COLOR_VIEWER_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include  <iostream>

class ColorViewer {
private:
	cv::Mat colorSquare;
	std::string colorSquareWindowName;
	int b_val;
	int g_val;
	int r_val;
	int avg_hue;
	int avg_sat;
	int avg_val;
public:
	ColorViewer(std::string input_name) {
		colorSquareWindowName = input_name;
	};
	void createColorSquare() {
		colorSquare = cv::Mat::zeros(100,250,CV_8UC3);
		setColorSquareBGR(0,0,0);
	}
	void setColorSquareBGR(int B, int G, int R) {
		colorSquare.setTo(cv::Scalar(B,G,R));
		b_val = B;
		g_val = G;
		r_val = R;
		cv::imshow(colorSquareWindowName, colorSquare);
	}
	void setColorSquareBGR(cv::Scalar BGR_scalar) {
		colorSquare.setTo(BGR_scalar);
		b_val = int(BGR_scalar[0]);
		g_val = int(BGR_scalar[1]);
		r_val = int(BGR_scalar[2]);
		cv::imshow(colorSquareWindowName, colorSquare);
	}
	void setColorSquareHSV(int H, int S, int V) {
		colorSquare.setTo(cv::Scalar(H,S,V));
		cv::cvtColor(colorSquare, colorSquare, CV_HSV2BGR);
		b_val = H;
		g_val = S;
		r_val = V;
		cv::imshow(colorSquareWindowName, colorSquare);
	}
	void setColorSquareHSV(cv::Scalar HSV_scalar) {
		colorSquare.setTo(HSV_scalar);
		cv::cvtColor(colorSquare, colorSquare, CV_HSV2BGR);
		b_val = int(HSV_scalar[0]);
		g_val = int(HSV_scalar[1]);
		r_val = int(HSV_scalar[2]);
		cv::imshow(colorSquareWindowName, colorSquare);
	}
	std::string getColorSquareWindowName() {
		return colorSquareWindowName;
	}
	cv::Mat getColorSquare() {
		return colorSquare;
	}
	cv::Scalar getColorSquareValue() {
		return cv::Scalar(b_val,g_val,r_val);
	}
	cv::Scalar getAverageHSV(cv::Mat& image) {
		cv::cvtColor(image, image, CV_BGR2HSV);
		std::vector<cv::Mat> hsv_planes;
		cv::split(image, hsv_planes);
		avg_hue = int(cv::mean(hsv_planes[0])[0]);
		avg_sat = int(cv::mean(hsv_planes[1])[0]);
		avg_val = int(cv::mean(hsv_planes[2])[0]);
		cv::cvtColor(image, image, CV_HSV2BGR);
		return cv::Scalar(avg_hue, avg_sat, avg_val);
	}
	cv::Scalar getModeHSV(cv::Mat& image) {
		cv::cvtColor(image, image, CV_BGR2HSV);
		std::vector<cv::Mat> hsv_planes;
		cv::split(image, hsv_planes);
	}
};

#endif COLOR_VIEWER
#ifndef HSV_HISTOGRAM_H
#define HSV_HISTOGRAM_H
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <iostream>

class HSVHistogram {
private:
	int h_histSize;
	int s_histSize;
	int v_histSize;
	float h_range[2];
	float s_range[2];
	float v_range[2];
	const float* h_histRange[1];
	const float* s_histRange[1];
	const float* v_histRange[1];
	int channels[1];
public:
	HSVHistogram() {
		h_histSize = 180;
		s_histSize = 255;
		v_histSize = 255;
		h_range[0] = 0; h_range[1] = 180;
		s_range[0] = 0; s_range[1] = 256;
		v_range[0] = 0; v_range[1] = 256;
		h_histRange[0] = h_range;
		s_histRange[0] = s_range;
		v_histRange[0] = v_range;
		channels[0] = 0;
	}
	cv::MatND getHueHistogram(const cv::Mat &image) {
		cv::cvtColor(image,image,CV_BGR2HSV);
		std::vector<cv::Mat> hsv_planes;
		cv::split(image,hsv_planes);
		cv::MatND hue_hist;
		cv::calcHist(&hsv_planes[0], 1, channels, cv::Mat(), hue_hist, 1, &h_histSize, h_histRange);
		cv::cvtColor(image,image,CV_HSV2BGR);
		return hue_hist;
	}
	cv::Mat getHueHistogramImage(const cv::Mat &image) {
		cv::MatND hue_hist = getHueHistogram(image);
		// min and max bin vals
		double minVal;
		double maxVal;
		cv::minMaxLoc(hue_hist,&minVal, &maxVal);
		// image on which to display histogram
		cv::Mat histImg(h_histSize,h_histSize,CV_8UC3,cv::Scalar(0));
		// set highest point at 90% of nbins
		int hpt = static_cast<int>(0.90*h_histSize);
		// Draw a vertical line for each bin
		for (int h=0; h<h_histSize; h++) {
			float binVal = hue_hist.at<float>(h);
			int intensity = static_cast<int>(binVal*hpt/maxVal);
			cv::Scalar HSV_scalar = cv::Scalar(h,255,255);
			cv::Mat temp = cv::Mat::zeros(1,1,CV_8UC3);
			temp.setTo(HSV_scalar);
			cv::cvtColor(temp, temp, CV_HSV2BGR);
			cv::Scalar BGR_scalar = temp.at<cv::Vec3b>(0,0);
			cv::line(histImg, cv::Point(h,h_histSize), cv::Point(h,h_histSize-intensity), BGR_scalar);
		}
		//cv::resize(histImg, histImg, cv::Size(), 1.5, 1.5);
		return histImg;
	}
	cv::MatND getSatHistogram(const cv::Mat &image) {
		cv::cvtColor(image,image,CV_BGR2HSV);
		std::vector<cv::Mat> hsv_planes;
		cv::split(image,hsv_planes);
		cv::MatND sat_hist;
		cv::calcHist(&hsv_planes[1], 1, channels, cv::Mat(), sat_hist, 1, &s_histSize, s_histRange);
		cv::cvtColor(image,image,CV_HSV2BGR);
		return sat_hist;
	}
	cv::Mat getSatHistogramImage(const cv::Mat &image) {
		cv::MatND sat_hist = getSatHistogram(image);
		// min and max bin vals
		double minVal;
		double maxVal;
		cv::minMaxLoc(sat_hist,&minVal, &maxVal);
		// image on which to display histogram
		cv::Mat histImg(s_histSize,s_histSize,CV_8U,cv::Scalar(0));
		// set highest point at 90% of nbins
		int hpt = static_cast<int>(0.9*s_histSize);
		// Draw a vertical line for each bin
		for (int h=0; h<s_histSize; h++) {
			float binVal = sat_hist.at<float>(h);
			int intensity = static_cast<int>(binVal*hpt/maxVal);
			cv::line(histImg, cv::Point(h,s_histSize), cv::Point(h,s_histSize-intensity), cv::Scalar::all(255));
		}
		return histImg;
	}
	cv::MatND getValHistogram(const cv::Mat &image) {
		cv::cvtColor(image,image,CV_BGR2HSV);
		std::vector<cv::Mat> hsv_planes;
		cv::split(image,hsv_planes);
		cv::MatND val_hist;
		cv::calcHist(&hsv_planes[2], 1, channels, cv::Mat(), val_hist, 1, &v_histSize, v_histRange);
		cv::cvtColor(image,image,CV_HSV2BGR);
		return val_hist;
	}
	cv::Mat getValHistogramImage(const cv::Mat &image) {
		cv::MatND val_hist = getValHistogram(image);
		// min and max bin vals
		double minVal;
		double maxVal;
		cv::minMaxLoc(val_hist,&minVal, &maxVal);
		// image on which to display histogram
		cv::Mat histImg(v_histSize,v_histSize,CV_8U,cv::Scalar(0));
		// set highest point at 90% of nbins
		int hpt = static_cast<int>(0.9*v_histSize);
		// Draw a vertical line for each bin
		for (int h=0; h<v_histSize; h++) {
			float binVal = val_hist.at<float>(h);
			int intensity = static_cast<int>(binVal*hpt/maxVal);
			cv::line(histImg, cv::Point(h,v_histSize), cv::Point(h,v_histSize-intensity), cv::Scalar::all(255));
		}
		return histImg;
	}
	int getMostAbundantHue(cv::MatND hist) {
		int mostAbundantHueVal = 0;
		int mostAbundantHueQuantity = 0;
		for (int i=0; i<180; i++) {
			int hueQuantity = int(hist.at<float>(i));
			//std::cout << "Hue " << i << "=" << hueQuantity << std::endl;
			if (hueQuantity > mostAbundantHueQuantity) {
				mostAbundantHueVal = i;
				mostAbundantHueQuantity = hueQuantity;
			}
		}
		//std::cout << std::endl;
		return mostAbundantHueVal;
	}
	int getMostAbundantSat(cv::MatND hist) {
		int mostAbundantSatVal = 0;
		int mostAbundantSatQuantity = 0;
		for (int i=0; i<255; i++) {
			int satQuantity = int(hist.at<float>(i));
			//std::cout << "Sat " << i << "=" << satQuantity << std::endl;
			if (satQuantity > mostAbundantSatQuantity) {
				mostAbundantSatVal = i;
				mostAbundantSatQuantity = satQuantity;
			}
		}
		//std::cout << std::endl;
		return mostAbundantSatVal;
	}
	int getMostAbundantVal(cv::MatND hist) {
		int mostAbundantValVal = 0;
		int mostAbundantValQuantity = 0;
		for (int i=0; i<255; i++) {
			int valQuantity = int(hist.at<float>(i));
			//std::cout << "Val " << i << "=" << valQuantity << std::endl;
			if (valQuantity > mostAbundantValQuantity) {
				mostAbundantValVal = i;
				mostAbundantValQuantity = valQuantity;
			}
		}
		//std::cout << std::endl;
		return mostAbundantValVal;
	}
};

#endif HSV_HISTOGRAM_H
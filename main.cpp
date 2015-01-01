#include "ColorViewer.h"
#include "HSVHistogram.h"
#include "MouseDetector.h"
#include <iostream>
#include <time.h>
using namespace std;

cv::Mat image;
cv::Mat centerROI;
cv::Mat centerROI_;
cv::Mat leftROI;
cv::Mat leftROI_;
cv::Mat rightROI;
cv::Mat rightROI_;
cv::Mat scrollROI;
cv::Mat scrollROI_;

MouseDetector MouseGlove;
HSVHistogram Hist;
ColorViewer CenterViewer("Center Marker");
ColorViewer LeftViewer("Left Marker");
ColorViewer RightViewer("Right Marker");
ColorViewer ScrollViewer("Scroll Marker");

cv::Point mouseCoordinates;

cv::VideoCapture camera(1);

bool leftClickStatus;
bool rightClickStatus;

bool center_ROI_defined;
bool left_ROI_defined;
bool right_ROI_defined;
bool scroll_ROI_defined;

bool center_HSV_defined;
bool left_HSV_defined;
bool right_HSV_defined;
bool scroll_HSV_defined;

cv::Scalar tealColor = cv::Scalar(255,255,0);

clock_t t;

float getTime() {
	return float(clock() - t)/CLOCKS_PER_SEC;
}

void resetTimer() {
	t = clock();
}

void toggleLeftClick() {
	leftClickStatus = MouseGlove.getLeftClickStatus();
	if (leftClickStatus == true) MouseGlove.leftClickDown(mouseCoordinates);
	else MouseGlove.leftClickUp(mouseCoordinates);
	resetTimer();
}

void toggleRightClick() {
	rightClickStatus = MouseGlove.getRightClickStatus();
	if (rightClickStatus == true) MouseGlove.rightClickDown(mouseCoordinates);
	else MouseGlove.rightClickUp(mouseCoordinates);
	resetTimer();
}

// Takes a rectangle and shrinks it to tighten its focus on the target's color
cv::Rect calibratedRect(const cv::Rect rect) {
	cv::Rect rect_;
	int x_padding = max(int(0.2*rect.width), 1);
	int y_padding = max(int(0.2*rect.height), 1);
	rect_.x = rect.x + x_padding;
	rect_.y = rect.y + y_padding;
	rect_.width = rect.width - 2*x_padding;
	rect_.height = rect.height - 2*y_padding;
	return rect_;
}

// Allows user to select desired target in image
void CallBackFunc(int evnt, int x, int y, int flags, void* userdata) {
	if (evnt == cv::EVENT_LBUTTONDOWN) {
		cv::Point clickedPoint = cv::Point(x,y);
		cv::Rect boundingRect = cv::Rect(clickedPoint - cv::Point(5,5), clickedPoint + cv::Point(5,5));
		if (!center_ROI_defined) {
			centerROI = image(calibratedRect(boundingRect));
			centerROI.copyTo(centerROI_);
			cout << "center ROI defined" << endl;
			center_ROI_defined = true;
		}
		else if (!left_ROI_defined) {
			leftROI = image(calibratedRect(boundingRect));
			leftROI.copyTo(leftROI_);
			cout << "left ROI defined" << endl;
			left_ROI_defined = true;
		}
		else if (!right_ROI_defined) {
			rightROI = image(calibratedRect(boundingRect));
			rightROI.copyTo(rightROI_);
			cout << "right ROI defined" << endl;
			right_ROI_defined = true;
		}
		else if (!scroll_ROI_defined) {
			scrollROI = image(calibratedRect(boundingRect));
			scrollROI.copyTo(scrollROI_);
			cout << "scroll ROI defined" << endl;
			scroll_ROI_defined = true;
		}
	}
}

// Wrapper function for CallBackFunc
void processMouseActions() {
	cv::setMouseCallback("Mouse Glove", CallBackFunc, NULL);
}

void prompt_for_center(cv::Mat& image) {
	double scale = 0.65;
	int thickness = 2;
	cv::putText(image, "Please click on center marker", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, scale, tealColor, thickness);
}

void prompt_for_left(cv::Mat& image) {
	double scale = 0.65;
	int thickness = 2;
	cv::putText(image, "Please click on left marker", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, scale, tealColor, thickness);
}

void prompt_for_right(cv::Mat& image) {
	double scale = 0.65;
	int thickness = 2;
	cv::putText(image, "Please click on right marker", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, scale, tealColor, thickness);
}

void prompt_for_scroll(cv::Mat& image) {
	double scale = 0.65;
	int thickness = 2;
	cv::putText(image, "Please click on scroll marker", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, scale, tealColor, thickness);
}

int main() {
    if (!camera.isOpened()) {
		camera = 0;
		if (!camera.isOpened())
			return 1;
	}
	resetTimer();
	//MouseGlove.setCenterHSV(104,238,203);
	//MouseGlove.setLeftHSV(150,150,232);
	//MouseGlove.setRightHSV(15,205,246);
	//MouseGlove.setScrollHSV(63,144,204);
	//MouseGlove.setCenterColorThreshold(50);
	//MouseGlove.setLeftColorThreshold(35);
	//MouseGlove.setRightColorThreshold(25);
	//MouseGlove.setScrollColorThreshold(30);
	MouseGlove.setScrollAnchorYCoordinate(240);
	MouseGlove.setMinArea(100);
	leftClickStatus = false;
	rightClickStatus = false;
	center_ROI_defined = false;
	left_ROI_defined = false;
	right_ROI_defined = false;
	scroll_ROI_defined = false;
	center_HSV_defined = false;
	left_HSV_defined = false;
	right_HSV_defined = false;
	scroll_HSV_defined = false;
    while(cv::waitKey(10) != 13) {
		if (!camera.read(image)) return 1;
		cv::flip(image,image,1);
		processMouseActions();
		if (!center_HSV_defined || !left_HSV_defined || !right_HSV_defined || !scroll_HSV_defined) {
			// Define HSV values here
			if (!center_HSV_defined) prompt_for_center(image);
			else if (!left_HSV_defined) prompt_for_left(image);
			else if (!right_HSV_defined) prompt_for_right(image);
			else if (!scroll_HSV_defined) prompt_for_scroll(image);
			if (center_ROI_defined) {
				cv::imshow("centerROI", centerROI_);
				int hue = Hist.getMostAbundantHue(Hist.getHueHistogram(centerROI_)); // Get hue [0,180]
				int sat = Hist.getMostAbundantSat(Hist.getSatHistogram(centerROI_)); // Get saturation [0,255]
				int val = Hist.getMostAbundantVal(Hist.getValHistogram(centerROI_)); // Get value [0,255]
				MouseGlove.setCenterHSV(hue, sat, val);
				//CenterViewer.setColorSquareHSV(hue, sat, val);
				center_HSV_defined = true;
				//cout << "center HSV defined" << endl;
			}
			if (left_ROI_defined) {
				int hue = Hist.getMostAbundantHue(Hist.getHueHistogram(leftROI_)); // Get hue [0,180]
				int sat = Hist.getMostAbundantSat(Hist.getSatHistogram(leftROI_)); // Get saturation [0,255]
				int val = Hist.getMostAbundantVal(Hist.getValHistogram(leftROI_)); // Get value [0,255]
				MouseGlove.setLeftHSV(hue, sat, val);
				//LeftViewer.setColorSquareHSV(hue, sat, val);
				left_HSV_defined = true;
				//cout << "left HSV defined" << endl;
			}
			if (right_ROI_defined) {
				int hue = Hist.getMostAbundantHue(Hist.getHueHistogram(rightROI_)); // Get hue [0,180]
				int sat = Hist.getMostAbundantSat(Hist.getSatHistogram(rightROI_)); // Get saturation [0,255]
				int val = Hist.getMostAbundantVal(Hist.getValHistogram(rightROI_)); // Get value [0,255]
				MouseGlove.setRightHSV(hue, sat, val);
				//RightViewer.setColorSquareHSV(hue, sat, val);
				right_HSV_defined = true;
				//cout << "right HSV defined" << endl;
			}
			if(scroll_ROI_defined) {
				int hue = Hist.getMostAbundantHue(Hist.getHueHistogram(scrollROI_)); // Get hue [0,180]
				int sat = Hist.getMostAbundantSat(Hist.getSatHistogram(scrollROI_)); // Get saturation [0,255]
				int val = Hist.getMostAbundantVal(Hist.getValHistogram(scrollROI_)); // Get value [0,255]
				MouseGlove.setScrollHSV(hue, sat, val);
				//ScrollViewer.setColorSquareHSV(hue, sat, val);
				scroll_HSV_defined = true;
				//cout << "scroll HSV defined" << endl;
			}
			MouseGlove.setCenterColorThreshold(30);
			MouseGlove.setLeftColorThreshold(25);
			MouseGlove.setRightColorThreshold(25);
			MouseGlove.setScrollColorThreshold(20);
		}
		else {
			MouseGlove.processCenterMarker(image);
			MouseGlove.processLeftMarker(image);
			MouseGlove.processRightMarker(image);
			MouseGlove.processScrollMarker(image);
			if (getTime() > 0.3) {
				mouseCoordinates = MouseGlove.getCenterMarkerCoordinates();
				MouseGlove.calibrateCoordinates(mouseCoordinates);
			}
			if (MouseGlove.mouseDetected()) {
				MouseGlove.moveMouse(mouseCoordinates);
			}
			if (MouseGlove.getLeftClickStatus() != leftClickStatus)
				toggleLeftClick();
			if (MouseGlove.getRightClickStatus() != rightClickStatus)
				toggleRightClick();
			if (MouseGlove.scrollDetected()) {
				MouseGlove.scroll(mouseCoordinates,MouseGlove.getScrollSpeed());
			}
			MouseGlove.displayMouseStatus(image);
		}
		cv::imshow("Mouse Glove",image);
    }
}

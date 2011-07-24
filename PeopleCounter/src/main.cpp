/*
 * main.cpp
 *
 *  Created on: 2011-07-20
 *      Author: Justin
 */
#define BOOST_THREAD_USE_LIB

#include <cv.h>
#include <highgui.h>
#include <stdio.h>
#include <iostream>
#include <string>
#include <sstream>
#include <cstring>


using namespace cv;

using namespace std;

int main(){
	cvNamedWindow("Stock", CV_WINDOW_AUTOSIZE);
	cvNamedWindow("Modified", CV_WINDOW_AUTOSIZE);
	std::string filename = "";
	//cin >> filename;
	filename = "video4.avi";
	CvCapture *fc = cvCreateFileCapture(filename.c_str());

	if(!fc)
		return 0;

	IplImage* frame = cvQueryFrame(fc);

	if (!frame) {
		return 0;
	}

	IplImage* greyImg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_8U, 1);
	IplImage* colourImg;
	IplImage* movingAverage = cvCreateImage(cvGetSize(frame), IPL_DEPTH_32F, 3);
	IplImage* difference;
	IplImage* temp;

	CvRect bndRect = cvRect(0,0,0,0);

	CvPoint pt1, pt2;

	int frame_count = 0;

	bool first = true;

	while((frame = cvQueryFrame(fc)) != NULL){

		colourImg = cvCloneImage(frame);

		if(!colourImg)
			return 0;


		if(first){
			difference = cvCloneImage(colourImg);
			temp = cvCloneImage(colourImg);
			cvConvertScale(colourImg, movingAverage, 1.0, 0.0);
			first = false;
		}else{
			cvRunningAvg(colourImg, movingAverage, 0.020, NULL);
		}

		cvConvertScale(movingAverage, temp, 1.0, 0.0);

		cvAbsDiff(colourImg, temp, difference);

		cvCvtColor(difference, greyImg, CV_RGB2GRAY);

		cvThreshold(greyImg, greyImg, 70, 255, CV_THRESH_BINARY);

		//cvErode(greyImg, greyImg, 0, 5);

		cvDilate(greyImg, greyImg, 0, 18);
		cvErode(greyImg, greyImg, 0, 10);

		cvShowImage("Stock", greyImg);

		CvMemStorage* storage = cvCreateMemStorage(0);
		CvSeq* contour = 0;
		cvFindContours(greyImg, storage, &contour, sizeof(CvContour), CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);

		for(;contour != 0; contour = contour->h_next){
			bndRect = cvBoundingRect(contour, 0);
			pt1.x = bndRect.x;
			pt1.y = bndRect.y;
			pt2.x = bndRect.x + bndRect.width;
			pt2.y = bndRect.y + bndRect.height;

			cvRectangle(colourImg, pt1, pt2, CV_RGB(255,0,0), 1);

		}

		cvShowImage("Modified", colourImg);


		frame_count++;
		char c = cvWaitKey(66);
		if (c == 27) {
			cvReleaseImage(&frame);
			cvReleaseImage(&temp);
			cvReleaseImage(&difference);
			cvReleaseImage(&greyImg);
			cvReleaseImage(&movingAverage);

			cvReleaseCapture(&fc);

			cvDestroyWindow("Stock");
			cvDestroyWindow("Modified");
			return 0;
		}
	}
}

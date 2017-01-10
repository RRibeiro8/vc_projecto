#include <iostream>
#include <vector>

#include <stdio.h>

#include <opencv2/core/core.hpp>        
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;
using namespace std;

#define CHESSBOARD_WIDTH 3
#define CHESSBOARD_HEIGHT 3

//Left and right corners
vector<Point2f> corners_l, corners_r;

Size board;
unsigned int size;

//State control
int state = 0;

//Callback for mouse events
void mouseEventsL(int event, int x, int y, int flags, void *param)
{
	if(event == EVENT_LBUTTONDOWN)
	{
		if(corners_l.size() < size)
		{
			corners_l.push_back(Point2f(x, y));
		}
	}
}

//Callback for mouse events
void mouseEventsR(int event, int x, int y, int flags, void *param)
{
	if(event == EVENT_LBUTTONDOWN)
	{
		if(corners_r.size() < size)
		{
			corners_r.push_back(Point2f(x, y));
		}
	}
}

//Main
int main(int argc, char **argv)
{
	//Chessboard Properties
	board = cvSize(CHESSBOARD_WIDTH, CHESSBOARD_HEIGHT);
	size = board.width * board.height;

	//Points
	vector<vector<Point3f> > virtual_points;
	vector<vector<Point2f> > image_points_l, image_points_r;

	//Configure window
	namedWindow("StereoL", CV_WINDOW_NORMAL);
	setMouseCallback("StereoL", mouseEventsL, nullptr);

	//Configure window
	namedWindow("StereoR", CV_WINDOW_NORMAL);
	setMouseCallback("StereoR", mouseEventsR, nullptr);

	//Calibrate parameters
	Mat camera_l, distortion_l;
	Mat camera_r, distortion_r;
	Mat R, T, E, F;

	Mat map1x, map1y, map2x, map2y;
	Mat R1, R2, P1, P2, Q;

	//Images
	Mat original_l, original_r;
	Mat img_l, img_r;
	Mat gray, undistorted_l, undistorted_r;
	Size resolution;

	//Chessboard coordinates
	vector<Point3f> chessboard;
	for(unsigned int j = 0; j < size; j++)
	{
		chessboard.push_back(Point3f(j / board.width, j % board.width, 0.0));
	}

	//Read image
	original_l = imread("../images/left_r01.jpg");
	original_r = imread("../images/right01.jpg");
	resolution = cvSize(original_l.cols, original_l.rows);

	//Failed to load images
	if(!original_r.data || !original_r.data)
	{
		printf("Could not load image");
		return -1;
	}

	while(true)
	{
		img_l = original_l;
		img_r = original_r;

		if(corners_l.size() == size && corners_r.size() == size)
		{
			image_points_l.push_back(corners_l);
			image_points_r.push_back(corners_r);
			virtual_points.push_back(chessboard);

			//Calibrate
			stereoCalibrate(virtual_points, image_points_l, image_points_r, camera_l, distortion_l, camera_r, distortion_r, resolution, R, T, E, F);

			//Undistort
			undistort(img_l, undistorted_l, camera_l, distortion_l);
			undistort(img_r, undistorted_r, camera_r, distortion_r);


			img_l = undistorted_l;
			img_r = undistorted_r;

			//Rectify
			stereoRectify(camera_l, distortion_l, camera_r, distortion_r, resolution, R, T, R1, R2, P1, P2, Q);


			initUndistortRectifyMap(camera_l, distortion_l, R1, P1, resolution, CV_32FC1, map1x, map1y);
			initUndistortRectifyMap(camera_r, distortion_r, R2, P2, resolution, CV_32FC1, map2x, map2y);

			remap(img_l, img_l, map1x, map1y, INTER_LINEAR);
			remap(img_r, img_r, map2x, map2y, INTER_LINEAR);

			Scalar c = CvScalar(123, 34, 234);
			for(int j = 0; j < resolution.height; j += 25)
			{
				line(img_l, Point2f(0, j), Point2f(resolution.width, j), c, 1);
				line(img_r, Point2f(0, j), Point2f(resolution.width, j), c, 1);
			}

			//Draw image
			imshow("StereoL", img_l);
			imshow("StereoR", img_r);

			break;
		}
		else
		{
			for(unsigned int i = 0; i < corners_l.size(); i++)
			{
				circle(img_l, corners_l[i], 2, Scalar(255,255,0), -1);
			}
			imshow("StereoL", img_l);

			for(unsigned int i = 0; i < corners_r.size(); i++)
			{
				circle(img_r, corners_r[i], 2, Scalar(255,255,0), -1);
			}
			imshow("StereoR", img_r);
		}

		//Get keyboard input
		int keyboard = waitKey(16);
		if(keyboard == 27)
		{
			break;
		}
	}

	waitKey(0);

	return 0;
}



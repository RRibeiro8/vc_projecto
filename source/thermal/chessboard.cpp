/***********************************************************************************
Name:           chessboard.cpp
Revision:
Author:         Paulo Dias
Comments:       ChessBoard Tracking


images
Revision:
Libraries:
***********************************************************************************/
#include <iostream>
#include <vector>

// OpenCV Includes
#include <stdio.h>
#include <opencv2/core/core.hpp>        
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/calib3d/calib3d.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace cv;
using namespace std;
// Function FindAndDisplayChessboard
// find corners in a cheesboard with board_w x board_h dimensions
// Display the corners in image and return number of detected corners
int FindAndDisplayChessboard(Mat image,int board_w,int board_h, vector<Point2f> *corners)
{
	int board_size = board_w * board_h;
	CvSize board_sz = cvSize(board_w,board_h);

	Mat grey_image;

	cvtColor(image, grey_image, CV_BGR2GRAY);

	// find chessboard corners
	bool found = findChessboardCorners(grey_image, board_sz, *corners,0);

	// Draw results
	if (true)
	{
		drawChessboardCorners(image, board_sz, Mat(*corners), found);
		imshow("Calibration",image);
		printf("\n Number of corners: %lu",corners->size());
		waitKey(0); 
	}
	return corners->size();
}

int main(int argc, char **argv)
{
	// ChessBoard Properties
	int n_boards = 9; //Number of images
	int board_w = 9;
	int board_h = 6;

	int board_sz = board_w * board_h;

	char filename[200];

	// Chessboard coordinates and image pixels
	vector<vector<Point3f> > object_points;
	vector<vector<Point2f> > image_points;

	// Corners detected in each image
	vector<Point2f> corners;

	int corner_count;

	Mat image;
	int i;


	int sucesses = 0;


	// chessboard coordinates
	vector<Point3f> obj;
	for(int j=0;j<board_sz;j++)
	obj.push_back(Point3f(float(j/board_w), float(j%board_w), 0.0));

	for (i=0;i<n_boards;i++)
	{
		// read image 
		sprintf(filename,"Chessboard_images/left_r%02d.jpg",i);
		printf("\nReading %s",filename);
		image = imread(filename, CV_LOAD_IMAGE_COLOR);


		if(!image.data) 
		{
			printf("\nCould not load image file: %s\n",filename);
			getchar();
			return 0;
		}

		// find and display corners
		corner_count = FindAndDisplayChessboard(image,board_w,board_h,&corners);

		if(corner_count == board_w * board_h)
		{
			image_points.push_back(corners);
			object_points.push_back(obj);
			sucesses++;
		}
	}

	vector<Mat> rvecs, tvecs;
		Mat camera_matrix = Mat(3, 3, CV_32FC1);
		Mat dist_coeffs;
		//CvSize sz = cvSize(board_w,board_h);
		CvSize sz = image.size();

		calibrateCamera(object_points, image_points, sz, camera_matrix, dist_coeffs, rvecs, tvecs, 0);

		cout << endl << "Intrinsics = "<< endl << " " << camera_matrix << endl << endl;
		
		cout << endl << "Distortion = "<< endl << " " << dist_coeffs << endl << endl;
		
		cout << endl << "Translations = "<< endl ;
		for (int k=0;k<n_boards;k++)
		{
			cout << endl << tvecs.at(k);
		}
		
		cout << endl << "Rotations= "<< endl ;

		for (int k=0;k<n_boards;k++)
		{
			cout << endl << rvecs.at(k);
		}	

		FileStorage fs("/home/pi/Desktop/Ribeiro/Projeto_VC/Chessboard_images/CamParams.xml", FileStorage::WRITE);
		fs << "cameraMatrix" << camera_matrix << "distCoeffs" << dist_coeffs;
		fs.release();
	
	return 0;
}

//Based on code from github.com/trishume/eyeLike made by Tristan Hume

#include "opencv2/opencv.hpp"

#ifndef PUPIL_DETECTOR
#define PUPIL_DETECTOR

using namespace std;
using namespace cv;

class PupilDetector
{
	public:
		static const int eye_width = 30;
		static const int blur_size = 3;
		static constexpr double gradient_threshold = 60.0;
		static const bool post_process_enabled = false;
		static constexpr float post_process_threshold = 0.97;

		static void scaleToFastSize(const Mat &src, Mat &dst)
		{
			resize(src, dst, Size(eye_width,((float)eye_width / src.cols) * src.rows));
		}

		static Mat getHorizontalGradient(const Mat &mat)
		{
			Mat out(mat.rows, mat.cols, CV_64F);

			for(int y = 0; y < mat.rows; ++y)
			{
				const uchar *Mr = mat.ptr<uchar>(y);
				double *Or = out.ptr<double>(y);

				Or[0] = Mr[1] - Mr[0];
				for(int x = 1; x < mat.cols - 1; ++x)
				{
					Or[x] = (Mr[x+1] - Mr[x-1])/2.0;
				}

				Or[mat.cols-1] = Mr[mat.cols-1] - Mr[mat.cols-2];
			}

			return out;
		}

		static bool insideBox(Point p, int y, int x)
		{
			return p.x >= 0 && p.x < x && p.y >= 0 && p.y < y;
		}

		static Mat removeEdges(Mat &mat)
		{
			rectangle(mat, Rect(0, 0, mat.cols, mat.rows), 255);

			Mat mask(mat.rows, mat.cols, CV_8U, 255);

			queue<Point> points;
			points.push(Point(0,0));

			while(!points.empty())
			{
				Point p = points.front();
				points.pop();

				if(mat.at<float>(p) == 0.0f)
				{
					continue;
				}

				//Fill in every direction
				//Right
				Point np(p.x + 1, p.y);
				if(insideBox(np, mat.rows, mat.cols))
				{
					points.push(np);
				}
				//Left
				np.x = p.x - 1; np.y = p.y;
				if(insideBox(np, mat.rows, mat.cols))
				{
					points.push(np);
				}
				//Down
				np.x = p.x; np.y = p.y + 1;
				if(insideBox(np, mat.rows, mat.cols))
				{
					points.push(np);
				}
				//Up
				np.x = p.x; np.y = p.y - 1;
				if(insideBox(np, mat.rows, mat.cols))
				{
					points.push(np);
				}

				//Kill it
				mat.at<float>(p) = 0.0f;
				mask.at<uchar>(p) = 0;
			}

			return mask;
		}

		static void testCenter(int x, int y, const Mat &weight, double gx, double gy, Mat &out)
		{
			//Test all possible centers
			for(int cy = 0; cy < out.rows; ++cy)
			{
				double *Or = out.ptr<double>(cy);
				const unsigned char *Wr = weight.ptr<unsigned char>(cy);

				for(int cx = 0; cx < out.cols; ++cx)
				{
					if(x == cx && y == cy)
					{
						continue;
					}

					//Create a vector from the possible center to the gradient origin
					Point candidate = Point(x - cx, y - cy);

					//Normalize d
					double magnitude = sqrt((candidate.x * candidate.x) + (candidate.y * candidate.y));
					candidate.x = candidate.x / magnitude;
					candidate.y = candidate.y / magnitude;

					double dot = candidate.x * gx + candidate.y * gy;
					dot = max(0.0, dot);

					//Square and multiply by the weight
					Or[cx] += dot * dot * Wr[cx];
				}
			}
		}

		static Point rescalePoint(Point point, Mat original)
		{
			float ratio = ((float)eye_width) / original.cols;

			int x = round(point.x / ratio);
			int y = round(point.y / ratio);

			return Point(x, y);
		}

		static Mat matrixMagnitude(const Mat &matX, const Mat &matY)
		{
			Mat mag(matX.rows, matX.cols, CV_64F);

			for(int y = 0; y < matX.rows; ++y)
			{
				const double *Xr = matX.ptr<double>(y), *Yr = matY.ptr<double>(y);
				double *Mr = mag.ptr<double>(y);

				for(int x = 0; x < matX.cols; ++x)
				{
					double gX = Xr[x], gY = Yr[x];
					double magnitude = sqrt((gX * gX) + (gY * gY));
					Mr[x] = magnitude;
				}
			}

			return mag;
		}

		static double computeDynamicThreshold(const cv::Mat &mat, double stdDevFactor)
		{
			Scalar stdMagnGrad, meanMagnGrad;
			meanStdDev(mat, meanMagnGrad, stdMagnGrad);
			double stdDev = stdMagnGrad[0] / sqrt(mat.rows * mat.cols);
			return stdDevFactor * stdDev + meanMagnGrad[0];
		}

		static Point findEyeCenter(Mat eye)
		{
			Mat eyeROIUnscaled = eye;
			Mat eyeROI;

			scaleToFastSize(eyeROIUnscaled, eyeROI);

			//Find the gradient
			Mat gradientX = getHorizontalGradient(eyeROI);
			Mat gradientY = getHorizontalGradient(eyeROI.t().a).t();

			//Compute all the magnitudes
			Mat mags = matrixMagnitude(gradientX, gradientY);

			//Compute the threshold
			double gradientThresh = computeDynamicThreshold(mags, gradient_threshold);

			//Normalize
			for(int y = 0; y < eyeROI.rows; ++y)
			{
				double *Xr = gradientX.ptr<double>(y), *Yr = gradientY.ptr<double>(y);
				const double *Mr = mags.ptr<double>(y);
				for(int x = 0; x < eyeROI.cols; ++x)
				{
					double gX = Xr[x], gY = Yr[x];
					double magnitude = Mr[x];
					if(magnitude > gradientThresh)
					{
						Xr[x] = gX / magnitude;
						Yr[x] = gY / magnitude;
					}
					else
					{
						Xr[x] = 0.0;
						Yr[x] = 0.0;
					}
				}
			}

			//Create a blurred and inverted image for weighting
			Mat weight;
			GaussianBlur(eyeROI, weight, Size(blur_size, blur_size), 0, 0);
			for(int y = 0; y < weight.rows; ++y)
			{
				unsigned char *row = weight.ptr<unsigned char>(y);

				for(int x = 0; x < weight.cols; ++x)
				{
					row[x] = 255 - row[x];
				}
			}

			//Run the algorithm
			Mat sum = Mat::zeros(eyeROI.rows,eyeROI.cols,CV_64F);

			//For each possible gradient location
			for(int y = 0; y < weight.rows; ++y)
			{
				const double *Xr = gradientX.ptr<double>(y), *Yr = gradientY.ptr<double>(y);
				for(int x = 0; x < weight.cols; ++x)
				{
					double gX = Xr[x], gY = Yr[x];
					if(gX == 0.0 && gY == 0.0)
					{
						continue;
					}
					testCenter(x, y, weight, gX, gY, sum);
				}
			}

			//Scale all the values down, basically averaging them
			double numGradients = weight.rows * weight.cols;

			Mat out;
			sum.convertTo(out, CV_32F, 1.0 / numGradients);

			//Find the maximum point
			Point max_point;
			double max_value;
			minMaxLoc(out, NULL, &max_value, NULL, &max_point);

			//Flood fill the edges
			if(post_process_enabled)
			{
				Mat floodClone;
				double floodThresh = max_value * post_process_threshold;
				threshold(out, floodClone, floodThresh, 0.0f, THRESH_TOZERO);

				Mat mask = removeEdges(floodClone);

				minMaxLoc(out, NULL, &max_value, NULL, &max_point,mask);
			}

			//Return point in the original scale
			return rescalePoint(max_point, eye);
		}
};
#endif

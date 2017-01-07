#include "opencv2/opencv.hpp"

#include "../math/Circle.cpp"
#include "../math/UUIDGenerator.cpp"

#ifndef EYE
#define EYE

using namespace cv;
using namespace std;

class Eye
{
	public:
		static const int LEFT = 0;
		static const int RIGHT = 1;

		//Eye geometry
		Circle circle;
		Point velocity;
		Point pupil;

		//Eye side
		int side;

		//Identification
		string uuid;

		//Constructor
		Eye()
		{
			this->circle = Circle();
			this->velocity = Point();
			this->pupil = Point();

			this->side = 0;

			this->uuid = UUIDGenerator::generate();
		}

		//Set eye side
		void setSide(int side)
		{
			this->side = side;
		}

		//Update eye state
		void update(Point center, float radius, Point pupil)
		{
			this->velocity.x = center.x - this->circle.x;
			this->velocity.y = center.y - this->circle.y;

			this->pupil.x = pupil.x;
			this->pupil.y = pupil.y;

			this->circle.x = center.x;
			this->circle.y = center.y;
			this->circle.radius = radius;
		}

		//Draw eye to image
		void draw(Mat image)
		{
			//Eye circle
			if(this->side == Eye::RIGHT)
			{
				cv::circle(image, this->circle.getCenter(), this->circle.radius, Scalar(0, 255, 255), 2);
			}
			else if(this->side == Eye::LEFT)
			{
				cv::circle(image, this->circle.getCenter(), this->circle.radius, Scalar(0, 255, 0), 2);
			}

			//Eye pupil
			line(image, this->circle.getCenter(), this->pupil, Scalar(255, 0, 255), 1);
			cv::circle(image, this->pupil, 3, Scalar(255, 0, 255), -1);
		}

		//String with eye description
		string toString()
		{
			return "UUID:" + this->uuid;
		}
};

#endif

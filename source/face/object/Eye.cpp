#include "opencv2/opencv.hpp"

#include "../math/Circle.cpp"
#include "../math/UUIDGenerator.cpp"

#ifndef EYE
#define EYE

using namespace cv;
using namespace std;

class Eye
{
	private:
		static const int DEFAULT_TTL = 10;

	public:
		static const int LEFT = 0;
		static const int RIGHT = 1;

		//Eye geometry
		Circle circle;
		Point velocity, pupil;

		//Time control
		bool updated;
		int frame, ttl;

		//Eye side
		int side;

		//Identification
		string uuid;

		//Constructor
		Eye()
		{
			updated = false;
			frame = 0;
			ttl = DEFAULT_TTL;

			side = 0;

			uuid = UUIDGenerator::generate();
		}

		//Set eye side
		void setSide(int side)
		{
			this->side = side;
		}

		//Set eye data
		void setEye(Point center, float radius, Point pupil)
		{
			velocity.x = center.x - this->circle.x;
			velocity.y = center.y - this->circle.y;

			circle.x = center.x;
			circle.y = center.y;
			circle.radius = radius;

			this->pupil.x = pupil.x;
			this->pupil.y = pupil.y;

			updated = true;
		}

		//Update
		bool update()
		{
			frame++;

			if(updated)
			{
				updated = false;
				ttl = DEFAULT_TTL;
			}
			else
			{
				ttl--;
			}

			return ttl > 0;
		}

		//Draw eye to image
		void draw(Mat image)
		{
			if(ttl > 0)
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
		}

		//String with eye description
		string toString()
		{
			return "UUID:" + this->uuid;
		}
};

#endif

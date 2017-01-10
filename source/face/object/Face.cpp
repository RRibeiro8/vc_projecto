#include <list>

#include "opencv2/opencv.hpp"

#include "../math/UUIDGenerator.cpp"
#include "Eye.cpp"
#include "Nose.cpp"
#include "Mouth.cpp"

#ifndef FACE
#define FACE

using namespace cv;
using namespace std;


class Face
{
	private:
		static const int DEFAULT_TTL = 30;

	public:
		Point position, velocity;
		Rect box;

		//Elements
		Eye eye_left, eye_right;
		Nose nose;
		Mouth mouth;

		//Time control
		bool updated;
		int frame, ttl;

		//Identification
		string uuid;

		//Constructor
		Face()
		{
			frame = 0;

			updated = false;
			ttl = DEFAULT_TTL;

			eye_left.setSide(Eye::LEFT);
			eye_right.setSide(Eye::RIGHT);

			uuid = UUIDGenerator::generate();
		}

		//Set face box
		void setBox(Rect rect)
		{
			velocity.x = (box.x + box.width * 0.5) - position.x;
			velocity.y = (box.y + box.height * 0.5) - position.y;

			position.x = box.x + box.width * 0.5;
			position.y = box.y + box.height * 0.5;

			box.x = rect.x;
			box.y = rect.y;
			box.width = rect.width;
			box.height = rect.height;

			updated = true;
		}

		//Set left eye
		void setLeftEye(Point center, float radius, Point pupil)
		{
			eye_left.setEye(center, radius, pupil);
		}

		//Set right eye
		void setRightEye(Point center, float radius, Point pupil)
		{
			eye_right.setEye(center, radius, pupil);
		}

		//Set nose box
		void setNoseBox(Rect rect)
		{
			nose.setBox(rect);
		}

		//Set mouth box
		void setMouthBox(Rect rect)
		{
			mouth.setBox(rect);
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

		//Draw to image
		void draw(Mat image)
		{
			//Draw text overlay
			putText(image, "UUID:" + uuid, Point(box.x, box.y - 10), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 0, 255), 1);
			putText(image, "Frame:" + to_string(frame) + " TTL:" + to_string(ttl) + " Speed: (" + to_string(velocity.x) + ", " + to_string(velocity.y) + ")", Point(box.x, box.y - 25), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 0, 255), 1);

			rectangle(image, box, Scalar(255, 0, 0), 2);

			eye_left.update();
			eye_left.draw(image);

			eye_right.update();
			eye_right.draw(image);

			nose.update();
			nose.draw(image);

			mouth.update();
			mouth.draw(image);
		}

		//Create string with face description
		string toString()
		{
			return "UUID:" + uuid + " Frame:" + to_string(frame) + " TTL:" + to_string(ttl) + " Speed: (" + to_string(velocity.x) + ", " + to_string(velocity.y) + ")";
		}
};

#endif

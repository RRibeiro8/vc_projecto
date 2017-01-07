#include <list>

#include "opencv2/opencv.hpp"

#include "../math/UUIDGenerator.cpp"
#include "Eye.cpp"
#include "Nose.cpp"

#ifndef FACE
#define FACE

using namespace cv;
using namespace std;


class Face
{
	public:
		static const int DEFAULT_TTL = 20;

		//Face Geometry
		Rect box;
		Point velocity;

		//Elements
		Eye eye_left, eye_right;
		Nose nose;

		//Time control
		bool updated;
		int frame, ttl;

		//Identification
		string uuid;

		//Constructor
		Face()
		{
			box = Rect();
			velocity = Point();

			frame = 0;

			updated = true;
			ttl = DEFAULT_TTL;

			eye_left.setSide(Eye::LEFT);
			eye_right.setSide(Eye::RIGHT);

			uuid = UUIDGenerator::generate();
		}

		//Update face box
		void update(Rect rect)
		{
			velocity.x = rect.x - box.x;
			velocity.y = rect.y - box.y;

			box.x = rect.x;
			box.y = rect.y;
			box.width = rect.width;
			box.height = rect.height;

			updated = true;
		}

		//Update tracking
		bool updateTracking()
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

		//Update left eye
		void updateLeftEye(Point center, float radius, Point pupil)
		{
			eye_left.update(center, radius, pupil);
		}

		//Update right eye
		void updateRightEye(Point center, float radius, Point pupil)
		{
			eye_right.update(center, radius, pupil);
		}

		//Update nose
		void updateNose(Rect rect)
		{
			nose.update(rect);
		}

		//Draw face to image
		void draw(Mat image)
		{
			putText(image, toString(), Point(box.x, box.y - 10), FONT_HERSHEY_SIMPLEX, 0.4, Scalar(255, 0, 255), 1);

			rectangle(image, box, Scalar(255, 0, 0), 2);

			eye_left.draw(image);
			eye_right.draw(image);

			nose.draw(image);
		}

		//Create string with face description
		string toString()
		{
			return "UUID:" + uuid + " Frame:" + to_string(frame) + " TTL:" + to_string(ttl);
		}
};

#endif

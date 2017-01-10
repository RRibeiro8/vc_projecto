#include "opencv2/opencv.hpp"

#include "../math/UUIDGenerator.cpp"

#ifndef MOUTH
#define MOUTH

using namespace cv;
using namespace std;

class Mouth
{
	private:
		static const int DEFAULT_TTL = 10;

	public:
		Point position, velocity;
		Rect box;

		//Time control
		bool updated;
		int frame, ttl;

		//Identification
		string uuid;

		//Constructor
		Mouth()
		{
			updated = false;
			frame = 0;
			ttl = DEFAULT_TTL;

			uuid = UUIDGenerator::generate();
		}

		//Set box
		void setBox(Rect box)
		{
			velocity.x = (box.x + box.width * 0.5) - position.x;
			velocity.y = (box.y + box.height * 0.5) - position.y;

			position.x = box.x + box.width * 0.5;
			position.y = box.y + box.height * 0.5;

			this->box.x = box.x;
			this->box.y = box.y;
			this->box.width = box.width;
			this->box.height = box.height;

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

		//Draw image
		void draw(Mat image)
		{
			if(ttl > 0)
			{
				ellipse(image, position, Size(box.width * 0.5, box.height * 0.5), 0, 0, 360, Scalar(201, 255, 120), 2);
			}
		}

		//String with Mouth description
		string toString()
		{
			return "UUID:" + uuid;
		}
};

#endif

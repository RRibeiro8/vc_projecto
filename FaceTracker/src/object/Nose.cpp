#include "opencv2/opencv.hpp"

#include "../math/UUIDGenerator.cpp"

#ifndef NOSE
#define NOSE

using namespace cv;
using namespace std;

class Nose
{
	public:
		Rect box;
		Point velocity;

		//Identification
		string uuid;

		//Constructor
		Nose()
		{
			this->box = Rect();
			this->velocity = Point();

			this->uuid = UUIDGenerator::generate();
		}

		//Update nose box
		void update(Rect box)
		{
			velocity.x = box.x - this->box.x;
			velocity.y = box.y - this->box.y;

			this->box.x = box.x;
			this->box.y = box.y;
			this->box.width = box.width;
			this->box.height = box.height;
		}

		//Draw nose to image
		void draw(Mat image)
		{
			Point center(box.x + box.width * 0.5, box.y + box.height * 0.5);
			cv::ellipse(image, center, Size(box.width * 0.5, box.height * 0.5), 0, 0, 360, Scalar(0, 0, 255), 2);
		}

		//String with nose description
		string toString()
		{
			return "UUID:" + uuid;
		}
};

#endif

#include "opencv2/opencv.hpp"

#include "/home/pi/Desktop/Ribeiro/Projeto_VC/Eye_Tracker/math/Circle.cpp"

#ifndef MATH_RECTUTILS
#define MATH_RECTUTILS

using namespace cv;

class RectUtils
{
	public:
		static bool contains(Rect rect, float x, float y)
		{
			return rect.x <= x && rect.x + rect.width >= x && rect.y <= y && rect.y + rect.height >= y;
		}

		static bool contains(Rect rect, Circle circle)
		{
			return (circle.x - circle.radius >= rect.x) && (circle.x + circle.radius <= rect.x + rect.width) && (circle.y - circle.radius >= rect.y) && (circle.y + circle.radius <= rect.y + rect.height);
		}

		static bool overlaps(Rect rect, Rect r)
		{
			return rect.x < r.x + r.width && rect.x + rect.width > r.x && rect.y < r.y + r.height && rect.y + rect.height > r.y;
		}
};

#endif

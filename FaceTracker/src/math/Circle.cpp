#include "opencv2/opencv.hpp"

#ifndef MATH_CIRCLE
#define MATH_CIRCLE

#define PI 3.14159265358979323846
using namespace cv;

class Circle
{
	public:
		float x, y;
		float radius;

		Circle()
		{
			this->x = 0.0;
			this->y = 0.0;
			this->radius = 1.0;
		}

		Circle(float x, float y, float r)
		{
			this->x = x;
			this->y = y;
			this->radius = r;
		}

		void set(float x, float y, float radius)
		{
			this->x = x;
			this->y = y;
			this->radius = radius;
		}

		Point getCenter()
		{
			return Point(x, y);
		}

		bool contains(float x, float y)
		{
			x = this->x - x;
			y = this->y - y;
			return x * x + y * y <= radius * radius;
		}

		bool contains (Circle c)
		{
			float rd = radius - c.radius;
			if(rd < 0.0)
			{
				return false;
			}

			float dx = x - c.x;
			float dy = y - c.y;
			float dst = dx * dx + dy * dy;
			float r = radius + c.radius;

			return !(rd * rd < dst) && (dst < r * r);
		}

		bool overlaps(Circle c)
		{
			float dx = x - c.x;
			float dy = y - c.y;
			float r = radius + c.radius;

			return (dx * dx + dy * dy) < (r * r);
		}

		float area()
		{
			return this->radius * this->radius * PI;
		}
};

#endif

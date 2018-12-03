#ifndef POINT2D_H
#define POINT2D_H

#include <math.h>

//A struct for 2-dimensional points
struct Point2D {
	float x; //X-position of the point
	float y; //Y-Position of the point
};

//Regular distance function for two point2D objects
float distance(Point2D p1, Point2D p2) {
	float x1 = p1.x;
	float y1 = p1.y;
	float x2 = p2.x;
	float y2 = p2.y;
	float d = pow(x2-x1, 2) + pow(y2-y1, 2);
	return pow(d, 0.5);
}

#endif
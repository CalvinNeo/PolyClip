#pragma once
#include "stdafx.h"
#include <vector>

#define ZERO (1e-14)
#define ISZERO(x) (abs(x)<=ZERO)

using std::vector;
/*
*点
*/
class Point
{
public:
	double x;
	double y;

	Point(){}
	Point(double x, double y)
	{
		this->x = x;
		this->y = y;
	}
	Point(const Point& p)
	{
		this->x = p.x;
		this->y = p.y;
	}

	CPoint ToCPoint()
	{
		CPoint cp((int)x,(int)y);
		return cp;
	}

	bool Point::operator==(Point& l)
	{
		if(ISZERO(l.x-x)&&ISZERO(l.y-y))
			return true;
		else
			return false;
	}

	bool Point::operator!=(Point& l)
	{
		return !(l==(*this));
	}

	double Point::operator*(Point& l)
	{
		return l.x*x+l.y*y;
	}

	static double CrossProduct(Point&l, Point&r)
	{
		return l.x*r.y-l.y*r.x;
	}
};
/*
*线
*/
class Line
{
public:
	Point startpoint;
	Point endpoint;
	Line(){}
	Line(double sx,double sy,double ex,double ey)
	{
		startpoint = Point(sx,sy);
		endpoint=Point(ex,ey);
	}
	Line(Point startPoint, Point endPoint)
	{
		this->startpoint = startPoint;
		this->endpoint = endPoint;
	}
	
};
/*
*圆
*/
struct Circle
{
public:
	Point center;
	double radius;
	Circle(){}
	Circle(double rx, double ry, double r)
	{
		center = Point(rx,ry);
		radius = r;
	}
};
/*
*边界
*/
class Boundary
{
public:
	void AddVertex(Point point)
	{
		vertexs.push_back(point);
		if(vertexs.size()==1)
		{
			leftPoint = point;
			rightPoint = point;
			topPoint = point;
			bottomPoint = point;
		}
		else
		{
			if(point.x<leftPoint.x)
				leftPoint = point;
			else if(point.x>rightPoint.x)
				rightPoint = point;
			if(point.y>topPoint.y)
				topPoint = point;
			else if(point.y<bottomPoint.y)
				bottomPoint = point;			
		}
		
	}
	vector<Point> vertexs;
	bool isConvex;
	Point leftPoint;
	Point rightPoint;
	Point topPoint;
	Point bottomPoint;

	Boundary(){}
	Boundary(const Boundary& b)
	{
		vertexs = b.vertexs;
		leftPoint = b.leftPoint;
		rightPoint = b.rightPoint;
		topPoint = b.topPoint;
		bottomPoint = b.bottomPoint;
		isConvex = b.isConvex;
	}


private:
	
};


/*
*位置关系
*/
enum Relation
{
	Inside,//内部
	Outside,//外部
	Intersect,//相交
	Tangent,//相切
	TangentAndInside//相切，且在内部
};
/*
*被裁减的线段
*/
struct LineClipped
{
	Point startPoint;
	Point endPoint;
};

/*
*一条线段被裁剪后的结果以及其互相的位置关系
*/
struct ClipLineResult
{
	vector<Point> intersection;//交点
	vector<LineClipped> linesClipped;//裁剪出来的线段，还没用到
	Relation relation;//线段与边界关系
};


/*
*被裁减的圆
*/
struct ArgClipped
{
	double startAngle;
	double endAngle;
};

/*
*一个圆被裁剪后的结果以及其互相的位置关系
*/
struct ClipCircleResult
{
	vector<double> angles;//裁剪角度区间，相切的为2个相同的角度
	vector<ArgClipped> clippedArgs;
	Relation relation;
};
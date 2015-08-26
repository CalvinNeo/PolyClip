#pragma once
#include "stdafx.h"
#include <vector>

#define ZERO (1e-14)
#define ISZERO(x) (abs(x)<=ZERO)

using std::vector;
/*
*��
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
*��
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
*Բ
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
*�߽�
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
*λ�ù�ϵ
*/
enum Relation
{
	Inside,//�ڲ�
	Outside,//�ⲿ
	Intersect,//�ཻ
	Tangent,//����
	TangentAndInside//���У������ڲ�
};
/*
*���ü����߶�
*/
struct LineClipped
{
	Point startPoint;
	Point endPoint;
};

/*
*һ���߶α��ü���Ľ���Լ��以���λ�ù�ϵ
*/
struct ClipLineResult
{
	vector<Point> intersection;//����
	vector<LineClipped> linesClipped;//�ü��������߶Σ���û�õ�
	Relation relation;//�߶���߽��ϵ
};


/*
*���ü���Բ
*/
struct ArgClipped
{
	double startAngle;
	double endAngle;
};

/*
*һ��Բ���ü���Ľ���Լ��以���λ�ù�ϵ
*/
struct ClipCircleResult
{
	vector<double> angles;//�ü��Ƕ����䣬���е�Ϊ2����ͬ�ĽǶ�
	vector<ArgClipped> clippedArgs;
	Relation relation;
};
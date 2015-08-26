#include "stdafx.h"
#include "PolygonClip.h"

#define CROSSPRODUCT(l,r) (l.x*r.y-l.y*r.x)

PolygonCircleClip::PolygonCircleClip(Boundary* pBoundary, Circle* pCircle) {
	this->pBoundary = pBoundary;
	this->pCircle = pCircle;
}

PolygonCircleClip::~PolygonCircleClip() {
}

BOOL PolygonCircleClip::GetClipResult(ClipCircleResult &result) {
	size_t vetexSize = pBoundary->vertexs.size();

	result.angles.clear();
	result.clippedArgs.clear();
	unsigned int i;
	for (i = 0; i < vetexSize - 1; i++) {
		GetClipByLine(pBoundary->vertexs[i], pBoundary->vertexs[i + 1], result);
	}
	if (result.angles.size() % 2 != 0)
		return FALSE;
	if (result.angles.size() == 0)//无交点
	{
		if (IsPointInside(pBoundary,pCircle->center))//圆心在内部，则圆就在内部
		{
			bool isbigger = false;
			for (size_t i = 0; i < pBoundary->vertexs.size(); i++)
			{
				if ( (pBoundary->vertexs[i].x - pCircle->center.x)*(pBoundary->vertexs[i].x - pCircle->center.x)
					+ (pBoundary->vertexs[i].y - pCircle->center.y)*(pBoundary->vertexs[i].y - pCircle->center.y)
					<= pCircle->radius*pCircle->radius
					)
				{
					isbigger = true;
				}
			}
			if (isbigger) 
				result.relation = Outside;
			else
				result.relation = Inside;
		}
		else
			result.relation = Outside;
		return TRUE;
	}
	else {
		result.relation = Intersect;
	}
	sort(result.angles.begin(), result.angles.end());
	for (i = 0; i < result.angles.size(); i++) {
		Point midPoint;
		double midAngle;
		double a1, a2;
		if (i == result.angles.size() - 1) {
			a1 = result.angles[i];
			a2 = result.angles[0];
		}
		else {
			a1 = result.angles[i];
			a2 = result.angles[i + 1];
		}
		if (a1 < a2) {
			midAngle = (a1 + a2) / 2.0;
		}
		else if (a1 >= a2) {
			if (a1 + a2 < M_PI) {
				midAngle = (a1 + a2) / 2.0 + M_PI;
			}
			else {
				midAngle = (a1 + a2) / 2.0 - M_PI;
			}
		}
		midPoint.x = pCircle->center.x + pCircle->radius*cos(midAngle);
		midPoint.y = pCircle->center.y + pCircle->radius*sin(midAngle);

		//中心圆弧是否在内部
		if (IsPointInside(pBoundary,midPoint)) {
			ArgClipped ac;
			ac.startAngle = a1;
			ac.endAngle = a2;
			result.clippedArgs.push_back(ac);//添加裁剪的弧 
		}
	}
	return TRUE;
}

bool PolygonCircleClip::IsPointInside(Boundary* pBoundary, Point & midPoint) {
	int count = 0;
	for (unsigned int k = 0; k < pBoundary->vertexs.size() - 1; k++) {
		double x0, x1, y0, y1;
		x0 = min(pBoundary->vertexs[k].x, pBoundary->vertexs[k + 1].x);
		x1 = max(pBoundary->vertexs[k].x, pBoundary->vertexs[k + 1].x);
		y0 = min(pBoundary->vertexs[k].y, pBoundary->vertexs[k + 1].y);
		y1 = max(pBoundary->vertexs[k].y, pBoundary->vertexs[k + 1].y);
		if (midPoint.y > y1 || midPoint.y < y0)//无交点			
		{
		}
		else {
			if (midPoint.x > x1)//无交点
			{

			}
			else if (midPoint.x < x0) {
				if (ISZERO(y0 - midPoint.y) && midPoint.y < y1)
					count++;//交点计数，与下边界重合
				else if ((y0 < midPoint.y && ISZERO(midPoint.y - y1)) ||
					(ISZERO(y0 - y1) && ISZERO(midPoint.y - y0))) {
					//交点不计
				}
				else {
					count++;//交点计数
				}

			}
			else {
				if (GetCross(midPoint, Point(LIMITPOINT.x, midPoint.y), pBoundary->vertexs[k], pBoundary->vertexs[k + 1])) {
					if (ISZERO(y0 - midPoint.y) && midPoint.y < y1)
						count++;
					else if ((y0 < midPoint.y && ISZERO(midPoint.y - y1)) ||
						(ISZERO(midPoint.y - y0) && ISZERO(midPoint.y - y1))) {
						//交点不计
					}
					else {
						count++;
					}
				}
			}

		}
	}
	if (count % 2 == 1)
		return true;
	else {
		return false;
	}
}


bool PolygonCircleClip::GetCross(Point & p1, Point & p2, Point & q1, Point & q2) {
	Point q2p1(p1.x - q2.x, p1.y - q2.y);
	Point q2q1(q1.x - q2.x, q1.y - q2.y);
	Point q2p2(p2.x - q2.x, p2.y - q2.y);
	Point p2p1(p1.x - p2.x, p1.y - p2.y);
	Point p2q1(q1.x - p2.x, q1.y - p2.y);
	Point p2q2(q2.x - p2.x, q2.y - p2.y);

	if (CROSSPRODUCT(q2p1, q2q1) * CROSSPRODUCT(q2p2, q2q1) <= 0 &&
		CROSSPRODUCT(p2q1, p2p1) * CROSSPRODUCT(p2q2, p2p1) <= 0)
		return true;
	else
		return false;
}

void PolygonCircleClip::GetClipByLine(Point& p1, Point& p2, ClipCircleResult& result) {
	double x0 = pCircle->center.x, y0 = pCircle->center.y, r = pCircle->radius, x1 = p1.x, x2 = p2.x, y1 = p1.y, y2 = p2.y;
	double A = x1*x1 + x2*x2 + y1*y1 + y2*y2 - 2 * x1*x2 - 2 * y1*y2;
	double B = 2 * (x0*x1 + x1*x2 + y0*y1 + y1*y2 - x0*x2 - y0*y2 - x1*x1 - y1*y1);
	double C = x0*x0 + x1*x1 + y0*y0 + y1*y1 - 2 * x0*x1 - 2 * y0*y1 - r*r;
	double delta = B*B - 4 * A*C;
	if (delta < 0)//无交点
		return;
	double t1, t2;
	result.relation = Intersect;
	if (ISZERO(delta))//相切,认为无交点
	{
		result.relation = Tangent;
		return;
	}
	else {
		t1 = (-B - sqrt(delta)) / (2.0*A);
		t2 = (-B + sqrt(delta)) / (2.0*A);
	}

	if (t1 >= 0 && t1 <= 1) {
		double x = x1 + (x2 - x1)*t1, y = y1 + (y2 - y1)*t1;
		double angle = acos((x - x0) / r);
		if (y < y0) angle = 2 * M_PI - angle;
		result.angles.push_back(angle);
	}
	if (t2 >= 0 && t2 <= 1) {
		double x = x1 + (x2 - x1)*t2, y = y1 + (y2 - y1)*t2;
		double angle = acos((x - x0) / r);
		if (y < y0) angle = 2 * M_PI - angle;
		result.angles.push_back(angle);
	}
}
#pragma once

#include "GraphicEntity.h"
#define _USE_MATH_DEFINES
#include "math.h"
#include <iostream>
#include <algorithm>
#include <mutex>
#include "MemoryBitmap.h"
#include "Drawer.h"

using namespace std;

#define CANVAS_WIDTH	800
#define CANVAS_HEIGHT	600
#define LIMITPOINT Point(1000,1000)//极限远的点，用于进行判断点关于多边形的位置

/*
 *多边形对线段的裁剪任务
 *采用坐标变换方法，先尽可能的去处无效边，进行坐标变换，再求裁剪线段的与剩余边的交点
 */
class PolygonLineClip
{
public:
	PolygonLineClip(Boundary* pBoundary, Line* pLine);
	~PolygonLineClip(void);
	BOOL GetClipResult(ClipLineResult &result);

private:
	Boundary* pBoundary;
	Line* pLine;
};


/*
 *多边形对圆的裁剪任务
 *采用对多边形的每条边遍历求与圆的交点个数的方式
 */
class PolygonCircleClip
{
public:
	PolygonCircleClip(Boundary* pBoundary, Circle* pCircle);
	~PolygonCircleClip();
	BOOL GetClipResult(ClipCircleResult &result);
	static bool IsPointInside(Boundary* pBoundary, Point &point);
	static bool GetCross(Point &p1, Point &p2, Point &q1, Point& q2);
private:
	Boundary* pBoundary;
	Circle* pCircle;

	void GetClipByLine(Point& p1, Point& p2, ClipCircleResult& result);
	
};

/*
*总裁剪任务
*/
class PolygonClipTask {
private:
	std::mutex drawMutex_;
public:
	PolygonClipTask() :
		memoryBitmap_(800, 600) {
	}

	vector<Line> lines;
	vector<Circle> circles;
	Boundary boundary;
	//存储所有的边界
	vector<Boundary> boundaries;

	int shapeCount;//总数
	int boundaryCount;//边界数
	int crossCount;//相交数
	int crossPointCount;//相交交点数
	int noCrossCount;//无交点数
	int insideCount;//界内数
	int outsiedCount;//界外数
	

	void clipLine(std::size_t i, std::size_t size);
	void clipCircle(std::size_t i, std::size_t size);
	void performClip();
	void clearTest();

	void drawArc(float s,float e, std::vector<std::vector<std::pair<std::uint32_t, std::uint32_t>>> & c);

	MemoryBitmap memoryBitmap_;
};
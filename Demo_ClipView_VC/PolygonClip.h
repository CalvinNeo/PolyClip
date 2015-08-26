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
#define LIMITPOINT Point(1000,1000)//����Զ�ĵ㣬���ڽ����жϵ���ڶ���ε�λ��

/*
 *����ζ��߶εĲü�����
 *��������任�������Ⱦ����ܵ�ȥ����Ч�ߣ���������任������ü��߶ε���ʣ��ߵĽ���
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
 *����ζ�Բ�Ĳü�����
 *���öԶ���ε�ÿ���߱�������Բ�Ľ�������ķ�ʽ
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
*�ܲü�����
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
	//�洢���еı߽�
	vector<Boundary> boundaries;

	int shapeCount;//����
	int boundaryCount;//�߽���
	int crossCount;//�ཻ��
	int crossPointCount;//�ཻ������
	int noCrossCount;//�޽�����
	int insideCount;//������
	int outsiedCount;//������
	

	void clipLine(std::size_t i, std::size_t size);
	void clipCircle(std::size_t i, std::size_t size);
	void performClip();
	void clearTest();

	void drawArc(float s,float e, std::vector<std::vector<std::pair<std::uint32_t, std::uint32_t>>> & c);

	MemoryBitmap memoryBitmap_;
};
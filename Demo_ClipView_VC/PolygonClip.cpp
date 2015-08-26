#include "stdafx.h"
#include "prjClip.h"
#include "mainDialog.h"
#include "afxdialogex.h"
#include <TlHelp32.h>
#include "PolygonClip.h"
#include <thread>
#include <memory>
#include "DataCollection.h"
#include "XMLParser.h"
#include <mutex>
#include "mainDialog.h"

void PolygonClipTask::clipLine(std::size_t k, std::size_t size) {
	std::size_t _crossCount = 0;						//相交数
	std::size_t _crossPointCount = 0;					//相交交点数
	std::size_t _noCrossCount = 0;						//无交点数
	std::size_t _insideCount = 0;						//界内数
	std::size_t _outsiedCount = 0;						//界外数

	ClipLineResult lineResult;
	auto end = k + size;
	for (std::size_t i = k; i < end; ++i) {
		Point sp = lines[i].startpoint;
		Point ep = lines[i].endpoint;
		PolygonLineClip plc(&boundary, &lines[i]);
		plc.GetClipResult(lineResult);
		_crossPointCount += lineResult.intersection.size();
		if (lineResult.relation == Inside)//线在内部
		{
			Drawer::DrawLine3(memoryBitmap_,
				std::uint32_t(lines[i].startpoint.x), std::uint32_t(lines[i].startpoint.y),
				std::uint32_t(lines[i].endpoint.x), std::uint32_t(lines[i].endpoint.y),
				MemoryBitmap::kGreenColor);

			_noCrossCount++;
			_insideCount++;
		}
		else if (lineResult.relation == Intersect)//x相交
		{
			_crossCount++;
			for (std::size_t j = 0; j < lineResult.linesClipped.size(); j++) {
				Drawer::DrawLine3(memoryBitmap_,
					std::uint32_t(lineResult.linesClipped[j].startPoint.x), std::uint32_t(lineResult.linesClipped[j].startPoint.y),
					std::uint32_t(lineResult.linesClipped[j].endPoint.x), std::uint32_t(lineResult.linesClipped[j].endPoint.y),
					MemoryBitmap::kGreenColor);
			}
		}
		else//线在外部
		{
			_noCrossCount++;
			_outsiedCount++;
		}
	}
	std::lock_guard<std::mutex> lock(drawMutex_);
	crossCount += _crossCount;
	crossPointCount += _crossPointCount;
	noCrossCount += _noCrossCount;
	insideCount += _insideCount;
	outsiedCount += _outsiedCount;
}

void PolygonClipTask::clipCircle(std::size_t k, std::size_t size) {
	std::size_t _crossCount = 0;						//相交数
	std::size_t _crossPointCount = 0;					//相交交点数
	std::size_t _noCrossCount = 0;						//无交点数
	std::size_t _insideCount = 0;						//界内数
	std::size_t _outsiedCount = 0;						//界外数

	std::vector<std::vector<std::pair<std::uint32_t, std::uint32_t>>> vec;
	vec.resize(9);
	for (size_t i = 0; i < 8; i++) {
		vec[i].reserve(600);
	}
	vec[8].reserve(6400);

	ClipCircleResult circleResult;
	auto end = k + size;
	for (std::size_t i = k; i < end; ++i) {
		PolygonCircleClip pcc(&boundary, &circles[i]);
		pcc.GetClipResult(circleResult);

		if (circleResult.relation == Relation::Inside)//在内部
		{
			_noCrossCount++;
			_insideCount++;
			Drawer::DrawCircle1(memoryBitmap_,
				std::uint32_t(circles[i].center.x), std::uint32_t(circles[i].center.y), std::uint32_t(circles[i].radius),
				MemoryBitmap::kBlueColor);
			continue;
		}
		if (circleResult.relation == Relation::Intersect) {
			_crossCount++;
			_crossPointCount += circleResult.clippedArgs.size() * 2;

			//Drawer::FillCircle2(vec, circles[i].center.x, circles[i].center.y, circles[i].radius);

			for (std::size_t j = 0; j < circleResult.clippedArgs.size(); j++) {
				//drawArc(circleResult.clippedArgs[j].startAngle, circleResult.clippedArgs[j].endAngle, vec);
				Drawer::DrawArc3(
					memoryBitmap_, 
					circles[i].center.x, circles[i].center.y, circles[i].radius, 
					circleResult.clippedArgs[j].startAngle, circleResult.clippedArgs[j].endAngle,
					MemoryBitmap::kBlueColor
					);
			}
		}
		else {
			_noCrossCount++;
			_outsiedCount++;
		}
	}

	std::lock_guard<std::mutex> lock(drawMutex_);
	crossCount += _crossCount;
	crossPointCount += _crossPointCount;
	noCrossCount += _noCrossCount;
	insideCount += _insideCount;
	outsiedCount += _outsiedCount;
}

void PolygonClipTask::drawArc(float s, float e, std::vector<std::vector<std::pair<std::uint32_t, std::uint32_t>>> & c) {
	if (s > e) {
		drawArc(s, 3.14159265358979323846 * 2, c);
		drawArc(0, e, c);
	} else {
		double pSize = static_cast<double>(c[8].size());
		std::size_t startI = std::uint32_t(pSize*  s / (2 * 3.14159265358979323846));
		std::size_t endI = std::uint32_t(pSize * e / (2 * 3.14159265358979323846));

		if (circles.size() > 10000) {
			std::uint32_t p = pSize*0.005;
			if (p < 3) {
				p = 3;
			}

			startI += p;

			if (endI > p) {
				endI -= p;
			} else {
				endI = 0;
			}
		}
		
		for (std::size_t i = startI; i < endI; ++i) {
			memoryBitmap_.SetPixel(c[8][i].first, c[8][i].second, MemoryBitmap::kBlueColor);
		}
	}
}

void PolygonClipTask::performClip() {
	memoryBitmap_.ClearPixelData();

	//算法部分
	unsigned int i = 0, j = 0;
	shapeCount = 0;							//总数
	boundaryCount = 1;						//边界数
	crossCount = 0;							//相交数
	crossPointCount = 0;					//相交交点数
	noCrossCount = 0;						//无交点数
	insideCount = 0;						//界内数
	outsiedCount = 0;						//界外数

	shapeCount += circles.size();			//圆的个数
	shapeCount += lines.size();				//线的个数

	const static std::size_t core = std::thread::hardware_concurrency(); //std::thread::hardware_concurrency()

#ifndef SINGLE_BOUNDARIES
	for (size_t boundary_index = 0; boundary_index < boundaries.size(); boundary_index++)
	{
		boundary = boundaries[boundary_index];
#endif
		{
			std::size_t groupSize = lines.size() / core;
			std::size_t size = lines.size();
			if (groupSize == 0) {
				groupSize = lines.size();
			}
			std::vector<std::shared_ptr<std::thread>> tts;
			for (std::size_t i = 0; i < size;) {
				auto curGroupSize = min(groupSize, size - i);
				auto tt = std::make_shared<std::thread>(&PolygonClipTask::clipLine, this, i, curGroupSize);
				tts.push_back(tt);
				i += curGroupSize;
			}

			for (auto & k : tts) {
				if (k->joinable()) {
					k->join();
				}
			}
		}

		{
			std::size_t groupSize = circles.size() / core;
			std::size_t size = circles.size();
			if (groupSize == 0) {
				groupSize = circles.size();
			}
			std::vector<std::shared_ptr<std::thread>> tts;
			for (std::size_t i = 0; i < size;) {
				auto curGroupSize = min(groupSize, size - i);
				auto tt = std::make_shared<std::thread>(&PolygonClipTask::clipCircle, this, i, curGroupSize);
				tts.push_back(tt);
				i += curGroupSize;
			}

			for (auto & k : tts) {
				if (k->joinable()) {
					k->join();
				}
			}
			tts.resize(0);
		}

		{
			vector<Point>::iterator iter = boundary.vertexs.begin();
			if (iter == boundary.vertexs.end())
				continue;
			Drawer::DrawLine3(memoryBitmap_,
				std::uint32_t(boundary.vertexs[0].x), std::uint32_t(boundary.vertexs[0].y),
				std::uint32_t(boundary.vertexs[boundary.vertexs.size() - 1].x), std::uint32_t(boundary.vertexs[boundary.vertexs.size() - 1].y),
				MemoryBitmap::kRedColor);
			for (std::size_t i = 0; i < boundary.vertexs.size() - 1; ++i) {
				Drawer::DrawLine3(memoryBitmap_,
					std::uint32_t(boundary.vertexs[i].x), std::uint32_t(boundary.vertexs[i].y),
					std::uint32_t(boundary.vertexs[i + 1].x), std::uint32_t(boundary.vertexs[i + 1].y),
					MemoryBitmap::kRedColor);
			}
		}

#ifndef SINGLE_BOUNDARIES
	}
#endif
}

void PolygonClipTask::clearTest() {
#ifndef SINGLE_BOUNDARIES
	boundaries.clear();
#else
	boundary.vertexs.clear();
#endif

	circles.clear();
	lines.clear();

	memoryBitmap_.ClearPixelData();
}
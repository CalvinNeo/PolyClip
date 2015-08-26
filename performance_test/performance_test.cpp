// performance_test.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include <Windows.h>
#include "..\Demo_ClipView_VC\scarlet\io\ioutil.hpp"
#include "..\Demo_ClipView_VC\MemoryBitmap.h"
#include "..\Demo_ClipView_VC\Drawer.h"
#include "..\Demo_ClipView_VC\scarlet\performance.hpp"

static const std::size_t testDrawLineCount = 1000000;
static const std::size_t testDrawCircleCount = 1000000;

int main(int argc,char * argv[]) {
	MemoryBitmap mbitmap(800, 600);

	/*std::vector<std::uint32_t> meshCircle;
	Drawer::FillCircle1(meshCircle, 400, 300, 300);
	printf_s("%12llu\n", meshCircle.size());

	scarlet::performance::timer timing;
	for (std::size_t i = 0; i < testDrawLineCount; ++i) {
		auto dx = i % 800;
		Drawer::DrawLine3(mbitmap, dx, 599, 799, 300, MemoryBitmap::kRedColor);
	}
	auto line3 = timing.timing();*/

	Drawer::DrawLine3(mbitmap, 400, 300, 637, 300, MemoryBitmap::kRedColor);
	Drawer::DrawLine3(mbitmap, 400, 300, 400, 537, MemoryBitmap::kRedColor);
	Drawer::DrawLine3(mbitmap, 400, 300, 163, 300, MemoryBitmap::kRedColor);
	Drawer::DrawLine3(mbitmap, 400, 300, 400, 63, MemoryBitmap::kRedColor);

	Drawer::DrawArc3(mbitmap, 400, 300, 237, 4.0, 1.9 ,MemoryBitmap::kBlackColor);

	/*for (std::size_t i = 0; i < testDrawCircleCount; ++i) {
		auto dx = i % 700 + 30;
		Drawer::DrawCircle1(mbitmap, dx, 299, 30, MemoryBitmap::kBlackColor);
	}
	auto draw1 = timing.timing();

	printf_s("line3 %12llu used %llu ms\n", testDrawLineCount, line3.milliseconds());
	printf_s("draw1 %12llu used %llu ms\n", testDrawCircleCount, draw1.milliseconds());*/

	scarlet::io::ioutil::writeFile(L"genBmp01.bmp", (char*)mbitmap.Data(), mbitmap.Size());

	return 0;
}

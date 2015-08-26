#pragma once
#include <cinttypes>
#include <math.h>
#include <algorithm>
#include "table\sin.h"
#include "table\cos.h"
#include "table\tan.h"
#include "table\asin.h"
#include "table\acos.h"
#include "table\atan.h"

class Drawer {
public:
	template<typename __RenderType>
	static void DrawLine3(__RenderType & target, const std::uint32_t & x1, const std::uint32_t & y1, const std::uint32_t & x2, const std::uint32_t & y2, const std::uint32_t & color) {
		std::int32_t dx = x2 - x1;
		std::int32_t dy = y2 - y1;
		std::int32_t ux = ((dx > 0) << 1) - 1;
		std::int32_t uy = ((dy > 0) << 1) - 1;
		std::int32_t x = x1, y = y1, eps = 0;

		dx = std::abs(static_cast<std::int32_t>(x2 - x1));
		dy = std::abs(static_cast<std::int32_t>(y2 - y1));

		if (dx > dy) {
			for (x = x1; x != x2; x += ux) {
				target.SetPixel(x, y, color);
				eps += dy;
				if ((eps << 1) >= dx) {
					y += uy; eps -= dx;
				}
			}
		} else {
			for (y = y1; y != y2; y += uy) {
				target.SetPixel(x, y, color);
				eps += dx;
				if ((eps << 1) >= dy) {
					x += ux; eps -= dy;
				}
			}
		}
	}

	template<typename __RenderType>
	static void DrawCircle1(__RenderType & target, const std::uint32_t & x, const std::uint32_t & y, const std::uint32_t & r, const std::uint32_t & color) {
		std::int32_t tx = 0, ty = r, d = 1 - r;

		while (tx <= ty) {
			target.SetPixel(x + tx, y + ty, color);
			target.SetPixel(x + tx, y - ty, color);
			target.SetPixel(x - tx, y + ty, color);
			target.SetPixel(x - tx, y - ty, color);
			target.SetPixel(x + ty, y + tx, color);
			target.SetPixel(x + ty, y - tx, color);
			target.SetPixel(x - ty, y + tx, color);
			target.SetPixel(x - ty, y - tx, color);
			if (d < 0) {
				d += 2 * tx + 3;
			} else {
				d += 2 * (tx - ty) + 5;
				ty--;
			}
			tx++;
		}
	}

	static void FillCircle1(std::vector<std::uint32_t> & target, const std::uint32_t & x, const std::uint32_t & y, const std::uint32_t & r) {
		target.clear();
		std::int32_t tx = 0, ty = r, d = 1 - r;
		while (tx <= ty) {
			target.push_back(x + tx);
			target.push_back(y + ty);
			target.push_back(x + tx);
			target.push_back(y - ty);
			target.push_back(x - tx);
			target.push_back(y + ty);
			target.push_back(x - tx);
			target.push_back(y - ty);
			target.push_back(x + ty);
			target.push_back(y + tx);
			target.push_back(x + ty);
			target.push_back(y - tx);
			target.push_back(x - ty);
			target.push_back(y + tx);
			target.push_back(x - ty);
			target.push_back(y - tx);
			if (d < 0) {
				d += 2 * tx + 3;
			} else {
				d += 2 * (tx - ty) + 5;
				ty--;
			}
			tx++;
		}
	}

	static void FillCircle2(std::vector<std::vector<std::pair<std::uint32_t, std::uint32_t>>> & target, const std::uint32_t & x, const std::uint32_t & y, const std::uint32_t & r) {
		for (auto & k: target) {
			k.clear();
		}
		
		std::int32_t tx = 0, ty = r, d = 1 - r;

		while (tx <= ty) {
			//target.SetPixel(x + tx, y + ty, color);		//2
			//target.SetPixel(x + tx, y - ty, color);		//7
			//target.SetPixel(x - tx, y + ty, color);		//3
			//target.SetPixel(x - tx, y - ty, color);		//6
			//target.SetPixel(x + ty, y + tx, color);		//1
			//target.SetPixel(x + ty, y - tx, color);		//8
			//target.SetPixel(x - ty, y + tx, color);		//4
			//target.SetPixel(x - ty, y - tx, color);		//5

			target[1].push_back({ x + tx, y + ty });
			target[6].push_back({ x + tx, y - ty });
			target[2].push_back({ x - tx, y + ty });
			target[5].push_back({ x - tx, y - ty });
			target[0].push_back({ x + ty, y + tx });
			target[7].push_back({ x + ty, y - tx });
			target[3].push_back({ x - ty, y + tx });
			target[4].push_back({ x - ty, y - tx });

			if (d < 0) {
				d += 2 * tx + 3;
			} else {
				d += 2 * (tx - ty) + 5;
				ty--;
			}
			tx++;
		}

		target[8].insert(target[8].end(), target[0].begin(), target[0].end());
		target[8].insert(target[8].end(), target[1].rbegin(), target[1].rend());
		target[8].insert(target[8].end(), target[2].begin(), target[2].end());
		target[8].insert(target[8].end(), target[3].rbegin(), target[3].rend());
		target[8].insert(target[8].end(), target[4].begin(), target[4].end());
		target[8].insert(target[8].end(), target[5].rbegin(), target[5].rend());
		target[8].insert(target[8].end(), target[6].begin(), target[6].end());
		target[8].insert(target[8].end(), target[7].rbegin(), target[7].rend());
	}

	template<typename __RenderType>
	static void DrawArc1(__RenderType & target, const std::uint32_t & x, const std::uint32_t & y, const std::uint32_t & r, float startAngle,  float endAngle, const std::uint32_t & color) {
		static float interval = 0.005;
		if (startAngle > endAngle) {
			std::swap(startAngle, endAngle);
		}
		for (float k = startAngle; endAngle - k >= interval; k+= interval) {
			target.SetPixel(x + cos(k)*r, y + sin(k)*r, color);
		}
	}

	template<typename __RenderType>
	static void DrawArc2(__RenderType & target, const std::uint32_t & x, const std::uint32_t & y, const std::uint32_t & r, float startAngle, float endAngle, const std::uint32_t & color) {
		std::vector<std::vector<std::pair<std::uint32_t, std::uint32_t>>> vec;
		vec.resize(9);
		FillCircle2(vec, x, y, r);
		std::size_t startI = std::uint32_t(vec[8].size() * startAngle / (2 * 3.14159265358979323846));
		std::size_t endI = std::uint32_t(vec[8].size() *endAngle / (2 * 3.14159265358979323846));
		if (startI > endI) {
			std::swap(startI, endI);
		}
		for (std::size_t i = startI; i < endI; ++i) {
			target.SetPixel(vec[8][i].first, vec[8][i].second, color);
		}
	}

	template<typename __RenderType>
	static void DrawArc3(__RenderType & target, int x, int y, int r, float alp1, float alp2, std::uint32_t rgb) {
		if (alp1 > alp2) {
			DrawArc3(target, x, y, r, alp1, 3.14159265358979323846 * 2, rgb);
			DrawArc3(target, x, y, r, 0, alp2, rgb);
		} else {
			int k = 0;
			float dlt1 = alp2 - alp1;
			int m = (int)(6.30f / dlt1 + 1);

			if (r < 10) k = 16 / m + 1;
			else if (r < 20) k = 32 / m + 1;
			else if (r < 40) k = 64 / m + 1;
			else k = 128 / m + 1;

			float dlt = dlt1 / k;
			float alp = alp1;
			int x1, y1, x2, y2;
			for (int i = 0; i < k; i++) {
				int dltx = (int)(r * _cosTable[((int)(alp * 10000)) % 63000]);
				int dlty = (int)(r * _sinTable[((int)(alp * 10000)) % 63000]);
				x1 = x + dltx;
				y1 = y + dlty;
				alp += dlt;
				dltx = (int)(r * _cosTable[((int)(alp * 10000)) % 63000]);
				dlty = (int)(r * _sinTable[((int)(alp * 10000)) % 63000]);
				x2 = x + dltx;
				y2 = y + dlty;
				DrawLine3(target, x1, y1, x2, y2, rgb);
			}
		}	
	}
};


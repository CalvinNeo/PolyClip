#pragma once
#include <wingdi.h>
#include <cinttypes>
#include <vector>

class MemoryBitmap {
private:

#pragma  pack(push,1)
	struct __Bitmap_Info {
		BITMAPFILEHEADER fileHeader;
		BITMAPINFOHEADER infoHeader;
	};
#pragma  pack(pop,1)

public:
	MemoryBitmap(const std::uint32_t & width, const std::uint32_t & height) {
		FillBitmapInfo(bi_, width, height);
		InitlizeBitmapBuffer();
	}

	~MemoryBitmap() {}

	/*
	__forceinline void SetPixel(const std::uint32_t & x, const std::uint32_t & y, const std::uint32_t & color) {
		std::size_t i = PointToIndex(x, y) * (bitCount / 8);
		*reinterpret_cast<std::uint32_t*>(&bmpData_[sizeof(__Bitmap_Info) + i]) = color;
	}
	*/

	__forceinline void SetPixel(const std::uint32_t & x, const std::uint32_t & y, const std::uint32_t & color) {
		std::size_t i = PointToIndex(x, y) * (bitCount / 8);
		bmpData_[sizeof(__Bitmap_Info) + i] = color & 0xFF;
		bmpData_[sizeof(__Bitmap_Info) + i + 1] = (color >> 8) & 0xFF;
		bmpData_[sizeof(__Bitmap_Info) + i + 2] = (color >> 16) & 0xFF;
	}

	std::uint32_t Width() const {
		return bi_.infoHeader.biWidth;
	}

	std::uint32_t Height() const {
		return bi_.infoHeader.biHeight;
	}

	std::uint32_t Size() const {
		return static_cast<std::uint32_t>(bmpData_.size());
	}

	std::uint32_t PixelDataSize() const {
		return bi_.infoHeader.biSizeImage;
	}

	BITMAPINFOHEADER BitmapInfoHeader() const {
		return bi_.infoHeader;
	}

	unsigned char * Data() {
		return &bmpData_[0];
	}

	void ClearPixelData() {
		std::fill(bmpData_.begin() + sizeof(bi_), bmpData_.end(), 0xFF);
	}

	static __forceinline std::uint32_t Rgba(const std::uint8_t & r, const std::uint8_t & g, const std::uint8_t & b, const std::uint8_t & a = 0x00) {
		return ((std::uint32_t)(((std::uint8_t)(b) | ((std::uint16_t)((std::uint8_t)(g)) << 8)) | (((std::uint32_t)(std::uint8_t)(r)) << 16) | (((std::uint32_t)(std::uint8_t)(a)) << 24)));
	}

	static const std::uint32_t kRedColor = 0x00FF0000;
	static const std::uint32_t kGreenColor = 0x0000FF00;
	static const std::uint32_t kBlueColor = 0x000000FF;
	static const std::uint32_t kBlackColor = 0x00000000;
	static const std::uint32_t kWhiteColor = 0x00FFFFFF;
private:
	__forceinline std::size_t PointToIndex(const std::uint32_t & x, const std::uint32_t & y) {
		auto ry = (bi_.infoHeader.biHeight - y - 1);
		return ry*bi_.infoHeader.biWidth + x;
	}

	void FillBitmapInfo(__Bitmap_Info & bi, const std::uint32_t & width, const std::uint32_t & height) {
		bi.fileHeader.bfType = 0x4D42;
		bi.fileHeader.bfSize = sizeof(__Bitmap_Info) + width*height * (bitCount / 8);
		bi.fileHeader.bfReserved1 = 0;
		bi.fileHeader.bfReserved2 = 0;
		bi.fileHeader.bfOffBits = 54;

		bi.infoHeader.biSize = 40;
		bi.infoHeader.biWidth = width;
		bi.infoHeader.biHeight = height;
		bi.infoHeader.biPlanes = 1;
		bi.infoHeader.biBitCount = bitCount;
		bi.infoHeader.biCompression = BI_RGB;
		bi.infoHeader.biSizeImage = 0;
		bi.infoHeader.biXPelsPerMeter = 0;
		bi.infoHeader.biYPelsPerMeter = 0;
		bi.infoHeader.biClrUsed = 0;
		bi.infoHeader.biClrImportant = 0;
	}

	void InitlizeBitmapBuffer() {
		bmpData_.resize(bi_.fileHeader.bfSize);
		std::fill(bmpData_.begin(), bmpData_.end(), 0xFF);
		memcpy_s(&bmpData_[0], bmpData_.size(), reinterpret_cast<const char*>(&bi_), sizeof(bi_));
	}

	constexpr static std::uint32_t bitCount = 24;
private:
	__Bitmap_Info bi_;
	std::vector<unsigned char> bmpData_;
};


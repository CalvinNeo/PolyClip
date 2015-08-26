#pragma once
#include "Types.h"
#include <windows.h>

namespace parser {
	class Mapping {
	public:

		Mapping() {
		}

		virtual ~Mapping() {
			UnmappingFile();
		}

		operator bool() const {
			return _initlized;
		}

		const void * Buffer() const {
			return _pMappingBuffer;
		}

		std::uint64_t Size() const {
			return _fileSize;
		}

		error MappingFile(const string & filename) {
			if (*this){
				UnmappingFile();
			}
			auto err = FileExist(filename);
			if (!err.second) {
				return err;
			}
			err = mappingFile(filename);
			if (!err.second) {
				return err;
			}
			return{ _T("成功"), true };
		}

		error UnmappingFile() {
			BOOL b1 = TRUE, b2 = TRUE, b3 = TRUE;
			_fileSize = 0ULL;
			if (_pMappingBuffer != nullptr) {
				b1 = UnmapViewOfFile(_pMappingBuffer);
				_pMappingBuffer = nullptr;
			}
			if (_hMapping != nullptr&&_hMapping != INVALID_HANDLE_VALUE) {
				b2 = CloseHandle(_hMapping);
				_hMapping = nullptr;
			}
			if (_hFile != nullptr&&_hFile != INVALID_HANDLE_VALUE) {
				b3 = CloseHandle(_hFile);
				_hFile = nullptr;
			}
			if (!b1 || !b2 || !b3) {
				return{ _T("卸载失败"), false };
			}
			_initlized = false;
			return{ _T("成功"), true };
		}
	private:
		error mappingFile(const string & filename) {
			_hFile = CreateFile(filename.c_str(), GENERIC_READ, 0, 0, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
			if (_hFile == INVALID_HANDLE_VALUE) {
				return{ _T("打开文件失败"), false };
			}
			LARGE_INTEGER file_size = { 0 };
			if (!GetFileSizeEx(_hFile, &file_size)) {
				return{ _T("获取文件大小失败"), false };
			}
			_fileSize = file_size.QuadPart;
			_hMapping = CreateFileMapping(_hFile, 0, PAGE_READONLY, 0, 0, nullptr);
			if (_hMapping == INVALID_HANDLE_VALUE) {
				return{ _T("创建内存映射失败"), false };
			}
			_pMappingBuffer = MapViewOfFile(_hMapping, FILE_MAP_READ, 0, 0, 0);
			if (_pMappingBuffer == nullptr) {
				return{ _T("映射文件失败"), false };
			}
			_initlized = true;
			return{ _T("成功"), true };
		}

		error FileExist(const string & filename) {
			WIN32_FIND_DATA wfd = { 0 };
			HANDLE hFind = FindFirstFile(filename.c_str(), &wfd);
			if (hFind == INVALID_HANDLE_VALUE) {
				if (GetLastError() == ERROR_FILE_NOT_FOUND) {
					return{ _T("文件不存在"), false };
				} else {
					return{ _T("未知错误"), false };
				}
			}
			if (!FindClose(hFind)) {
				return{ _T("文件句柄关闭失败"), true };
			}
			return{ _T("成功"), true };
		}

	private:
		bool _initlized = false;
		HANDLE _hFile = nullptr, _hMapping = nullptr;
		void * _pMappingBuffer = nullptr;
		std::uint64_t _fileSize;
	};

}
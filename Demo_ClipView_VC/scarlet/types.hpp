#pragma once
#include <cstddef>
#include <cinttypes>
#include <string>
#include <utility>

namespace scarlet {
	class error final{
	public:
		error() = default;

		error(const char * what, const bool b = false) :
			_b(b) {
			if (!what) {
				return;
			}
			std::size_t len = strlen(what);
			std::size_t bufLen = 0;
			if (len <= _smallBufLen - 1) {
				_largeWhat = _what;
				bufLen = _smallBufLen;
			} else {
				_largeWhat = new char[len + 1];
				bufLen = len + 1;
			}
			strcpy_s(_largeWhat, bufLen, what);
		}

		~error() {
			if (_largeWhat != _what) {
				delete[] _largeWhat;
			}
		}

		const std::string what() const {
			if (_largeWhat) {
				return std::string(_largeWhat);
			}
			return std::string("null");
		}

		const bool isok() const {
			return _b;
		}

		operator bool() const {
			return _b;
		}
	private:
		static const std::size_t _smallBufLen = 256;

		char _what[_smallBufLen];
		char * _largeWhat = nullptr;
		bool _b = true;
	};
}
#pragma once
#include <cinttypes>

namespace scarlet {
	namespace strconv {
		template<typename _Char, typename _Value>
		bool parse(_Char, _Value);

		template<typename _Char>
		bool parse(_Char * p, bool & value) {
			if ((p[0] == 't' || p[0] == 'T') &&
				(p[1] == 'r' || p[1] == 'R') &&
				(p[2] == 'u' || p[2] == 'U') &&
				(p[3] == 'e' || p[3] == 'E')) {
				value = true;
			} else if ((p[0] == 'f' || p[0] == 'F') &&
				(p[1] == 'a' || p[1] == 'A') &&
				(p[2] == 'l' || p[2] == 'L') &&
				(p[3] == 's' || p[3] == 'S') &&
				(p[4] == 'e' || p[4] == 'E')) {
				value = false;
			} else {
				return false;
			}
			return true;
		}

		template<typename _Char>
		bool parse(_Char * p, std::uint64_t & value) {
			std::uint64_t val = 0;
			while (*p) {
				if (*p >= '0' && *p <= '9') {
					val = (val * 10) + (*p - '0');
					++p;
				} else {
					return false;
				}		
			}
			value = val;
			return true;
		}

		template<typename _Char>
		bool parse(_Char * p, std::int64_t & value) {
			std::int64_t val = 0;
			bool neg = false;
			if (*p == '-') {
				neg = true;
				++p;
			}
			while (*p) {
				if (*p >= '0' && *p <= '9') {
					val = (val * 10) + (*p - '0');
					++p;
				} else {
					return false;
				}
			}
			if (neg) {
				val = -val;
			}
			value = val;
			return true;
		}

		template<typename _Char>
		bool parse(_Char * p, std::uint32_t & value) {
			std::uint64_t val = 0;
			auto b = parse(p, val);
			value = static_cast<std::uint32_t>(val);
			return b;
		}

		template<typename _Char>
		bool parse(_Char * p, std::int32_t & value) {
			std::int64_t val = 0;
			auto b = parse(p, val);
			value = static_cast<std::int32_t>(val);
			return b;
		}

		template<typename _Char>
		bool parse(_Char * p, std::uint16_t & value) {
			std::uint64_t val = 0;
			auto b = parse(p, val);
			value = static_cast<std::uint16_t>(val);
			return b;
		}

		template<typename _Char>
		bool parse(_Char * p, std::int16_t & value) {
			std::int64_t val = 0;
			auto b = parse(p, val);
			value = static_cast<std::int16_t>(val);
			return b;
		}

		template<typename _Char>
		bool parse(_Char * p, std::uint8_t & value) {
			std::uint64_t val = 0;
			auto b = parse(p, val);
			value = static_cast<std::uint8_t>(val);
			return b;
		}

		template<typename _Char>
		bool parse(_Char * p, std::int8_t & value) {
			std::int64_t val = 0;
			auto b = parse(p, val);
			value = static_cast<std::int8_t>(val);
			return b;
		}

		template<typename _Char, typename _Value>
		bool parse(std::basic_string<_Char, std::char_traits<_Char>, std::allocator<_Char>> p, _Value & value) {
			return parse(p.c_str(), value);
		}
	}
}
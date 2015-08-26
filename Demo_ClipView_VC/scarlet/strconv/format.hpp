#pragma once
#include "../private/format.hpp"
#include <cinttypes>
#include <string>

namespace scarlet {
	namespace strconv {
		template<typename _Char>
		inline void format(bool value, _Char buffer);

		template<typename _Char>
		inline void format(bool value, _Char * buffer) {
			if (value){
				*buffer++ = 't';
				*buffer++ = 'r';
				*buffer++ = 'u';
				*buffer++ = 'e';
				*buffer++ = '\0';
			} else {
				*buffer++ = 'f';
				*buffer++ = 'a';
				*buffer++ = 'l';
				*buffer++ = 's';
				*buffer++ = 'e';
				*buffer++ = '\0';
			}
		}

		template<typename _Char>
		inline void format(bool value, std::basic_string<_Char, std::char_traits<_Char>, std::allocator<_Char> > & buffer) {
			buffer.reserve(6);
			_Char b[6] = { 0 };
			format(value, b);
			buffer.append(b);
		}

		//double处理位数上限为17
		template<typename _Char>
		inline void format(double value, _Char buffer);

		template<typename _Char>
		inline void format(double value, _Char * buffer) {
			if (value == 0) {
				buffer[0] = '0';
				buffer[1] = '.';
				buffer[2] = '0';
				buffer[3] = '\0';
			} else {
				if (value < 0) {
					*buffer++ = '-';
					value = -value;
				}
				int length = 0, K = 0;
				_private::grisu2(value, buffer, &length, &K);
				_private::prettify(buffer, length, K);
			}
		}

		template<typename _Char>
		inline void format(double value, std::basic_string<_Char, std::char_traits<_Char>, std::allocator<_Char> > & buffer) {
			_Char b[312] = { 0 };
			format(value, b);
			buffer.append(b);
		}

		template<typename _Char>
		inline void format(const std::uint8_t value, _Char buffer);

		template<typename _Char>
		inline void format(const std::uint8_t value, _Char * buffer) {
			static const _Char digitsLut[256][4] = {
				{ 48 }, { 49 }, { 50 }, { 51 }, { 52 }, { 53 }, { 54 }, { 55 }, { 56 }, { 57 }, { 49, 48 }, { 49, 49 }, { 49, 50 }, { 49, 51 }, { 49, 52 }, { 49, 53 }, { 49, 54 },
				{ 49, 55 }, { 49, 56 }, { 49, 57 }, { 50, 48 }, { 50, 49 }, { 50, 50 }, { 50, 51 }, { 50, 52 }, { 50, 53 }, { 50, 54 }, { 50, 55 }, { 50, 56 }, { 50, 57 }, { 51, 48 }, { 51, 49 }, { 51, 50 },
				{ 51, 51 }, { 51, 52 }, { 51, 53 }, { 51, 54 }, { 51, 55 }, { 51, 56 }, { 51, 57 }, { 52, 48 }, { 52, 49 }, { 52, 50 }, { 52, 51 }, { 52, 52 }, { 52, 53 }, { 52, 54 }, { 52, 55 }, { 52, 56 },
				{ 52, 57 }, { 53, 48 }, { 53, 49 }, { 53, 50 }, { 53, 51 }, { 53, 52 }, { 53, 53 }, { 53, 54 }, { 53, 55 }, { 53, 56 }, { 53, 57 }, { 54, 48 }, { 54, 49 }, { 54, 50 }, { 54, 51 }, { 54, 52 },
				{ 54, 53 }, { 54, 54 }, { 54, 55 }, { 54, 56 }, { 54, 57 }, { 55, 48 }, { 55, 49 }, { 55, 50 }, { 55, 51 }, { 55, 52 }, { 55, 53 }, { 55, 54 }, { 55, 55 }, { 55, 56 }, { 55, 57 }, { 56, 48 },
				{ 56, 49 }, { 56, 50 }, { 56, 51 }, { 56, 52 }, { 56, 53 }, { 56, 54 }, { 56, 55 }, { 56, 56 }, { 56, 57 }, { 57, 48 }, { 57, 49 }, { 57, 50 }, { 57, 51 }, { 57, 52 }, { 57, 53 }, { 57, 54 },
				{ 57, 55 }, { 57, 56 }, { 57, 57 }, { 49, 48, 48 }, { 49, 48, 49 }, { 49, 48, 50 }, { 49, 48, 51 }, { 49, 48, 52 }, { 49, 48, 53 }, { 49, 48, 54 }, { 49, 48, 55 }, { 49, 48, 56 }, { 49, 48, 57 }, { 49, 49, 48 }, { 49, 49, 49 }, { 49, 49, 50 },
				{ 49, 49, 51 }, { 49, 49, 52 }, { 49, 49, 53 }, { 49, 49, 54 }, { 49, 49, 55 }, { 49, 49, 56 }, { 49, 49, 57 }, { 49, 50, 48 }, { 49, 50, 49 }, { 49, 50, 50 }, { 49, 50, 51 }, { 49, 50, 52 }, { 49, 50, 53 }, { 49, 50, 54 }, { 49, 50, 55 }, { 49, 50, 56 },
				{ 49, 50, 57 }, { 49, 51, 48 }, { 49, 51, 49 }, { 49, 51, 50 }, { 49, 51, 51 }, { 49, 51, 52 }, { 49, 51, 53 }, { 49, 51, 54 }, { 49, 51, 55 }, { 49, 51, 56 }, { 49, 51, 57 }, { 49, 52, 48 }, { 49, 52, 49 }, { 49, 52, 50 }, { 49, 52, 51 }, { 49, 52, 52 },
				{ 49, 52, 53 }, { 49, 52, 54 }, { 49, 52, 55 }, { 49, 52, 56 }, { 49, 52, 57 }, { 49, 53, 48 }, { 49, 53, 49 }, { 49, 53, 50 }, { 49, 53, 51 }, { 49, 53, 52 }, { 49, 53, 53 }, { 49, 53, 54 }, { 49, 53, 55 }, { 49, 53, 56 }, { 49, 53, 57 }, { 49, 54, 48 },
				{ 49, 54, 49 }, { 49, 54, 50 }, { 49, 54, 51 }, { 49, 54, 52 }, { 49, 54, 53 }, { 49, 54, 54 }, { 49, 54, 55 }, { 49, 54, 56 }, { 49, 54, 57 }, { 49, 55, 48 }, { 49, 55, 49 }, { 49, 55, 50 }, { 49, 55, 51 }, { 49, 55, 52 }, { 49, 55, 53 }, { 49, 55, 54 },
				{ 49, 55, 55 }, { 49, 55, 56 }, { 49, 55, 57 }, { 49, 56, 48 }, { 49, 56, 49 }, { 49, 56, 50 }, { 49, 56, 51 }, { 49, 56, 52 }, { 49, 56, 53 }, { 49, 56, 54 }, { 49, 56, 55 }, { 49, 56, 56 }, { 49, 56, 57 }, { 49, 57, 48 }, { 49, 57, 49 }, { 49, 57, 50 },
				{ 49, 57, 51 }, { 49, 57, 52 }, { 49, 57, 53 }, { 49, 57, 54 }, { 49, 57, 55 }, { 49, 57, 56 }, { 49, 57, 57 }, { 50, 48, 48 }, { 50, 48, 49 }, { 50, 48, 50 }, { 50, 48, 51 }, { 50, 48, 52 }, { 50, 48, 53 }, { 50, 48, 54 }, { 50, 48, 55 }, { 50, 48, 56 },
				{ 50, 48, 57 }, { 50, 49, 48 }, { 50, 49, 49 }, { 50, 49, 50 }, { 50, 49, 51 }, { 50, 49, 52 }, { 50, 49, 53 }, { 50, 49, 54 }, { 50, 49, 55 }, { 50, 49, 56 }, { 50, 49, 57 }, { 50, 50, 48 }, { 50, 50, 49 }, { 50, 50, 50 }, { 50, 50, 51 }, { 50, 50, 52 },
				{ 50, 50, 53 }, { 50, 50, 54 }, { 50, 50, 55 }, { 50, 50, 56 }, { 50, 50, 57 }, { 50, 51, 48 }, { 50, 51, 49 }, { 50, 51, 50 }, { 50, 51, 51 }, { 50, 51, 52 }, { 50, 51, 53 }, { 50, 51, 54 }, { 50, 51, 55 }, { 50, 51, 56 }, { 50, 51, 57 }, { 50, 52, 48 },
				{ 50, 52, 49 }, { 50, 52, 50 }, { 50, 52, 51 }, { 50, 52, 52 }, { 50, 52, 53 }, { 50, 52, 54 }, { 50, 52, 55 }, { 50, 52, 56 }, { 50, 52, 57 }, { 50, 53, 48 }, { 50, 53, 49 }, { 50, 53, 50 }, { 50, 53, 51 }, { 50, 53, 52 }, { 50, 53, 53 }
			};
			memcpy_s(buffer, 4 * sizeof(_Char), digitsLut[value], 4 * sizeof(_Char));
		}

		template<typename _Char>
		inline void format(const std::int8_t value, _Char buffer);

		template<typename _Char>
		inline void format(const std::int8_t value, _Char * buffer) {
			std::uint8_t u = static_cast<std::uint8_t>(value);
			if (value < 0) {
				*buffer++ = '-';
				u = ~u + 1;
			}
			format(u, buffer);
		}

		template<typename _Char>
		inline void format(const std::uint8_t value, std::basic_string<_Char, std::char_traits<_Char>, std::allocator<_Char> > & buffer) {
			buffer.reserve(5);
			_Char b[4] = { 0 };
			format(value, b);
			buffer.append(b);
		}

		template<typename _Char>
		inline void format(const std::int8_t value, std::basic_string<_Char, std::char_traits<_Char>, std::allocator<_Char> > & buffer) {
			buffer.reserve(5);
			std::uint8_t u = static_cast<std::uint8_t>(value);
			if (value < 0) {
				buffer += '-';
				u = ~u + 1;
			}
			_Char b[5] = { 0 };
			format(u, b);
			buffer.append(b);
		}

		template<typename _Char>
		inline void format(std::uint32_t value, _Char buffer);

		template<typename _Char>
		inline void format(std::uint32_t value, _Char * buffer) {
			static const _Char digitsLut[200] = {
				'0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
				'1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
				'2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
				'3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
				'4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
				'5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
				'6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
				'7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
				'8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
				'9', '0', '9', '1', '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'
			};

			if (value < 10000) {
				const std::uint32_t d1 = (value / 100) << 1;
				const std::uint32_t d2 = (value % 100) << 1;

				if (value >= 1000)
					*buffer++ = digitsLut[d1];
				if (value >= 100)
					*buffer++ = digitsLut[d1 + 1];
				if (value >= 10)
					*buffer++ = digitsLut[d2];
				*buffer++ = digitsLut[d2 + 1];
			} else if (value < 100000000) {
				const std::uint32_t b = value / 10000;
				const std::uint32_t c = value % 10000;

				const std::uint32_t d1 = (b / 100) << 1;
				const std::uint32_t d2 = (b % 100) << 1;

				const std::uint32_t d3 = (c / 100) << 1;
				const std::uint32_t d4 = (c % 100) << 1;

				if (value >= 10000000)
					*buffer++ = digitsLut[d1];
				if (value >= 1000000)
					*buffer++ = digitsLut[d1 + 1];
				if (value >= 100000)
					*buffer++ = digitsLut[d2];
				*buffer++ = digitsLut[d2 + 1];

				*buffer++ = digitsLut[d3];
				*buffer++ = digitsLut[d3 + 1];
				*buffer++ = digitsLut[d4];
				*buffer++ = digitsLut[d4 + 1];
			} else {
				const std::uint32_t a = value / 100000000;
				value %= 100000000;

				if (a >= 10) {
					const unsigned int i = a << 1;
					*buffer++ = digitsLut[i];
					*buffer++ = digitsLut[i + 1];
				} else
					*buffer++ = '0' + static_cast<_Char>(a);

				const std::uint32_t b = value / 10000;
				const std::uint32_t c = value % 10000;

				const std::uint32_t d1 = (b / 100) << 1;
				const std::uint32_t d2 = (b % 100) << 1;

				const std::uint32_t d3 = (c / 100) << 1;
				const std::uint32_t d4 = (c % 100) << 1;

				*buffer++ = digitsLut[d1];
				*buffer++ = digitsLut[d1 + 1];
				*buffer++ = digitsLut[d2];
				*buffer++ = digitsLut[d2 + 1];
				*buffer++ = digitsLut[d3];
				*buffer++ = digitsLut[d3 + 1];
				*buffer++ = digitsLut[d4];
				*buffer++ = digitsLut[d4 + 1];
			}
			*buffer++ = '\0';
		}

		template<typename _Char>
		inline void format(std::int32_t value, _Char buffer);

		template<typename _Char>
		inline void format(std::int32_t value, _Char * buffer) {
			std::uint32_t u = static_cast<std::uint32_t>(value);
			if (value < 0) {
				*buffer++ = '-';
				u = ~u + 1;
			}
			format(u, buffer);
		}

		template<typename _Char>
		inline void format(std::uint32_t value, std::basic_string<_Char, std::char_traits<_Char>, std::allocator<_Char> > & buffer) {
			buffer.reserve(12);
			_Char b[12] = { 0 };
			format(value, b);
			buffer.append(b);
		}

		template<typename _Char>
		inline void format(const std::int32_t value, std::basic_string<_Char, std::char_traits<_Char>, std::allocator<_Char> > & buffer) {
			buffer.reserve(12);
			std::uint32_t u = static_cast<std::uint32_t>(value);
			if (value < 0) {
				buffer += '-';
				u = ~u + 1;
			}
			_Char b[12] = { 0 };
			format(u, b);
			buffer.append(b);
		}

		template<typename _Char>
		inline void format(std::uint64_t value, _Char buffer);

		template<typename _Char>
		inline void format(std::uint64_t value, _Char * buffer) {
			static const _Char digitsLut[200] = {
				'0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
				'1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
				'2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
				'3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
				'4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
				'5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
				'6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
				'7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
				'8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
				'9', '0', '9', '1', '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'
			};

			if (value < 100000000) {
				std::uint32_t v = static_cast<std::uint32_t>(value);
				if (v < 10000) {
					const std::uint32_t d1 = (v / 100) << 1;
					const std::uint32_t d2 = (v % 100) << 1;

					if (v >= 1000)
						*buffer++ = digitsLut[d1];
					if (v >= 100)
						*buffer++ = digitsLut[d1 + 1];
					if (v >= 10)
						*buffer++ = digitsLut[d2];
					*buffer++ = digitsLut[d2 + 1];
				} else {
					const std::uint32_t b = v / 10000;
					const std::uint32_t c = v % 10000;

					const std::uint32_t d1 = (b / 100) << 1;
					const std::uint32_t d2 = (b % 100) << 1;

					const std::uint32_t d3 = (c / 100) << 1;
					const std::uint32_t d4 = (c % 100) << 1;

					if (value >= 10000000)
						*buffer++ = digitsLut[d1];
					if (value >= 1000000)
						*buffer++ = digitsLut[d1 + 1];
					if (value >= 100000)
						*buffer++ = digitsLut[d2];
					*buffer++ = digitsLut[d2 + 1];

					*buffer++ = digitsLut[d3];
					*buffer++ = digitsLut[d3 + 1];
					*buffer++ = digitsLut[d4];
					*buffer++ = digitsLut[d4 + 1];
				}
			} else if (value < 10000000000000000) {
				const std::uint32_t v0 = static_cast<std::uint32_t>(value / 100000000);
				const std::uint32_t v1 = static_cast<std::uint32_t>(value % 100000000);

				const std::uint32_t b0 = v0 / 10000;
				const std::uint32_t c0 = v0 % 10000;

				const std::uint32_t d1 = (b0 / 100) << 1;
				const std::uint32_t d2 = (b0 % 100) << 1;

				const std::uint32_t d3 = (c0 / 100) << 1;
				const std::uint32_t d4 = (c0 % 100) << 1;

				const std::uint32_t b1 = v1 / 10000;
				const std::uint32_t c1 = v1 % 10000;

				const std::uint32_t d5 = (b1 / 100) << 1;
				const std::uint32_t d6 = (b1 % 100) << 1;

				const std::uint32_t d7 = (c1 / 100) << 1;
				const std::uint32_t d8 = (c1 % 100) << 1;

				if (value >= 1000000000000000)
					*buffer++ = digitsLut[d1];
				if (value >= 100000000000000)
					*buffer++ = digitsLut[d1 + 1];
				if (value >= 10000000000000)
					*buffer++ = digitsLut[d2];
				if (value >= 1000000000000)
					*buffer++ = digitsLut[d2 + 1];
				if (value >= 100000000000)
					*buffer++ = digitsLut[d3];
				if (value >= 10000000000)
					*buffer++ = digitsLut[d3 + 1];
				if (value >= 1000000000)
					*buffer++ = digitsLut[d4];
				if (value >= 100000000)
					*buffer++ = digitsLut[d4 + 1];

				*buffer++ = digitsLut[d5];
				*buffer++ = digitsLut[d5 + 1];
				*buffer++ = digitsLut[d6];
				*buffer++ = digitsLut[d6 + 1];
				*buffer++ = digitsLut[d7];
				*buffer++ = digitsLut[d7 + 1];
				*buffer++ = digitsLut[d8];
				*buffer++ = digitsLut[d8 + 1];
			} else {
				const std::uint32_t a = static_cast<std::uint32_t>(value / 10000000000000000); // 1 to 1844
				value %= 10000000000000000;

				if (a < 10)
					*buffer++ = '0' + static_cast<_Char>(a);
				else if (a < 100) {
					const std::uint32_t i = a << 1;
					*buffer++ = digitsLut[i];
					*buffer++ = digitsLut[i + 1];
				} else if (a < 1000) {
					*buffer++ = '0' + static_cast<_Char>(a / 100);

					const std::uint32_t i = (a % 100) << 1;
					*buffer++ = digitsLut[i];
					*buffer++ = digitsLut[i + 1];
				} else {
					const std::uint32_t i = (a / 100) << 1;
					const std::uint32_t j = (a % 100) << 1;
					*buffer++ = digitsLut[i];
					*buffer++ = digitsLut[i + 1];
					*buffer++ = digitsLut[j];
					*buffer++ = digitsLut[j + 1];
				}

				const std::uint32_t v0 = static_cast<std::uint32_t>(value / 100000000);
				const std::uint32_t v1 = static_cast<std::uint32_t>(value % 100000000);

				const std::uint32_t b0 = v0 / 10000;
				const std::uint32_t c0 = v0 % 10000;

				const std::uint32_t d1 = (b0 / 100) << 1;
				const std::uint32_t d2 = (b0 % 100) << 1;

				const std::uint32_t d3 = (c0 / 100) << 1;
				const std::uint32_t d4 = (c0 % 100) << 1;

				const std::uint32_t b1 = v1 / 10000;
				const std::uint32_t c1 = v1 % 10000;

				const std::uint32_t d5 = (b1 / 100) << 1;
				const std::uint32_t d6 = (b1 % 100) << 1;

				const std::uint32_t d7 = (c1 / 100) << 1;
				const std::uint32_t d8 = (c1 % 100) << 1;

				*buffer++ = digitsLut[d1];
				*buffer++ = digitsLut[d1 + 1];
				*buffer++ = digitsLut[d2];
				*buffer++ = digitsLut[d2 + 1];
				*buffer++ = digitsLut[d3];
				*buffer++ = digitsLut[d3 + 1];
				*buffer++ = digitsLut[d4];
				*buffer++ = digitsLut[d4 + 1];
				*buffer++ = digitsLut[d5];
				*buffer++ = digitsLut[d5 + 1];
				*buffer++ = digitsLut[d6];
				*buffer++ = digitsLut[d6 + 1];
				*buffer++ = digitsLut[d7];
				*buffer++ = digitsLut[d7 + 1];
				*buffer++ = digitsLut[d8];
				*buffer++ = digitsLut[d8 + 1];
			}

			*buffer = '\0';
		}

		template<typename _Char>
		inline void format(std::int64_t value, _Char buffer);

		template<typename _Char>
		inline void format(std::int64_t value, _Char * buffer) {
			std::uint64_t u = static_cast<std::uint64_t>(value);
			if (value < 0) {
				*buffer++ = '-';
				u = ~u + 1;
			}
			format(u, buffer);
		}

		template<typename _Char>
		inline void format(std::uint64_t value, std::basic_string<_Char, std::char_traits<_Char>, std::allocator<_Char> > & buffer) {
			buffer.reserve(22);
			_Char b[22] = { 0 };
			format(value, b);
			buffer.append(b);
		}

		template<typename _Char>
		inline void format(std::int64_t value, std::basic_string<_Char, std::char_traits<_Char>, std::allocator<_Char> > & buffer) {
			buffer.reserve(22);
			std::uint64_t u = static_cast<std::uint64_t>(value);
			if (value < 0) {
				buffer += '-';
				u = ~u + 1;
			}
			_Char b[22] = { 0 };
			format(u, b);
			buffer.append(b);
		}
	}
}
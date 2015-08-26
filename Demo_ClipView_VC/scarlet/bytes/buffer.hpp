#pragma once
#ifndef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS 0
#else
#undef _SCL_SECURE_NO_WARNINGS
#define _SCL_SECURE_NO_WARNINGS 0
#endif
#undef min
#undef max
#include "../io/interface.hpp"
#include <cstddef>
#include <cinttypes>
#include <string>
#include <algorithm>

namespace scarlet {
	namespace bytes {
		class buffer :public io::readWriter{
		public:
			buffer() = default;

			buffer(buffer && b) {
				if (this != &b) {
					if (b._buf == b._small_buf) {
						std::copy(b._small_buf, b._small_buf + _small_cap, _small_buf);

						_buf = _small_buf;
						_roff = b._roff;
						_woff = b._woff;
						_len = b._len;
						_cap = b._cap;
					} else {
						_buf = b._buf;
						_roff = b._roff;
						_woff = b._woff;
						_len = b._len;
						_cap = b._cap;
					}					

					b._buf = nullptr;
					b._roff = 0;
					b._woff = 0;
					b._len = 0;
					b._cap = 0;
				}
			}

			buffer(const std::size_t & n) {
				grow(n);
			}

			buffer(const std::uint8_t * buf, const std::size_t & n) {
				write(buf, n);
			}

			buffer(const std::int8_t * buf, const std::size_t & n) {
				write((const std::uint8_t*)buf, n);
			}

			buffer(const std::string & buf) {
				write((const std::uint8_t*)buf.c_str(), buf.size());
			}

			~buffer() {
				destory();
			}

			std::pair<std::size_t, error> write(const std::uint8_t * buf, const std::size_t & n) {
				grow(n);
				std::copy(buf, buf + n, _buf + _woff);
				_len += n;
				_woff += n;
				return std::make_pair(n, error{});
			}

			std::pair<std::size_t, error> write(const std::int8_t * buf, const std::size_t & n) {
				write((const std::uint8_t*)buf, n);
				return std::make_pair(n, error{});
			}

			std::pair<std::size_t, error> write(const char * buf, const std::size_t & n) {
				write((const std::uint8_t*)buf, n);
				return std::make_pair(n, error{});
			}

			std::pair<std::size_t, error> read(std::uint8_t * b, const std::size_t & n) {
				auto nn = std::min(n, _len);
				if (nn > 0) {
					std::copy(_buf + _roff, _buf + _roff + nn, b);
					_roff += nn;
					_len -= nn;
				}
				return std::make_pair(nn, error{});
			}

			std::pair<const std::uint8_t *, const std::size_t> next(const std::size_t & n) {
				auto nn = std::min(n, _len);
				auto b = bytes();
				_roff += nn;
				_len -= nn;
				return std::make_pair(b, nn);
			}

			const std::size_t len() const {
				return _len;
			}

			const std::uint8_t * bytes() const {
				return _buf + _roff;
			}

			void truncate(const std::size_t & n) {
				if (_buf == nullptr){
					return;
				}
				if (n > _len) {
					return;
				}
				std::copy(_buf + _roff, _buf + _roff + n, _buf);
				_roff = 0;
				_woff = std::min(n, _len);
				_len = std::min(n, _len);
			}

			void grow(const std::size_t & n) {
				if (_len == 0 && (_roff != 0 || _woff != 0)) {
					truncate(0);
				}
				if (_len + n > _cap) {
					std::uint8_t * buf = nullptr;
					if (_buf == nullptr && n <= _small_cap) {
						buf = _small_buf;
						_cap = _small_cap;
					} else if (_len + n <= _cap / 2) {
						std::copy(_buf + _roff, _buf + _roff + _len, _buf);
					} else {
						buf = make(_cap * 2 + n);
						std::copy(_buf + _roff, _buf + _roff + _len, buf);
						_cap = _cap * 2 + n;
						destory();
					}
					_roff = 0;
					_woff = _len;
					_buf = buf;
				}
			}
		private:
			void destory() {
				if (_small_buf != _buf) {
					delete[] _buf;
				}
			}

			std::uint8_t * make(const std::size_t & n) {
				return new std::uint8_t[n];
			}
		private:
			std::uint8_t * _buf = nullptr;
			std::size_t _roff = 0;
			std::size_t _woff = 0;
			std::size_t _len = 0;
			std::size_t _cap = 0;

			static const std::size_t _small_cap = 512;
			std::uint8_t _small_buf[_small_cap];
		};
	}
}

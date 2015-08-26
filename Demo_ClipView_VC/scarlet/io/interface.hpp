#pragma once
#include <cstddef>
#include <cinttypes>
#include "../types.hpp"

namespace scarlet {
	namespace io {
		class reader {
		public:
			virtual std::pair<std::size_t, error> read(std::uint8_t * b, const std::size_t & n) = 0;
		};

		class writer {
		public:
			virtual std::pair<std::size_t, error> write(const std::uint8_t * buf, const std::size_t & n) = 0;

			virtual std::pair<std::size_t, error> write(const std::int8_t * buf, const std::size_t & n) {
				return write((const std::uint8_t*)buf, n);
			}

			virtual std::pair<std::size_t, error> write(const char * buf, const std::size_t & n) {
				return write((const std::uint8_t*)buf, n);
			}
		};

		class readWriter :public reader, public writer {};
	}
}
#pragma once
#include <cstddef>
#include <cinttypes>
#include "io/interface.hpp"

namespace scarlet {
	class binary {
	public:
		static std::pair<std::size_t, error> readFloat(io::reader * w, const float & v) {
			return read(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> readDouble(io::reader * w, const double & v) {
			return read(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> readBool(io::reader * w, const bool & v) {
			return read(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> readInt8(io::reader * w, const std::int8_t & v) {
			return read(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> readUint8(io::reader * w, const std::uint8_t & v) {
			return read(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> readInt16(io::reader * w, const std::int16_t & v) {
			return read(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> readUint16(io::reader * w, const std::uint16_t & v) {
			return read(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> readInt32(io::reader * w, const std::int32_t & v) {
			return read(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> readUint32(io::reader * w, const std::uint32_t & v) {
			return read(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> readInt64(io::reader * w, const std::int64_t & v) {
			return read(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> readUint64(io::reader * w, const std::uint64_t & v) {
			return read(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> writeFloat(io::writer * w, const float v) {
			return write(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> writeDouble(io::writer * w, const double v) {
			return write(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> writeBool(io::writer * w, const bool v) {
			return write(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> writeInt8(io::writer * w, const std::int8_t v) {
			return write(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> writeUint8(io::writer * w, const std::uint8_t v) {
			return write(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> writeInt16(io::writer * w, const std::int16_t v) {
			return write(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> writeUint16(io::writer * w, const std::uint16_t v) {
			return write(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> writeInt32(io::writer * w, const std::int32_t v) {
			return write(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> writeUint32(io::writer * w, const std::uint32_t v) {
			return write(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> writeInt64(io::writer * w, const std::int64_t v) {
			return write(w, (std::uint8_t*)&v, sizeof(v));
		}

		static std::pair<std::size_t, error> writeUint64(io::writer * w, const std::uint64_t v) {
			return write(w, (std::uint8_t*)&v, sizeof(v));
		}

	private:
		static std::pair<std::size_t, error> write(io::writer * w, const std::uint8_t * b, const std::size_t & n) {
			return w->write(b, n);
		}

		static std::pair<std::size_t, error> read(io::reader * w, std::uint8_t * b, const std::size_t & n) {
			std::uint8_t buf[8] = { 0 };
			auto err = w->read(buf, n);
			if (!err.second){
				return err;
			}
			if (err.first != n){
				return std::make_pair(err.first, error{ "读取的字节数错误" });
			}

			memcpy_s(b, n, buf, n);
			return std::make_pair(n, error{});
		}
	};
}
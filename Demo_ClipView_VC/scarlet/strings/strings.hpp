#pragma once
#include <string>
#include <vector>

namespace scarlet {
	namespace strings {
		template<typename T, typename E>
		std::vector<T> split(const T & str, const E & delim) {
			std::vector<T> vec;
			T tstr{ str };
			T tdelim{ delim };

			tstr += tdelim;

			auto size = tstr.size();
			for (std::size_t i = 0; i < size; ++i) {
				auto pos = tstr.find(tdelim, i);
				if (pos < size) {
					auto s = tstr.substr(i, pos - i);
					vec.push_back(s);
					i = pos + tdelim.size() - 1;
				}
			}
			return std::move(vec);
		}
	}
}
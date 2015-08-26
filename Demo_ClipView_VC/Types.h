#pragma once
#include <tchar.h>
#include <cinttypes>
#include <tuple>
#include <string>

namespace parser {
	using string = std::basic_string < TCHAR, std::char_traits<TCHAR>, std::allocator<TCHAR> > ;
	using error = std::pair < string, bool >;
}
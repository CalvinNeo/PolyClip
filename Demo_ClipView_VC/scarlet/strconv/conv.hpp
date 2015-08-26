#pragma once
#include <string>
#include <locale.h>

namespace scarlet {
	namespace strconv {
		static std::wstring stringToWstring(const std::string& str) {
			std::size_t convertedSize = 0;
			std::wstring out;
			_locale_t chs = _create_locale(LC_ALL, "");
			out.resize(str.size() + 1);
			if (_mbstowcs_s_l(&convertedSize, &out[0], out.size(), str.c_str(), str.size(), chs) == std::size_t(-1)) {
				return std::wstring{ L"" };
			}
			return std::move(out);
		}

		static std::string wstringToString(const std::wstring& wstr) {
			std::size_t convertedSize = 0;
			std::string out;
			_locale_t chs = _create_locale(LC_ALL, "");
			out.resize(wstr.size() + 1);
			if (_wcstombs_s_l(&convertedSize, &out[0], out.size(), wstr.c_str(), wstr.size(), chs) == std::size_t(-1)) {
				return std::string{ "" };
			}
			return std::move(out);
		}

		class encoder {
		public:
			std::wstring stringToWstring(const std::string& str) {
				std::size_t convertedSize = 0;
				std::wstring out;
				out.resize(str.size() + 1);
				if (_mbstowcs_s_l(&convertedSize, &out[0], out.size(), str.c_str(), str.size(), _chs) == std::size_t(-1)) {
					return std::wstring{ L"" };
				}
				return std::move(out);
			}

			std::string wstringToString(const std::wstring& wstr) {
				std::size_t convertedSize = 0;
				std::string out;
				out.resize(wstr.size() + 1);
				if (_wcstombs_s_l(&convertedSize, &out[0], out.size(), wstr.c_str(), wstr.size(), _chs) == std::size_t(-1)) {
					return std::string{ "" };
				}
				return std::move(out);
			}
		private:
			_locale_t _chs = _create_locale(LC_ALL, "");
		};
		
	};
}
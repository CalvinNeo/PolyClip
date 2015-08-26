#ifndef _SCARLET_STRINGHELP_H
#define _SCARLET_STRINGHELP_H

#include <vector>
#include <memory>
#include <string>
#include <locale>

namespace scarlet{
		class string_help
		{
		public:
			string_help()
			{}
			~string_help()
			{
				if (m_cs_ptr != nullptr)
				{
					delete[] m_cs_ptr;
				}
				if (m_wcs_ptr != nullptr)
				{
					delete[] m_wcs_ptr;
				}
				m_cs_ptr = nullptr;
				m_wcs_ptr = nullptr;
			}

			template<typename _Elem>
			static _Elem trim(const _Elem & _Object, const _Elem & _Char_Set) {
				if (_Char_Set.empty()) {
					return _Object;
				}
				std::vector<_Elem::value_type> char_set(_Char_Set.begin(), _Char_Set.end());
				std::sort(char_set.begin(), char_set.end());
				_Elem trim_object;
				for (const auto & k : _Object) {
					if (!std::binary_search(char_set.begin(), char_set.end(), k)) {
						trim_object += k;
					}
				}
				return trim_object;
			}

			template<typename _Elem>
			static std::shared_ptr<std::vector<_Elem> > split(const _Elem & _Object, const _Elem & _Delimiter)
			{
				auto ret_vector = std::make_shared<std::vector<_Elem> >();

				auto pos_01 = _Object.find(_Delimiter);
				if (pos_01 == _Elem::npos)
				{
					ret_vector->push_back(_Object);
					return std::move(ret_vector);
				}
				ret_vector->push_back(_Object.substr(0, pos_01));
				auto pos_02 = _Object.find(_Delimiter, pos_01 + 1);

				if (pos_02 == _Elem::npos)
				{
					if (pos_01 != _Object.length() - 1)
					{
						ret_vector->push_back(_Object.substr(pos_01 + 1, _Object.length() - pos_01 - 1));
					}
					return std::move(ret_vector);
				}
				while (pos_02 != _Elem::npos)
				{
					if (pos_01 < pos_02 - 1)
					{
						ret_vector->push_back(_Object.substr(pos_01 + 1, pos_02 - pos_01 - 1));
					}
					pos_01 = pos_02;
					pos_02 = _Object.find(_Delimiter, pos_01 + 1);
				}
				if (pos_01 != _Object.length() - 1)
				{
					ret_vector->push_back(_Object.substr(pos_01 + 1, _Object.length() - pos_01 - 1));
				}

				return std::move(ret_vector);
			}

			//不是线程安全，且返回指针在类生命周期内有效，只读
			const char * wcs_to_cs(const wchar_t * _wcs_ptr)
			{
				auto orig = std::locale::global(std::locale(""));

				if (_wcs_ptr == nullptr)
				{
					return nullptr;
				}
				std::size_t wcsLen = wcslen(_wcs_ptr);
				if (wcsLen == 0)
				{
					return nullptr;
				}
				if (wcsLen * 2 >= m_ch_cov_alloc_size)
				{
					delete[] m_cs_ptr;
					m_cs_ptr = new char[(wcsLen + 1) * 2];
					m_ch_cov_alloc_size = (wcsLen + 1) * 2;
				}
				wcstombs_s(&wcsLen, m_cs_ptr, m_ch_cov_alloc_size,_wcs_ptr, ((size_t) - 1));

				std::locale::global(orig);

				return m_cs_ptr;
			}

			//不是线程安全，且返回指针在类生命周期内有效，只读
			const wchar_t * cs_to_wcs(const char * _cs_ptr)
			{
				auto orig = std::locale::global(std::locale(""));

				if (_cs_ptr == nullptr)
				{
					return nullptr;
				}
				std::size_t csLen = strlen(_cs_ptr);
				if (csLen == 0)
				{
					return nullptr;
				}
				if (csLen * 2 >= m_wch_cov_alloc_size)
				{
					delete[] m_wcs_ptr;
					m_wcs_ptr = new wchar_t[(csLen + 1) * 2];
					m_wch_cov_alloc_size = (csLen + 1) * 2;
				}
				mbstowcs_s(&csLen, m_wcs_ptr, m_wch_cov_alloc_size, _cs_ptr,((size_t)-1));

				std::locale::global(orig);

				return m_wcs_ptr;
			}

			std::string ws_to_str(const std::wstring _ws)
			{
				if (_ws.length() == 0)
				{
					return "";
				}
				return std::move(std::string(wcs_to_cs(_ws.c_str())));
			}

			std::wstring str_to_ws(const std::string _str)
			{
				if (_str.length() == 0)
				{
					return L"";
				}
				return std::move(std::wstring(cs_to_wcs(_str.c_str())));
			}
		private:
			char * m_cs_ptr = nullptr;
			wchar_t * m_wcs_ptr = nullptr;
			std::size_t m_ch_cov_alloc_size = 0;
			std::size_t m_wch_cov_alloc_size = 0;
		};
}
#endif

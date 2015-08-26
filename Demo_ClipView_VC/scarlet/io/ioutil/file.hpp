#pragma once
#include <stdio.h>
#include <sys/stat.h>
#include "../../types.hpp"
#include "../../strconv.hpp"

namespace scarlet {
	namespace io {
		namespace ioutil {
			static error writeFile(const std::wstring & filename, const char * buf, const std::size_t bufLen) {
				FILE * fp = nullptr;
				if (_wfopen_s(&fp, filename.c_str(), L"wb") != 0) {
					return error{ "���ļ�ʧ��", false };
				}

				if (fwrite(buf, 1, bufLen, fp) != bufLen) {
					fclose(fp);
					return error{ "д���ļ�ʧ��", false };
				}

				fclose(fp);
				return error{ nullptr, true };
			}

			static error writeFile(const std::string & filename, const char * buf, const std::size_t bufLen) {
				return writeFile(strconv::stringToWstring(filename), buf, bufLen);
			}

			static error readFile(const std::wstring & filename, std::string & out) {
				struct _stat statbuff = { 0 };
				if (_wstat(filename.c_str(), &statbuff) < 0) {
					return error{ "��ȡ�ļ���Сʧ��", false };
				}

				FILE * fp = nullptr;
				if (_wfopen_s(&fp, filename.c_str(), L"rb") != 0) {
					return error{ "���ļ�ʧ��", false };
				}
				out.clear();
				out.reserve(statbuff.st_size);
				char buf[4096] = { 0 };
				std::size_t rlen = 0;

				while (feof(fp) == 0) {
					rlen = fread_s(buf, 4096, 1, 4096, fp);
					out.append(buf, rlen);
					if (ferror(fp) != 0) {
						fclose(fp);
						return error{ "��ȡ�ļ�ʧ��", false };
					}
				}

				fclose(fp);
				return error{ nullptr, true };
			}

			static error readFile(const std::string & filename, std::string & out) {
				return readFile(strconv::stringToWstring(filename), out);
			}
		}
	}
}
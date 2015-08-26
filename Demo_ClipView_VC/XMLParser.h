#pragma once
#include "Types.h"
#include "Mapping.h"
#include <cctype>
#include <functional>
#include <memory>
#include <list>
#include "DataCollection.h"

namespace parser {
	class XMLParser {
	private:
		enum class ParserStatus {
			Ready,
			TestCaseAttr,
			EntityAttr,
			BoundaryAttr,
			TestCaseClose,
			EntityClose,
			BoundaryClose,
			EntityLine,
			EntityCircle,
			Boundary,
			RootClose,
			Failed,
			End
		};
	public:
		XMLParser() {
			initTables();
		}

		error Parse(const char * buffer, DataCollection & dc) {
			//if (!(*mapping)) {
			//	return{ _T("内存映射无效"), false };
			//}

			_xml_buffer = buffer;
			auto err = parse_xml_header();
			if (!err.second) {
				return err;
			}
			err = parse_xml_test_root();
			if (!err.second) {
				return err;
			}

			//保存指针，用来处理解析的数据
			_ptr_dc_tmp = &dc;

			bool end = false;
			ParserStatus machine_status = ParserStatus::Ready;
			while (!end) {
				switch (machine_status) {
				case XMLParser::ParserStatus::Ready:
					machine_status = parse_xml_tag();
					break;
				case XMLParser::ParserStatus::TestCaseAttr:
					machine_status = parse_xml_test_case_attr();
					break;
				case XMLParser::ParserStatus::EntityAttr:
					machine_status = parse_xml_entity_attr();
					break;
				case XMLParser::ParserStatus::BoundaryAttr:
					machine_status = parse_xml_boundary_attr();
					break;
				case XMLParser::ParserStatus::TestCaseClose:
					skip_until_ch('<');
					machine_status = ParserStatus::Ready;
					break;
				case XMLParser::ParserStatus::EntityClose:
					skip_until_ch('<');
					machine_status = ParserStatus::Ready;
					break;
				case XMLParser::ParserStatus::BoundaryClose:
					skip_until_ch('<');
					machine_status = ParserStatus::Ready;
					break;
				case XMLParser::ParserStatus::EntityLine:
					machine_status = parse_xml_entity_line();
					break;
				case XMLParser::ParserStatus::EntityCircle:
					machine_status = parse_xml_entity_circle();
					break;
				case XMLParser::ParserStatus::Boundary:
					machine_status = parse_xml_boundary();
					break;
				case XMLParser::ParserStatus::RootClose:
					machine_status = XMLParser::ParserStatus::End;
					break;
				case XMLParser::ParserStatus::Failed:
				{
					TCHAR errstr[260] = { 0 };
					_stprintf_s(errstr, _T("错误字符偏移：%u"), _cur_pos);
					_ptr_dc_tmp = nullptr;
					return{ errstr, false };
				}
				break;
				case XMLParser::ParserStatus::End:
					end = true;
					break;
				default:
					break;
				}
			}
			_ptr_dc_tmp = nullptr;
			return{ _T("成功"), true };
		}

	private:

		ParserStatus parse_xml_boundary() {
			std::uint16_t sx = 0, sy = 0;
			while (pick_tow_point_vertex(sx, sy)) {
				_ptr_dc_tmp->NewBoundaryPoint({ sx, sy });
#ifdef OUTPUT_DEBUG_STRING
				printf_s("[Boundary Line] %u %u\n", sx, sy);
#endif
			}
			return ParserStatus::Ready;
		}

		ParserStatus parse_xml_entity_circle() {
			std::uint16_t sx = 0, sy = 0, r = 0;
			if (!pick_tow_point_center_point(sx, sy)) {
				return ParserStatus::Failed;
			}
			if (!pick_one_point_radius(r)) {
				return ParserStatus::Failed;
			}
			_ptr_dc_tmp->NewCircle({ sx, sy }, r);
#ifdef OUTPUT_DEBUG_STRING
			printf_s("[Circle] %u %u-%u\n", sx, sy, r);
#endif
			return ParserStatus::Ready;
		}

		ParserStatus parse_xml_entity_line() {
			std::uint16_t sx = 0, sy = 0, ex = 0, ey = 0;
			if (!pick_tow_point_start_point(sx, sy)) {
				return ParserStatus::Failed;
			}
			skip_until_ch('<');
			if (!pick_tow_point_end_point(ex, ey)) {
				return ParserStatus::Failed;
			}
			_ptr_dc_tmp->NewLine({ sx, sy }, { ex, ey });
#ifdef OUTPUT_DEBUG_STRING
			printf_s("[Line] %u %u->%u %u\n", sx, sy, ex, ey);
#endif
			return ParserStatus::Ready;
		}

		ParserStatus parse_xml_boundary_attr() {
			while (true) {
				if (skip_ch('>')) {
					break;
				}
				skip_until_ch('\"');
				if (!skip_ch('\"')) {
					return ParserStatus::Failed;
				}
				skip_until_ch('\"');
				if (!skip_ch('\"')) {
					return ParserStatus::Failed;
				}
				skip_space_ch();
			}
			skip_until_ch('<');
#ifdef OUTPUT_DEBUG_STRING
			printf_s("[Boundary]\n");
#endif
			//处理数据
			_ptr_dc_tmp->NewBoundary();
			return ParserStatus::Boundary;
		}

		ParserStatus parse_xml_entity_attr() {
			auto status = ParserStatus::Failed;
			while (true) {
				if (skip_ch('>')) {
					break;
				}
				skip_until_ch('\"');
				if (!skip_ch('\"')) {
					return ParserStatus::Failed;
				}
				auto len = skip_until_ch('\"');
				if (len == 0) {
					return ParserStatus::Failed;
				}
				if (isline(_xml_buffer + _cur_pos - len, len)) {
					status = ParserStatus::EntityLine;
				} else if (iscircle(_xml_buffer + _cur_pos - len, len)) {
					status = ParserStatus::EntityCircle;
				}

				if (!skip_ch('\"')) {
					return ParserStatus::Failed;
				}
				skip_space_ch();
			}
			skip_until_ch('<');

			return status;
		}

		ParserStatus parse_xml_test_case_attr() {
			std::string id, desc, type, complex;
			while (true) {
				std::string * ref_str = nullptr;
				if (skip_ch('i')) {
					ref_str = &id;
				} else if (skip_ch('d')) {
					ref_str = &desc;
				} else if (skip_ch('t')) {
					ref_str = &type;
				} else if (skip_ch('c')) {
					ref_str = &complex;
				} else if (skip_ch('>')) {
					break;
				}
				skip_until_ch('\"');
				if (!skip_ch('\"')) {
					return ParserStatus::Failed;
				}
				auto len = skip_until_ch('\"');
				if (ref_str) {
					ref_str->assign(_xml_buffer + _cur_pos - len, len);
				}
				if (!skip_ch('\"')) {
					return ParserStatus::Failed;
				}
				skip_space_ch();
			}
#ifdef OUTPUT_DEBUG_STRING
			printf_s("[TestCase] %s %s %s\n", id.c_str(), desc.c_str(), type.c_str());
#endif
			//处理数据
			_ptr_dc_tmp->NewCase(complex);

			skip_until_ch('<');
			return ParserStatus::Ready;
		}

		ParserStatus parse_xml_tag() {
			if (skip_xml_tag_test_case()) {
				return ParserStatus::TestCaseAttr;
			} else if (skip_xml_tag_entity()) {
				return ParserStatus::EntityAttr;
			} else if (skip_xml_tag_boundary()) {
				return ParserStatus::BoundaryAttr;
			} else if (skip_xml_tag_test_case_close()) {
				return ParserStatus::TestCaseClose;
			} else if (skip_xml_tag_entity_close()) {
				return ParserStatus::EntityClose;
			} else if (skip_xml_tag_boundary_close()) {
				return ParserStatus::BoundaryClose;
			} else if (skip_xml_tag_test_root_close()) {
				return ParserStatus::RootClose;
			}
			return ParserStatus::Failed;
		}

		error parse_xml_test_root() {
			if (!skip_xml_tag_test_root()) {
				return{ _T("未发现测试数据"), false };
			}
			skip_until_ch('<');
			return{ _T(""), true };
		}

		error parse_xml_header() {
			skip_bom();
			if (!skip_xml_header()) {
				return{ _T("XML文件头解析失败"), false };
			}
			skip_until_ch('<');
			return{ _T(""), true };
		}

		bool pick_one_point(std::uint16_t & s) {
			skip_not_number_ch();
			auto len = skip_number_ch();
			//strncpy_s(buf1, _xml_buffer + _cur_pos - len, len);
			//s = atoi(buf1, len);
			s = atoi(_xml_buffer + _cur_pos - len, len);
			skip_until_ch('<');
			if (s == 0) {
				return false;
			}
			return true;
		}

		bool pick_one_point_radius(std::uint16_t & s) {
			if (!skip_xml_tag_radius()) {
				return false;
			}
			if (!pick_one_point(s)) {
				return false;
			}
			if (!skip_xml_tag_radius_close()) {
				return false;
			}
			skip_until_ch('<');
			return true;
		}

		bool pick_tow_point(std::uint16_t & s, std::uint16_t & e) {
			skip_not_number_ch();
			auto len = skip_number_ch();
			s = atoi(_xml_buffer + _cur_pos - len, len);
			skip_not_number_ch();
			len = skip_number_ch();
			e = atoi(_xml_buffer + _cur_pos - len, len);
			skip_until_ch('<');
			if (s == 0 || e == 0) {
				return false;
			}
			return true;
		}

		bool pick_tow_point_start_point(std::uint16_t & s, std::uint16_t & e) {
			if (!skip_xml_tag_start_point()) {
				return false;
			}
			if (!pick_tow_point(s, e)) {
				return false;
			}
			if (!skip_xml_tag_start_point_close()) {
				return false;
			}
			skip_until_ch('<');
			return true;
		}

		bool pick_tow_point_end_point(std::uint16_t & s, std::uint16_t & e) {
			if (!skip_xml_tag_end_point()) {
				return false;
			}
			if (!pick_tow_point(s, e)) {
				return false;
			}
			if (!skip_xml_tag_end_point_close()) {
				return false;
			}
			skip_until_ch('<');
			return true;
		}

		bool pick_tow_point_vertex(std::uint16_t & s, std::uint16_t & e) {
			if (!skip_xml_tag_vertex()) {
				return false;
			}
			if (!pick_tow_point(s, e)) {
				return false;
			}
			if (!skip_xml_tag_vertex_close()) {
				return false;
			}
			skip_until_ch('<');
			return true;
		}

		bool pick_tow_point_center_point(std::uint16_t & s, std::uint16_t & e) {
			if (!skip_xml_tag_center_point()) {
				return false;
			}
			if (!pick_tow_point(s, e)) {
				return false;
			}
			if (!skip_xml_tag_center_point_close()) {
				return false;
			}
			skip_until_ch('<');
			return true;
		}
	private:
		bool skip_xml_tag_radius_close() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == '/' && get_ch(_cur_pos + 2) == 'r' &&
				get_ch(_cur_pos + 3) == 'a' && get_ch(_cur_pos + 4) == 'd' &&
				get_ch(_cur_pos + 5) == 'i' && get_ch(_cur_pos + 6) == 'u' &&
				get_ch(_cur_pos + 7) == 's' && get_ch(_cur_pos + 8) == '>') {
				_cur_pos += 9;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_radius() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == 'r' &&
				get_ch(_cur_pos + 2) == 'a' && get_ch(_cur_pos + 3) == 'd' &&
				get_ch(_cur_pos + 4) == 'i' && get_ch(_cur_pos + 5) == 'u' &&
				get_ch(_cur_pos + 6) == 's' && get_ch(_cur_pos + 7) == '>') {
				_cur_pos += 8;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_center_point_close() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == '/' && get_ch(_cur_pos + 2) == 'c' &&
				get_ch(_cur_pos + 3) == 'e' && get_ch(_cur_pos + 4) == 'n' &&
				get_ch(_cur_pos + 5) == 't' && get_ch(_cur_pos + 6) == 'e' &&
				get_ch(_cur_pos + 7) == 'r' && get_ch(_cur_pos + 8) == 'p' &&
				get_ch(_cur_pos + 9) == 'o' && get_ch(_cur_pos + 10) == 'i' &&
				get_ch(_cur_pos + 11) == 'n' && get_ch(_cur_pos + 12) == 't' && get_ch(_cur_pos + 13) == '>') {
				_cur_pos += 14;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_center_point() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == 'c' &&
				get_ch(_cur_pos + 2) == 'e' && get_ch(_cur_pos + 3) == 'n' &&
				get_ch(_cur_pos + 4) == 't' && get_ch(_cur_pos + 5) == 'e' &&
				get_ch(_cur_pos + 6) == 'r' && get_ch(_cur_pos + 7) == 'p' &&
				get_ch(_cur_pos + 8) == 'o' && get_ch(_cur_pos + 9) == 'i' &&
				get_ch(_cur_pos + 10) == 'n' && get_ch(_cur_pos + 11) == 't' && get_ch(_cur_pos + 12) == '>') {
				_cur_pos += 13;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_vertex_close() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == '/' && get_ch(_cur_pos + 2) == 'v' &&
				get_ch(_cur_pos + 3) == 'e' && get_ch(_cur_pos + 4) == 'r' &&
				get_ch(_cur_pos + 5) == 't' && get_ch(_cur_pos + 6) == 'e' &&
				get_ch(_cur_pos + 7) == 'x' && get_ch(_cur_pos + 8) == '>') {
				_cur_pos += 9;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_vertex() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == 'v' &&
				get_ch(_cur_pos + 2) == 'e' && get_ch(_cur_pos + 3) == 'r' &&
				get_ch(_cur_pos + 4) == 't' && get_ch(_cur_pos + 5) == 'e' &&
				get_ch(_cur_pos + 6) == 'x' && get_ch(_cur_pos + 7) == '>') {
				_cur_pos += 8;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_end_point_close() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == '/' && get_ch(_cur_pos + 2) == 'e' &&
				get_ch(_cur_pos + 3) == 'n' && get_ch(_cur_pos + 4) == 'd' &&
				get_ch(_cur_pos + 5) == 'p' && get_ch(_cur_pos + 6) == 'o' &&
				get_ch(_cur_pos + 7) == 'i' && get_ch(_cur_pos + 8) == 'n' &&
				get_ch(_cur_pos + 9) == 't' && get_ch(_cur_pos + 10) == '>') {
				_cur_pos += 11;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_end_point() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == 'e' &&
				get_ch(_cur_pos + 2) == 'n' && get_ch(_cur_pos + 3) == 'd' &&
				get_ch(_cur_pos + 4) == 'p' && get_ch(_cur_pos + 5) == 'o' &&
				get_ch(_cur_pos + 6) == 'i' && get_ch(_cur_pos + 7) == 'n' &&
				get_ch(_cur_pos + 8) == 't' && get_ch(_cur_pos + 9) == '>') {
				_cur_pos += 10;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_start_point_close() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == '/' && get_ch(_cur_pos + 2) == 's' &&
				get_ch(_cur_pos + 3) == 't' && get_ch(_cur_pos + 4) == 'a' &&
				get_ch(_cur_pos + 5) == 'r' && get_ch(_cur_pos + 6) == 't' &&
				get_ch(_cur_pos + 7) == 'p' && get_ch(_cur_pos + 8) == 'o' &&
				get_ch(_cur_pos + 9) == 'i' && get_ch(_cur_pos + 10) == 'n' &&
				get_ch(_cur_pos + 11) == 't'&& get_ch(_cur_pos + 12) == '>') {
				_cur_pos += 13;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_start_point() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == 's' &&
				get_ch(_cur_pos + 2) == 't' && get_ch(_cur_pos + 3) == 'a' &&
				get_ch(_cur_pos + 4) == 'r' && get_ch(_cur_pos + 5) == 't' &&
				get_ch(_cur_pos + 6) == 'p' && get_ch(_cur_pos + 7) == 'o' &&
				get_ch(_cur_pos + 8) == 'i' && get_ch(_cur_pos + 9) == 'n' &&
				get_ch(_cur_pos + 10) == 't'&& get_ch(_cur_pos + 11) == '>') {
				_cur_pos += 12;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_test_root_close() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == '/' && get_ch(_cur_pos + 2) == 't' &&
				get_ch(_cur_pos + 3) == 'e' && get_ch(_cur_pos + 4) == 's' &&
				get_ch(_cur_pos + 5) == 't' && get_ch(_cur_pos + 6) == 'r' &&
				get_ch(_cur_pos + 7) == 'o' && get_ch(_cur_pos + 8) == 'o' &&
				get_ch(_cur_pos + 9) == 't' && get_ch(_cur_pos + 10) == '>') {
				_cur_pos += 11;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_boundary_close() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == '/' && get_ch(_cur_pos + 2) == 'b' &&
				get_ch(_cur_pos + 3) == 'o' && get_ch(_cur_pos + 4) == 'u' &&
				get_ch(_cur_pos + 5) == 'n' && get_ch(_cur_pos + 6) == 'd' &&
				get_ch(_cur_pos + 7) == 'a' && get_ch(_cur_pos + 8) == 'r' &&
				get_ch(_cur_pos + 9) == 'y' && get_ch(_cur_pos + 10) == '>') {
				_cur_pos += 11;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_entity_close() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == '/' && get_ch(_cur_pos + 2) == 'e' &&
				get_ch(_cur_pos + 3) == 'n' && get_ch(_cur_pos + 4) == 't' &&
				get_ch(_cur_pos + 5) == 'i' && get_ch(_cur_pos + 6) == 't' &&
				get_ch(_cur_pos + 7) == 'y' && get_ch(_cur_pos + 8) == '>') {
				_cur_pos += 9;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_test_case_close() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == '/' && get_ch(_cur_pos + 2) == 't' &&
				get_ch(_cur_pos + 3) == 'e' && get_ch(_cur_pos + 4) == 's' &&
				get_ch(_cur_pos + 5) == 't' && get_ch(_cur_pos + 6) == 'c' &&
				get_ch(_cur_pos + 7) == 'a' && get_ch(_cur_pos + 8) == 's' &&
				get_ch(_cur_pos + 9) == 'e' && get_ch(_cur_pos + 10) == '>') {
				_cur_pos += 11;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_boundary() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == 'b' &&
				get_ch(_cur_pos + 2) == 'o' && get_ch(_cur_pos + 3) == 'u' &&
				get_ch(_cur_pos + 4) == 'n' && get_ch(_cur_pos + 5) == 'd' &&
				get_ch(_cur_pos + 6) == 'a' && get_ch(_cur_pos + 7) == 'r' &&
				get_ch(_cur_pos + 8) == 'y' && get_ch(_cur_pos + 9) == ' ') {
				_cur_pos += 10;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_entity() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == 'e' &&
				get_ch(_cur_pos + 2) == 'n' && get_ch(_cur_pos + 3) == 't' &&
				get_ch(_cur_pos + 4) == 'i' && get_ch(_cur_pos + 5) == 't' &&
				get_ch(_cur_pos + 6) == 'y' && get_ch(_cur_pos + 7) == ' ') {
				_cur_pos += 8;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_test_case() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == 't' &&
				get_ch(_cur_pos + 2) == 'e' && get_ch(_cur_pos + 3) == 's' &&
				get_ch(_cur_pos + 4) == 't' && get_ch(_cur_pos + 5) == 'c' &&
				get_ch(_cur_pos + 6) == 'a' && get_ch(_cur_pos + 7) == 's' &&
				get_ch(_cur_pos + 8) == 'e' && get_ch(_cur_pos + 9) == ' ') {
				_cur_pos += 10;
				return true;
			}
			return false;
		}

		bool skip_xml_tag_test_root() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == 't' &&
				get_ch(_cur_pos + 2) == 'e' && get_ch(_cur_pos + 3) == 's' &&
				get_ch(_cur_pos + 4) == 't' && get_ch(_cur_pos + 5) == 'r' &&
				get_ch(_cur_pos + 6) == 'o' && get_ch(_cur_pos + 7) == 'o' &&
				get_ch(_cur_pos + 8) == 't' && get_ch(_cur_pos + 9) == '>') {
				_cur_pos += 10;
				return true;
			}
			return false;
		}

		bool skip_xml_header() {
			if (get_ch(_cur_pos) == '<' && get_ch(_cur_pos + 1) == '?' &&
				get_ch(_cur_pos + 2) == 'x' && get_ch(_cur_pos + 3) == 'm' &&
				get_ch(_cur_pos + 4) == 'l' && get_ch(_cur_pos + 5) == ' ' &&
				get_ch(_cur_pos + 6) == 'v' && get_ch(_cur_pos + 7) == 'e' &&
				get_ch(_cur_pos + 8) == 'r' && get_ch(_cur_pos + 9) == 's' &&
				get_ch(_cur_pos + 10) == 'i' && get_ch(_cur_pos + 11) == 'o' &&
				get_ch(_cur_pos + 12) == 'n' && get_ch(_cur_pos + 13) == '=' &&
				get_ch(_cur_pos + 14) == '\"' && get_ch(_cur_pos + 15) == '1' &&
				get_ch(_cur_pos + 16) == '.' && get_ch(_cur_pos + 17) == '0' &&
				get_ch(_cur_pos + 18) == '\"' && get_ch(_cur_pos + 19) == ' ') {
				_cur_pos += 20;
				return true;
			}
			return false;
		}

	private:
		inline char get_ch(const std::uint32_t & pos) {
			return tolower(_xml_buffer[pos]);
		}

		inline void skip_bom() {
			if ((unsigned char)_xml_buffer[0] == 0xef &&
				(unsigned char)_xml_buffer[1] == 0xbb &&
				(unsigned char)_xml_buffer[2] == 0xbf) {
				_cur_pos = 3;
			}
		}

		inline bool skip_ch(unsigned char ch) {
			if (tolower(_xml_buffer[_cur_pos]) == ch) {
				++_cur_pos;
				return true;
			}
			return false;
		}

		inline std::uint32_t skip_until_ch(unsigned char ch) {
			std::uint32_t tmp_pos = _cur_pos;
			while (tolower(_xml_buffer[_cur_pos]) != ch) {
				++_cur_pos;
			}
			return _cur_pos - tmp_pos;

#ifdef USE_SSE42
			
			if (tolower(_xml_buffer[_cur_pos]) == ch) {
				return 0;
			}
			std::uint32_t tmp_pos = _cur_pos;

			++_cur_pos;

			const char* next_aligned = reinterpret_cast<const char*>(
				(reinterpret_cast<std::size_t>(_xml_buffer + _cur_pos) + 15) & ~15);

			while (&_xml_buffer[_cur_pos] != next_aligned) {
				if (tolower(_xml_buffer[_cur_pos]) == ch) {
					return _cur_pos - tmp_pos;
				}
				++_cur_pos;
			}

			static unsigned char chs[16] = { 0 };
			chs[0] = ch;
			const __m128i w = _mm_load_si128((const __m128i *)&chs[0]);

			for (;; _cur_pos += 16) {
				const __m128i s = _mm_load_si128((const __m128i *)&_xml_buffer[_cur_pos]);
				const unsigned r = _mm_cvtsi128_si32(_mm_cmpistrm(w, s,
					_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY |
					_SIDD_BIT_MASK | _SIDD_NEGATIVE_POLARITY));

				if (r != 65535) {
					unsigned long offset = 0;
					_BitScanForward(&offset, ~r);
					_cur_pos += offset;
					return _cur_pos - tmp_pos;
				}
			}
			return _cur_pos - tmp_pos;
			
#endif
		}

		inline std::uint32_t skip_number_ch() {
			std::uint32_t tmp_pos = _cur_pos;
			while (isdigit(_xml_buffer[_cur_pos])) {
				++_cur_pos;
			}
			return _cur_pos - tmp_pos;
#ifdef USE_SSE42
			
			if (!isdigit(_xml_buffer[_cur_pos])) {
				return 0;
			}
			std::uint32_t tmp_pos = _cur_pos;

			++_cur_pos;

			const char* next_aligned = reinterpret_cast<const char*>(
				(reinterpret_cast<std::size_t>(_xml_buffer + _cur_pos) + 15) & ~15);

			while (&_xml_buffer[_cur_pos] != next_aligned) {
				if (!isdigit(_xml_buffer[_cur_pos])) {
					return _cur_pos - tmp_pos;
				}
				++_cur_pos;
			}

			static const char number[16] = "0123456789";
			const __m128i w = _mm_load_si128((const __m128i *)&number[0]);

			for (;; _cur_pos += 16) {
				const __m128i s = _mm_load_si128((const __m128i *)&_xml_buffer[_cur_pos]);
				const unsigned r = _mm_cvtsi128_si32(_mm_cmpistrm(w, s,
					_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY |
					_SIDD_BIT_MASK | _SIDD_NEGATIVE_POLARITY));

				if (r != 0) {
					unsigned long offset = 0;
					_BitScanForward(&offset, r);
					_cur_pos += offset;
					return _cur_pos - tmp_pos;
				}
			}
			return _cur_pos - tmp_pos;
			
#endif
		}

		inline std::uint32_t skip_not_number_ch() {
			
			std::uint32_t tmp_pos = _cur_pos;
			while (!isdigit(_xml_buffer[_cur_pos])) {
				++_cur_pos;
			}
			return _cur_pos - tmp_pos;
#ifdef USE_SSE42
			
			if (isdigit(_xml_buffer[_cur_pos])) {
				return 0;
			}
			std::uint32_t tmp_pos = _cur_pos;

			++_cur_pos;

			const char* next_aligned = reinterpret_cast<const char*>(
				(reinterpret_cast<std::size_t>(_xml_buffer + _cur_pos) + 15) & ~15);

			while (&_xml_buffer[_cur_pos] != next_aligned) {
				if (isdigit(_xml_buffer[_cur_pos])) {
					return _cur_pos - tmp_pos;
				}
				++_cur_pos;
			}

			static const char number[16] = "0123456789";
			const __m128i w = _mm_load_si128((const __m128i *)&number[0]);

			for (;; _cur_pos += 16) {
				const __m128i s = _mm_load_si128((const __m128i *)&_xml_buffer[_cur_pos]);
				const unsigned r = _mm_cvtsi128_si32(_mm_cmpistrm(w, s,
					_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY |
					_SIDD_BIT_MASK | _SIDD_NEGATIVE_POLARITY));

				if (r != 65535) {
					unsigned long offset = 0;
					_BitScanForward(&offset, ~r);
					_cur_pos += offset;
					return _cur_pos - tmp_pos;
				}
			}
			return _cur_pos - tmp_pos;
			
#endif
		}

		inline std::uint32_t skip_space_ch() {
			if (!isspace(_xml_buffer[_cur_pos])) {
				return 0;
			}
			std::uint32_t tmp_pos = _cur_pos;

			++_cur_pos;
			
			const char* next_aligned = reinterpret_cast<const char*>(
				(reinterpret_cast<std::size_t>(_xml_buffer + _cur_pos) + 15) & ~15);

			while (&_xml_buffer[_cur_pos] != next_aligned) {
				if (!isspace(_xml_buffer[_cur_pos])) {
					return _cur_pos - tmp_pos;
				}
				++_cur_pos;
			}	

			static const char whitespace[16] = " \n\r\t";
			const __m128i w = _mm_load_si128((const __m128i *)&whitespace[0]);

			for (;; _cur_pos += 16) {
				const __m128i s = _mm_load_si128((const __m128i *)&_xml_buffer[_cur_pos]);
				const unsigned r = _mm_cvtsi128_si32(_mm_cmpistrm(w, s,
					_SIDD_UBYTE_OPS | _SIDD_CMP_EQUAL_ANY |
					_SIDD_BIT_MASK | _SIDD_NEGATIVE_POLARITY));

				if (r != 0) {
					unsigned long offset = 0;
					_BitScanForward(&offset, r);
					_cur_pos += offset;
					return _cur_pos - tmp_pos;
				}
			}
			return _cur_pos - tmp_pos;
		}
	private:
		bool isline(const char * str, int len) {
			if (len != 4 ||
				tolower(str[0]) != 'l' || tolower(str[1]) != 'i' ||
				tolower(str[2]) != 'n' || tolower(str[3]) != 'e') {
				return false;
			}
			return true;
		}

		bool iscircle(const char * str, int len) {
			if (len != 6 ||
				tolower(str[0]) != 'c' || tolower(str[1]) != 'i' ||
				tolower(str[2]) != 'r' || tolower(str[3]) != 'c' ||
				tolower(str[4]) != 'l' || tolower(str[5]) != 'e') {
				return false;
			}
			return true;
		}

		char tolower(const char c) const {
			return _cc_tables[c];
		}

		bool isdigit(const char c) const {
			return _digit_tables[c];
		}

		bool isspace(const char c) const {
			return _space_tables[c];
		}

		std::uint16_t atoi(const char * str, int len) {
			if (len > 4 || len == 0) {
				return 0;
			}
			switch (len) {
			case 1:
				return str[0] - '0';
				break;
			case 2:
				return (str[0] - '0') * 10 + (str[1] - '0');
				break;
			case 3:
				return (str[0] - '0') * 100 + (str[1] - '0') * 10 + (str[2] - '0');
				break;
			case 4:
				return (str[0] - '0') * 1000 + (str[1] - '0') * 100 + (str[2] - '0') * 10 + (str[3] - '0');
				break;
			}
			return 0;
		}
	private:
		const char * _xml_buffer = nullptr;
		std::uint32_t _cur_pos = 0;

		DataCollection * _ptr_dc_tmp = nullptr;

	private:
		void initTables() {
			initConvertChTable();
			initDigitTable();
			initSpaceTable();
		}

		void initConvertChTable() {
			for (int i = 0; i <= 256; ++i) {
				if ((char)i >= 'A' && (char)i <= 'Z') {
					_cc_tables[i] = i + 32;
				} else {
					_cc_tables[i] = i;
				}
			}
		}

		void initDigitTable() {
			for (int i = 0; i <= 256; ++i) {
				if ((char)i >= '0' && (char)i <= '9') {
					_digit_tables[i] = true;
				} else {
					_digit_tables[i] = false;
				}
			}
		}

		void initSpaceTable() {
			for (int i = 0; i <= 256; ++i) {
				_space_tables[i] = false;
			}
			_space_tables[' '] = true;
			_space_tables['\r'] = true;
			_space_tables['\n'] = true;
			_space_tables['\t'] = true;
		}
	private:
		char _cc_tables[256];
		bool _digit_tables[256];
		bool _space_tables[256];
	};

}
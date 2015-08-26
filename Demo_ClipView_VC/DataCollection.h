#pragma once
#include <cinttypes>
#include <vector>
#include <list>
#include <thread>
#include <memory>

namespace parser {
	class DataCollection {
	public:
		using Point = std::pair < std::uint16_t, std::uint16_t > ;
		struct Boundary {
			Boundary() {
				p.reserve(8);
			}
			std::vector<Point> p;
			//Point minPt, maxPt;
		};

		struct Circle {
			Point p;
			std::uint16_t r;
		};

		struct Line {
			Point start;
			Point end;
		};

		struct Case {
			Case(const std::string & comp):
				_comp(comp){
				boundarys.reserve(1024 * 64);
				circles.reserve(1024 * 128);
				lines.reserve(1024 * 128);
			}
			Case() :
				_comp("") {
				boundarys.reserve(1024 * 64);
				circles.reserve(1024 * 128);
				lines.reserve(1024 * 128);
			}
			std::string _comp;
			std::size_t _vertex_size = 0;
			std::vector<Boundary> boundarys;
			std::vector<Circle> circles;
			std::vector<Line> lines;
		};
	public:

		DataCollection() {
		}

		virtual ~DataCollection() {
		}

		void NewCase(const std::string & comp) {
			_cases.push_back(Case{ comp });
			_last_case_iter = (--(_cases.end()));
		}

		void NewBoundary() {
			_last_case_iter->boundarys.push_back(Boundary{});
			_last_boundary_iter = (--(_last_case_iter->boundarys.end()));
		}

		void NewBoundaryPoint(const Point & p) {
			++(_last_case_iter->_vertex_size);
			_last_boundary_iter->p.push_back(p);
		}

		void NewLine(const Point & s, const Point & e) {
			_last_case_iter->lines.push_back(Line{ s, e });
		}

		void NewCircle(const Point & p, const std::uint16_t r) {
			_last_case_iter->circles.push_back(Circle{ p, r });
		}

		std::vector<Case>::iterator begin() {
			return _cases.begin();
		}

		std::vector<Case>::iterator end() {
			return _cases.end();
		}

		std::vector<Case>::const_iterator begin() const {
			return _cases.cbegin();
		}

		std::vector<Case>::const_iterator end() const {
			return _cases.cend();
		}

// 		std::size_t vertex_size()const {
// 			return _vertex_size;
// 		}
	private:
		std::vector<Case> _cases;

		std::vector<Case>::iterator _last_case_iter;
		std::vector<Boundary>::iterator _last_boundary_iter;
	};

}
#ifndef _SCARLET_WIN_ACCURATETIME_H
#define _SCARLET_WIN_ACCURATETIME_H

#include <Windows.h>

namespace scarlet {
	namespace win {
		class accurate_timing {
		public:
			//微秒（us）
			static const unsigned long microsecond = 1UL;
			//毫秒（ms）
			static const unsigned long millisecond = 1000UL;
			//秒（s）
			static const unsigned long second = 1000000UL;
		public:
			accurate_timing::accurate_timing() {
				m_inited = init();
			}
			bool accurate_timing::reset_frequency() {
				clear();
				return (m_inited = init());
			}
			bool accurate_timing::start() {
				LARGE_INTEGER startCount;
				if (QueryPerformanceCounter(&startCount) == 0) {
					return false;
				}
				m_startCount = startCount.QuadPart;
				return true;
			}
			double accurate_timing::end() {
				if (m_startCount == 0) {
					return 0;
				}

				LARGE_INTEGER now_count;
				QueryPerformanceCounter(&now_count);
				auto us = (now_count.QuadPart - m_startCount) / m_performanceFrequency;
				reset_frequency();
				return us;
			}
			bool accurate_timing::SetThreadAffinityMask(const unsigned long long _mask, HANDLE _thread) {
				if (_mask == 0) {
					return false;
				}
				bool ret = false;
				_thread == nullptr ? ret = SetThreadAffinityMask(_mask, GetCurrentThread()) : ret = SetThreadAffinityMask(_mask, _thread);
				return ret;
			}
		private:
			bool init() {
				LARGE_INTEGER performanceFrequency;
				if (QueryPerformanceFrequency(&performanceFrequency) == 0)
					return false;
				m_performanceFrequency = (double)(performanceFrequency.QuadPart) / 1000 / 1000;
				return true;
			}
			void clear() {
				m_performanceFrequency = 0.0;
				m_startCount = 0;
			}
		private:
			unsigned long long m_startCount;
			double m_performanceFrequency;		//每 微秒 CPU高性能计数器计数次数
			bool m_inited = false;
		};
	}
}
#endif
#ifndef _SCARLET_WIN_SYNC_H
#define _SCARLET_WIN_SYNC_H
#include <Windows.h>

namespace scarlet{
	namespace win{
		class mutex
		{
		public:
			inline mutex(unsigned long spin = 6000)		{ InitializeCriticalSectionAndSpinCount(&cs, spin); }
			inline ~mutex(void)		{ DeleteCriticalSection(&cs); }
			inline void lock()		{ EnterCriticalSection(&cs); }
			inline void unlock()	{ LeaveCriticalSection(&cs); }
		private:
			CRITICAL_SECTION cs;
		};

		template<typename LOCK>
		class lock_guard
		{
		public:
			inline lock_guard(LOCK & _mutex)
				: m_mutex(_mutex)
			{
				m_mutex.lock();
			}
			inline ~lock_guard(void)
			{
				m_mutex.unlock();
			}
			lock_guard() = delete;
		private:
			LOCK & m_mutex;
		};
	}
}
#endif

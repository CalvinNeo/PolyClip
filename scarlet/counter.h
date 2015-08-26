#ifndef _SCARLET_COUNTER_H
#define _SCARLET_COUNTER_H
#include <scarlet/sync.h>

namespace scarlet{
	template<typename LOCK>
	class counter
	{
	public:
		void add(unsigned long long _inc)
		{
			win::lock_guard<LOCK> guard(m_lock);
			if (MAXULONG64 - _inc > m_count)
			{
				m_count = 0;
			}
			m_count += _inc;
		}

		void dec(unsigned long long _dec)
		{
			win::lock_guard<LOCK> guard(m_lock);
			if (_dec > m_count)
			{
				m_count = _dec;
			}
			m_count -= _dec;
		}

		void done()
		{
			win::lock_guard<LOCK> guard(m_lock);
			--m_count;
		}

		unsigned long long count()
		{
			win::lock_guard<LOCK> guard(m_lock);
			return m_count;
		}

		unsigned long long reset()
		{
			win::lock_guard<LOCK> guard(m_lock);
			auto saved_value = m_count;
			m_count = 0;
			return saved_value;
		}
	private:
		LOCK m_lock;
		unsigned long long m_count = 0;
	};
}
#endif
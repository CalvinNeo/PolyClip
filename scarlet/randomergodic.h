#ifndef _SCARLET_RANDOMERGODIC_H
#define _SCARLET_RANDOMERGODIC_H
#include <ctime>
#include <random>
#include <memory>

namespace scarlet{
		class randomergodic
		{
		public:

			randomergodic(unsigned long long elements_size)
			{
				reset(elements_size);
			}

			~randomergodic()
			{
				if (m_visited != nullptr)
				{
					delete[] m_visited;
				}
				m_visited = nullptr;
			}

			unsigned long long next()
			{
				if (done())
				{
					return m_elements_size + 1;
				}

				auto rn = (*m_uid_engine)(*m_mt_engine);
				while (m_visited[rn])
				{
					rn = (*m_uid_engine)(*m_mt_engine);
				}
				m_visited[rn] = true;
				++m_searches;

				return rn;
			}

			bool done()
			{
				return m_searches == m_elements_size;
			}

			void clear()
			{
				if (m_visited != nullptr)
				{
					delete[] m_visited;
				}
				m_visited = nullptr;
				m_elements_size = 0;
				m_searches = 0;
			}

			void reset(unsigned long long elements_size)
			{
				clear();

				if (elements_size <= 0)
				{
					return;
				}
				m_elements_size = elements_size;
				m_visited = new bool[m_elements_size];
				memset(m_visited, 0, m_elements_size);

				m_mt_engine.reset(new std::mt19937((unsigned int)std::time(nullptr)));
				m_uid_engine.reset(new std::uniform_int_distribution<unsigned long long>(0, m_elements_size - 1));
			}
		private:
			unsigned long long m_elements_size = 0;
			unsigned long long m_searches = 0;
			bool * m_visited = nullptr;

			std::unique_ptr<std::mt19937> m_mt_engine;
			std::unique_ptr<std::uniform_int_distribution<unsigned long long>> m_uid_engine;
		};
}
#endif
#ifndef _SCARLET_PRIME_H
#define _SCARLET_PRIME_H

namespace scarlet{
		class prime
		{
		public:
			inline bool is_prime(unsigned long long p)
			{
				if (((p & 1) != 0) && (p % 3 != 0) && (p > 2) && M_R(2, p) && ((p <= 7) || M_R(7, p)) 
					&& ((p <= 61) || M_R(61, p)) || (p == 2) || (p == 3)) return true;
				return false;
			}

			unsigned long long get_prime_greater(unsigned long long p)
			{
				while (!is_prime(++p))
				{}

				return p;
			}
		private:
			bool M_R(unsigned long long base, unsigned long long num)
			{
				m_d = num - 1;
				while ((m_d & 1) == 0)
				{
					m_d = (m_d >> 1);
				}
				if ((paw_mod(base, m_d, num) == 1) || (paw_mod(base, m_d, num) == num - 1))
					return true;
				else
				{
					m_t = (num - 1) / 2;
					while (m_d != m_t)
					{
						m_d = (m_d << 1);
						if (paw_mod(base, m_d, num) == num - 1)
							return true;
					}
					return false;
				}
			}
			unsigned long long paw_mod(unsigned long long bs, unsigned long long power, unsigned long long diver)
			{
				if (power == 0) return(1);
				else if (power == 1) return(bs);
				else if ((power & 1) == 0) return(paw_mod(bs*bs%diver, (power >> 1), diver));
				else return(paw_mod(bs*bs%diver, power / 2, diver)*bs%diver);
			}

		private:
			unsigned long long m_d = 0, m_t = 0;
		};
}
#endif
#ifndef _SCARLET_LEVENSHTEIN_H
#define _SCARLET_LEVENSHTEIN_H
#include <string>
#include <algorithm>
#undef min
#undef max

namespace scarlet{
		class levenshtein
		{
		public:
			std::string::size_type levenshteinDistance(const std::wstring & str1, const std::wstring & str2)
			{
				auto n = str1.length();
				auto m = str2.length();

				decltype(n) i;
				decltype(n) j;
				decltype(n) temp;
				wchar_t ch1;
				wchar_t ch2;

				if (n == 0)
				{
					return m;
				}
				if (m == 0)
				{
					return n;
				}
				
				decltype(n) ** d;
				d = new decltype(n) *[n + 1];  
				for (int i = 0; i < n + 1; i++)
					d[i] = new decltype(n)[m+1];

				for (i = 0; i <= n; i++)
				{
					d[i][0] = i;
				}
				for (j = 0; j <= m; j++)
				{
					d[0][j] = j;
				}
				for (i = 1; i <= n; i++)
				{
					ch1 = str1[i - 1];
					for (j = 1; j <= m; j++)
					{
						ch2 = str2[j - 1];
						if (ch1 == ch2)
						{
							temp = 0;
						}
						else
						{
							temp = 1;
						}

						d[i][j] = exmin(d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + temp);
					}
				}
				auto ret = d[n][m];
				for (i = 0; i < n + 1; i++)
					delete[] d[i];
				delete[] d;

				return ret;
			}

			double getSimilarityRatio(const std::wstring & str1, const std::wstring & str2)
			{
				std::string::size_type ld = levenshteinDistance(str1,str2);
				return 1 - (double)ld / std::max(str1.length(), str2.length());
			}
		private:
			unsigned long long exmin(unsigned long long one, unsigned long long two, unsigned long long three)
			{
				decltype(one) min = one;
				if (two < min)
				{
					min = two;
				}
				if (three < min)
				{
					min = three;
				}
				return min;
			}
		};
}
#endif
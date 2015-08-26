#pragma once
#include <cinttypes>
#include <string.h>

#if defined(_MSC_VER)
#include <intrin.h>
#else
#include <stdint.h>
#endif

namespace scarlet {
	namespace strconv {
		namespace _private {
			struct fp {
				fp() {}

				fp(std::uint64_t f, int e) : f(f), e(e) {}

				fp(double d) {
					union {
						double d;
						std::uint64_t u64;
					} u = { d };

					int biased_e = (u.u64 & kDpExponentMask) >> kDpSignificandSize;
					std::uint64_t significand = (u.u64 & kDpSignificandMask);
					if (biased_e != 0) {
						f = significand + kDpHiddenBit;
						e = biased_e - kDpExponentBias;
					} else {
						f = significand;
						e = kDpMinExponent + 1;
					}
				}

				fp operator-(const fp& rhs) const {
					return fp(f - rhs.f, e);
				}

				fp operator*(const fp& rhs) const {
#if defined(_MSC_VER) && defined(_M_AMD64)
					std::uint64_t h;
					std::uint64_t l = _umul128(f, rhs.f, &h);
					if (l & (std::uint64_t(1) << 63)) // rounding
						h++;
					return fp(h, e + rhs.e + 64);
#elif (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 6)) && defined(__x86_64__)
					unsigned __int128 p = static_cast<unsigned __int128>(f)* static_cast<unsigned __int128>(rhs.f);
					std::uint64_t h = p >> 64;
					std::uint64_t l = static_cast<std::uint64_t>(p);
					if (l & (std::uint64_t(1) << 63)) // rounding
						h++;
					return fp(h, e + rhs.e + 64);
#else
					const std::uint64_t M32 = 0xFFFFFFFF;
					const std::uint64_t a = f >> 32;
					const std::uint64_t b = f & M32;
					const std::uint64_t c = rhs.f >> 32;
					const std::uint64_t d = rhs.f & M32;
					const std::uint64_t ac = a * c;
					const std::uint64_t bc = b * c;
					const std::uint64_t ad = a * d;
					const std::uint64_t bd = b * d;
					std::uint64_t tmp = (bd >> 32) + (ad & M32) + (bc & M32);
					tmp += 1U << 31;  /// mult_round
					return fp(ac + (ad >> 32) + (bc >> 32) + (tmp >> 32), e + rhs.e + 64);
#endif
				}

				fp normalize() const {
#if defined(_MSC_VER) && defined(_M_AMD64)
					unsigned long index;
					_BitScanReverse64(&index, f);
					return fp(f << (63 - index), e - (63 - index));
#elif defined(__GNUC__)
					int s = __builtin_clzll(f);
					return fp(f << s, e - s);
#else
					fp res = *this;
					while (!(res.f & kDpHiddenBit)) {
						res.f <<= 1;
						res.e--;
					}
					res.f <<= (kDiySignificandSize - kDpSignificandSize - 1);
					res.e = res.e - (kDiySignificandSize - kDpSignificandSize - 1);
					return res;
#endif
				}

				fp normalizeBoundary() const {
#if defined(_MSC_VER) && defined(_M_AMD64)
					unsigned long index;
					_BitScanReverse64(&index, f);
					return fp(f << (63 - index), e - (63 - index));
#else
					fp res = *this;
					while (!(res.f & (kDpHiddenBit << 1))) {
						res.f <<= 1;
						res.e--;
					}
					res.f <<= (kDiySignificandSize - kDpSignificandSize - 2);
					res.e = res.e - (kDiySignificandSize - kDpSignificandSize - 2);
					return res;
#endif
				}

				void normalizedBoundaries(fp* minus, fp* plus) const {
					fp pl = fp((f << 1) + 1, e - 1).normalizeBoundary();
					fp mi = (f == kDpHiddenBit) ? fp((f << 2) - 1, e - 2) : fp((f << 1) - 1, e - 1);
					mi.f <<= mi.e - pl.e;
					mi.e = pl.e;
					*plus = pl;
					*minus = mi;
				}

				static const int kDiySignificandSize = 64;
				static const int kDpSignificandSize = 52;
				static const int kDpExponentBias = 0x3FF + kDpSignificandSize;
				static const int kDpMinExponent = -kDpExponentBias;
				static const std::uint64_t kDpExponentMask = ((static_cast<std::uint64_t>(0x7FF00000) << 32) | static_cast<std::uint64_t>(0x00000000));
				static const std::uint64_t kDpSignificandMask = ((static_cast<std::uint64_t>(0x000FFFFF) << 32) | static_cast<std::uint64_t>(0xFFFFFFFF));
				static const std::uint64_t kDpHiddenBit = ((static_cast<std::uint64_t>(0x00100000) << 32) | static_cast<std::uint64_t>(0x00000000));

				std::uint64_t f;
				int e;
			};

			inline fp cachedPower(int e, int* K) {
				// 10^-348, 10^-340, ..., 10^340
				static const std::uint64_t kCachedPowers_F[] = {
					((static_cast<std::uint64_t>(0xfa8fd5a0) << 32) | static_cast<std::uint64_t>(0x081c0288)),
					((static_cast<std::uint64_t>(0xbaaee17f) << 32) | static_cast<std::uint64_t>(0xa23ebf76)),
					((static_cast<std::uint64_t>(0x8b16fb20) << 32) | static_cast<std::uint64_t>(0x3055ac76)),
					((static_cast<std::uint64_t>(0xcf42894a) << 32) | static_cast<std::uint64_t>(0x5dce35ea)),
					((static_cast<std::uint64_t>(0x9a6bb0aa) << 32) | static_cast<std::uint64_t>(0x55653b2d)),
					((static_cast<std::uint64_t>(0xe61acf03) << 32) | static_cast<std::uint64_t>(0x3d1a45df)),
					((static_cast<std::uint64_t>(0xab70fe17) << 32) | static_cast<std::uint64_t>(0xc79ac6ca)),
					((static_cast<std::uint64_t>(0xff77b1fc) << 32) | static_cast<std::uint64_t>(0xbebcdc4f)),
					((static_cast<std::uint64_t>(0xbe5691ef) << 32) | static_cast<std::uint64_t>(0x416bd60c)),
					((static_cast<std::uint64_t>(0x8dd01fad) << 32) | static_cast<std::uint64_t>(0x907ffc3c)),
					((static_cast<std::uint64_t>(0xd3515c28) << 32) | static_cast<std::uint64_t>(0x31559a83)),
					((static_cast<std::uint64_t>(0x9d71ac8f) << 32) | static_cast<std::uint64_t>(0xada6c9b5)),
					((static_cast<std::uint64_t>(0xea9c2277) << 32) | static_cast<std::uint64_t>(0x23ee8bcb)),
					((static_cast<std::uint64_t>(0xaecc4991) << 32) | static_cast<std::uint64_t>(0x4078536d)),
					((static_cast<std::uint64_t>(0x823c1279) << 32) | static_cast<std::uint64_t>(0x5db6ce57)),
					((static_cast<std::uint64_t>(0xc2109436) << 32) | static_cast<std::uint64_t>(0x4dfb5637)),
					((static_cast<std::uint64_t>(0x9096ea6f) << 32) | static_cast<std::uint64_t>(0x3848984f)),
					((static_cast<std::uint64_t>(0xd77485cb) << 32) | static_cast<std::uint64_t>(0x25823ac7)),
					((static_cast<std::uint64_t>(0xa086cfcd) << 32) | static_cast<std::uint64_t>(0x97bf97f4)),
					((static_cast<std::uint64_t>(0xef340a98) << 32) | static_cast<std::uint64_t>(0x172aace5)),
					((static_cast<std::uint64_t>(0xb23867fb) << 32) | static_cast<std::uint64_t>(0x2a35b28e)),
					((static_cast<std::uint64_t>(0x84c8d4df) << 32) | static_cast<std::uint64_t>(0xd2c63f3b)),
					((static_cast<std::uint64_t>(0xc5dd4427) << 32) | static_cast<std::uint64_t>(0x1ad3cdba)),
					((static_cast<std::uint64_t>(0x936b9fce) << 32) | static_cast<std::uint64_t>(0xbb25c996)),
					((static_cast<std::uint64_t>(0xdbac6c24) << 32) | static_cast<std::uint64_t>(0x7d62a584)),
					((static_cast<std::uint64_t>(0xa3ab6658) << 32) | static_cast<std::uint64_t>(0x0d5fdaf6)),
					((static_cast<std::uint64_t>(0xf3e2f893) << 32) | static_cast<std::uint64_t>(0xdec3f126)),
					((static_cast<std::uint64_t>(0xb5b5ada8) << 32) | static_cast<std::uint64_t>(0xaaff80b8)),
					((static_cast<std::uint64_t>(0x87625f05) << 32) | static_cast<std::uint64_t>(0x6c7c4a8b)),
					((static_cast<std::uint64_t>(0xc9bcff60) << 32) | static_cast<std::uint64_t>(0x34c13053)),
					((static_cast<std::uint64_t>(0x964e858c) << 32) | static_cast<std::uint64_t>(0x91ba2655)),
					((static_cast<std::uint64_t>(0xdff97724) << 32) | static_cast<std::uint64_t>(0x70297ebd)),
					((static_cast<std::uint64_t>(0xa6dfbd9f) << 32) | static_cast<std::uint64_t>(0xb8e5b88f)),
					((static_cast<std::uint64_t>(0xf8a95fcf) << 32) | static_cast<std::uint64_t>(0x88747d94)),
					((static_cast<std::uint64_t>(0xb9447093) << 32) | static_cast<std::uint64_t>(0x8fa89bcf)),
					((static_cast<std::uint64_t>(0x8a08f0f8) << 32) | static_cast<std::uint64_t>(0xbf0f156b)),
					((static_cast<std::uint64_t>(0xcdb02555) << 32) | static_cast<std::uint64_t>(0x653131b6)),
					((static_cast<std::uint64_t>(0x993fe2c6) << 32) | static_cast<std::uint64_t>(0xd07b7fac)),
					((static_cast<std::uint64_t>(0xe45c10c4) << 32) | static_cast<std::uint64_t>(0x2a2b3b06)),
					((static_cast<std::uint64_t>(0xaa242499) << 32) | static_cast<std::uint64_t>(0x697392d3)),
					((static_cast<std::uint64_t>(0xfd87b5f2) << 32) | static_cast<std::uint64_t>(0x8300ca0e)),
					((static_cast<std::uint64_t>(0xbce50864) << 32) | static_cast<std::uint64_t>(0x92111aeb)),
					((static_cast<std::uint64_t>(0x8cbccc09) << 32) | static_cast<std::uint64_t>(0x6f5088cc)),
					((static_cast<std::uint64_t>(0xd1b71758) << 32) | static_cast<std::uint64_t>(0xe219652c)),
					((static_cast<std::uint64_t>(0x9c400000) << 32) | static_cast<std::uint64_t>(0x00000000)),
					((static_cast<std::uint64_t>(0xe8d4a510) << 32) | static_cast<std::uint64_t>(0x00000000)),
					((static_cast<std::uint64_t>(0xad78ebc5) << 32) | static_cast<std::uint64_t>(0xac620000)),
					((static_cast<std::uint64_t>(0x813f3978) << 32) | static_cast<std::uint64_t>(0xf8940984)),
					((static_cast<std::uint64_t>(0xc097ce7b) << 32) | static_cast<std::uint64_t>(0xc90715b3)),
					((static_cast<std::uint64_t>(0x8f7e32ce) << 32) | static_cast<std::uint64_t>(0x7bea5c70)),
					((static_cast<std::uint64_t>(0xd5d238a4) << 32) | static_cast<std::uint64_t>(0xabe98068)),
					((static_cast<std::uint64_t>(0x9f4f2726) << 32) | static_cast<std::uint64_t>(0x179a2245)),
					((static_cast<std::uint64_t>(0xed63a231) << 32) | static_cast<std::uint64_t>(0xd4c4fb27)),
					((static_cast<std::uint64_t>(0xb0de6538) << 32) | static_cast<std::uint64_t>(0x8cc8ada8)),
					((static_cast<std::uint64_t>(0x83c7088e) << 32) | static_cast<std::uint64_t>(0x1aab65db)),
					((static_cast<std::uint64_t>(0xc45d1df9) << 32) | static_cast<std::uint64_t>(0x42711d9a)),
					((static_cast<std::uint64_t>(0x924d692c) << 32) | static_cast<std::uint64_t>(0xa61be758)),
					((static_cast<std::uint64_t>(0xda01ee64) << 32) | static_cast<std::uint64_t>(0x1a708dea)),
					((static_cast<std::uint64_t>(0xa26da399) << 32) | static_cast<std::uint64_t>(0x9aef774a)),
					((static_cast<std::uint64_t>(0xf209787b) << 32) | static_cast<std::uint64_t>(0xb47d6b85)),
					((static_cast<std::uint64_t>(0xb454e4a1) << 32) | static_cast<std::uint64_t>(0x79dd1877)),
					((static_cast<std::uint64_t>(0x865b8692) << 32) | static_cast<std::uint64_t>(0x5b9bc5c2)),
					((static_cast<std::uint64_t>(0xc83553c5) << 32) | static_cast<std::uint64_t>(0xc8965d3d)),
					((static_cast<std::uint64_t>(0x952ab45c) << 32) | static_cast<std::uint64_t>(0xfa97a0b3)),
					((static_cast<std::uint64_t>(0xde469fbd) << 32) | static_cast<std::uint64_t>(0x99a05fe3)),
					((static_cast<std::uint64_t>(0xa59bc234) << 32) | static_cast<std::uint64_t>(0xdb398c25)),
					((static_cast<std::uint64_t>(0xf6c69a72) << 32) | static_cast<std::uint64_t>(0xa3989f5c)),
					((static_cast<std::uint64_t>(0xb7dcbf53) << 32) | static_cast<std::uint64_t>(0x54e9bece)),
					((static_cast<std::uint64_t>(0x88fcf317) << 32) | static_cast<std::uint64_t>(0xf22241e2)),
					((static_cast<std::uint64_t>(0xcc20ce9b) << 32) | static_cast<std::uint64_t>(0xd35c78a5)),
					((static_cast<std::uint64_t>(0x98165af3) << 32) | static_cast<std::uint64_t>(0x7b2153df)),
					((static_cast<std::uint64_t>(0xe2a0b5dc) << 32) | static_cast<std::uint64_t>(0x971f303a)),
					((static_cast<std::uint64_t>(0xa8d9d153) << 32) | static_cast<std::uint64_t>(0x5ce3b396)),
					((static_cast<std::uint64_t>(0xfb9b7cd9) << 32) | static_cast<std::uint64_t>(0xa4a7443c)),
					((static_cast<std::uint64_t>(0xbb764c4c) << 32) | static_cast<std::uint64_t>(0xa7a44410)),
					((static_cast<std::uint64_t>(0x8bab8eef) << 32) | static_cast<std::uint64_t>(0xb6409c1a)),
					((static_cast<std::uint64_t>(0xd01fef10) << 32) | static_cast<std::uint64_t>(0xa657842c)),
					((static_cast<std::uint64_t>(0x9b10a4e5) << 32) | static_cast<std::uint64_t>(0xe9913129)),
					((static_cast<std::uint64_t>(0xe7109bfb) << 32) | static_cast<std::uint64_t>(0xa19c0c9d)),
					((static_cast<std::uint64_t>(0xac2820d9) << 32) | static_cast<std::uint64_t>(0x623bf429)),
					((static_cast<std::uint64_t>(0x80444b5e) << 32) | static_cast<std::uint64_t>(0x7aa7cf85)),
					((static_cast<std::uint64_t>(0xbf21e440) << 32) | static_cast<std::uint64_t>(0x03acdd2d)),
					((static_cast<std::uint64_t>(0x8e679c2f) << 32) | static_cast<std::uint64_t>(0x5e44ff8f)),
					((static_cast<std::uint64_t>(0xd433179d) << 32) | static_cast<std::uint64_t>(0x9c8cb841)),
					((static_cast<std::uint64_t>(0x9e19db92) << 32) | static_cast<std::uint64_t>(0xb4e31ba9)),
					((static_cast<std::uint64_t>(0xeb96bf6e) << 32) | static_cast<std::uint64_t>(0xbadf77d9)),
					((static_cast<std::uint64_t>(0xaf87023b) << 32) | static_cast<std::uint64_t>(0x9bf0ee6b))
				};
				static const std::int16_t kCachedPowers_E[] = {
					-1220, -1193, -1166, -1140, -1113, -1087, -1060, -1034, -1007, -980,
					-954, -927, -901, -874, -847, -821, -794, -768, -741, -715,
					-688, -661, -635, -608, -582, -555, -529, -502, -475, -449,
					-422, -396, -369, -343, -316, -289, -263, -236, -210, -183,
					-157, -130, -103, -77, -50, -24, 3, 30, 56, 83,
					109, 136, 162, 189, 216, 242, 269, 295, 322, 348,
					375, 402, 428, 455, 481, 508, 534, 561, 588, 614,
					641, 667, 694, 720, 747, 774, 800, 827, 853, 880,
					907, 933, 960, 986, 1013, 1039, 1066
				};

				//int k = static_cast<int>(ceil((-61 - e) * 0.30102999566398114)) + 374;
				double dk = (-61 - e) * 0.30102999566398114 + 347;	// dk must be positive, so can do ceiling in positive
				int k = static_cast<int>(dk);
				if (k != dk)
					k++;

				unsigned index = static_cast<unsigned>((k >> 3) + 1);
				*K = -(-348 + static_cast<int>(index << 3));	// decimal exponent no need lookup table

				return fp(kCachedPowers_F[index], kCachedPowers_E[index]);
			}

			template<typename _Char>
			inline void grisuRound(_Char buffer, int len, std::uint64_t delta, std::uint64_t rest, std::uint64_t ten_kappa, std::uint64_t wp_w);

			template<typename _Char>
			inline void grisuRound(_Char * buffer, int len, std::uint64_t delta, std::uint64_t rest, std::uint64_t ten_kappa, std::uint64_t wp_w) {
				while (rest < wp_w && delta - rest >= ten_kappa &&
					(rest + ten_kappa < wp_w ||  /// closer
					wp_w - rest > rest + ten_kappa - wp_w)) {
					buffer[len - 1]--;
					rest += ten_kappa;
				}
			}

			inline unsigned countDecimalDigit32(std::uint32_t n) {
				// Simple pure C++ implementation was faster than __builtin_clz version in this situation.
				if (n < 10) return 1;
				if (n < 100) return 2;
				if (n < 1000) return 3;
				if (n < 10000) return 4;
				if (n < 100000) return 5;
				if (n < 1000000) return 6;
				if (n < 10000000) return 7;
				if (n < 100000000) return 8;
				if (n < 1000000000) return 9;
				return 10;
			}

			template<typename _Char>
			inline void digitGen(const fp& W, const fp& Mp, std::uint64_t delta, _Char buffer, int* len, int* K);

			template<typename _Char>
			inline void digitGen(const fp& W, const fp& Mp, std::uint64_t delta, _Char * buffer, int* len, int* K) {
				static const std::uint32_t kPow10[] = { 1, 10, 100, 1000, 10000, 100000, 1000000, 10000000, 100000000, 1000000000 };
				const fp one(std::uint64_t(1) << -Mp.e, Mp.e);
				const fp wp_w = Mp - W;
				std::uint32_t p1 = static_cast<std::uint32_t>(Mp.f >> -one.e);
				std::uint64_t p2 = Mp.f & (one.f - 1);
				int kappa = static_cast<int>(countDecimalDigit32(p1));
				*len = 0;

				while (kappa > 0) {
					std::uint32_t d;
					switch (kappa) {
					case 10: d = p1 / 1000000000; p1 %= 1000000000; break;
					case  9: d = p1 / 100000000; p1 %= 100000000; break;
					case  8: d = p1 / 10000000; p1 %= 10000000; break;
					case  7: d = p1 / 1000000; p1 %= 1000000; break;
					case  6: d = p1 / 100000; p1 %= 100000; break;
					case  5: d = p1 / 10000; p1 %= 10000; break;
					case  4: d = p1 / 1000; p1 %= 1000; break;
					case  3: d = p1 / 100; p1 %= 100; break;
					case  2: d = p1 / 10; p1 %= 10; break;
					case  1: d = p1;              p1 = 0; break;
					default:
#if defined(_MSC_VER)
						__assume(0);
#elif __GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 5)
						__builtin_unreachable();
#else
						d = 0;
#endif
					}
					if (d || *len)
						buffer[(*len)++] = '0' + static_cast<_Char>(d);
					kappa--;
					std::uint64_t tmp = (static_cast<std::uint64_t>(p1) << -one.e) + p2;
					if (tmp <= delta) {
						*K += kappa;
						grisuRound(buffer, *len, delta, tmp, static_cast<std::uint64_t>(kPow10[kappa]) << -one.e, wp_w.f);
						return;
					}
				}

				// kappa = 0
				for (;;) {
					p2 *= 10;
					delta *= 10;
					_Char d = static_cast<_Char>(p2 >> -one.e);
					if (d || *len)
						buffer[(*len)++] = '0' + d;
					p2 &= one.f - 1;
					kappa--;
					if (p2 < delta) {
						*K += kappa;
						grisuRound(buffer, *len, delta, p2, one.f, wp_w.f * kPow10[-kappa]);
						return;
					}
				}
			}

			template<typename _Char>
			inline void grisu2(double value, _Char buffer, int* length, int* K);

			template<typename _Char>
			inline void grisu2(double value, _Char * buffer, int* length, int* K) {
				const fp v(value);
				fp w_m, w_p;
				v.normalizedBoundaries(&w_m, &w_p);

				const fp c_mk = cachedPower(w_p.e, K);
				const fp W = v.normalize() * c_mk;
				fp Wp = w_p * c_mk;
				fp Wm = w_m * c_mk;
				Wm.f++;
				Wp.f--;
				digitGen(W, Wp, Wp.f - Wm.f, buffer, length, K);
			}

			template<typename _Char>
			inline const _Char * digitsLut() {
				static const _Char cDigitsLut[200] = {
					'0', '0', '0', '1', '0', '2', '0', '3', '0', '4', '0', '5', '0', '6', '0', '7', '0', '8', '0', '9',
					'1', '0', '1', '1', '1', '2', '1', '3', '1', '4', '1', '5', '1', '6', '1', '7', '1', '8', '1', '9',
					'2', '0', '2', '1', '2', '2', '2', '3', '2', '4', '2', '5', '2', '6', '2', '7', '2', '8', '2', '9',
					'3', '0', '3', '1', '3', '2', '3', '3', '3', '4', '3', '5', '3', '6', '3', '7', '3', '8', '3', '9',
					'4', '0', '4', '1', '4', '2', '4', '3', '4', '4', '4', '5', '4', '6', '4', '7', '4', '8', '4', '9',
					'5', '0', '5', '1', '5', '2', '5', '3', '5', '4', '5', '5', '5', '6', '5', '7', '5', '8', '5', '9',
					'6', '0', '6', '1', '6', '2', '6', '3', '6', '4', '6', '5', '6', '6', '6', '7', '6', '8', '6', '9',
					'7', '0', '7', '1', '7', '2', '7', '3', '7', '4', '7', '5', '7', '6', '7', '7', '7', '8', '7', '9',
					'8', '0', '8', '1', '8', '2', '8', '3', '8', '4', '8', '5', '8', '6', '8', '7', '8', '8', '8', '9',
					'9', '0', '9', '1', '9', '2', '9', '3', '9', '4', '9', '5', '9', '6', '9', '7', '9', '8', '9', '9'
				};
				return cDigitsLut;
			}

			template<typename _Char>
			inline void writeExponent(int K, _Char buffer);

			template<typename _Char>
			inline void writeExponent(int K, _Char * buffer) {
				if (K < 0) {
					*buffer++ = '-';
					K = -K;
				}

				if (K >= 100) {
					*buffer++ = '0' + static_cast<_Char>(K / 100);
					K %= 100;
					const _Char * d = digitsLut<_Char>() + K * 2;
					*buffer++ = d[0];
					*buffer++ = d[1];
				} else if (K >= 10) {
					const _Char * d = digitsLut<_Char>() + K * 2;
					*buffer++ = d[0];
					*buffer++ = d[1];
				} else
					*buffer++ = '0' + static_cast<_Char>(K);

				*buffer = '\0';
			}

			template<typename _Char>
			inline void prettify(_Char buffer, int length, int k);

			template<typename _Char>
			inline void prettify(_Char * buffer, int length, int k) {
				const int kk = length + k;	// 10^(kk-1) <= v < 10^kk

				if (length <= kk && kk <= 21) {
					// 1234e7 -> 12340000000
					for (int i = length; i < kk; i++)
						buffer[i] = '0';
					buffer[kk] = '.';
					buffer[kk + 1] = '0';
					buffer[kk + 2] = '\0';
				} else if (0 < kk && kk <= 21) {
					// 1234e-2 -> 12.34
					memmove(&buffer[kk + 1], &buffer[kk], (length - kk)*sizeof(_Char));
					buffer[kk] = '.';
					buffer[length + 1] = '\0';
				} else if (-6 < kk && kk <= 0) {
					// 1234e-6 -> 0.001234
					const int offset = 2 - kk;
					memmove(&buffer[offset], &buffer[0], length*sizeof(_Char));
					buffer[0] = '0';
					buffer[1] = '.';
					for (int i = 2; i < offset; i++)
						buffer[i] = '0';
					buffer[length + offset] = '\0';
				} else if (length == 1) {
					// 1e30
					buffer[1] = 'e';
					writeExponent(kk - 1, &buffer[2]);
				} else {
					// 1234e30 -> 1.234e33
					memmove(&buffer[2], &buffer[1], (length - 1)*sizeof(_Char));
					buffer[1] = '.';
					buffer[length + 1] = 'e';
					writeExponent(kk - 1, &buffer[0 + length + 2]);
				}
			}
		}
	}
}
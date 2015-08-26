#pragma once
#include <cinttypes>
#include <chrono>
#include <Windows.h>
#include <psapi.h>
#pragma comment(lib,"psapi.lib")

namespace scarlet {
	namespace performance {
		class time_interval {
		public:
			time_interval(const std::chrono::duration<long long, std::ratio_multiply<std::ratio<100LL, 1LL>, std::nano>> & i) :
				interval_(i) {}
			~time_interval() = default;

			std::uint64_t nanoseconds() const {
				return std::chrono::duration_cast<std::chrono::milliseconds>(interval_).count();
			}
			std::uint64_t microseconds() const {
				return std::chrono::duration_cast<std::chrono::microseconds>(interval_).count();
			}
			std::uint64_t milliseconds() const {
				return std::chrono::duration_cast<std::chrono::milliseconds>(interval_).count();
			}
			std::uint64_t seconds() const {
				return std::chrono::duration_cast<std::chrono::seconds>(interval_).count();
			}
			std::uint64_t minutes() const {
				return std::chrono::duration_cast<std::chrono::minutes>(interval_).count();
			}
		private:
			std::chrono::duration<long long, std::ratio_multiply<std::ratio<100LL, 1LL>, std::nano>> interval_;
		};

		class memory {
		public:
			class __memory_used {
			public:
				__memory_used(const std::uint64_t used) :
					used_(used) {}
				__memory_used(const __memory_used & used):
					used_(used.used_){}
				~__memory_used() = default;

				__memory_used & operator=(const __memory_used & used) {
					used_ = used.used_;
					return *this;
				}

				friend __memory_used operator-(const __memory_used & mu, const __memory_used & mu2) {
					return{ mu.used_ - mu2.used_ };
				}

				std::uint64_t byte() const {
					return used_;
				}

				std::uint64_t kb() const {
					return byte() / 1024;
				}

				std::uint64_t mb() const {
					return kb() / 1024;
				}

				std::uint64_t gb() const {
					return mb() / 1024;
				}
			private:
				std::uint64_t used_;
			};

		public:
			static __memory_used memory_used() {
				PROCESS_MEMORY_COUNTERS pmc = { sizeof(PROCESS_MEMORY_COUNTERS) };
				if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc)) == 0) {
					return __memory_used(0);
				}
				return __memory_used(static_cast<std::uint64_t>(pmc.WorkingSetSize));
				//return __memory_used(static_cast<std::uint64_t>(pmc.PagefileUsage));
			}
		};

		class timer {
		public:
			timer() :initlize_clock_(std::chrono::system_clock::now()) {}
			~timer() = default;

			time_interval timing() const {
				return std::chrono::system_clock::now() - initlize_clock_;
			}

			void reset() {
				initlize_clock_ = std::chrono::system_clock::now();
			}
		private:
			std::chrono::system_clock::time_point initlize_clock_;
		};

	}
}
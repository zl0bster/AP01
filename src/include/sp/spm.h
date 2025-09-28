#pragma once
#ifndef SPM_H
#define SPM_H

#include <complex>
#include <span>


#include <vector>
#include <algorithm>
#include <concepts>
#include <type_traits>
#include <iostream>
#include <cassert>
#include <fstream>
#include <math.h>

namespace spm {
	//#include <span>


	/* math const definition */
#ifndef M_PI
#define M_PI        3.1415926535897932384626433832795
#endif

#ifndef M_2PI
#define M_2PI       6.283185307179586476925286766559
#endif

	template <typename T>
	inline void normalize(std::vector<T>& arg) {
		static_assert(std::is_floating_point_v<T>);
		auto maxpos = std::max_element(arg.cbegin(), arg.cend(), [](T a, T b)
			{return std::abs(a) < std::abs(b); });
		T norm = std::abs(*maxpos);
		if (norm > 0) {
			for (auto& val : arg) {
				val /= norm;
			}
		}
	};

	template <typename T>
	inline void normalize(std::span<T> arg) {
		static_assert(std::is_floating_point_v<T>);
		auto maxpos = std::max_element(arg.cbegin(), arg.cend(), [](T a, T b)
			{return std::abs(a) < std::abs(b); });
		T norm = std::abs(*maxpos);
		if (norm > 0) {
			for (auto& val : arg) {
				val /= norm;
			}
		}
	};

	template <typename T>
	inline void correl(const std::vector<T>& a, const std::vector<T>& b, std::vector<T>& result) {
		static_assert(std::is_floating_point_v<T>);
		assert(a.size() > 0); // checked signal
		assert(b.size() > 0); // searched sample
		assert(result.size() >= a.size());
		int bbase = b.size() / 2;
		int btail = b.size() - bbase;
		for (int posCnt{ 0 }; posCnt < a.size(); ++posCnt) {
			int abegin = (posCnt >= bbase) ? (posCnt - bbase) : 0;
			//aend = (posCnt < (a.size() - btail)) ? (posCnt + btail) : a.size();
			int bbegin = (posCnt >= bbase) ? 0 : (bbase - posCnt);
			int bend = (posCnt < (a.size() - btail)) ? b.size() : (bbase + a.size() - posCnt);
			T accumulator{ 0 };
			int apos = abegin;
			for (int bpos = bbegin; bpos < bend; ++bpos) {
				accumulator += a[apos] * b[bpos];
				++apos;
			}
			result[posCnt] = accumulator;
		}
	};

	inline void correl1(const std::vector<float>& a, const std::vector<float>& b, std::vector<float>& result) {
		assert(a.size() > 0); // checked signal
		assert(b.size() > 0); // searched sample
		assert(result.size() >= a.size());
		int bbase = b.size() / 2;
		int btail = b.size() - bbase;
		for (int posCnt{ 0 }; posCnt < a.size(); ++posCnt) {
			int abegin = (posCnt >= bbase) ? (posCnt - bbase) : 0;
			//aend = (posCnt < (a.size() - btail)) ? (posCnt + btail) : a.size();
			int bbegin = (posCnt >= bbase) ? 0 : (bbase - posCnt);
			int bend = (posCnt < (a.size() - btail)) ? b.size() : (bbase + a.size() - posCnt);
			float accumulator{ 0 };
			int apos = abegin;
			for (int bpos = bbegin; bpos < bend; ++bpos) {
				accumulator += a[apos] * b[bpos];
				++apos;
			}
			result[posCnt] = accumulator;
		}
	};

	template<typename T>
	inline void printvec(const std::vector<T>& a) {
		//static_assert((std::is_floating_point_v<T>) || (std::is_complex_t<T>));
		std::cout << std::endl << "[ ";
		for (auto val : a) {
			std::cout << val << ' ';
		}
		std::cout << ']' << std::endl;
	};

	template <typename T>
	inline void saveVec2txt(std::string fileName, const std::vector<T>& a) {
		std::ofstream file(fileName);
		if (file.is_open()) {
			for (auto val : a) {
				file << val << std::endl;
			}
			file.close();
		}
	}

	template <typename T>
	inline void windowHann(std::vector<T>& a) {
		// fills the given vector with Hann window values
		// https://habr.com/ru/articles/430536/
		// https://en.wikipedia.org/wiki/Hann_function

		static_assert(std::is_floating_point_v<T>);
		const T coef{static_cast<T> (M_2PI / (a.size() - 1)) };
		int i{ 0 };
		for (auto& val : a) {
			val = static_cast<T> (0.5 * (1 - cos(i * coef)));
			++i;
		}
	}

	template <typename T>
	inline void windowHann(std::span<T> a) {
		// fills the given vector with Hann window values
		// https://habr.com/ru/articles/430536/
		// https://en.wikipedia.org/wiki/Hann_function

		static_assert(std::is_floating_point_v<T>);
		const T coef{ static_cast<T> (M_2PI / (a.size() - 1)) };
		int i{ 0 };
		for (auto& val : a) {
			val = static_cast<T> (0.5 * (1 - cos(i * coef)));
			++i;
		}
	}

}; // end namespace spm
#endif SPM_H

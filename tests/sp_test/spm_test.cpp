#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
//#include <vector>
#include "doctest.h"
#include "spm.h"
#include <span>
#include <unsupported/Eigen/FFT>


using namespace Eigen;


TEST_CASE("normalization test") {

	SUBCASE("norm: check max is positive") {
		std::vector<float> sample{ 0, 1 ,2 ,3 ,-4 ,5 ,6 ,7 ,8 ,9 ,10 };
		std::vector<float> result{ 0, .1, .2, .3, -.4, .5, .6, .7, .8, .9, 1 };
		bool cp_result{ true };
		//spm::normalize(std::span<float>(sample));
		spm::normalize(sample);
		for (size_t i = 0; i < sample.size(); ++i) {
			//float delta = std::abs((sample[i] - result[i]) / (sample[i] + result[i]) / 2);
			//cp_result &= (delta <= 0.001);
			cp_result &= (sample[i] == result[i]);
		}
		CHECK(cp_result == true);
	}
	SUBCASE("norm: check max is negative") {
		std::vector<float> sample{ 0, 1 ,2 ,3 ,-4 ,5 ,6 ,7 ,8 ,9 ,-10 };
		std::vector<float> result{ 0, .1, .2, .3, -.4, .5, .6, .7, .8, .9, -1 };
		bool cp_result{ true };
		spm::normalize(sample);
		for (size_t i = 0; i < sample.size(); ++i) {
			cp_result &= (sample[i] == result[i]);
		}
		CHECK(cp_result == true);
	}

}

TEST_CASE("correl test") {

	SUBCASE("correl1: check max pos") {
		std::vector<float> signal{ 0,1,10,0,6,8,9,6,0,0,3,4,5,3,0 };
		std::vector<float> probe{ 0,3,4,5,3,0 };
		spm::normalize(signal);
		spm::normalize(probe);
		std::vector<float> result(signal.size(), 0);
		spm::correl1(signal, probe, result);
		spm::normalize(result);
		auto maxpos = std::max_element(result.cbegin(), result.cend());
		int corrpos = std::distance(result.cbegin(), maxpos);
		CHECK(corrpos == 6);
		spm::printvec(result);
	}

	SUBCASE("correl: check max pos") {
		std::vector<float> signal{ 0,1,10,0,6,8,9,6,0,0,3,4,5,3,0 };
		std::vector<float> probe{ 0,3,4,5,3,0 };
		spm::normalize(signal);
		spm::normalize(probe);
		std::vector<float> result(signal.size(), 0);
		spm::correl(signal, probe, result);
		spm::normalize(result);
		auto maxpos = std::max_element(result.cbegin(), result.cend());
		int corrpos = std::distance(result.cbegin(), maxpos);
		CHECK(corrpos == 6);
		spm::printvec(result);
	}
}

TEST_CASE("EIGEN FFT test") {

	using ScalarVector = std::vector<float>;
	using ComplexVector = std::vector<std::complex<float>>;

	SUBCASE("fft vector scalar test") {
		std::vector<float> window(1024, 0);
		spm::windowHann(window);
		spm::saveVec2txt("windowHann.csv", window);
		std::vector<std::complex<float>> out;
		FFT<float> fft;
		fft.SetFlag(fft.HalfSpectrum);
		fft.fwd(out, window);
		spm::saveVec2txt("windowHannFd.csv", out);
		//spm::printvec(out);
	}
}
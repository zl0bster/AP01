#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
//#include <vector>
#include "doctest.h"
#include "n_model.h"
#include <numeric>
#include <iostream>

using namespace nmd;

TEST_CASE("test noise model class") {

	const size_t FRAME_SIZE = 8;
	const size_t WINDOW_SIZE = 5;
	using SAMPLE_T = std::array<std::complex<float>, FRAME_SIZE>;
	using ITEM_T = std::complex<float>;

	SUBCASE("check ") {
		N_Model<float, FRAME_SIZE> nm(WINDOW_SIZE);
		SAMPLE_T noise1{1,2,3,4,5,6,7,8};
		SAMPLE_T signal;
		nm.update(noise1);
		CHECK(nm[7] == noise1[7].real());
		CHECK(nm.spectralPower() == 204);
		std::copy(noise1.cbegin(), noise1.cend(), signal.begin());
		ITEM_T signalSum= std::accumulate(signal.cbegin(), signal.cend(), (ITEM_T)0.0);
		nm.denoise(signal);
		ITEM_T result= std::accumulate(signal.cbegin(), signal.cend(),(ITEM_T) 0.0);
		ITEM_T testVal = ((ITEM_T)0.01) * signalSum;
		bool check = (0.000001> abs((result - testVal).real()));
		CHECK(check);
	}
}
#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
//#include <vector>
#include "doctest.h"
//#include <span>
#include "fdbuffer.h"

using namespace fdb;

TEST_CASE("ring buffer test") {

	const size_t FRAME_SIZE = 8;
	const size_t BUF_SIZE = 10;
	const long INIT_ID = 1000;

	SUBCASE("check initialization") {

		FDBuffer<int, FRAME_SIZE, BUF_SIZE> b{ INIT_ID };
		b.getFrame();
		CHECK(b.currentFrameID() == INIT_ID);
		CHECK(b.contains(INIT_ID + 2) == false);
		CHECK(b.contains(INIT_ID) == true);
		auto frame = b.currentFrame();
		int i = INIT_ID;
		for (auto& cell : frame) {
			cell = i;
			++i;
		}
		auto testedFrame = b.frame(INIT_ID);
		int result{ 0 };
		i = INIT_ID;
		for (auto& cell : testedFrame) {
			result += cell-i;
			++i;
		}
		CHECK(result == 0);

		for (int i = 0; i < 12; ++i) {
			b.getFrame();
		}
		CHECK(b.currentFrameID() == INIT_ID+12);
		CHECK(b.contains(INIT_ID + 3) == true);
		CHECK(b.contains(INIT_ID + 2) == false);
		CHECK(b.contains(INIT_ID) == false);

	}

}
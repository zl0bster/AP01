#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "tdbuffer.h"
#include <iostream>
#include <string>


TEST_CASE("tdbuffer test") {
	using namespace tdb;
	const size_t BufferSize = 128;
	const size_t HopVal = 32;
	const size_t WinSize = 32;
	const int FS = 200;
	const std::string FileName{ "tdb_test1.wav" };

	SUBCASE("check fill and save") {
		TDBuffer b;
		CHECK(b.readyToWindowing()==false);
		CHECK(b.readyToSave() == false);
		b.setFs(FS);
		CHECK(b.fs() == FS);
		b.setWindowSize(WinSize);
		CHECK(b.windowSize() == WinSize);
		b.setHopValue(HopVal);
		CHECK(b.startWindowing() == false);
		b.initializeBuf(BufferSize);
		CHECK(b.readyToWindowing() == true);
		CHECK(b.startWindowing() == true);
		CHECK(b.currentWindowNumber() == 0);
		CHECK(b.bufSize() == BufferSize);
		float accVal{ 0 };
		float fillVal{0};
		do {
			auto w = b.currentWindow();
			for (auto& s : w) {
				s = fillVal;
				++fillVal;
				accVal += s;
			}
		} while (b.switchNextWin());
		size_t winNumber = BufferSize / HopVal -1;
		CHECK(b.currentWindowNumber() == winNumber);
		std::cout << "buf is filled until: " << fillVal<<std::endl;

		CHECK(b.readyToWindowing() == true);
		CHECK(b.startWindowing() == true);
		CHECK(b.currentWindowNumber() == 0);
		do {
			auto w = b.currentWindow();
			for (auto s : w) {
				accVal -=s;
			}
		} while (b.switchNextWin());
		CHECK(accVal == 0);

		CHECK(b.readyToSave() == true);
		CHECK(b.saveFile(FileName) == true);
	}

	SUBCASE("check load and consistency") {
		TDBuffer b;
		CHECK(b.loadFile("wrong_file_name.wav")==false);
		CHECK(b.loadFile(FileName) == true);
		CHECK(b.fs() == FS);
		CHECK(b.bufSize() == BufferSize);
		b.setWindowSize(WinSize);
		b.setHopValue(HopVal);
		CHECK(b.readyToWindowing() == true);
		CHECK(b.startWindowing() == true);
		CHECK(b.currentWindowNumber() == 0);
		CHECK(b.bufSize() == BufferSize);
		float accVal{ 0 };
		float fillVal{ 0 };
		do {
			auto w = b.currentWindow();
			for (auto s : w) {
				bool compareResult = (s == fillVal);
				++fillVal;
				accVal += 1*compareResult;
			}
		} while (b.switchNextWin());
		size_t winNumber = BufferSize / HopVal - 1;
		CHECK(b.currentWindowNumber() == winNumber);
		std::cout << "buf is filled until: " << fillVal << std::endl;
		std::cout << "found correct values: " << accVal << std::endl;
		CHECK(accVal == BufferSize);
	}
}
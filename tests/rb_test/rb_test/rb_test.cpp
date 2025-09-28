#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "doctest.h"
#include "rbuffer.h"
//#include <algorithm>


TEST_CASE("rbuffer test") {
	Rbuffer<int, 4> b;
	std::vector<int> sample{ 1, 2, 3, 4, 5, 6 };
	std::vector<int> result{ 5, 6, 3, 4 };
	for (auto item : sample) {
		b.pushback(item);
	}
	SUBCASE("check container consistence []") {
		bool cp_result{ true };
		for (size_t i = 0; i < b.size(); ++i) {
			cp_result &= (b[i] == result[i]);
		}
		CHECK(cp_result == true);
	}
	SUBCASE("check access for(auto x:v )") {
		std::vector<int> data;
		for (auto x : b) {
			data.push_back(x);
		}
		bool cp_result = std::equal(data.begin(), data.end(), result.begin(), result.end());
		CHECK(cp_result == true);
	}
	SUBCASE("check access begin - end") {
		bool cp_result = std::equal(b.begin(), b.end(), result.begin(), result.end());
		CHECK(cp_result == true);
	}
	SUBCASE("check contains()") {
		auto condition1 = [](int x) {return x == 6; };
		CHECK(b.contains(condition1) == true);
		auto condition2 = [](int x) {return x == 1; };
		CHECK(b.contains(condition2) == false);
	}
	SUBCASE("check top()") {
		CHECK(b.top() == 6);
	}
}

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
//#include <vector>
#include "doctest.h"
#include <numeric>
#include <iostream>
#include "vocaldetector.h"

TEST_CASE("test ") {

	const size_t FRAME_SIZE = 20;
	const size_t FS_val = 100; // set it for half of spectrum

	using DATA_T = float;
	using SIGNAL_T = std::array< std::complex<DATA_T>, FRAME_SIZE>;
	using namespace vdh;
	using namespace nmd;

	auto printSignature = [](ArtifactSignature<DATA_T>& s) {
		std::cout << s.frameID <<": "<<s.probability << " - " << s.framePower << std::endl;
		std::cout << s.signature[0].freqHz << "Hz - "<< s.signature[0].magn << std::endl;
		};

	SUBCASE("check ") {	
		// create signal without F0 in vocal range
		SIGNAL_T noise{ 2,3,4,4, 4,4,4,3, 4,3,1,2, 4,5,3,2, 5,4,6,5 };
		// create signal with F0 in vocal range
		SIGNAL_T sound{ 2,4,4,6, 4,5,4,3, 4,3,2,2, 4,5,3,2, 5,6,7,5 };
		// create signal with F0 and formants in vocal range
		SIGNAL_T vocal{ 2,4,7,4, 4,5,4,3, 4,3,2,3, 5,4,3,2, 5,8,7,6 };



		VocalDetector<DATA_T, FRAME_SIZE> vd;
		N_Model<DATA_T, FRAME_SIZE> nm;
		vd.setFs(FS_val);
		vd.setVocalRangeLowFreq(10);
		vd.setVocalRangeHiFreq(40);
		vd.setNoiseModel(&nm);
		vd.setPowerThreshold(0.10);
		vd.setProbabilityThreshold(0.30);

		nm.update(noise);
		CHECK(vd.isArtifDetected(noise) == false);
		ArtifactSignature<DATA_T> s_noise;
		s_noise.frameID = 1;
		vd.signature(s_noise);
		CHECK(s_noise.framePower == nm.spectralPower());
		printSignature(s_noise);
		ArtifactSignature<DATA_T> s_sound;
		s_sound.frameID = 2;
		CHECK(vd.isArtifDetected(sound) == true);
		vd.signature(s_sound);
		printSignature(s_sound);
		ArtifactSignature<DATA_T> s_voice;
		s_voice.frameID = 3;
		CHECK(vd.isArtifDetected(vocal) == true);
		vd.signature(s_voice);
		printSignature(s_voice);
	}
}
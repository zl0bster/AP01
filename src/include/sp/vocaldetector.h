#pragma once
//----------------------------
/* Class Vocal Detector analizes FD-frame
*	check power of vocal freq segment
*	looks for F0 in vocal segment and next freqs
*	it defines Artifact Signature
*/

#ifndef VOCAL_DETECTOR
#define VOCAL_DETECTOR

#include "qdebug.h"
//#include "fd_artifact_detector.h"
#include "n_model.h"
#include "../include/sp/artif_sign.h"
#include <algorithm>
#include <span>
#include <complex>
#include <array>
#include <QDataStream>
//#include <iostream>

namespace vdh {
	//using namespace fdad;
    using namespace asa;
	using PROBABILITY = double;
    // const size_t N_FREQS_TO_CHECK = 3;

    // template <typename T = float>
    // struct FrData {
    // 	T freqHz;	//
    // 	T magn;		// magnitude
    // };
    
 //    template <typename T>
 //    QDataStream &operator<<(QDataStream &ds, const FrData<T> &f) {
 //        return ds<<f.freqHz<<f.magn;
 //    };

 //    template <typename T>
 //    QDataStream &operator>>(QDataStream &ds, const FrData<T> &f) {
 //        ds>>f.freqHz;
 //        ds>>f.magn;
 //        return ds;
 //    };

 //    template<typename T = float, size_t FSize = N_FREQS_TO_CHECK>
    // struct ArtifactSignature {
    // 	long frameID;
    // 	T probability; // [0 ... 1.0]
    // 	T framePower;
    // 	std::array<FrData<T>, FSize> signature;
    // };

 //    template<typename T , size_t FSize>
 //    QDataStream &operator<<(QDataStream &ds, const ArtifactSignature <T, FSize> &as){
 //        return ds<<as.frameID<<as.probability<<as.framePower<<as.signature[0]<<as.signature[1];
 //    };

 //    template<typename T , size_t FSize>
 //    QDataStream &operator>>(QDataStream &ds, const ArtifactSignature <T, FSize> &as){
 //        ds>>as.frameID;
 //        ds>>as.probability;
 //        ds>>as.framePower;
 //        ds>>as.signature[0];
 //        ds>>as.signature[1];
 //        return ds;
 //    };

	template<typename T, size_t FrameSize>
	class VocalDetector {
		using N_MODEL_T = nmd::N_Model<T, FrameSize>;
		using INPUT_RANGE_T = std::span<std::complex<T>>;
		using OUTPUT_RANGE_T = std::span<T>;
		using MAGN_T = T;

        const PROBABILITY StepPerCriter = 0.05;
        const T HalfSpecCoef = 0.5;
        T m_fs; // Hz
		T m_vocalRangeLowFreq{ 0 };	// Hz
		T m_vocalRangeHiFreq{ 0 };	// Hz
		size_t m_rangeLoLimit{ 0 };
		size_t m_rangeHiLimit{ 0 };
		MAGN_T m_frameVocalRangePower{ 0 };
        N_MODEL_T* p_noiseModel{ nullptr };
		std::array<T, FrameSize> m_magnitude;
		PROBABILITY m_probability;
		T m_probThreshold{ 0.3 };
		MAGN_T m_frameTotalPower{ 0 };
		MAGN_T m_averagePowerForFreq{ 0 };
		MAGN_T m_powerThreshold{ 0.15 };
		std::array<FrData<T>, N_FREQS_TO_CHECK> m_freqs;

	public:
		bool isArtifDetected(const INPUT_RANGE_T signal);
		void signature(ArtifactSignature<T>& s) const;
		//virtual bool isArtifDetected(const INPUT_RANGE_T signal) override;
		//virtual void signature(ArtifactSignature<T>& s) const override;
		void setFs(T fsHz) { m_fs = fsHz; };
		void setVocalRangeLowFreq(T hz) { m_vocalRangeLowFreq = hz; }
		void setVocalRangeHiFreq(T hz) { m_vocalRangeHiFreq = hz; }
		void setNoiseModel(N_MODEL_T* p) { p_noiseModel = p; }
		void setProbabilityThreshold(T thr) { m_probThreshold = thr; }
		void setPowerThreshold(T thr) { m_powerThreshold = static_cast<MAGN_T>(thr); };
	private:
		PROBABILITY checkFramePowerCriteria();
		PROBABILITY checkNoisePowerCriteria();
		PROBABILITY detectF0();
		//OUTPUT_RANGE_T getVocalRange() const {
		//	return std::span<T>{m_magnitude.cbegin() + m_rangeLoLimit
		//		, m_rangeHiLimit - m_rangeLoLimit + 1 };
		//};
		OUTPUT_RANGE_T getVocalRange() {
			size_t len = m_rangeHiLimit - m_rangeLoLimit + 1;
			return std::span<T>{m_magnitude.data() + m_rangeLoLimit, len };
		};
		void preprocessFrameData(const INPUT_RANGE_T signal);
		T vocalRangeCurrentPower() const;
        size_t freqHz2pos(T freqHz)const { return static_cast<size_t>((freqHz / (m_fs * HalfSpecCoef)) * FrameSize); };
        T pos2freqHz(size_t pos)const { return (m_fs * HalfSpecCoef) * static_cast<T>(pos) / static_cast<T>(FrameSize); };
	};

	template<typename T, size_t FrameSize>
	inline bool VocalDetector<T, FrameSize>::isArtifDetected(const INPUT_RANGE_T signal) {
		m_probability = 0;
		preprocessFrameData(signal);
		m_probability += checkFramePowerCriteria();
		m_probability += checkNoisePowerCriteria();
		m_probability += detectF0();
		return m_probability >= m_probThreshold;
	}

	template<typename T, size_t FrameSize>
	inline void VocalDetector<T, FrameSize>::signature(ArtifactSignature<T>& s) const {
		s.framePower = static_cast<T>(m_frameTotalPower);
		s.probability = static_cast<T>(m_probability);
		size_t freqQty = std::min(s.signature.size(), m_freqs.size());
		std::copy_n(m_freqs.cbegin(), freqQty, s.signature.begin());
		if (s.signature.size() > freqQty) {
			std::fill(s.signature.begin() + freqQty, s.signature.end(), FrData<T>());
			//std::fill(std::begin(s.signature)+freqQty, s.signature.end(), FrData<T>());
		}
	}

	template<typename T, size_t FrameSize>
	inline PROBABILITY VocalDetector<T, FrameSize>::checkFramePowerCriteria() {
		PROBABILITY prob{ 0 };

		// check delta vocal_range_power / average_current_frame_power
		// for general range of same size
		// is it more? than average?

		T vocalRangeAveragePower = m_averagePowerForFreq * (m_rangeHiLimit - m_rangeLoLimit);
		T delta1 = (m_frameVocalRangePower - vocalRangeAveragePower)
			/ vocalRangeAveragePower;
		if (delta1 >= m_powerThreshold) { prob += StepPerCriter; }
		if (delta1 >= (m_powerThreshold * 2)) { prob += StepPerCriter; }

		return prob;
	}

	template<typename T, size_t FrameSize>
	inline PROBABILITY VocalDetector<T, FrameSize>::checkNoisePowerCriteria()
	{
		PROBABILITY prob{ 0 };

		// check delta vocal_range_power / same_range_power
		// for non-vocal frame from noise model
		// (it stores an amplitude, so it should be ^2)
		auto calcElement = [](T sum, T element) {return sum + element * element;  };
		auto noiseInVocalRange = p_noiseModel->getRange(m_rangeHiLimit, m_rangeHiLimit - m_rangeLoLimit + 1);
		MAGN_T vocalRangeNoisePower =
			std::accumulate(noiseInVocalRange.begin(), noiseInVocalRange.end(), T(0), calcElement);
		//std::accumulate(p_noiseModel[rangeLoLimit]
		//, p_noiseModel[rangeHiLimit]
		//, T(0), calcElement);
		T delta2 = m_frameVocalRangePower / vocalRangeNoisePower;
		if (delta2 >= (1 + m_powerThreshold)) { prob += StepPerCriter; };
		if (delta2 >= (1 + m_powerThreshold * 5)) { prob += StepPerCriter; }
		return prob;
	}

	template<typename T, size_t FrameSize>
	inline PROBABILITY VocalDetector<T, FrameSize>::detectF0() {
		PROBABILITY prob{ 0 };

		auto vocalRange = getVocalRange();
		//auto vocalRange= std::span<MAGN_T>{m_magnitude.data() + m_rangeLoLimit, m_rangeHiLimit - m_rangeLoLimit + 1};
		//using ARR_ITER_T = decltype(m_magnitude.begin());
		//ARR_ITER_T startSearchIt = m_magnitude.begin() + rangeLoLimit;
		//ARR_ITER_T finishSearchIt = m_magnitude.begin() + rangeHiLimit;
		//ARR_ITER_T maxPowerInVocalRangeIt =
		auto maxPowerInVocalRangeIt =
			//std::max_element(startSearchIt, finishSearchIt );
			//std::max_element(m_magnitude[rangeLoLimit], m_magnitude[rangeHiLimit]);
			std::max_element(vocalRange.begin(), vocalRange.end());
		//std::cout << **maxPowerInVocalRangeIt << std::endl;
		MAGN_T maxPowerInVocalRange = static_cast<MAGN_T>(*maxPowerInVocalRangeIt);
		T delta1 = (maxPowerInVocalRange - m_averagePowerForFreq) / m_averagePowerForFreq;
		if (delta1 >= m_powerThreshold) {
			prob += StepPerCriter;
            //int pos = std::distance(m_magnitude.begin(), maxPowerInVocalRangeIt);
        }
        int pos =std::distance(vocalRange.begin(), maxPowerInVocalRangeIt);
        pos += m_rangeLoLimit ;
        // qInfo()<<"Freq rng: ["<<m_vocalRangeLowFreq<<".."<<m_vocalRangeHiFreq<<"] ["<<m_rangeLoLimit<<".."<<m_rangeHiLimit<<"] -"<<pos;
        m_freqs[0].freqHz = pos2freqHz(pos);
        m_freqs[0].magn = maxPowerInVocalRange;
        // if (delta1 >= (m_powerThreshold * 2)) { prob += StepPerCriter; }
        // if (delta1 >= (m_powerThreshold * 5)) { prob += StepPerCriter; }

		return prob;
	}

	template<typename T, size_t FrameSize>
	inline void VocalDetector<T, FrameSize>::preprocessFrameData(const INPUT_RANGE_T signal) {
		//#TODO - apply band-pass filter (for vocal range till 1500-2200Hz)
		// in FD to signal before power calculation
		// it may be done in magnitude buffer after copy of signal before it ^2

		m_rangeLoLimit = freqHz2pos(m_vocalRangeLowFreq);
		m_rangeHiLimit = freqHz2pos(m_vocalRangeHiFreq) + 1;

		std::transform(signal.begin(), signal.end(), m_magnitude.begin()
			, [](std::complex<T> val) {return val.real() * val.real(); });
		m_frameTotalPower = std::accumulate(m_magnitude.cbegin(), m_magnitude.cend(), T{ 0 });
		m_averagePowerForFreq = m_frameTotalPower / FrameSize;
		m_frameVocalRangePower = vocalRangeCurrentPower();
        // qInfo()<<"Freq rng: ["<<m_vocalRangeLowFreq<<".."<<m_vocalRangeHiFreq<<"] ["<<m_rangeLoLimit<<".."<<m_rangeHiLimit<<"]"<<" Fs: "<<m_fs;
        // qInfo()<<"Frm power: "<<m_frameTotalPower<<" Avg freq power: "<<m_averagePowerForFreq<<" Vocal power: "<<m_frameVocalRangePower;
	}

	template<typename T, size_t FrameSize>
	inline T VocalDetector<T, FrameSize>::vocalRangeCurrentPower() const {
		//auto vocalRange = std::span<MAGN_T>{ m_magnitude.data() + m_rangeLoLimit, m_rangeHiLimit - m_rangeLoLimit + 1 };
		//MAGN_T vocalRangeCurrentPower = std::accumulate(m_magnitude[rangeLoLimit]
		/*auto result = std::accumulate(m_magnitude[m_rangeLoLimit]
			, m_magnitude[m_rangeHiLimit], T(0));*/
		//OUTPUT_RANGE_T vocalRange{ getVocalRange() };
		//auto result = std::accumulate(vocalRange.begin(), vocalRange.end(), T(0));
		T result{ 0 };
		for (size_t i = m_rangeLoLimit; i < m_rangeHiLimit; ++i) {
			result += m_magnitude[i];
		}
		return result;
	}

}

#endif

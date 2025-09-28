#pragma once
//------------------------------
/* Class Artifact Analizer collects
 *   artifact signatures from Voice Detector
 *   finds continous sequence of vocal frames
 *   belonging to one sentence
 *   provide frame list for
 *   xcorr calculation
 *   operator analize
 */
#ifndef VD_ARTIFANALIZER_H
#define VD_ARTIFANALIZER_H

#include "../include/sp/vocaldetector.h"
#include "vd_coef.h"

namespace artan {

using namespace vdh;
using SDATA_T = float;
const size_t BUFFERSIZE_N = 80;
// const size_t N_FREQS_TO_CHECK = 3;
using SIGNATURE_T = ArtifactSignature<SDATA_T, N_FREQS_TO_CHECK>;
using RESULT_LIST_T = std::vector<long>;

enum class SEQUENCE_STATE { Clear, Started, Interrupted, Continues, Finished };

template <typename T = SDATA_T, size_t BufferSize = BUFFERSIZE_N>
class VD_ArtifactAnalizer {

  std::array<ArtifactSignature<T, N_FREQS_TO_CHECK>, BufferSize> m_sbuf;
  size_t m_currentPos{BufferSize - 1}; // when nothing inserted
  size_t m_inputID{0};                 // when nothing inserted
  size_t m_outputID{0};
  SEQUENCE_STATE m_sequenceIs{SEQUENCE_STATE::Clear};
  size_t m_seqBegin;
  size_t m_seqEnd;

  size_t m_signalTimingThr; // in frames not seconds
  size_t m_pauseTimingThr;  // in frames not seconds
  T m_f0MagnThr;            // level threshold
  T m_f0FreqTolerance;      // f0 frequency deviation
  T m_probabilityThr;
  T m_f0CurrentVal; // initialize when signal started

public:
  void setCoefs(const VD_Coef_Frame &vcf, const AA_Coef_Frame &acf);
  void processSignature(SIGNATURE_T &s);
  bool isSentenceDetected();
  size_t seqID() const { return m_outputID; }
  RESULT_LIST_T getXcorrSeq();
  RESULT_LIST_T getOperSeq();

  void setSignalTimingThr(size_t nFrames) { m_signalTimingThr = nFrames; }
  void setPauseTimingThr(size_t nFrames) { m_pauseTimingThr = nFrames; }
  void setF0MagnThr(T thr) { m_f0MagnThr = thr; };
  void setF0ReqTolerance(T tol) { m_f0FreqTolerance = tol; };
  void setProbabilityThr(T thr) { m_probabilityThr = thr; };

private:
  void switchToNext();
  size_t getNext(size_t current) const;
  size_t getPrevious(size_t current) const;
  void startedCheck();
  void clearCheck();
  void continuesCheck();
  void interruptedCheck();
  void finishedCheck();

  bool isMagnCondition(size_t frameN);
  bool isFreqCondition(size_t frameN);
  bool isProbCondition(size_t frameN);
  bool isPauseTCondition(size_t frameN);
  bool isSignalTCondition(size_t frameN);
};
//---------------------------------------------------------------

template <typename T, size_t BufferSize>
void VD_ArtifactAnalizer<T, BufferSize>::setCoefs(const VD_Coef_Frame &vcf,
                                                  const AA_Coef_Frame &acf) {
  m_probabilityThr = vcf.VD_ProbabilityThreshold;
  m_f0MagnThr = vcf.VD_powerThreshold;
  m_f0FreqTolerance = acf.AA_freqTol;
  m_signalTimingThr = acf.AA_signalTimingThr;
  m_pauseTimingThr = acf.AA_pauseTimingThr;
};

template <typename T, size_t BufferSize>
void VD_ArtifactAnalizer<T, BufferSize>::processSignature(SIGNATURE_T &s) {
  switchToNext();
  m_sbuf[m_currentPos] = s;
  switch (m_sequenceIs) {
  case SEQUENCE_STATE::Clear:
    clearCheck();
    break;
  case SEQUENCE_STATE::Started:
    startedCheck();
    break;
  case SEQUENCE_STATE::Continues:
    continuesCheck();
    break;
  case SEQUENCE_STATE::Interrupted:
    interruptedCheck();
    break;
  case SEQUENCE_STATE::Finished:
    finishedCheck();
    break;
  default:
    break;
  }
};

template <typename T, size_t BufferSize>
bool VD_ArtifactAnalizer<T, BufferSize>::isSentenceDetected() {
  return (m_sequenceIs == SEQUENCE_STATE::Finished);
};

template <typename T, size_t BufferSize>
RESULT_LIST_T VD_ArtifactAnalizer<T, BufferSize>::getXcorrSeq() {};

template <typename T, size_t BufferSize>
RESULT_LIST_T VD_ArtifactAnalizer<T, BufferSize>::getOperSeq() {
  RESULT_LIST_T vec;
  vec.reserve(BufferSize);
  for (size_t i = m_seqBegin; i <= m_seqEnd; i = getNext(i)) {
    vec.push_back(m_sbuf[i].frameID);
  }
  return vec;
};

template <typename T, size_t BufferSize>
void VD_ArtifactAnalizer<T, BufferSize>::switchToNext() {
  ++m_inputID;
  m_currentPos = getNext(m_currentPos);
};

template <typename T, size_t BufferSize>
size_t VD_ArtifactAnalizer<T, BufferSize>::getNext(size_t current) const {
  return (current < (BufferSize - 1)) ? (current + 1) : (0);
};

template <typename T, size_t BufferSize>
size_t VD_ArtifactAnalizer<T, BufferSize>::getPrevious(size_t current) const {
  return (current == 0) ? (BufferSize - 1) : (current - 1);
};

//-------------------------------------------

template <typename T, size_t BufferSize>
void VD_ArtifactAnalizer<T, BufferSize>::startedCheck() {
  bool criteria = isMagnCondition(m_currentPos) &&
                  isProbCondition(m_currentPos) &&
                  isFreqCondition(m_currentPos);
  if (criteria) {
    if (isSignalTCondition(m_currentPos)) {
      m_sequenceIs = SEQUENCE_STATE::Continues;
    } else {
      m_sequenceIs = SEQUENCE_STATE::Clear;
    }
  }
};

template <typename T, size_t BufferSize>
void VD_ArtifactAnalizer<T, BufferSize>::clearCheck() {
  bool criteria =
      isMagnCondition(m_currentPos) && isProbCondition(m_currentPos);
  if (criteria) {
    m_seqBegin = m_currentPos;
    m_sequenceIs = SEQUENCE_STATE::Started;
    m_f0CurrentVal = m_sbuf[m_currentPos].signature[0].freqHz;
  }
};

template <typename T, size_t BufferSize>
void VD_ArtifactAnalizer<T, BufferSize>::continuesCheck() {
  bool criteria = isMagnCondition(m_currentPos) &&
                  isProbCondition(m_currentPos) &&
                  isFreqCondition(m_currentPos);
  if (!criteria) {
    m_seqEnd = m_currentPos;
    m_sequenceIs = SEQUENCE_STATE::Interrupted;
  }
};

template <typename T, size_t BufferSize>
void VD_ArtifactAnalizer<T, BufferSize>::interruptedCheck() {
  bool criteria = !isMagnCondition(m_currentPos);
  if (criteria) {
    if (!isPauseTCondition(m_currentPos)) {
      m_sequenceIs = SEQUENCE_STATE::Finished;
      ++m_outputID;
    }
  } else {
    if (isProbCondition(m_currentPos)) {
      m_sequenceIs = SEQUENCE_STATE::Continues;
    }
  }
};

template <typename T, size_t BufferSize>
void VD_ArtifactAnalizer<T, BufferSize>::finishedCheck() {
  m_sequenceIs = SEQUENCE_STATE::Clear;
  clearCheck();
};

//---------------------------------------------

template <typename T, size_t BufferSize>
bool VD_ArtifactAnalizer<T, BufferSize>::isMagnCondition(size_t frameN) {
  T powerPortion = m_sbuf[frameN].signature[0].magn / m_sbuf[frameN].framePower;
  bool result = (powerPortion >= m_f0MagnThr);
  return (result);
};

template <typename T, size_t BufferSize>
bool VD_ArtifactAnalizer<T, BufferSize>::isFreqCondition(size_t frameN) {
  T deltaFreq =
      abs(m_sbuf[frameN].signature[0].freqHz - m_f0CurrentVal) / m_f0CurrentVal;
  bool result = (deltaFreq <= m_f0FreqTolerance);
  if (result && isProbCondition(frameN)) {
    m_f0CurrentVal = m_sbuf[frameN].signature[0].freqHz;
  }
  return result;
};

template <typename T, size_t BufferSize>
bool VD_ArtifactAnalizer<T, BufferSize>::isProbCondition(size_t frameN) {
  bool result = (m_sbuf[frameN].probability >= m_probabilityThr);
  return result;
};

template <typename T, size_t BufferSize>
bool VD_ArtifactAnalizer<T, BufferSize>::isPauseTCondition(size_t frameN) {
  size_t distanceToStop = frameN - m_seqEnd;
  bool result = (distanceToStop <= m_pauseTimingThr);
  return result;
};

template <typename T, size_t BufferSize>
bool VD_ArtifactAnalizer<T, BufferSize>::isSignalTCondition(size_t frameN) {
  size_t distanceToStart = frameN - m_seqBegin;
  bool result = (distanceToStart >= m_signalTimingThr);
  return result;
};

} // namespace artan
#endif // VD_ARTIFANALIZER_H

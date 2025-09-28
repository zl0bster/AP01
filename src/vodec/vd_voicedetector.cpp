#include "vd_voicedetector.h"
#include "../include/sp/spm.h"

#include "../../../libs/eigen-3.4.0/unsupported/Eigen/FFT"
#include <algorithm>
#include <qDebug>

using namespace vd_detector;
using namespace Eigen;

VD_VoiceDetector::VD_VoiceDetector() { m_vd.setNoiseModel(&m_nmodel); }

bool VD_VoiceDetector::processFile(std::string fileName) {
  // try to open file if error return false
  if (!m_tdInputBuf.loadFile(fileName)) {
    qDebug() << "VD: open file error:" << fileName;
    return false;
  }
  m_processingFileName = fileName;
  m_isArtifactDetected = false;
  // init TD input buffer
  m_fs = m_tdInputBuf.fs();
  m_vd.setFs(m_fs);
  m_tdInputBuf.setWindowSize(TDWindowSize);
  m_tdInputBuf.setHopValue(TDWinHopSize);
  // init FD frame buffer
  m_fdBuf.switchNextFrame();
  // prepare td => fd conversion
  std::vector<VDATA_T> window(1024, 0);
  spm::windowHann(window);
  FFT<float> fft;
  fft.SetFlag(fft.HalfSpectrum);
  std::vector<VDATA_T> fftIn(TDWindowSize, 0);
  std::vector<std::complex<VDATA_T>> fftOut(FDFrameSize, 0);
  // process wav data
  do {
    // apply Hann window to input TD window
    auto signal = m_tdInputBuf.currentWindow();
    for (uint i{0}; i < TDWindowSize; ++i) {
      fftIn[i] = window[i] * signal[i];
    }
    // transform TD window to FD
    fft.fwd(fftOut, fftIn);
    // ------ apply filter to FD data -------

    // copy FD to frame buffer
    auto frame = m_fdBuf.currentFrame();
    std::copy(fftOut.begin(), fftOut.end(), frame.begin());
    // process FD frame by voice detector
    if (!m_vd.isArtifDetected(frame)) {
      m_nmodel.update(frame);
    }
    // give  artifact signature to artifact analizer
    SIGNATURE_T s;
    m_vd.signature(s);
    s.frameID = m_tdInputBuf.currentWindowNumber();
    qInfo()<<s.frameID<<", "<<s.probability<<", "<<s.framePower<<", "<<s.signature[0].freqHz<<", "<<s.signature[0].magn;
    m_aa.processSignature(s);
    // if artifact analizer found finalized sound
    // create td buffer sound and save to wav
    if (m_aa.isSentenceDetected()) {
      generateFileNames();
      m_isArtifactDetected |= true;
      createWAVdata(m_aa.getOperSeq());
      if (!m_tdOutputBuf.readyToSave()) {
        qDebug() << "VD: output file process error";
      } else {
        if (!m_tdOutputBuf.saveFile(m_voiceFileName)) {
          qDebug() << "VD: save file error:" << m_voiceFileName;
        } else {
          qInfo() << "VD: saved file: " << m_voiceFileName;
          if (p_resultSaved != nullptr) {
              p_resultSaved->acceptRESULT(m_voiceFileName); // emit message with filename
          }
        }
      }
    }
  } while (m_tdInputBuf.switchNextWin());
  return true;
}

void VD_VoiceDetector::setupDetector(const VD_Coef_Frame &f) {
  m_vd.setPowerThreshold(f.VD_powerThreshold);
  m_vd.setProbabilityThreshold(f.VD_ProbabilityThreshold);
  m_vd.setVocalRangeLowFreq(f.VD_vocalRangeLoFreq);
  m_vd.setVocalRangeHiFreq(f.VD_vocalRangeHiFreq);
}

void VD_VoiceDetector::setupNoiseMod(const NM_Coef_Frame &f) {
  m_nmodel.setNoiseMixinCoef(f.NM_noiseMixinCoef);
  m_nmodel.setRefreshThreshold(f.NM_weightRefreshThreshold);
  m_nmodel.setScopeParameter(f.NM_scopeLength);
}

void VD_VoiceDetector::setupArtAn(const VD_Coef_Frame &vf,
                                  const AA_Coef_Frame &af) {
  m_aa.setCoefs(vf, af);
}

void VD_VoiceDetector::createWAVdata(RESULT_LIST_T l) {
  // init FFT
  std::vector<VDATA_T> ifftOut(TDWindowSize, 0);
  std::vector<std::complex<VDATA_T>> ifftIn(FDFrameSize, 0);
  FFT<float> fft;
  fft.SetFlag(fft.HalfSpectrum);
  // init TD output buffer
  m_tdOutputBuf.setFs(m_tdInputBuf.fs());
  m_tdOutputBuf.setWindowSize(TDWindowSize);
  m_tdOutputBuf.setHopValue(TDWinHopSize);
  m_tdOutputBuf.initializeBuf(l.size() * TDWinHopSize);
  m_tdOutputBuf.startWindowing();
  // get frames from list, trans'em to TD window and put to output buff
  for (auto id : l) {
    if (m_tdOutputBuf.readyToWindowing()) {
      auto frame = m_fdBuf.frame(id);
      std::copy(frame.begin(), frame.end(), ifftIn.begin());
      fft.inv(ifftOut, ifftIn);
      auto window = m_tdOutputBuf.currentWindow();
      for (uint i{0}; i < TDWindowSize; ++i) {
        window[i] += ifftOut[i];
      }
      m_tdOutputBuf.switchNextWin();
    }
  }
}

void VD_VoiceDetector::generateFileNames() {
    // #todo - remove path from "temp" to provide saving file to project folder
  size_t nameBaseLen = m_processingFileName.size() - 4;
  std::string temp = m_processingFileName.substr(0, nameBaseLen);
  std::string postfix = "_" + std::to_string(m_aa.seqID());
  m_sampleFileName = temp + postfix + "-x.wav"; // XCORR wav
  m_voiceFileName = temp + postfix + ".wav";    // OPERATORS wav
}

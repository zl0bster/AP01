// Class VD_VoiceDetector processes the input file
// using using all subclasses like
//  - ArtAn
//  - VocalDetector
//  - NModel
//  - TDBuffer
//  - FDBuffer
// It saves found artifact to WAV files and end emits filename registration callback
// it can detect multiple artifacts in processed file

#ifndef VD_VOICEDETECTOR_H
#define VD_VOICEDETECTOR_H

// #include <memory>
#include "../include/sp/fdbuffer.h"
#include "../include/sp/n_model.h"
#include "../include/sp/tdbuffer.h"
#include "../include/sp/vocaldetector.h"
#include "../include/callbackproto.h"
#include "vd_artifanalizer.h"
#include "vd_coef.h"
#include <string>

namespace vd_detector {

using namespace tdb;
using namespace fdb;
using namespace nmd;
using namespace vdh;
using namespace artan;

using VDATA_T = float;
const size_t FDFrameSize = 1024;
const size_t TDWindowSize = 2048;
const size_t TDWinHopSize = 1024;
const size_t FDNumFramesInBuf = 128; // ring buf length in frames
// number of frames per second depends on Fs - defined on input file Fs
// to store N = FDNumFramesInBuf/(Fs/TDWinHopSize) - seconds of input signal

class VD_VoiceDetector {
  TDBuffer m_tdInputBuf;
  TDBuffer m_tdOutputBuf;
  N_Model<VDATA_T, FDFrameSize> m_nmodel;
  VocalDetector<VDATA_T, FDFrameSize> m_vd;
  FDBuffer<std::complex<VDATA_T>, FDFrameSize, FDNumFramesInBuf> m_fdBuf;
  VD_ArtifactAnalizer<VDATA_T, FDNumFramesInBuf> m_aa;

  VDATA_T m_fs;
  bool m_isArtifactDetected;
  std::string m_sampleFileName;      // XCORR sample
  std::string m_voiceFileName;       // OPERATOR recognition sample
  std::string m_processingFileName;  // current WAV filename in progress
  CallBackProto* p_resultSaved{nullptr}; // emit message with filename

public:
  VD_VoiceDetector();
  bool processFile(std::string fileName); // true if no errors while open or save
  bool isArtifactDetected() const {
    return m_isArtifactDetected;
  } // true if detected
  void setupDetector(const VD_Coef_Frame &f);
  void setupNoiseMod(const NM_Coef_Frame &f);
  void setupArtAn(const VD_Coef_Frame &vf, const AA_Coef_Frame &af);
  void setResultPtr(CallBackProto * p) { p_resultSaved = p; }

private:
  void createWAVdata(RESULT_LIST_T l);
  void generateFileNames();
};
} // namespace vd_detector
#endif // VD_VOICEDETECTOR_H

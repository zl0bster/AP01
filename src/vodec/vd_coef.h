#ifndef VD_COEF_H
#define VD_COEF_H
#include <QDataStream>
// #include <iostream>

struct VD_Coef_Frame {
  float VD_vocalRangeLoFreq;     // 60 Hz
  float VD_vocalRangeHiFreq;     // 400 Hz
  float VD_ProbabilityThreshold; // 0.3 - main detection threshold
  float VD_powerThreshold;       // 0.15 - general sensitivity coef
};

// std::ostream &operator<<(std::ostream &os, const VD_Coef_Frame &f) {
//   return os << f.VD_vocalRangeLoFreq << ", " << f.VD_vocalRangeHiFreq << ", "
//             << f.VD_powerThreshold << ", " << f.VD_ProbabilityThreshold;
// };

// QDataStream &operator<<(QDataStream &ds, const VD_Coef_Frame &f) {
//   return ds << f.VD_vocalRangeLoFreq << f.VD_vocalRangeHiFreq << f.VD_powerThreshold << f.VD_ProbabilityThreshold;
// }

struct NM_Coef_Frame {
  float NM_scopeLength;            // 10.0 / 5.0 - refresh speed in frames
  float NM_weightRefreshThreshold; // 0.4 - if frame power decreased for this
                                   // coef model will be refreshed
  float NM_noiseMixinCoef;         // 0.01
};

// std::ostream &operator<<(std::ostream &os, const NM_Coef_Frame &f) {
//   return os << f.NM_weightRefreshThreshold << ", " << f.NM_scopeLength << ", "
//             << f.NM_noiseMixinCoef;
// };

// QDataStream &operator<<(QDataStream &ds, const NM_Coef_Frame &f) {
//   return ds << f.NM_weightRefreshThreshold << f.NM_scopeLength <<
//             << f.NM_noiseMixinCoef;
// };

struct AA_Coef_Frame {
  unsigned AA_signalTimingThr; // in frames
  unsigned AA_pauseTimingThr;  // in frames
  float AA_freqTol;            // (0.1) relative part of current val
};

// std::ostream &operator<<(std::ostream &os, const AA_Coef_Frame &f) {
//   return os << f.AA_freqTol << ", " << f.AA_signalTimingThr << ", "
//             << f.AA_pauseTimingThr;
// };
// QDataStream &operator<<(QDataStream &ds, const AA_Coef_Frame &f) {
//   return ds << f.AA_freqTol << f.AA_signalTimingThr
//             << f.AA_pauseTimingThr;
// };
#endif // VD_COEF_H

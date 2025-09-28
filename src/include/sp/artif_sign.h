#pragma once
//----------------------------
/* Artifact Signature description
 * and serializing / deserializing
 * procedures
 * to / from QDataStream
 * to / from string
 */

#ifndef ARTIF_SIGN_H
#define ARTIF_SIGN_H

#include <QDataStream>
#include <array>
// #include <complex>

namespace asa {

const size_t N_FREQS_TO_CHECK = 3;

template <typename T = float> struct FrData {
  T freqHz; //
  T magn;   // magnitude
};

template <typename T = float, size_t FSize = N_FREQS_TO_CHECK>
struct ArtifactSignature {
  long frameID;
  T probability; // [0 ... 1.0]
  T framePower;
  std::array<FrData<T>, FSize> signature;
};

template <typename T>
QDataStream &operator<<(QDataStream &ds, const FrData<T> &f) {
    return ds << f.freqHz << f.magn;
};

template <typename T>
QDataStream &operator>>(QDataStream &ds, const FrData<T> &f) {
    ds >> f.freqHz;
    ds >> f.magn;
    return ds;
};

template <typename T, size_t FSize>
QDataStream &operator<<(QDataStream &ds,
                        const ArtifactSignature<T, FSize> &as) {
  return ds << as.frameID << as.probability << as.framePower << as.signature[0]
            << as.signature[1];
};

template <typename T, size_t FSize>
QDataStream &operator>>(QDataStream &ds,
                        const ArtifactSignature<T, FSize> &as) {
  ds >> as.frameID;
  ds >> as.probability;
  ds >> as.framePower;
  ds >> as.signature[0];
  ds >> as.signature[1];
  return ds;
};
} // namespace asa
#endif // ARTIF_SIGN_H

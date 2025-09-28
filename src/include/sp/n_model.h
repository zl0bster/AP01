#pragma once
//-----------------------------------
/* Class Noise Model is used for nise reduction in signal processing unit
 * it contains the NOISE_MODEL which is build by frames without speech artifacts
 * noise amplitude frame is subtracted of signal frame by demand to reduce the
 * noise
 */
//-----------------------------------

#ifndef N_MODEL
#define N_MODEL

#include <array>
#include <cassert>
#include <complex>
#include <span>
// #include <algorithm>
#include <numeric>

namespace nmd {

template <typename T, size_t FrameSize> class N_Model {
  using COMPLEX_T = std::complex<T>;
  using INPUT_RANGE_T = std::span<COMPLEX_T>;
  using OUTPUT_RANGE_T = std::span<T>;

  std::array<T, FrameSize> m_buf; // noise model buffer
  bool m_empty{true};
  size_t m_frameCounter{0};
  T m_scopeLength; // used for update speed control
  T m_weightRefreshThreshold{0.4}; // threshold to
    //  update noise model element urgent if level decreased
  T m_noiseMixinCoef{0.01}; // if noise subtraction result is negative
  T m_meanSpectralPower;    // #TODO to be used in model update
  T m_currentFramePower;    // #TODO to be used in model update
public:
  N_Model(size_t windowSize = 10) : m_scopeLength(static_cast<T>(windowSize)){};
  void setScopeParameter(size_t windowSize) {
    m_scopeLength = static_cast<T>(windowSize);
  }
  void setScopeParameter(T windowSize) { m_scopeLength = windowSize; }
  void setRefreshThreshold(T threshold) {
    m_weightRefreshThreshold = threshold;
  }
  void setNoiseMixinCoef(T coef) { m_noiseMixinCoef = coef; }

  void refresh() { m_empty = true; }
  void update(const INPUT_RANGE_T signal);
  void denoise(INPUT_RANGE_T signal);
  T operator[](size_t index) const { return m_buf[index]; }
  T spectralPower() const {return m_currentFramePower;}
  OUTPUT_RANGE_T getRange(size_t pos, size_t len) {
    return std::span<T>{m_buf.data() + pos, len};
  };

private:
  T spectralRangePower(const INPUT_RANGE_T signal) const;
  T updateElement(T noise, T signal);
  T updateElement(T noise, T signal, T weight);
  T denoiseElement(T noise, T signal);
};

template <typename T, size_t FrameSize>
inline void N_Model<T, FrameSize>::update(const INPUT_RANGE_T signal) {
  assert(signal.size() == FrameSize);
  m_currentFramePower = spectralRangePower(signal);
  ++m_frameCounter;

  // #TODO if current frame power is less than mean frame power before
  //  noise model should be updated next frame

  if (m_empty) {
    // copy signal to noise buffer
    size_t pos{0};
    for (const auto &cell : signal) {
      m_buf[pos] = cell.real();
      ++pos;
    }
    m_empty = false;
    return;
  }
  if (m_frameCounter < m_scopeLength) {
    // buffer update strategy in the beginning
    size_t pos{0};
    for (const auto &cell : signal) {
      m_buf[pos] = updateElement(m_buf[pos], cell.real(),
                                 static_cast<T>(m_frameCounter));
      ++pos;
    }
    return;
  } else {
    // stationary buffer update strategy
    size_t pos{0};
    for (const auto &cell : signal) {
      m_buf[pos] = updateElement(m_buf[pos], cell.real());
      ++pos;
    }
  }
  return;
}

template <typename T, size_t FrameSize>
inline void N_Model<T, FrameSize>::denoise(INPUT_RANGE_T signal) {
  assert(m_frameCounter > 0);
  assert(signal.size() == FrameSize);
  size_t pos{0};
  for (auto &cell : signal) {
    cell.real(denoiseElement(m_buf[pos], cell.real()));
    ++pos;
  }
}

template <typename T, size_t FrameSize>
inline T
N_Model<T, FrameSize>::spectralRangePower(const INPUT_RANGE_T signal) const {
  auto calcElement = [](COMPLEX_T sum, COMPLEX_T element) {
    return sum + element * element;
  };
  COMPLEX_T val =
      std::accumulate(signal.begin(), signal.end(), COMPLEX_T(0), calcElement);
  return val.real();
}

template <typename T, size_t FrameSize>
inline T N_Model<T, FrameSize>::updateElement(T noise, T signal) {
  T noise_delta_coef = (signal - noise) / noise;
  bool noise_became_much_lower =
      (noise_delta_coef < 0) &&
      (abs(noise_delta_coef) > m_weightRefreshThreshold);
  if (noise_became_much_lower) {
    return signal;
  }
  return updateElement(noise, signal, m_scopeLength);
}

template <typename T, size_t FrameSize>
inline T N_Model<T, FrameSize>::updateElement(T noise, T signal, T weight) {
  T result = (noise * (weight - 1) + signal) / weight;
  return result;
}

template <typename T, size_t FrameSize>
inline T N_Model<T, FrameSize>::denoiseElement(T noise, T signal) {
  T result = signal - noise;
  if (result > 0)
    return result;
  return signal * m_noiseMixinCoef;
}
} // namespace nmd
#endif // N_MODEL

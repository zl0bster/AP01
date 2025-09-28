#pragma once
//-----------------------------------
/*Class TimeDomainBuffer may be used to
 * -1 read data from file and disassemble them to windows
 * -2 assemble new sound and save to file
 * if data read from WAV file it means Fs is defined in file metadata
 * if data collected from soft then Fs should be predefined
 */
//-----------------------------------

#ifndef TDBUFFER_H
#define TDBUFFER_H

#include "../../import/AudioFile.h"
#include <string>
#include <span>

namespace tdb {
// const size_t _WindowSize = 2048;
// const size_t TDWindowSize = 2048;
const size_t _BitDepth = 32; // means float
const size_t _NumChan = 1;
using TDWindow = std::span<float>;
// using TDWindow = std::span<float, TDWindowSize>;

class TDBuffer {
public:
  TDBuffer();
  //------- setup windowing parameters
  void setHopValue(int val) { m_hopValue = val; }; // window overlapping
  int windowSize() const { return m_windowSize; };
  void setWindowSize(size_t size) { m_windowSize = size; };
  int fs() const { return m_buf.getSampleRate(); };   // frequency in Hz
  void setFs(int fs) { m_buf.setSampleRate(fs); }; // frequency in Hz
  int bufSize() const { return m_buf.getNumSamplesPerChannel(); }

  void initializeBuf(int sizeFloats);
  // if going to assembly TD and save file (fill by zeroes)

  bool readyToWindowing() const; // if data loaded or buf initialized
  bool readyToSave() const;
  //------ file operations ---
  void setFileName(const std::string filename) { m_fileName = filename; };
  bool loadFile(const std::string filename = "");
  bool saveFile(const std::string filename = "");
  //------ window operation ---
  bool startWindowing();
  int currentWindowNumber() const { return m_currentWindowNumber; };
  TDWindow currentWindow() ;
  // std::span<float> currentWindow() const;
  // std::span<float,TDWindowSize> currentWindow();
  // std::span<float,2048> currentWindow();
  bool switchNextWin();

private:
  std::string m_fileName;
  int m_hopValue;
  size_t m_windowSize;
  AudioFile<float> m_buf;
  int m_currentWindowNumber;
  bool m_dataLoaded;
  bool m_dataProcessed;
  bool m_bufInitialized;

  void setWavHeader();
};
} // namespace tdb
#endif // TDBUFFER_H

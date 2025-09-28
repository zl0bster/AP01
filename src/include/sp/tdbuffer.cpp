//#include "../include/sp/tdbuffer.h"
#include "tdbuffer.h"
//#include "qdebug.h"

tdb::TDBuffer::TDBuffer() {
  m_bufInitialized = false;
  m_dataLoaded = false;
  m_dataProcessed = false;
}

void tdb::TDBuffer::initializeBuf(int sizeFloats) {
  // int numWindows = sizeFloats/m_windowSize;
  // int sizeTmp = ((sizeFloats%m_windowSize)>0) ? (numWindows+1)*m_windowSize :
  // sizeFloats; m_buf.setAudioBufferSize(_NumChan,sizeTmp);
  m_buf.setBitDepth(_BitDepth);
  m_buf.setAudioBufferSize(_NumChan, sizeFloats);
  m_bufInitialized = m_buf.samples.data()->size() > m_windowSize;
  m_dataProcessed = false;
  m_dataLoaded = false;
  //qInfo() << "TDBuffer initialized. Size: " << sizeFloats;
}

bool tdb::TDBuffer::readyToWindowing() const {
  bool ready = m_dataLoaded || m_bufInitialized;
  return ready;
}

bool tdb::TDBuffer::readyToSave() const {
  bool ready = m_dataProcessed && m_bufInitialized;
  return ready;
}

bool tdb::TDBuffer::loadFile(const std::string filename) {
  if (!filename.empty()) {
    m_fileName = filename;
  }
  bool result = m_buf.load(m_fileName);
  if (result) {
    m_dataLoaded = true;
    m_buf.printSummary();
  }
  return result;
}

bool tdb::TDBuffer::saveFile(const std::string filename) {
  if (!filename.empty()) {
    m_fileName = filename;
  }
  setWavHeader();
  m_buf.printSummary();
  return m_buf.save(m_fileName, AudioFileFormat::Wave);
}

bool tdb::TDBuffer::startWindowing() {
  m_currentWindowNumber = 0;
  return m_bufInitialized||m_dataLoaded;
}

tdb::TDWindow tdb::TDBuffer::currentWindow()  {
  int pos = m_currentWindowNumber * m_hopValue;
  TDWindow tmp{m_buf.samples.data()->data() + pos, m_windowSize};
  m_dataProcessed = true;
  return tmp;
}

bool tdb::TDBuffer::switchNextWin() {
  // check hop is valid then hop else return false
  int nextPos = (m_currentWindowNumber + 1) * m_hopValue;
  if ((nextPos >= 0) && (nextPos < m_buf.samples.data()->size())) {
    ++m_currentWindowNumber;
    return true;
  }
  return false;
}

void tdb::TDBuffer::setWavHeader() {
  m_buf.setNumChannels(_NumChan);
  m_buf.setBitDepth(_BitDepth);
}

#ifndef SAVEWAV_H
#define SAVEWAV_H

// #include <QAudioFormat>
#include "a_codelist.h"
#include "data_structures.h"
#include <QFile>
#include <QObject>
#include <QThread>

//-------------------------------------------------------------------
struct WAVFILEHEADER {
  char RiffName[4];              // Заголовок RIFF
  unsigned long nRiffLength;     // RIFF Chunk Size
  char WavName[4];               // WAVE Header
  char FmtName[4];               // FMT header
  unsigned long nFmtLength;      // Size of the fmt chunk
  unsigned short nAudioFormat;   // Audio format 1=PCM,6=mulaw,7=alaw, 257=IBM,
                                 // Mu-Law, 258=IBM A-Law, 259=ADPCM
  unsigned short nChannelNumber; // Number of channels 1=Mono 2=Stereo
  unsigned long nSampleRate;     // Sampling Frequency in Hz
  unsigned long nBytesPerSecond; // bytes per second
  unsigned short blockAlign;     // 2=16-bit mono, 4=16-bit stereo
  unsigned short nBitsPerSample; // Sample quality 8-bit 16-bit 32-bit
  char DATANAME[4]; // Заголовок блока в блоке данных
  unsigned long nDataLength; // DATA chunk length
};

//-------------------------------------------------------------------
namespace WR {
constexpr unsigned short numChannels = 1;
constexpr unsigned short audioFormat = 1; // означает кодировку по PCM;
} // namespace WR
//-------------------------------------------------------------------

class SaveWAV : public QObject {
  Q_OBJECT
public:
  explicit SaveWAV(QObject *parent = nullptr,
                   WavFileData const *data = nullptr);
  virtual ~SaveWAV() = default;
  void setWAVData(WavFileData const *data = nullptr) { setMembers(data); };

public slots:
  void slotSaveFile(
      const QString fName,
      const QByteArrayView *buf = nullptr); // if filename is empty - execute
                                            // with current settings
  void setResultPath(const QString &p) { m_resultPath = p; }

signals:
  void signalStatus(ASR_statusCode, QString);

private:
  WAVFILEHEADER m_wh;
  QString m_fileName;
  QString m_resultPath;
  //  fileName is used only in 1 current saving session for report messages
  //  it is never used repeatedly

private:
  void setMembers(WavFileData const *data);
  void sendStatus(ASR_statusCode s, QString line = "");
  void printSummary() const;
};

#endif // SAVEWAV_H

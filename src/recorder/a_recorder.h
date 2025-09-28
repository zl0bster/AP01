#ifndef A_RECORDER_H
#define A_RECORDER_H

#include <QIODevice>
#include <QObject>
#include <QString>

#include <QAudioDevice>
#include <QAudioFormat>
#include <QAudioInput>
#include <QtMultimedia/QAudioSource>
#include <QtMultimedia/QMediaDevices>

#include "a_buffer.h"
#include "../include/a_codelist.h"

//-------------------------------------------------------------------
namespace WRR {
constexpr unsigned short numChannels = 1;
constexpr QAudioFormat::ChannelConfig channelConf =
    QAudioFormat::ChannelConfigMono;
} // namespace WRR
//-------------------------------------------------------------------

class A_Recorder : public QObject {
  Q_OBJECT
public:
  explicit A_Recorder(QObject *parent = nullptr,
                      A_Buffer *buffer_manager = nullptr);
  ~A_Recorder();
  // all next is prohibited when recording is started
  // and should be done BEFORE start recording
  bool setSampleRate(uint32_t newSampleRate);
  bool setSource(uint32_t numberInList);
  bool setFormat(uint32_t bytesPerSample);

signals:
  void signalStatus(ASR_statusCode, QString);

public slots:
  void slotStart();
  void slotStop();
  void slotStatusCheck(); // to emit signalOperationStatus

private:
  QAudioFormat m_aFormat;
  QAudioDevice m_aDevice;
  QAudioSource *m_aSource = nullptr;
  uint32_t m_sampleRate;
  bool m_isRecording{false};
  A_Buffer *m_buf = nullptr;
  QAudioFormat::SampleFormat m_sampleFormat;

private:
  void setAFormat();
};

#endif // A_RECORDER_H

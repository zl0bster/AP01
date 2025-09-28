#include "a_recorder.h"
#include <QDebug>
// #include <iostream>

A_Recorder::A_Recorder(QObject *parent, A_Buffer *buffer_manager) {
  if (parent)
    setParent(parent);
  assert(buffer_manager != nullptr);
  m_buf = buffer_manager;
}

A_Recorder::~A_Recorder() {
  m_aSource->stop();
  delete m_aSource;
  m_aSource = nullptr;
}

void A_Recorder::slotStart() {

  assert(!m_aDevice.isNull());
  assert(m_buf);
  m_isRecording = true;
  setAFormat();
  if (!m_aDevice.isFormatSupported(m_aFormat)) {
    QString warning = "ERROR! Format is not supported by Audio device: ";
    warning.append(m_aDevice.description());
    qWarning() << warning;
    emit signalStatus(ASR_statusCode::Format_unsupported, warning);
    return;
  } else {
    qInfo() << "connecting to " << m_aDevice.description();
  }
  m_aSource = new QAudioSource(m_aDevice, m_aFormat);
  qInfo() << "bytesPerSample = " << m_aFormat.bytesPerSample();
  // m_aSource->setBufferSize(SOURCE_BUF_SIZE);
  m_buf->open(QIODevice::WriteOnly);
  qInfo() << "sors io state = " << m_aSource->state()
          << m_aSource->bufferSize();
  qInfo() << "buf  io state = " << m_buf->isOpen()
          << m_buf->openMode();
  m_aSource->start(m_buf); // audio source writes directly to buffer
  qInfo() << "sors io state = " << m_aSource->state()
          << m_aSource->bufferSize();
}

void A_Recorder::slotStop() {
  m_isRecording = false;
  m_aSource->stop();
  m_buf->close();
  delete m_aSource;
  m_aSource = nullptr;
}

void A_Recorder::slotStatusCheck() {
  ASR_statusCode s = ASR_statusCode::Status_ok;
  qInfo() << "status requested:" << m_aSource->state();
  QString warning;
  switch (m_aSource->error()) {
  case QAudio::NoError:
    break;
  default:
    s = ASR_statusCode::Device_error;
    warning.append(m_aDevice.description());
    qDebug() << m_aSource->error() << m_aDevice.description();
    break;
  }
  emit signalStatus(s, warning);
}

bool A_Recorder::setSampleRate(
    uint32_t newSampleRate) { // should be done BEFORE start recording
  if (m_isRecording)
    return false;
  m_sampleRate = newSampleRate;
  return true;
}

bool A_Recorder::setSource(
    uint32_t numberInList) { // should be done BEFORE start recording
  if (m_isRecording)
    return false;
  const auto deviceInfo = QMediaDevices::audioInputs();
  m_aDevice = deviceInfo[numberInList];
  if (m_aDevice.isNull())
    return false;
  return true;
}

bool A_Recorder::setFormat(uint32_t bytesPerSample) {
  if (m_isRecording)
    return false;
  switch (bytesPerSample) {
  case 2:
    m_sampleFormat = QAudioFormat::Int16;
    break;
  case 3:
    m_sampleFormat = QAudioFormat::Float;
    break;
  case 4:
    m_sampleFormat = QAudioFormat::Float;
    break;
  default:
    break;
  }
  return true;
}

void A_Recorder::setAFormat() {
  m_aFormat.setSampleRate(m_sampleRate);
  m_aFormat.setChannelCount(WRR::numChannels);
  m_aFormat.setSampleFormat(m_sampleFormat);
  m_aFormat.setChannelConfig(WRR::channelConf);
}

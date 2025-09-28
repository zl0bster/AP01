#include "savewav.h"
// #include <iostream>

// SaveWAV::SaveWAV(QObject *parent, AWAVData const *data) {
//   QObject::setParent(parent);
//   // assert(data != nullptr);

//   // Initialize file header
//   qstrcpy(m_wh.RiffName, "RIFF");
//   qstrcpy(m_wh.WavName, "WAVE");
//   qstrcpy(m_wh.FmtName, "fmt ");
//   qstrcpy(m_wh.DATANAME, "data");

//   m_wh.nFmtLength = 16; // Указывает длину блока FMT
//   m_wh.nAudioFormat = WR::audioFormat; // означает кодировку по PCM;
//   m_wh.nChannelNumber = WR::numChannels;

//   // Два значения nBytesPerSample и nBytesPerSecond рассчитываются заданными
//   // параметрами; Блок выравнивания блока данных (количество байтов,
//   необходимых
//   // для каждой выборки = количество каналов × количество битов выборки
//   данных
//   // на выборку / 8)

//   if (data)
//     setMembers(data);
// }

SaveWAV::SaveWAV(QObject *parent, const WavFileData *data) {
  QObject::setParent(parent);
  // assert(data != nullptr);

  // Initialize file header
  qstrcpy(m_wh.RiffName, "RIFF");
  qstrcpy(m_wh.WavName, "WAVE");
  qstrcpy(m_wh.FmtName, "fmt ");
  qstrcpy(m_wh.DATANAME, "data");

  m_wh.nFmtLength = 16; // Указывает длину блока FMT
  m_wh.nAudioFormat = WR::audioFormat; // означает кодировку по PCM;
  m_wh.nChannelNumber = WR::numChannels;

  // Два значения nBytesPerSample и nBytesPerSecond рассчитываются заданными
  // параметрами; Блок выравнивания блока данных (количество байтов, необходимых
  // для каждой выборки = количество каналов × количество битов выборки данных
  // на выборку / 8)

  if (data)
    setMembers(data);
}

void SaveWAV::slotSaveFile(const QString fName, const QByteArrayView *buf) {
  assert(buf != nullptr);
  if (!fName.isEmpty()) {
    m_fileName = fName;
  }
  if (buf->isEmpty()) {
    sendStatus(ASR_statusCode::Buffer_is_empty);
    qDebug() << "SAVER: empty buffer";
    return;
  }
  int headerSize = sizeof(m_wh);
  m_wh.nDataLength = static_cast<unsigned long>(buf->size());
  m_wh.nRiffLength = static_cast<unsigned long>(buf->size() + headerSize);
  QString symb, pathAndName;
  if (!m_resultPath.isEmpty()) {
    if (!m_resultPath.endsWith("/"))
      symb = "/";
  }
  pathAndName = m_resultPath + symb + m_fileName;
  qInfo()<<"Result filename: "<< pathAndName;
  QFile wavFile(pathAndName);
  if (!wavFile.open(QIODevice::WriteOnly)) {
    sendStatus(ASR_statusCode::File_open_error);
    qDebug() << "SAVER: cannt create file";
    return;
  }
  // printSummary();
  wavFile.write((const char *)&m_wh, headerSize);
  wavFile.write((const char *)buf->constData(), buf->size());
  wavFile.close();
  qInfo() << "SAVER: file saved: " << m_fileName;
  printSummary();
  sendStatus(ASR_statusCode::File_saved);
}

// Скорость передачи данных осциллограммы
// (Среднее количество байтов в секунду = частота выборки × количество каналов
// × количество бит данных выборки на выборку / 8 = частота выборки ×
// количество байтов, необходимых для каждой выборки)

void SaveWAV::setMembers(const WavFileData *data) {
  m_wh.blockAlign = data->bytesPerSample * m_wh.nChannelNumber;
  m_wh.nBitsPerSample = data->bytesPerSample * m_wh.nChannelNumber * 8;
  // Количество бит данных выборки на выборку;
  m_wh.nSampleRate = data->sampleRate; // Частота дискретизации;
  m_wh.nBytesPerSecond =
      data->bytesPerSample * data->sampleRate * m_wh.nChannelNumber;
  m_resultPath = data->resultPath;
  m_fileName = data->fileName;
}

void SaveWAV::sendStatus(ASR_statusCode s, QString line) {
  if (s == ASR_statusCode::File_saved)
    line.append(m_fileName);
  emit signalStatus(s, line);
}

void SaveWAV::printSummary() const {
  qInfo() << "|======================================|";
  qInfo() << "| Num Channels: " << m_wh.nChannelNumber;
  qInfo() << "| Num Samples Per Channel: "
           << m_wh.nRiffLength / m_wh.nChannelNumber;
  qInfo() << "| Sample Rate: " << m_wh.nSampleRate;
  qInfo() << "| Bit Depth:  " << m_wh.nBitsPerSample;
  qInfo() << "| Length in Seconds: "
           << (double)m_wh.nRiffLength / (double)m_wh.nSampleRate /
                  (double)m_wh.nChannelNumber;
  qInfo() << "|======================================|";
}

#include "psg_init.h"
#include "../../import/ini.h"
#include <QCommandLineParser>

PSG_Init::PSG_Init(QObject *parent, QStringList args) : InitUnit{parent, args} {
  p_fileData = QSharedPointer<WavFileData>(new WavFileData);
  p_genData = QSharedPointer<PSGenData>(new PSGenData);
}

void PSG_Init::readIniFile(QString iniFileName) {
  InitUnit::readIniFile(iniFileName);
  std::string fileName{iniFileName.toStdString()};
  ini::File configFile;
  try {
    qInfo() << fileName;
    configFile = ini::open(iniFileName.toStdString());
  } catch (const std::invalid_argument &e) {
    qDebug() << e.what();
  }
  std::string tmpStr;
  size_t uintVal;

  // ini file reading for file data and generator settings

  tmpStr = configFile["FileConfig"]["fileName"];
  p_fileData->fileName = QString::fromStdString(tmpStr);
  tmpStr = configFile["FileConfig"]["resultPath"];
  p_fileData->resultPath = QString::fromStdString(tmpStr);
  uintVal = configFile["FileConfig"].get<size_t>("sampleRate");
  p_fileData->sampleRate = static_cast<uint32_t>(uintVal);
  uintVal = configFile["FileConfig"].get<size_t>("bytesPerSample");
  p_fileData->bytesPerSample = static_cast<uint32_t>(uintVal);
  qInfo() << p_fileData->resultPath << p_fileData->fileName
          << p_fileData->sampleRate << p_fileData->bytesPerSample;

  uintVal = configFile["GenConfig"].get<size_t>("freq1");
  p_genData->freq1 = static_cast<uint32_t>(uintVal);
  uintVal = configFile["GenConfig"].get<size_t>("freq2");
  p_genData->freq2 = static_cast<uint32_t>(uintVal);
  uintVal = configFile["GenConfig"].get<size_t>("freq3");
  p_genData->freq3 = static_cast<uint32_t>(uintVal);
  uintVal = configFile["GenConfig"].get<size_t>("durationMSec");
  p_genData->durationMSec = static_cast<uint32_t>(uintVal);
  qInfo() << p_genData->durationMSec << p_genData->freq1 << p_genData->freq2
          << p_genData->freq3;
}

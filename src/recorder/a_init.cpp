#include "a_init.h"
#include "../../import/ini.h"
#include <QCommandLineParser>

A_Init::A_Init(QObject *parent, QStringList args) : QObject{parent} {
  QCommandLineParser parser;
  const QCommandLineOption mode({"s", "start"}, "Start record immediately.");
  const QCommandLineOption file({"f", "file"}, "Config file name.", "fileName"/*,
                                "C:/VP_WORKS/PROJECTS_CPP/P002/areader.ini"*/);
  //                                "C:/VP_WORKS/PROJECTS_CPP/P002/areader.ini");
  parser.addOptions({mode, file});
  parser.process(args);
  iniFile = parser.value(file);
  aPaused = !parser.isSet(mode);
  p_linkData = QSharedPointer<ALinkData>(new ALinkData);
  p_recData = QSharedPointer<ARecData>(new ARecData);
}

void A_Init::initialize() { readIniFile(); }

bool A_Init::isPaused() const { return aPaused; }

void A_Init::readIniFile() {
  std::string fileName{iniFile.toStdString()};
  ini::File configFile;
  try {
    qDebug() << fileName;
    configFile = ini::open(iniFile.toStdString());
  } catch (const std::invalid_argument &e) {
    qDebug() << e.what();
  }
  std::string tmpStr = configFile["LinkConfig"]["ownAddr"];
  qDebug() << tmpStr;
  p_linkData->ownAddr = QHostAddress(QString::fromStdString(tmpStr));
  size_t uintVal = configFile["LinkConfig"].get<size_t>("ownPort");
  qDebug() << uintVal;
  p_linkData->ownPort = static_cast<quint16>(uintVal);
  tmpStr = configFile["LinkConfig"]["dispAddr"];
  qDebug() << tmpStr;
  p_linkData->dispAddr = QHostAddress(QString::fromStdString(tmpStr));
  uintVal = configFile["LinkConfig"].get<size_t>("dispPort");
  qDebug() << uintVal;
  p_linkData->dispPort = static_cast<quint16>(uintVal);
  p_linkData->unitID = configFile["LinkConfig"].get<size_t>("unitID");
  qDebug() << p_linkData->dispPort << p_linkData->dispAddr
           << p_linkData->ownPort << p_linkData->ownAddr << p_linkData->unitID;

  tmpStr = configFile["RecConfig"]["projectName"];
  p_recData->projectName = QString::fromStdString(tmpStr);
  tmpStr = configFile["RecConfig"]["chanID"];
  p_recData->chanID = QString::fromStdString(tmpStr);
  tmpStr = configFile["RecConfig"]["resultPath"];
  p_recData->resultPath = QString::fromStdString(tmpStr);
  uintVal = configFile["RecConfig"].get<size_t>("sorsID");
  p_recData->sorsID = uintVal;
  uintVal = configFile["RecConfig"].get<size_t>("freqS");
  p_recData->freqS = uintVal;
  uintVal = configFile["RecConfig"].get<size_t>("recDurationSec");
  p_recData->recDurationSec = uintVal;
  uintVal = configFile["RecConfig"].get<size_t>("chunkSec");
  p_recData->chunkSec = uintVal;
  uintVal = configFile["RecConfig"].get<size_t>("bytesPerSample");
  p_recData->bytesPerSample = uintVal;
  qDebug() << p_recData->projectName << p_recData->chanID << p_recData->sorsID
           << p_recData->freqS << p_recData->recDurationSec
           << p_recData->chunkSec;
}

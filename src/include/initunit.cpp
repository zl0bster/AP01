#include "initunit.h"
#include "../../import/ini.h"
#include <QCommandLineParser>

InitUnit::InitUnit(QObject *parent, QStringList args)
    : QObject{parent}, m_args(args) {
  p_linkData = QSharedPointer<ALinkData>(new ALinkData);
}

void InitUnit::initialize() {
  parseArgs(m_args);
  readIniFile(m_iniFile);
}

void InitUnit::readIniFile(QString iniFileName) {
  std::string fileName{iniFileName.toStdString()};
  ini::File configFile;
  try {
    qInfo() << fileName;
    configFile = ini::open(iniFileName.toStdString());
  } catch (const std::invalid_argument &e) {
    qDebug() << e.what();
  }
  std::string tmpStr = configFile["LinkConfig"]["ownAddr"];
  p_linkData->ownAddr = QHostAddress(QString::fromStdString(tmpStr));
  size_t uintVal = configFile["LinkConfig"].get<size_t>("ownPort");
  p_linkData->ownPort = static_cast<quint16>(uintVal);
  tmpStr = configFile["LinkConfig"]["dispAddr"];
  p_linkData->dispAddr = QHostAddress(QString::fromStdString(tmpStr));
  uintVal = configFile["LinkConfig"].get<size_t>("dispPort");
  p_linkData->dispPort = static_cast<quint16>(uintVal);
  p_linkData->unitID = configFile["LinkConfig"].get<size_t>("unitID");
  qInfo() << p_linkData->dispPort << p_linkData->dispAddr << p_linkData->ownPort
          << p_linkData->ownAddr << p_linkData->unitID;
}

void InitUnit::parseArgs(QStringList args) {
  qInfo() << args;
  QCommandLineParser parser;
  const QCommandLineOption mode({"s", "start"}, "Run at start.");
  const QCommandLineOption testm({"t", "test"}, "Run test mode.");
  const QCommandLineOption file({"f", "file"}, "Config file name.", "fileName");
  parser.addOptions({file});
  parser.addOptions({testm});
  parser.addOptions({mode});
  parser.process(args);
  qInfo()<< "INIT parser not recognized: " << parser.unknownOptionNames();
  qInfo()<< "INIT parser has recognized: " << parser.optionNames();
  m_iniFile = parser.value(file);
  m_paused = !parser.isSet(mode);
  m_testModeOn = parser.isSet(testm);
}

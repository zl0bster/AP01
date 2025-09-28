#include "bm_hfactory.h"
#include "../../import/ini.h"
#include "../include/data_structures.h"
#include <QHostAddress>
#include <QSharedPointer>

BM_hFactory::BM_hFactory(QObject *parent) : QObject{parent} {}

void BM_hFactory::initRHFactory(QString iniFile) {
  std::string fileName{iniFile.toStdString()};
  ini::File configFile;
  try {
    configFile = ini::open(fileName);

  } catch (const std::invalid_argument &e) {
    qDebug() << e.what();
  }
  qInfo() << "HFACTORY: processing " << fileName;

  std::string tmpStr = configFile["LinkConfig"]["hostIP"];
  m_HostIP = QString::fromStdString(tmpStr);
  qInfo() << "hostIP:" << m_HostIP;
  size_t uintVal = configFile["LinkConfig"].get<size_t>("hostPort");
  qInfo() << "hostPort:" << uintVal;
  m_HostPortCounter = static_cast<uint16_t>(uintVal);
  tmpStr = configFile["LinkConfig"]["unitIP"];
  m_UnitIP = QString::fromStdString(tmpStr);
  qInfo() << "unitIP:" << m_UnitIP;
  uintVal = configFile["LinkConfig"].get<size_t>("unitPort");
  qInfo() << "unitPort:" << uintVal;
  m_UnitPortCounter = static_cast<uint16_t>(uintVal);
  m_IDCounter = configFile["LinkConfig"].get<size_t>("unitID");
  qInfo() << "====== Factory link config done ======";

  // read INI sections in cycle to fill m_unitDescript

  size_t sectionCount = 0;
  while (true) {
    std::string sectionName = "Unit";
    sectionName.append(std::to_string(sectionCount));
    qInfo() << "try read section:" << sectionName;
    if (!configFile.has_section(sectionName)) {
      qInfo() << "NO section:" << sectionName;
      break;
    }
    BM_HandlerTypeDescription descr;
    size_t unitTypeCode = configFile[sectionName].get<size_t>("unitType");
    tmpStr = configFile[sectionName]["unitTypeName"];
    qInfo() << "unitTypeName:" << tmpStr;
    descr.unitTypeName = QString::fromStdString(tmpStr);
    tmpStr = configFile[sectionName]["unitPGMname"];
    qInfo() << "unitPGMname:" << tmpStr;
    descr.unitPGMname = QString::fromStdString(tmpStr);
    tmpStr = configFile[sectionName]["unitPGMpath"];
    qInfo() << "unitPGMpath:" << tmpStr;
    descr.unitPGMpath = QString::fromStdString(tmpStr);
    m_unitDescript[static_cast<BM_HandlerType>(unitTypeCode)] = descr;
    qInfo() << "====== " << sectionName << " config done ======";
    ++sectionCount;
  }
  isConfigured = true;
}

BM_HandlerRecorder *BM_hFactory::getRecHandler() {
  if (!isConfigured) {
    qDebug() << "HFACTORY: not cofigured";
    return nullptr;
  }
  qInfo() << "config started";
  BM_HandlerRecorder *p_handler = new BM_HandlerRecorder(this->parent());
  QSharedPointer<ALinkData> p_link = QSharedPointer<ALinkData>(new ALinkData);
  fillHandlerData(BM_HandlerType::Recorder, p_handler, p_link);
  p_handler->setLinker(p_link);
  qInfo() << "handler is configured";
  return p_handler;
}

BM_HandlerPSGenerator *BM_hFactory::getGenHandler() {
  if (!isConfigured) {
    qDebug() << "HFACTORY: not cofigured";
    return nullptr;
  }
  qInfo() << "config started";
  BM_HandlerPSGenerator *p_handler = new BM_HandlerPSGenerator(this->parent());
  QSharedPointer<ALinkData> p_link = QSharedPointer<ALinkData>(new ALinkData);
  fillHandlerData(BM_HandlerType::Generator, p_handler, p_link);
  p_handler->setLinker(p_link);
  qInfo() << "handler is configured";
  return p_handler;
}

BM_HandlerSPlayer *BM_hFactory::getPlrHandler() {
  if (!isConfigured) {
    qDebug() << "HFACTORY: not cofigured";
    return nullptr;
  }
  qInfo() << "config started";
  auto p_handler = new BM_HandlerSPlayer(this->parent());
  QSharedPointer<ALinkData> p_link = QSharedPointer<ALinkData>(new ALinkData);
  fillHandlerData(BM_HandlerType::Player, p_handler, p_link);
  p_handler->setLinker(p_link);
  qInfo() << "handler is configured";
  return p_handler;
}

void BM_hFactory::fillHandlerData(BM_HandlerType t, BM_HandlerGeneral *h,
                                  QSharedPointer<ALinkData> l) {
  h->setUnitID(m_IDCounter++);
  h->setUnitType(t);
  h->setUnitTypeName(m_unitDescript[t].unitTypeName);
  h->setUnitPGM(m_unitDescript[t].unitPGMname);
  h->setUnitPath(m_unitDescript[t].unitPGMpath);
  l->unitID = m_IDCounter++;
  l->dispAddr = QHostAddress(m_HostIP);
  l->ownAddr = QHostAddress(m_UnitIP);
  l->dispPort = m_HostPortCounter++;
  l->ownPort = m_UnitPortCounter++;
}

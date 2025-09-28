#include "vd_unit.h"

#include <assert.h>

VD_Unit::VD_Unit(QObject *parent) : ProtoUnit{parent} {}

void VD_Unit::initialize() {
  assert(p_init != nullptr);
  m_vd.setResultPtr(this);
  setupVD();
  isSingleShot = !p_init->isPaused();
  if (isSingleShot) {
    m_taskFile = p_init->getTestWAVname();
      isTaskSet = true;
    slotStart();
  }
}

void VD_Unit::acceptRESULT(const std::string &s) {
  qInfo() << "VDU: accept result - " << s;
  emit signalStatus(BM_uState::Result_saved, QString::fromStdString(s));
}

void VD_Unit::slotStart() {
  qInfo() << "VD_Unit: processing file: " << m_taskFile;
    if(!isTaskSet){return;}
  isProcessingNow = true;
  isDetected = false;
  m_vd.processFile(m_taskFile);
  isDetected = m_vd.isArtifactDetected();
  isProcessingNow = false;
  isTaskSet = false;
  if (isSingleShot) {
    slotStop();
  }
}

void VD_Unit::slotStop() {
  qInfo() << "VD_Unit: close app!";
  // #TODO emit SAVEALL when work in threads
  // #TODO provide exit with delay timer
  exit(0);
}

void VD_Unit::slotStatusCheck() {
  // #TODO check bool state and create report msg
}

void VD_Unit::setOpt1(QString s) {
  m_taskFile = s.toStdString();
  isTaskSet = true;
}

void VD_Unit::setOpt2(QString s) {
  m_paramFile = s.toStdString();
  p_init->readIniFile(s);
  setupVD();
}

void VD_Unit::setOpt3(QString) {
  // set test mode or reset
}

void VD_Unit::setOpt4(QString)
{
}

void VD_Unit::innerError()
{
}

void VD_Unit::innerStateChg()
{
}

void VD_Unit::setupVD() {
  m_vd.setupNoiseMod(*p_init->getNMData());
  m_vd.setupDetector(*p_init->getVDData());
  m_vd.setupArtAn(*p_init->getVDData(), *p_init->getAAData());
}

#include "coreunit.h"
#include "../include/dispunit.h"
#include "../include/initunit.h"
#include "../include/protounit.h"

CoreUnit::CoreUnit(QObject *parent, QStringList args) : QObject{parent} {
}

void CoreUnit::initialize() {
  qInfo() << "COREUNIT: initialize";
  m_disp = new DispUnit(m_init->parent(), m_init->getLinkData());
  if (m_unit == nullptr) {
    qWarning() << "Task Unit is not created yet";
    return;
  }
  m_unit->initialize();
  connect(m_disp, SIGNAL(execute(QString)), m_unit, SLOT(slotStart()));
  connect(m_disp, SIGNAL(pause()), m_unit, SLOT(slotPause()));
  connect(m_disp, SIGNAL(exitUnit()), m_unit, SLOT(slotStop()));
  connect(m_disp, SIGNAL(checkStatus()), m_unit, SLOT(slotStatusCheck()));
  connect(m_disp, SIGNAL(setOption1(QString)), m_unit, SLOT(setOpt1(QString)));
  connect(m_disp, SIGNAL(setOption2(QString)), m_unit, SLOT(setOpt2(QString)));
  connect(m_disp, SIGNAL(setOption3(QString)), m_unit, SLOT(setOpt3(QString)));
  connect(m_disp, SIGNAL(setOption4(QString)), m_unit, SLOT(setOpt4(QString)));
  connect(m_unit, SIGNAL(signalStatus(BM_uState, QString)), m_disp,
          SLOT(recieveReport(BM_uState, QString)));
}

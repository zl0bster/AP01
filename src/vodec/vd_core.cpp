#include "vd_core.h"
#include "qdebug.h"

VD_Core::VD_Core(QObject *parent, QStringList args) : CoreUnit{parent, args} {
  m_vdinit = new VD_Init(parent, args);
  m_init = m_vdinit;
}

void VD_Core::initialize() {
  qInfo() << "VD_CORE: initialize";
  m_vdunit = new VD_Unit(m_vdinit->parent());
  m_unit = m_vdunit;
  m_vdinit->initialize();
  m_vdunit->setVDInit(m_vdinit);
  CoreUnit::initialize(); //  m_unit->initialize();
}

#include "sp_core.h"
#include "../include/initunit.h"
#include "sp_player.h"

SP_Core::SP_Core(QObject *parent, QStringList args) : CoreUnit{parent, args} {
    m_init = new InitUnit(parent, args);}

void SP_Core::initialize() {
  qInfo() << "SP_CORE: initialize";
  m_plr = new SP_Player(m_init->parent());
  m_unit = m_plr;
  m_init->initialize();
  CoreUnit::initialize(); //  m_unit->initialize();
}

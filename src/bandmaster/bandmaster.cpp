#include "bandmaster.h"
#include "bm_dispatcher.h"
#include "bm_masterpsgen.h"
#include "bm_masterrecord.h"
#include "bm_mastersplayer.h"
#include "qdir.h"

BandMaster::BandMaster(QObject *parent) : QObject{parent} {}

void BandMaster::initialize(BandMasterGUI *gui) {
  p_gui = gui;
  //---------------
  p_dispatcher = new BM_dispatcher(this);
  QDir dir;
  QString iniFile = dir.absolutePath() + "/";
  iniFile += "bmhfactory.ini";
  qInfo() << iniFile;
  p_dispatcher->setHFactoryINIfile(iniFile);
  p_dispatcher->initialize();
  //---------------
  p_recorder = new BM_MasterRecord(this);
  p_recorder->setP_handleMaster(p_gui->getHandleMaster());
  p_recorder->setP_dispatcher(p_dispatcher);
  p_recorder->initialize();
  //---------------
  p_psGen = new BM_MasterPSGen(this);
  p_psGen->setP_handleMaster(p_gui->getHandleMaster());
  p_psGen->setP_dispatcher(p_dispatcher);
  p_psGen->initialize();
  //---------------
  p_splayer = new BM_MasterSPlayer(this);
  p_splayer->setP_handleMaster(p_gui->getHandleMaster());
  p_splayer->setP_dispatcher(p_dispatcher);
  p_splayer->initialize();
}

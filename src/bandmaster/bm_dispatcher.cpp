#include "bm_dispatcher.h"
#include "bm_hfactory.h"
#include "bm_mastergeneral.h"
#include <cassert>

BM_dispatcher::BM_dispatcher(QObject *parent) : QObject{parent} {}

BM_dispatcher::~BM_dispatcher() { closeAllHandlers(); }

void BM_dispatcher::initialize() {
  if (!p_hFactory)
    p_hFactory = new BM_hFactory(this);
  assert(!m_hFactoryINIfile.isEmpty());
  p_hFactory->initRHFactory(m_hFactoryINIfile);
}

int BM_dispatcher::createRecHandler(QSharedPointer<ARecData> p,
                                    BM_MasterGeneral *master) {
  // check all necessary data are ready
  assert(p_hFactory != nullptr);
  assert(!p.isNull());
  // create handler and set ARecData and other
  BM_HandlerRecorder *rh = p_hFactory->getRecHandler();
  rh->setRecorder(p);
  h_list[rh->unitID()] = rh;
  connectHandler(rh, master);
  qInfo() << "DISP: rec handler initialized:" << rh->unitID();
  return rh->unitID();
}

int BM_dispatcher::createPSGHandler(QSharedPointer<WavFileData> file,
                                    QSharedPointer<PSGenData> signal,
                                    BM_MasterGeneral *master) {
  // check all necessary data are ready
  assert(p_hFactory != nullptr);
  assert(!file.isNull());
  assert(!signal.isNull());
  // create handler and set parameters
  BM_HandlerPSGenerator *gh = p_hFactory->getGenHandler();
  gh->setGenData(signal);
  gh->setWavData(file);
  h_list[gh->unitID()] = gh;
  // gh->set
  // connect signals-slots
  connectHandler(gh, master);
  qInfo() << "DISP: gen handler initialized:" << gh->unitID();
  return gh->unitID();
}

int BM_dispatcher::createPlrHandler(BM_MasterGeneral *master) {
  // check all necessary data are ready
  assert(p_hFactory != nullptr);
  BM_HandlerSPlayer *ph = p_hFactory->getPlrHandler();

  h_list[ph->unitID()] = ph;
  // connect signals-slots
  connectHandler(ph, master);
  qInfo() << "DISP: plr handler initialized:" << ph->unitID();
  return ph->unitID();
}

bool BM_dispatcher::closeHandler(uint32_t id) {
  auto handlerIt = h_list.find(id);
  if (handlerIt == h_list.end())
    qDebug() << "DISP: wrong Handler to delete: " << id;
  return false;
  qInfo() << "DISP: handler deleting: " + QString::number(id);
  delete handlerIt->second;
  h_list.erase(handlerIt);
  return true;
}

void BM_dispatcher::closeAllHandlers() {
  for (auto &[ch, hc] : h_list) {
    qInfo() << "DISP: handler deleting: " + QString::number(hc->unitID());
    delete hc;
    hc = nullptr;
  }
  h_list.clear();
}

void BM_dispatcher::connectHandler(BM_HandlerGeneral *h, BM_MasterGeneral *m) {
  connect(h, SIGNAL(statusReport(uint32_t, BM_uState, QString)), m,
          SLOT(reportAccepted(uint32_t, BM_uState, QString)));
  connect(m, SIGNAL(activate()), h, SLOT(activateUnit()));
  connect(m, SIGNAL(close()), h, SLOT(closeUnit()));
  connect(m, SIGNAL(start()), h, SLOT(startUnit()));
  connect(m, SIGNAL(pause()), h, SLOT(pauseUnit()));
  connect(m, SIGNAL(statusRequest()), h, SLOT(unitStatusRequest()));
  connect(m, SIGNAL(setTask(DataPtrs)), h, SLOT(getTask(DataPtrs)));
  qInfo() << "DISP: handler connected to master:" << h->unitTypeName() << h->unitID();
}

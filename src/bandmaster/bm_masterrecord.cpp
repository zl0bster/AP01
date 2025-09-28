#include "bm_masterrecord.h"
#include <QDebug>
#include <QTimer>

BM_MasterRecord::BM_MasterRecord(QObject *parent) : BM_MasterGeneral{parent} {}

BM_MasterRecord::~BM_MasterRecord() {}

void BM_MasterRecord::initialize() {
  auto guiRecIFc = p_handleMaster->get_ciRec();
  connect(guiRecIFc, SIGNAL(start()), this, SLOT(slotStart()));
  connect(guiRecIFc, SIGNAL(stop()), this, SLOT(slotStop()));
  connect(guiRecIFc, SIGNAL(pause()), this, SLOT(slotPause()));
  activateWatchman();
  qInfo("RecordMaster connected to gui");
}

void BM_MasterRecord::reportAccepted(uint32_t id, BM_uState s, QString arg) {
  p_watchman->reportAccepted(id, s, arg);
  if (s != BM_uState::Processing) {
    auto message = createTextMsg(id, s, arg);
    p_handleMaster->get_ciRec()->MSGOutput(message);
    qInfo() << "Message to show on GUI:" << message;
  }
}

void BM_MasterRecord::unitsClosed() {
  // #TODO clear chandata set state to idle to be ready for restart session
  if (m_masterState == BM_hState::AboutToQuit) {
    if (p_watchman->checkUnitsClosed()) {
      qInfo("units close check : ok");
      // return;
    }
  }
  for (auto &chan : m_chanData) {
    chan = ChanData();
  }
  p_dispatcher->closeAllHandlers();
  p_watchman->stopChecking();
  m_masterState = BM_hState::Idle;
  qInfo("record master is in IDLE state");
}

void BM_MasterRecord::getGUIData() {
  QSharedPointer<ProjectParameters> project = p_handleMaster->get_pp();
  m_projectName = project->projectName();
  m_resultPath = project->folderPath();
  qInfo() << "Project data read: " << m_projectName << m_resultPath;

  QSharedPointer<RecParameters> parameters = p_handleMaster->get_rp();
  m_recQuality = parameters->bitFormat(); // in bits
  m_freqS = parameters->sampleRate();
  m_recTimeSec = parameters->recTime();
  m_recChunkSec = parameters->fileDuration();
  qInfo() << "Record parameters: " << m_recQuality << m_freqS << m_recChunkSec
          << m_recTimeSec;

  QSharedPointer<RecSources> sources = p_handleMaster->get_rs();
  // NOTE: for setting channel source
  // 0-not active or n-1 = source number in list
  // convert while setting source
  auto setSource = [&](int source, int chan) {
    assert(chan < NumChannels);
    const auto id{"ABCDEFGHIJK"};
    if (source) {
      m_chanData[chan].chName = id[chan];
      m_chanData[chan].source = source - 1;
      m_chanData[chan].isActivated = true;
      return;
    }
    m_chanData[chan].isActivated = false;
  };

  setSource(sources->sourceRec1(), 0);
  setSource(sources->sourceRec2(), 1);
  setSource(sources->sourceRec3(), 2);
  setSource(sources->sourceRec4(), 3);
  qInfo() << "Record sources :" << m_chanData[0].source << m_chanData[1].source
          << m_chanData[2].source << m_chanData[3].source;
}

QSharedPointer<ARecData> BM_MasterRecord::fillRecData(const ChanData &chan) {

  QSharedPointer<ARecData> rd = QSharedPointer<ARecData>(new ARecData);
  rd->chanID = chan.chName;
  rd->projectName = m_projectName;
  rd->resultPath = m_resultPath;
  rd->bytesPerSample = m_recQuality / 8; // quality in bits from gui
  rd->chunkSec = m_recChunkSec;
  rd->recDurationSec = m_recTimeSec;
  rd->freqS = m_freqS;
  rd->sorsID = chan.source;

  qInfo() << "Fill rec data:" << rd->chanID << rd->projectName << rd->freqS;
  return rd;
}

void BM_MasterRecord::createHandlers() {
  for (auto &channel : m_chanData) {
    if (channel.isActivated) {
      channel.unitID =
          p_dispatcher->createRecHandler(fillRecData(channel), this);
    }
  }
}

bool BM_MasterRecord::isActive() const {
  bool result(false);
  for (auto &chan : m_chanData) {
    result |= chan.isActivated;
  }
  return result;
}

QVector<BM_WatchmanGeneral::ChanDiagData> BM_MasterRecord::getChanData() const {
  QVector<BM_WatchmanGeneral::ChanDiagData> v;
  for (auto &chan : m_chanData) {
    if (chan.isActivated) {
      BM_WatchmanGeneral::ChanDiagData temp;
      temp.chName = chan.chName;
      temp.unitID = chan.unitID;
      v.emplaceBack(temp);
    }
  }
  return v;
}

void BM_MasterRecord::activateWatchman() {
  p_watchman = new BM_WatchmanGeneral(this);
  p_watchman->setCheckActivity(400); // ms
  p_watchman->setCheckResultSaved(m_recChunkSec);
  connect(p_watchman, SIGNAL(abnormalDetected(QString)),
          p_handleMaster->get_ciRec(), SLOT(MSGOutput(QString)));
  connect(p_watchman, SIGNAL(requestStatus()), this, SIGNAL(statusRequest()));
}

QString BM_MasterRecord::createTextMsg(uint32_t id, BM_uState s,
                                       QString arg) const {
  QString chName{"WrongID="};
  chName.append(QString::number(id));
  for (const auto &c : m_chanData) {
    if (c.unitID == id) {
      chName = c.chName;
    }
  }
  QString message{"CH: "};
  message += chName;
  message += " (" + BcodeToStr(s) + ") ";
  message += arg;
  return message;
}

void BM_MasterRecord::slotStart() {
  qInfo("slot start acted");
  if (m_masterState != BM_hState::Idle) {
    qDebug() << "Cannot start! Master State is: " << HCodeToStr(m_masterState);
    return;
  }
  m_masterState = BM_hState::Starting;
  getGUIData();
  p_handleMaster->get_ciRec()->MSGOutput("RECORD_MASTER: process starts");
  createHandlers();
  emit activate();
  // if idle - create handlers and send start to them
  // if paused - send start
  QTimer::singleShot(200, [this]() {
    emit start();
    m_masterState = BM_hState::Processing;
  });

  p_watchman->setCheckResultSaved(m_recChunkSec);
  p_watchman->initialize(getChanData()); // start watchman checks
  QTimer::singleShot(300, [this]() { p_watchman->startChecking(); });
}

void BM_MasterRecord::slotStop() {
  qInfo("slot stop acted");
  if (m_masterState == BM_hState::AboutToQuit) {
    if (p_watchman->checkUnitsClosed()) {
      qInfo("units close check : ok");
      return;
    }
  }
  bool workCondition = (m_masterState == BM_hState::Processing) ||
                       (m_masterState == BM_hState::Starting);
  if (!workCondition)
    return;
  emit close();
  m_masterState = BM_hState::AboutToQuit;
  p_watchman->checkUnitsClosed();
  p_watchman->stopChecking();
  unitsClosed();
  // if processing - send stop and close
  // if other - nop
  // check all handlers for close state after small delay
}

void BM_MasterRecord::slotPause() { qInfo("slot pause acted"); }

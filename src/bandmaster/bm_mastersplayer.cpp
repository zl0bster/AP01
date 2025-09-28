#include "bm_mastersplayer.h"
#include "qtimer.h"

BM_MasterSPlayer::BM_MasterSPlayer(QObject *parent)
    : BM_MasterGeneral{parent} {}

BM_MasterSPlayer::~BM_MasterSPlayer() {}

// SPlayer starts with Recorder commands
// but separate SP Command Interface exists in GUI handles
void BM_MasterSPlayer::initialize() {
  initSignalData(m_s1);
  initSignalData(m_s2);
  connect(m_s1.sigTimer, SIGNAL(timeout()), this, SLOT(slotTimer1()));
  connect(m_s2.sigTimer, SIGNAL(timeout()), this, SLOT(slotTimer2()));
  qInfo() << "Play Timers connected in init" << this->objectName()
          << this->metaObject()->className();
  auto guiRecIFc = p_handleMaster->get_ciRec();
  connect(guiRecIFc, SIGNAL(start()), this, SLOT(slotStart()));
  connect(guiRecIFc, SIGNAL(stop()), this, SLOT(slotStop()));
  connect(guiRecIFc, SIGNAL(pause()), this, SLOT(slotPause()));
  qInfo("PLRMASTER: connected to gui");
}

void BM_MasterSPlayer::slotStart() {
  qInfo("slot play acted");
  getGUIData();
  // if not activated- create handler
  if (!m_handler.isActivated) {
    createHandler();
    m_handler.isActivated = true;
    emit activate();
  }
  if (m_s1.isActivated) {
    qInfo() << "PLRMASTER: start timer1";
    m_s1.sigTimer->start();
  };
  if (m_s2.isActivated) {
    qInfo() << "PLRMASTER: start timer1";
    m_s2.sigTimer->start();
  };
}

void BM_MasterSPlayer::slotStop() {
  emit close();
  m_handler.isActivated = false;
  m_s1.sigTimer->stop();
  m_s2.sigTimer->stop();
}

void BM_MasterSPlayer::reportAccepted(uint32_t id, BM_uState s, QString arg) {
  if (s != BM_uState::Processing) {
    auto message = createTextMsg(id, s, arg);
    p_handleMaster->get_ciRec()->MSGOutput(message);
    qInfo() << "Message to show on GUI:" << message;
  }
}

void BM_MasterSPlayer::slotTimer1() {
  qInfo() << "PLRMASTER: Try to play signal 1";
  m_s1.sigTimer->start();
  playSignal(m_s1, 1);
}

void BM_MasterSPlayer::slotTimer2() {
  qInfo() << "PLRMASTER: Try to play signal 2";
  m_s2.sigTimer->start();
  playSignal(m_s2, 2);
}

void BM_MasterSPlayer::getGUIData() {
  auto guiData = p_handleMaster->get_pd();
  m_s1.isActivated = (guiData->isFile1active() && guiData->playDevice() > 0);
  if (m_s1.isActivated) {
    m_s1.sigData->fileNamePath = guiData->psfile1();
    m_s1.sigData->outputID =
        guiData->playDevice(); // if (=0) output is not selected
    m_s1.sigData->level = guiData->pslevel1();
    m_s1.sigTimer->setInterval(guiData->psperiod1() * 1000); // SEC TO MS
  }
  m_s2.isActivated = (guiData->isFile2active() && guiData->playDevice() > 0);
  if (m_s2.isActivated) {
    m_s2.sigData->fileNamePath = guiData->psfile2();
    m_s2.sigData->outputID =
        guiData->playDevice(); // if (=0) output is not selected
    m_s2.sigData->level = guiData->pslevel2();
    m_s2.sigTimer->setInterval(guiData->psperiod2() * 1000); // SEC TO MS
  }
}

void BM_MasterSPlayer::createHandler() {
  m_handler.unitID = p_dispatcher->createPlrHandler(this);
}

QString BM_MasterSPlayer::createTextMsg(uint32_t id, BM_uState s,
                                        QString arg) const {
  QString message{"SPLR: "};
  message += QString::number(id);
  message += " (" + BcodeToStr(s) + ") ";
  message += arg;
  return message;
}

void BM_MasterSPlayer::initSignalData(SignalData &sd) {
  sd.sigData = QSharedPointer<SPlayerData>(new SPlayerData);
  sd.sigTimer = new QTimer(this);
  sd.isActivated = false;
}

void BM_MasterSPlayer::playSignal(SignalData &sd, int signalNumber) {
  if (!sd.isActivated) {
    qWarning() << "Try to play inactive signal:" << signalNumber;
    return;
  }
  if (lastPlayed != signalNumber) {
    DataPtrs d;
    d.splayData = sd.sigData;
    emit setTask(d);
    lastPlayed = signalNumber;
  }
  emit start();
}

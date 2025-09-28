#include "bm_handlergeneral.h"

static const std::unordered_map<BM_HandlerType, BM_hStateMachine::SM_WorkMode>
    m_modeMap{
        {BM_HandlerType::Recorder, BM_hStateMachine::SM_WorkMode::Process},
        {BM_HandlerType::Generator, BM_hStateMachine::SM_WorkMode::Task},
        {BM_HandlerType::Player, BM_hStateMachine::SM_WorkMode::Task},
        {BM_HandlerType::Convolutor, BM_hStateMachine::SM_WorkMode::Task}};

BM_HandlerGeneral::BM_HandlerGeneral(QObject *parent) : QObject{parent} {}

BM_HandlerGeneral::~BM_HandlerGeneral() {
  delete p_linker;
  delete p_stMachine;
}

void BM_HandlerGeneral::activateUnit() {
  initSM();
  qInfo() << "init State machine";
  m_startPrsImmediate = true;
  p_stMachine->initialState();
}

void BM_HandlerGeneral::pauseUnit() { emit requestStChg(BM_hState::Idle); }

void BM_HandlerGeneral::startUnit() {
  qInfo() << "emit start";
  m_startPrsImmediate = true;
  emit requestStChg(BM_hState::Processing);
}

void BM_HandlerGeneral::closeUnit() {
  qInfo() << "emit close";
  emit requestStChg(BM_hState::AboutToQuit);
}

void BM_HandlerGeneral::unitStatusRequest() {
  if (ifQProcessOk() && ifLinkerOk())
    emit sendMSG(BcomToInt(BM_command::Status), "");
}

void BM_HandlerGeneral::processError(QProcess::ProcessError e) {
  if (p_stMachine->currentState() == BM_hState::Error) {
    qWarning() << "PGM error yet found, try to stop hlr ";
    emit requestStChg(BM_hState::AboutToQuit);
    return;
  }
  qWarning() << "PGM error. processing";
  qDebug() << e;
  switch (e) {
  case QProcess::ProcessError::FailedToStart:
    m_unitState = BM_uState::Software_error;
    emit statusReport(m_unitID, m_unitState, "QProcess::FailedToStart");
    qWarning() << "PGM error. QProcess::FailedToStart";
    break;
  case QProcess::ProcessError::Crashed:
    m_unitState = BM_uState::Software_error;
    emit statusReport(m_unitID, m_unitState, "QProcess::Crashed");
    qWarning() << "PGM error. QProcess::Crashed";
    break;
  case QProcess::ProcessError::Timedout:
    m_unitState = BM_uState::Software_error;
    emit statusReport(m_unitID, m_unitState, "QProcess::Timedout");
    qWarning() << "PGM error. QProcess::Timedout";
    break;
  case QProcess::ProcessError::UnknownError:
    m_unitState = BM_uState::Software_error;
    emit statusReport(m_unitID, m_unitState, "QProcess::UnknownError");
    qWarning() << "PGM error. QProcess::UnknownError";
    break;
  default:
    break;
  }
  emit requestStChg(BM_hState::Error);
}

void BM_HandlerGeneral::terminateProcess() {
  if (isQProcess()) {
    m_unit->terminate();
  }
}

void BM_HandlerGeneral::processFinished(int exitCode,
                                        QProcess::ExitStatus exitStatus) {
  if (exitStatus == QProcess::CrashExit)
    processError(m_unit->error());
  else {
    emit statusReport(m_unitID, BM_uState::Quit_app,
                      "component exit code: " + QString::number(exitCode));
  }
  qDebug() << "PGM finished. Exit code" + QString::number(exitCode),
      emit requestStChg(BM_hState::AppExit);
  closeObjects();
}

void BM_HandlerGeneral::recieveMSG(uint32_t report, QString s) {
  m_unitState = intToBcode(report);
  m_lastMSGmoment = p_linker->lastMSGmoment();
  emit acceptReport(m_unitState);
  emit statusReport(m_unitID, m_unitState, s);
}

void BM_HandlerGeneral::initHObjects() {
  assert(!p_linkData.isNull());
  if (m_unitINIfile.isEmpty()) {
    createINIfile();
    m_taskArg = m_unitINIfile;
  }
  initLinker();
  initQProcess();
  // check started instance after delay
  QTimer::singleShot(CHECK_DELAY, this, &BM_HandlerGeneral::unitStatusRequest);
}

void BM_HandlerGeneral::requestStart() {
  if (!(ifQProcessOk() && ifLinkerOk())) {
    qWarning() << "HLR: CANNOT start by command. QProcess OK: "
               << ifQProcessOk() << "Linker OK:" << ifLinkerOk();
    // return;
  }
  emit sendMSG(BcomToInt(BM_command::Start), m_taskArg);
  qInfo() << "HLR: Start msg sent";
}

void BM_HandlerGeneral::requestPause() {
  if (ifQProcessOk() && ifLinkerOk())
    emit sendMSG(BcomToInt(BM_command::Pause), "");
}

void BM_HandlerGeneral::requestClose() {
  if (ifQProcessOk() && ifLinkerOk())
    emit sendMSG(BcomToInt(BM_command::Stop), "");
}

void BM_HandlerGeneral::finishPGM() { qInfo() << "HLR: SM has finished"; }

void BM_HandlerGeneral::closeObjects() {
  qInfo() << "HLR: closing QProcess and Linker";
  if (isQProcess()) {
    m_unit->close();
    delete m_unit;
    m_unit = nullptr;
  }
  if (p_linker) {
    delete p_linker;
    p_linker = nullptr;
  }
  emit requestStChg(BM_hState::QPrsClosed);
}

void BM_HandlerGeneral::initLinker() {
  assert(!p_linkData.isNull());
  ALinkData *p = new ALinkData;
  QSharedPointer<ALinkData> linkerConfig(p);
  linkerConfig->dispAddr = p_linkData->ownAddr;
  linkerConfig->dispPort = p_linkData->ownPort;
  linkerConfig->ownAddr = p_linkData->dispAddr;
  linkerConfig->ownPort = p_linkData->dispPort;
  linkerConfig->unitID = m_unitID;
  p_linker = new LinkerUDP(this, linkerConfig);
  connect(this, SIGNAL(sendMSG(uint32_t, QString)), p_linker,
          SLOT(sendReport(uint32_t, QString)));
  connect(p_linker, SIGNAL(commandRecieved(uint32_t, QString)), this,
          SLOT(recieveMSG(uint32_t, QString)));
  qInfo() << "linker initiated";
}

void BM_HandlerGeneral::initQProcess() {
  // #TODO adjust unit work folder
  QStringList clArgs;
  if (m_startPrsImmediate)
    clArgs << "-s";
  clArgs << "-f" << m_unitINIfile;
  m_unit = new QProcess(this);
  m_unit->setProgram(m_unitPath + "\\" + m_unitPGM);
  m_unit->setArguments(clArgs);
  qInfo() << m_unit->program();
  for (auto &s : m_unit->arguments()) {
    qInfo() << s;
  }
  connect(m_unit, SIGNAL(errorOccurred(QProcess::ProcessError)), this,
          SLOT(processError(QProcess::ProcessError)));
  connect(m_unit, SIGNAL(finished(int, QProcess::ExitStatus)), this,
          SLOT(processFinished(int, QProcess::ExitStatus)));
  connect(m_unit, &QProcess::started,
          []() { qInfo() << "HLR: SIGNAL: pgm started"; });
  m_unit->start();
  qInfo() << "QProcess initialized";
}

void BM_HandlerGeneral::initSM() {
  qDebug() << "HLR:" << m_unitTypeName << m_unitChan << m_unitID << ":"
           << "init SM";
  p_stMachine = new BM_hStateMachine(this);
  p_stMachine->setWorkMode(m_modeMap.at(m_unitType));
  connect(this, SIGNAL(requestStChg(BM_hState)), p_stMachine,
          SLOT(requestStateChg(BM_hState)));
  connect(this, SIGNAL(acceptReport(BM_uState)), p_stMachine,
          SLOT(acceptReport(BM_uState)));
  connect(p_stMachine, SIGNAL(initHObjects()), this, SLOT(initHObjects()));
  connect(p_stMachine, SIGNAL(startUnit()), this, SLOT(requestStart()));
  connect(p_stMachine, SIGNAL(pauseUnit()), this, SLOT(requestPause()));
  connect(p_stMachine, SIGNAL(closeUnit()), this, SLOT(requestClose()));
  // connect(p_stMachine, SIGNAL(closeHObjects()), this, SLOT(closeObjects()));
  connect(p_stMachine, SIGNAL(SMfinished()), this, SLOT(finishPGM()));
}

void BM_HandlerGeneral::stopQProcess() {

  qInfo() << "HLR: kill QProcess";
  if (isQProcess())
    m_unit->kill();
}

bool BM_HandlerGeneral::ifLinkerOk() {
  bool result = true;
  size_t currentErrMsgCount = p_linker->errorMSGsCount();
  if (currentErrMsgCount > NORM_LINK_ERROR) {
    if (currentErrMsgCount > m_errMSGsCount) {
      result = false; // minor alarm
      m_unitState = BM_uState::Connection_problem;
      emit statusReport(m_unitID, m_unitState,
                        QString::number(currentErrMsgCount) + " errors");
      if (currentErrMsgCount >= LIMIT_LINK_ERROR) {
        emit requestStChg(BM_hState::Error); // major alarm
      }
    }
  }
  m_errMSGsCount = currentErrMsgCount;
  return result;
}

bool BM_HandlerGeneral::ifQProcessOk() {
  // QProcess state and errors are checked by connection with SLOT processError
  if (!isQProcess())
    return false;
  m_unit->readAllStandardError();
  switch (m_unit->state()) {
  case QProcess::Running:
    return true;
  case QProcess::NotRunning:
    processError(m_unit->error());
    // emit requestStChg(BM_hState::AppExit);
    break;
  case QProcess::Starting:
  default:
    break;
  }
  return false;
}

bool BM_HandlerGeneral::isQProcess() {
  if (m_unit == nullptr) {
    m_unitState = BM_uState::Software_error;
    emit statusReport(m_unitID, m_unitState, "QProcess is null");
    emit requestStChg(BM_hState::QPrsClosed);
    return false;
  }
  qInfo() << "HLR: " << m_unit->state();
  return true;
}

QString BM_HandlerGeneral::makeINIfileName() {
  return "conf_" + m_unitTypeName + "_" + m_unitChan + "_" +
         QString::number(m_unitID) + ".ini";
}

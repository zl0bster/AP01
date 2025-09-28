#include "bm_watchmangeneral.h"
#include "qdebug.h"

BM_WatchmanGeneral::BM_WatchmanGeneral(QObject *parent) : QObject{parent} {
  t_activityCheck = new QTimer(this);
  connect(t_activityCheck, SIGNAL(timeout()), this, SLOT(checkActivity()));
  t_resultSaveCheck = new QTimer(this);
  connect(t_resultSaveCheck, SIGNAL(timeout()), this, SLOT(checkResultSave()));
}

BM_WatchmanGeneral::~BM_WatchmanGeneral() {}

void BM_WatchmanGeneral::initialize(QVector<ChanDiagData> unitData) {
  m_unitData = std::move(unitData);
  qInfo() << "Watchman initialized";
}

bool BM_WatchmanGeneral::checkUnitsClosed() {
  // qInfo() << "Watchman checks units closed";
  bool result{true};
  auto checkFinished = [](const LogRecord &r) {
    return (r.state == BM_uState::Quit_app);
  };
  for (auto &[chan, log] : m_log) {
    result &= (log.contains(checkFinished));
  };
  return result;
}

void BM_WatchmanGeneral::stopChecking() {
  t_activityCheck->stop();
  t_resultSaveCheck->stop();
  m_log.clear();
  qInfo("Watchman stopped checking !");
}

void BM_WatchmanGeneral::startChecking() {
  if (m_activityCheckPeriod > 0) {                       // if >0 means active
    t_activityCheck->setInterval(m_activityCheckPeriod); // ms
    t_activityCheck->start();
  }
  if (m_resultSaveCheckPeriod > 0) { // if >0 means active
    t_resultSaveCheck->setInterval(m_resultSaveCheckPeriod * 1000); // sec to ms
    t_resultSaveCheck->start();
  }
  qInfo("Watchman started checking !");
}

void BM_WatchmanGeneral::reportAccepted(uint32_t id, BM_uState s, QString arg) {
  Q_UNUSED(arg)

  m_log[id].pushback({s, steady_clock::now()});
  qInfo() << "report accepted: " << BcodeToStr(s);
}

void BM_WatchmanGeneral::checkActivity() {
  if (checkUnitsClosed()) {
    qInfo("Watchman found units closed !");
  }
  emit requestStatus();
  qInfo("Watchman checks units activity");
  for (auto &log : m_log) {
    if (log.second.empty())
      break;
    if (!isActivityPeriodOk(log.second)) {
      QString msg{getChName(log.first) + " status delay!"};
      emit abnormalDetected(msg);
      qInfo() << msg;
    }
    if (isAnyError(log.second)) {
      QString msg{getChName(log.first) + " error detected! "};
      msg.append(getErrorMsg(log.second));
      emit abnormalDetected(msg);
      qInfo() << msg;
    }
  }
}

void BM_WatchmanGeneral::checkResultSave() {
  qInfo("Watchman checks units result saving");
//-----------
  auto isResultSavedInTime = [base = m_resultSaveCheckPeriod] (const LogRecord &rec){
      bool result = (rec.state == BM_uState::Result_saved);
      if (!result)
          return result;
      auto period = duration_cast<seconds>(steady_clock::now() - rec.moment);
      return ((base) > (period.count()));
  };
//-----------
  for (auto &log : m_log) {
    if (log.second.empty()) {
      qInfo("Watchman found empty log while result save check !");
      break;
    }
    if (!log.second.contains(isResultSavedInTime)) {
        QString msg{getChName(log.first) + " result not saved!"};
        emit abnormalDetected(msg);
        qInfo() << msg;
    };
  }
}

bool BM_WatchmanGeneral::isActivityPeriodOk(const LogType &log) const {
  if (log.empty())
    return true;
  auto period =
      duration_cast<milliseconds>(steady_clock::now() - log.top().moment);
  return ((m_activityCheckPeriod * 3) > (period.count()));
}

bool BM_WatchmanGeneral::isAnyError(const LogType &log) const {
  if (log.empty())
    return false;
  // check all error state
  bool result = (log.top().state == BM_uState::Connection_problem) ||
                (log.top().state == BM_uState::Device_error) ||
                (log.top().state == BM_uState::File_open_error) ||
                (log.top().state == BM_uState::Software_error) ||
                (log.top().state == BM_uState::File_save_failed);
  return result;
}

QString BM_WatchmanGeneral::getErrorMsg(const LogType &log) const {
  QString result = "Log is empty. Wrong call!";
  if (log.empty())
    return result;
  result = BcodeToStr(log.top().state);
  return result;
}

QString BM_WatchmanGeneral::getChName(uint32_t id) const {
  QString result = "Wrong ID: " + QString::number(id);
  auto position =
      std::find_if(m_unitData.cbegin(), m_unitData.cend(),
                   [id](const ChanDiagData &x) { return (x.unitID == id); });
  if (position != m_unitData.cend()) {
    result = "CH: " + position->chName;
  }
  return result;
}

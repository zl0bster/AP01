#ifndef BM_WATCHMANGENERAL_H
#define BM_WATCHMANGENERAL_H

#include "../include/bm_codelist.h"
#include "../include/rbuffer.h"
#include <QObject>
#include <QTimer>
#include <chrono>

constexpr std::size_t reportsQty{32}; // size of LOG record array for each channel
using namespace std::chrono;

class BM_WatchmanGeneral : public QObject {
  Q_OBJECT

public:
  //-------------------------------
  struct ChanDiagData {
    QString chName;
    uint32_t unitID;
  };
  //-------------------------------
  explicit BM_WatchmanGeneral(QObject *parent = nullptr);
  ~BM_WatchmanGeneral();
  void initialize(QVector<ChanDiagData> unitData);
  void setCheckActivity(int period_ms) { m_activityCheckPeriod = period_ms; };
  void setCheckResultSaved(int period_sec) {
    m_resultSaveCheckPeriod = period_sec;
  };
  bool checkUnitsClosed();
  void stopChecking();
  void startChecking();
public slots:
  void reportAccepted(uint32_t id, BM_uState s, QString arg);
  void checkActivity();
  void checkResultSave();
signals:
  void abnormalDetected(QString);
  // void unitsClosed();
  void requestStatus(); // insert connection to handlers signal to master

private:
  struct LogRecord {
    BM_uState state;
    time_point<steady_clock> moment;
  };
  using LogType = Rbuffer<LogRecord, reportsQty>;

  QVector<ChanDiagData> m_unitData;
  std::unordered_map<uint32_t, LogType> m_log;
  int m_activityCheckPeriod{0};   // milliseconds, 0 if not used
  int m_resultSaveCheckPeriod{0}; // seconds, 0 if not used
  QTimer *t_activityCheck;
  QTimer *t_resultSaveCheck;

private:
  bool isActivityPeriodOk(const LogType &log) const;
  bool isAnyError(const LogType &log) const;
  QString getErrorMsg(const LogType &log) const;
  QString getChName(uint32_t id) const;
};

#endif // BM_WATCHMANGENERAL_H

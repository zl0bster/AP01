#ifndef BM_HANDLERGENERAL_H
#define BM_HANDLERGENERAL_H

#include "../include/bm_codelist.h"
#include "../include/data_structures.h"
#include "../include/linkerudp.h"
#include "bm_hstatemachine.h"
#include <QDateTime>
#include <QObject>
#include <QProcess>
#include <QString>

const quint32 CHECK_DELAY = 400; // ms
const size_t LIMIT_LINK_ERROR = 12;
const size_t NORM_LINK_ERROR = 2;

enum class BM_HandlerType { Recorder, Player, Convolutor, Generator };

class BM_HandlerGeneral : public QObject {
  Q_OBJECT

public:
  explicit BM_HandlerGeneral(QObject *parent = nullptr);
  virtual ~BM_HandlerGeneral();
  // general handler data
  void setUnitType(BM_HandlerType ht) { m_unitType = ht; };
  void setUnitID(uint32_t id) { m_unitID = id; };
  void setUnitChan(char c) { m_unitChan = c; };
  void setUnitTypeName(QString s) { m_unitTypeName = s; };
  void setUnitPGM(QString s) { m_unitPGM = s; };
  void setUnitPath(QString s) { m_unitPath = s; };
  void setStartImmed(bool b) { m_startPrsImmediate = b; };

  BM_HandlerType unitType() const { return m_unitType; };
  uint32_t unitID() const { return m_unitID; };
  QString unitName() const { return m_unitChan; };
  QDateTime lastMSGmoment() const { return m_lastMSGmoment; }
  QString unitTypeName() const { return m_unitTypeName; }

protected:
  BM_HandlerType m_unitType;
  QString m_unitTypeName;
  uint32_t m_unitID;
  QString m_unitChan;
  QProcess *m_unit;
  QString m_unitPGM;     // program filename
  QString m_unitPath;    // path to save unit's INI file
  QString m_unitINIfile; // filled after its creation
  QString m_taskArg;     // arg to send with Start command
  BM_uState m_unitState;
  bool m_startPrsImmediate;
  QDateTime m_lastMSGmoment;

  QSharedPointer<ALinkData> p_linkData;
  LinkerUDP *p_linker;
  BM_hStateMachine *p_stMachine;
  size_t m_errMSGsCount;
  bool m_isDebug{true};

signals:
  void requestStChg(BM_hState);    // interact with state machine
  void acceptReport(BM_uState);    // interact with state machine
  void sendMSG(uint32_t, QString); // command code to Linker
  //---- connect to bandmaster ----
  void statusReport(uint32_t ID, BM_uState code, QString s);

  // BM_AbstractComponentHandler interface
public slots:
  virtual void getTask(DataPtrs)=0;
  virtual void activateUnit();
  virtual void pauseUnit();
  virtual void startUnit();
  virtual void closeUnit();
  virtual void unitStatusRequest();
  virtual void processError(QProcess::ProcessError);
  virtual void terminateProcess();
  virtual void processFinished(int exitCode, QProcess::ExitStatus exitStatus);
  void setLinker(QSharedPointer<ALinkData> linkDataPtr) {
    p_linkData = linkDataPtr;
  }
  void recieveMSG(uint32_t report, QString s);

public slots:
  void initHObjects();
  void requestStart();
  void requestPause();
  void requestClose();
  void finishPGM();
  //-----------------

protected:
  virtual void createINIfile() = 0;
  void initQProcess();
  void initLinker(); // executed after setLink;er
  void closeObjects();
  void initSM();
  void stopQProcess();
  bool ifLinkerOk();
  bool ifQProcessOk();
  bool isQProcess();
  QString makeINIfileName();
};

#endif // BM_HANDLERGENERAL_H

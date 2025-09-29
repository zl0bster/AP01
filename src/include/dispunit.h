#ifndef DISPUNIT_H
#define DISPUNIT_H

#include <QObject>

#include "../include/a_codelist.h"
#include "../include/bm_codelist.h"
#include "../include/data_structures.h"
#include "../include/linkerudp.h"
#include <QSharedPointer>

class DispUnit : public QObject {
  Q_OBJECT
public:
  explicit DispUnit(QObject *parent, QSharedPointer<ALinkData> ld);
  virtual ~DispUnit() = default;
  void setLinkData(QSharedPointer<ALinkData> p) { p_linkData = p; }

protected:
  QSharedPointer<ALinkData> p_linkData;
  LinkerUDP *m_linker;

signals:
  int sendReport(uint32_t, QString); //(ASR_report, QString)
  void execute(QString);
  void pause();
  void checkStatus();
  void exitUnit();
  void setOption1(QString);
  void setOption2(QString);
  void setOption3(QString);
  void setOption4(QString);

public slots:
  void recieveCommand(
      uint32_t code,
      QString s); // (ASR_command code)from manager program throug A_Link
  void recieveReport(ASR_statusCode code, QString s);
  void recieveReport(BM_uState code, QString s);
  void quit();

protected:
  void initLinker();
};

#endif // DISPUNIT_H

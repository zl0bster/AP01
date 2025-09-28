#ifndef PROTOUNIT_H
#define PROTOUNIT_H
#include "bm_codelist.h"
#include <QObject>
class ProtoUnit : public QObject {
  Q_OBJECT
public:
  explicit ProtoUnit(QObject *parent) : QObject{parent} {};
  virtual ~ProtoUnit(){};
  virtual void initialize() = 0;

public slots:
  virtual void slotStart() = 0; // from dispatcher
  virtual void slotPause(){};
  virtual void slotStop() = 0;
  virtual void slotStatusCheck() = 0;
  virtual void setOpt1(QString){};
  virtual void setOpt2(QString){};
  virtual void setOpt3(QString){};
  virtual void setOpt4(QString){};
  virtual void innerError(){};
  virtual void innerStateChg(){};
signals:
  void signalStatus(BM_uState, QString);
};
#endif // PROTOUNIT_H

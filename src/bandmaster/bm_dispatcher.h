#ifndef BM_DISPATCHER_H
#define BM_DISPATCHER_H

#include "../include/data_structures.h"
#include <QObject>
#include <QSharedPointer>
#include <QString>

class BM_MasterGeneral;
class BM_hFactory;
class BM_HandlerGeneral;


class BM_dispatcher : public QObject {
  Q_OBJECT

private:
  BM_hFactory *p_hFactory;
  QString m_hFactoryINIfile;
  std::unordered_map<uint32_t, BM_HandlerGeneral *>
      h_list; // total handlers list by ID

public:
  explicit BM_dispatcher(QObject *parent = nullptr);
  ~BM_dispatcher();
  void initialize();
  int createRecHandler(QSharedPointer<ARecData> p, BM_MasterGeneral *master);
  int createPSGHandler(QSharedPointer<WavFileData> file, QSharedPointer<PSGenData> signal, BM_MasterGeneral *master );
  int createPlrHandler(BM_MasterGeneral *master);
  bool closeHandler(uint32_t id);
  void setHFactoryINIfile(QString s) { m_hFactoryINIfile = s; }
  void closeAllHandlers(); // #TODO: provide close handlers in the end of session
  // public slots:
  //   void signalStop(); //#TODO check handler signal about its turning off

private:
  void connectHandler(BM_HandlerGeneral * h,BM_MasterGeneral *m);
  // void initPSGHandler(); // #TODO: gen write procedure to use ifc class
  // void closeAllHandlers(); // #TODO: provide close handlers in the end of session
};

#endif // BM_DISPATCHER_H

// using RepSlotFX = void (BM_dispatcher::*)(BM_uState, QString);
// BM_uState getUState(char c) const { return h_list.at(c).m_uState; }
// BM_hState getHState(char c) const { return h_list.at(c).m_hState; }

// BM_InterfaceRec *getIfcRecorder() { return p_ifcRec; };
// BM_InterfacePSGen *getIfcPSGenerator() { return p_ifcPSGen; };
// void handleReport(char ch, BM_uState ust, QString s);
// bool activateRecorders();
// bool activateGenerators();

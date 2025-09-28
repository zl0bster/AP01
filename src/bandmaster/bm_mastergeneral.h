#ifndef BM_MASTERGENERAL_H
#define BM_MASTERGENERAL_H

#include "../include/bm_codelist.h"
#include "../include/bm_guihandles.h"
#include "bm_statemap.h"
#include "bm_dispatcher.h"
#include <QObject>

class BM_MasterGeneral : public QObject {
  Q_OBJECT
protected:
  GUIHandleMaster *p_handleMaster;
  BM_dispatcher *p_dispatcher;

  BM_hState m_masterState{BM_hState::Idle};

public:
  explicit BM_MasterGeneral(QObject *parent = nullptr);
  virtual ~BM_MasterGeneral(){};

  void setP_handleMaster(GUIHandleMaster *newHandleMaster);

  void setP_dispatcher(BM_dispatcher *newP_dispatcher);

public slots:
  // --------- connect to GUI handles ---------
  virtual void slotStart() = 0;
  virtual void slotStop() = 0;
  virtual void slotPause() = 0;
  // --------- connect to dispatcher handlers ---------
  virtual void reportAccepted(uint32_t id, BM_uState s,
                              QString arg) = 0; // to accept from handler
  // --------- crutches for master player
  virtual void slotTimer1() {};
  virtual void slotTimer2() {};

signals:
  void activate();
  void start();
  void pause();
  void close();
  void statusRequest();
  void setTask(DataPtrs);
};

#endif // BM_MASTERGENERAL_H

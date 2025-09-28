#ifndef BM_MASTERSPLAYER_H
#define BM_MASTERSPLAYER_H

#include "bm_mastergeneral.h"

class QTimer;
using SlotType = void (*)();

class BM_MasterSPlayer : public BM_MasterGeneral {
  struct HandlerData {
    uint32_t unitID;
    bool isActivated{false};
    BM_hState handlerState;
    bool operator()() { return isActivated; }
  };

  struct SignalData {
    QSharedPointer<SPlayerData> sigData;
    QTimer *sigTimer;
    bool operator()() { return isActivated; }
    bool isActivated{false};
  };

public:
  explicit BM_MasterSPlayer(QObject *parent = nullptr);
  ~BM_MasterSPlayer();
  void initialize();

public slots:

  // BM_MasterGeneral interface
public slots:
  virtual void slotStart() override;
  virtual void slotStop() override;
  virtual void slotPause() override{};
  virtual void reportAccepted(uint32_t id, BM_uState s, QString arg) override;
  // ------- crutches to connect Timer to slots
public slots:
  virtual void slotTimer1() override;
  virtual void slotTimer2() override;

private:
  HandlerData m_handler;
  SignalData m_s1;
  SignalData m_s2;
  int lastPlayed{0};

  void getGUIData();
  void createHandler();
  QString createTextMsg(uint32_t id, BM_uState s, QString arg) const;
  void initSignalData(SignalData &sd);
  // void initSignalData(SignalData &sd,  SlotType *pSlot);
  void playSignal(SignalData &sd, int signalNumber);
};

#endif // BM_MASTERSPLAYER_H

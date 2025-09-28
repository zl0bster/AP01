#ifndef BM_MASTERPSGEN_H
#define BM_MASTERPSGEN_H

#include "bm_mastergeneral.h"
// #include "bm_watchmangeneral.h"

class BM_MasterPSGen : public BM_MasterGeneral {

  struct HandlerData {
    uint32_t unitID;
    bool isActivated{false};
    BM_hState handlerState;
    bool operator()() { return isActivated; }
  };

public:
  explicit BM_MasterPSGen(QObject *parent = nullptr);
  virtual ~BM_MasterPSGen();
  void initialize();

  // BM_MasterGeneral interface
public slots:
  virtual void slotStart() override;
  virtual void slotStop() override;
  virtual void slotPause() override{};
  virtual void reportAccepted(uint32_t id, BM_uState s, QString arg) override;

private:
  QSharedPointer<WavFileData> p_wFileData;
  QSharedPointer<PSGenData> p_genData;
  // BM_WatchmanGeneral * p_watchman;
  HandlerData m_handler;

  void getGUIData();
  void createHandler();
  QString createTextMsg(uint32_t id, BM_uState s, QString arg) const;
  QString getFileName() const;
};

#endif // BM_MASTERPSGEN_H

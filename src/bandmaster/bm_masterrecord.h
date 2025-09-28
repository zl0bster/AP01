#ifndef BM_MASTERRECORD_H
#define BM_MASTERRECORD_H

#include "bm_mastergeneral.h"
#include "bm_watchmangeneral.h"

class BM_MasterRecord : public BM_MasterGeneral {
  //-------------------------------
  static constexpr int NumChannels = 4;
  struct ChanData {
    QString chName;
    // uint32_t handlerID;
    uint32_t unitID;
    uint32_t source;
    bool isActivated;
    BM_hState channelState;
    bool operator()() { return isActivated; }
  };

  std::array<ChanData, NumChannels> m_chanData;
  BM_WatchmanGeneral * p_watchman;

  QString m_projectName;
  QString m_resultPath;
  uint32_t m_recQuality; // 2-3-4 16int - 32int - float
  uint32_t m_freqS;
  uint32_t m_recTimeSec;
  uint32_t m_recChunkSec;
  //-------------------------------

public:
  explicit BM_MasterRecord(QObject *parent = nullptr);
  virtual ~BM_MasterRecord();
  void initialize();

private:
  void getGUIData();
  QSharedPointer<ARecData> fillRecData(const ChanData &);
  void createHandlers();
  bool isActive() const;
  QVector<BM_WatchmanGeneral::ChanDiagData> getChanData() const;
  void activateWatchman();
  QString createTextMsg(uint32_t id, BM_uState s, QString arg) const;

  // BM_MasterGeneral interface
public slots:
  virtual void slotStart() override;
  virtual void slotStop() override;
  virtual void slotPause() override;
  virtual void reportAccepted(uint32_t id, BM_uState s,
                              QString arg) override; // to accept from handler
  //-------------
  void unitsClosed();
};
#endif // BM_MASTERRECORD_H

// std::array<QSharedPointer<ARecData>, NumChannels> m_recData;
// bool isPtrDataConsistent;

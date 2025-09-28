#ifndef LINKERUDP_H
#define LINKERUDP_H

#include "data_structures.h"
#include <QByteArray>
#include <QDataStream>
#include <QDateTime>
#include <QObject>
#include <QSharedPointer>
#include <QString>
#include <QTimer>
#include <QUdpSocket>
#include <QNetworkDatagram>
#include <map>

const uint32_t RESEND_DELAY = 1000; // ms

//-------------------------------------------------------------------
struct AMSGData {
  uint32_t unitID; // internal application id from ALinkData
  uint32_t msgID;  // current msg counter
  uint32_t
      reqID; // id of recieved msg with request (command) for acceptance approval ONLY (for a while)/   #CDCDCDCD if empty
  uint32_t msgCode; // code of operation / command / reaction  cannot be empty
  QDateTime moment; // utc
  QString msgArg;   // [filename or etc] may be empty
  QString
      msgTxt; // [[description of operation / command / reaction]] may be empty
};
//-------------------------------------------------------------------

class LinkerUDP : public QObject { ///   unlimited resend of nonapproved msgs
                                  ///   #TODO add limitation
  Q_OBJECT
public:
  explicit LinkerUDP(QObject *parent = nullptr,
                    QSharedPointer<ALinkData> ld = nullptr);
  ~LinkerUDP();
  QDateTime lastMSGmoment() { return m_lastMSGmoment; }
  size_t errorMSGsCount() const {
    return msgLog.empty() ? 0 : msgLog.size();
  } // number of unapproved msgs
  bool isDebug{true};

private:
  QSharedPointer<ALinkData> p_linkData;
  qint32 m_msgCounter{0}; // keeps previous sent msg number
  QUdpSocket *m_udpSock;
  std::map<uint32_t, AMSGData>
      msgLog; // empty if all sent msgs approved to recieve by opposite side
  QTimer *p_resendTimer;
  qint32 m_msgsUnsent{0};
  QDateTime m_lastMSGmoment;
  bool m_isSockReady{false};

public slots:
  int sendReport(uint32_t code, QString s); //(ASR_report code, QString s);
  void recieveDG();
  void resendMSGs();

signals:
  void commandRecieved(uint32_t code, QString s); // (ASR_command code);
    void debugOutput(QString);

private:
    void initSocket();
  void generateMSG(AMSGData &d);
  void packMSG(const AMSGData &d, QByteArray *m);
  void unpackMSG(AMSGData &d, QByteArray *m);
  int sendMSG(const AMSGData &d);
  void processInput(QNetworkDatagram& dgdata);
  // void processInput();
  void respondCommand(AMSGData &recieved);
  bool isApprovalRecieved(const AMSGData &d);
  // void debugMSG(QString s);
  // void checkSock();
};

#endif // LINKERUDP_H

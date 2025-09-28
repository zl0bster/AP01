#include "linkerudp.h"
#include "../include/bm_codelist.h"
// #include <algorithm>

LinkerUDP::LinkerUDP(QObject *parent, QSharedPointer<ALinkData> ld)
    : QObject{parent}, p_linkData(ld) {
  msgLog.empty();
  initSocket();
}

LinkerUDP::~LinkerUDP() {
  m_udpSock->close();
  delete m_udpSock;
}

int LinkerUDP::sendReport(uint32_t code,
                          QString s) { //(ASR_report code, QString s)
  if (!m_isSockReady)
    initSocket();
  // form message
  if (!m_udpSock->isValid()) {
    qDebug() << "UDP socket is not valid while sending.";
  }
  AMSGData output;
  generateMSG(output);
  output.msgCode = code;
  output.msgArg = s;
  qInfo() << "Try send msg #" << output.msgID << " code: " << output.msgCode
          << " " << s;
  auto result = sendMSG(output);
  msgLog.insert({m_msgCounter, output});
  p_resendTimer->start(RESEND_DELAY);
  qInfo() << "MSG send " + QString::number(result);
  return result;
}

void LinkerUDP::recieveDG() {
  if (!m_udpSock->isValid()) {
    qDebug() << "UDP socket is not valid while recieving.";
  }
  while (m_udpSock->hasPendingDatagrams()) {
    QNetworkDatagram dg{m_udpSock->receiveDatagram()};
    processInput(dg);
  }
}

void LinkerUDP::resendMSGs() {
  if (msgLog.empty()) {
    p_resendTimer->stop();
    return;
  }
  for (auto &d : msgLog) {
    sendMSG(d.second);
  }
  p_resendTimer->start(RESEND_DELAY);
}

void LinkerUDP::initSocket() {
  if (m_isSockReady)
    return;
  // create udpSocket
  m_udpSock = new QUdpSocket(this);
  // bind udpSocket channel to A_Dispatcher adress & port
  m_udpSock->bind(p_linkData->ownAddr, p_linkData->ownPort);
  m_udpSock->connectToHost(p_linkData->dispAddr, p_linkData->dispPort);
  //  connect udpSocket signal to LinkerUDP slot
  connect(m_udpSock, SIGNAL(readyRead()), this, SLOT(recieveDG()));
  //  create resend timer
  p_resendTimer = new QTimer(this);
  p_resendTimer->setSingleShot(true);
  connect(p_resendTimer, SIGNAL(timeout()), this, SLOT(resendMSGs()));
  m_isSockReady = m_udpSock->isValid();
  if (!m_isSockReady)
    qDebug() << "UDP socket init os not valid";
}

void LinkerUDP::generateMSG(
    AMSGData &d) // adds metadata, not command code end args
{
  d.unitID = p_linkData->unitID;
  d.msgID = ++m_msgCounter;
  d.moment = QDateTime::currentDateTimeUtc();
  d.reqID = 0xCDCDCDCD;
}

void LinkerUDP::packMSG(const AMSGData &d, QByteArray *m) {
  QDataStream ds(m, QIODevice::WriteOnly);
  ds << d.unitID;
  ds << d.msgID;
  ds << d.reqID;
  ds << d.msgCode;
  ds << d.moment;
  if (!d.msgArg.isEmpty())
    ds << d.msgArg;
  if (!d.msgTxt.isEmpty())
    ds << d.msgTxt;
}

void LinkerUDP::unpackMSG(AMSGData &d, QByteArray *m) {
  QDataStream ds(m, QIODevice::ReadOnly);
  ds >> d.unitID;
  ds >> d.msgID;
  ds >> d.reqID;
  ds >> d.msgCode;
  ds >> d.moment;
  if (!ds.atEnd())
    ds >> d.msgArg;
  if (!ds.atEnd())
    ds >> d.msgTxt;
}

int LinkerUDP::sendMSG(const AMSGData &d) {
  if (!m_udpSock->isValid()) {
    qDebug() << "UDP socket is not valid while sending.";
  }
  QByteArray dg;
  packMSG(d, &dg);
  QNetworkDatagram dgram;
  dgram.setData(dg);
  dgram.setDestination(p_linkData->dispAddr, p_linkData->dispPort);
  m_udpSock->writeDatagram(dgram);
  // m_udpSock->write(dg.data(), dg.size());
  // if (m_udpSock->error() != 0)  // retry 5 times or until no error
  //   for (int i = 5; i == 0; --i) {
  //     m_udpSock->write(dg.data(), dg.size());
  //     if (m_udpSock->error() == 0)
  //       break;
  //   }
  return m_udpSock->error();
}

void LinkerUDP::processInput(QNetworkDatagram &dgdata) {
  bool isDGValid = dgdata.isValid() &&
                   dgdata.destinationAddress() == p_linkData->ownAddr &&
                   dgdata.destinationPort() == p_linkData->ownPort;
  if (!isDGValid)
    return;
  // accept datagram
  QByteArray dg{dgdata.data()};
  // m_udpSock->read(dg.data(), dg.size());)
  AMSGData inputData;
  unpackMSG(inputData, &dg);
  // check if response for previous msg recieved - report recieve approved
  if (isApprovalRecieved(inputData))
    return;
  if (inputData.msgCode == 0)
    return; // code is unused for transfer
  m_lastMSGmoment = inputData.moment;
  emit commandRecieved(inputData.msgCode, inputData.msgArg);
  // send fast respond
  respondCommand(inputData);
  qInfo() << "recieved # " + QString::number(inputData.msgID) +
                 " from: " + QString::number(inputData.unitID) +
                 " code: " + QString::number(inputData.msgCode) +
                 inputData.msgArg;
}

// void LinkerUDP::processInput() {
//   // accept datagram
//   QByteArray dg;
//   m_udpSock->read(dg.data(), dg.size());
//   // if(m_udpSock->)
//   AMSGData inputData;
//   unpackMSG(inputData, &dg);
//   // check if response for previous msg recieved - report recieve approved
//   if (isApprovalRecieved(inputData))
//     return;
//   m_lastMSGmoment = inputData.moment;
//   emit commandRecieved(inputData.msgCode, "");
//   // send fast respond
//   respondCommand(inputData);
//   debugMSG("recieved #" + QString::number(inputData.reqID) +
//            " from:" + QString::number(inputData.unitID) +
//            " code:" + QString::number(inputData.msgCode));
// }

void LinkerUDP::respondCommand(AMSGData &recieved) {
  AMSGData responce;
  generateMSG(responce);
  responce.msgCode = BcodeToInt(BM_uState::Responce_ok);
  responce.reqID = recieved.msgID;
  sendMSG(responce);
}

bool LinkerUDP::isApprovalRecieved(const AMSGData &d) {
  bool isTransactionApproval =
      (d.msgCode == BcodeToInt(BM_uState::Responce_ok)) &&
      (msgLog.find(d.reqID) != msgLog.end());
  if (isTransactionApproval)
    return (msgLog.erase(d.reqID) > 0);
  return false;
}

// void LinkerUDP::debugMSG(QString s) {
//   if (isDebug) {
//     qDebug() << "LINKER:" << s;
//     // emit debugOutput("LINKER: " + s);
//   }
// }

// void LinkerUDP::checkSock()
// {

// }

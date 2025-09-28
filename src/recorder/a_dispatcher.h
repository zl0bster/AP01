#ifndef A_DISPATCHER_H
#define A_DISPATCHER_H

#include "../include/a_codelist.h"
#include "../include/data_structures.h"
#include "../include/linkerudp.h"
#include "../include/savewav.h"
#include "a_buffer.h"
#include "a_recorder.h"
#include <QObject>
#include <QSharedPointer>
#include <QThread>
#include <QTimer>

const size_t LIMIT_LINK_ERROR = 12;

class A_Dispatcher : public QObject {
  Q_OBJECT
  //--------------
  enum class A_DispState { StandBy, Running, StoppedToQuit };
  //--------------
public:
  explicit A_Dispatcher(QObject *parent = nullptr,
                        QSharedPointer<ALinkData> ld = nullptr,
                        QSharedPointer<ARecData> rd = nullptr,
                        bool isPaused = false);
  ~A_Dispatcher();
  // int process();

private:
  A_DispState m_state{A_DispState::StandBy};
  QSharedPointer<ALinkData> p_linkData;
  QSharedPointer<ARecData> p_recData;
  A_Buffer *m_buf;
  A_Recorder *m_rec;
  SaveWAV *m_saver;
  LinkerUDP *m_linker;
  // AWAVData *m_WAVData;
  WavFileData *m_WFData;
  QTimer *m_autoFinish;
  size_t m_errMSGsCount;
  int m_delayOff;
  QThread t_saver;
  // QThread t_linker;

signals:
  int sendReport(uint32_t, QString); //(ASR_report, QString)
  void saveFile(const QString, const QByteArrayView *);
  void signalStart();
  void signalStop();
  void checkStatus();

public slots:
  void recieveCommand(
      uint32_t code,
      QString s); // (ASR_command code); // from manager program throug A_Link
  void recieveReport(ASR_statusCode code,
                     QString s); // from A_Recorder or A_Savefile
  void saveBufToFile(const QByteArrayView *buf);
  void stopRecord();

private:
  void startRecord();
  void pauseRecord();
  void statusRequest();
  void initBuffer();
  void initSaver();
  void initRecorder();
  void initLinker();
  QString getFNameVsTime(const QString &fn) const;
  void fillWavData();
  bool ifLinkerOk();
  // bool ifRecorderOk();
};

#endif // A_DISPATCHER_H

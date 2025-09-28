#ifndef PSG_DISPATCHER_H
#define PSG_DISPATCHER_H

#include <QObject>

#include "../include/data_structures.h"
#include "../include/linkerudp.h"
#include "../include/savewav.h"
#include <QSharedPointer>
#include <psg_generator.h>
#include <psg_init.h>

class PSG_Dispatcher : public QObject {
  Q_OBJECT
public:
  explicit PSG_Dispatcher(QObject *parent = nullptr,
                          QSharedPointer<WavFileData> fd = nullptr,
                          QSharedPointer<ALinkData> ld = nullptr,
                          QSharedPointer<PSGenData> gd = nullptr);
  ~PSG_Dispatcher();

private:
  QSharedPointer<WavFileData> p_fileData;
  QSharedPointer<ALinkData> p_linkData;
  QSharedPointer<PSGenData> p_genData;
  SaveWAV *m_saver;
  LinkerUDP *m_linker;
  PSG_Generator *m_gen;
  PSG_Init *m_init;

signals:
  int sendReport(uint32_t, QString); //(ASR_report, QString)
  void saveFile(const QString, const QByteArrayView *);

public slots:
  void recieveCommand(
      uint32_t code,
      QString s); // (ASR_command code)from manager program throug A_Link
  void recieveReport(ASR_statusCode code, QString s);

private:
  void initSaver();
  void initLinker();
  void execute(QString iniFileName); // if filename is empty - execute with
                                     // current settings (after start)
  void reportStatus();
  void quit();
};

#endif // PSG_DISPATCHER_H

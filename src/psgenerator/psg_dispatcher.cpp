#include "psg_dispatcher.h"
#include "../include/bm_codelist.h"
#include <QByteArray>
#include <QTimer>

PSG_Dispatcher::PSG_Dispatcher(QObject *parent, QSharedPointer<WavFileData> fd,
                               QSharedPointer<ALinkData> ld,
                               QSharedPointer<PSGenData> gd)
    : p_fileData(fd), p_linkData(ld), p_genData(gd) {
  QObject::setParent(parent);
  initLinker();
  initSaver();
  m_gen = new PSG_Generator;
  m_init = new PSG_Init;
}

PSG_Dispatcher::~PSG_Dispatcher() {
  delete m_init;
  delete m_gen;
  delete m_saver;
  delete m_linker;
  qInfo() << "destructor finished";
}

void PSG_Dispatcher::recieveCommand(uint32_t code, QString s) {
  qInfo() << "command recieved:" << code << s;
  BM_command command = intToBcom(code);
  switch (command) {
  case BM_command::Start:
    execute(s);
    break;
  case BM_command::Pause:
    break;
  case BM_command::Stop:
    quit();
    break;
  case BM_command::Status:
    reportStatus();
    break;
  default:
    break;
  }
}

void PSG_Dispatcher::recieveReport(ASR_statusCode code, QString s) {
  qInfo() << "internal report:" << ACodeToStr(code) << s;
  switch (code) {
  case ASR_statusCode::File_saved:
    emit sendReport(BcodeToInt(BM_uState::Result_saved), s);
    QTimer::singleShot(
        50, [&]() { emit sendReport(BcodeToInt(BM_uState::Stand_by), ""); });
    break;

  case ASR_statusCode::File_open_error:
    emit sendReport(BcodeToInt(BM_uState::File_save_failed), s);
    break;

  case ASR_statusCode::Status_ok:
    emit sendReport(BcodeToInt(BM_uState::Processing), s);
    break;

  case ASR_statusCode::Connection_error:
    emit sendReport(BcodeToInt(BM_uState::Connection_problem), s);
    break;

  default:
    emit sendReport(BcodeToInt(BM_uState::Software_error), s);
    break;
  }
}

void PSG_Dispatcher::initSaver() {
  m_saver = new SaveWAV(this, p_fileData.data());
  // connect workers by signal-slot
  connect(m_saver, SIGNAL(signalStatus(ASR_statusCode, QString)), this,
          SLOT(recieveReport(ASR_statusCode, QString)));
  connect(this, SIGNAL(saveFile(QString, const QByteArrayView *)), m_saver,
          SLOT(slotSaveFile(QString, const QByteArrayView *)));
}

void PSG_Dispatcher::initLinker() {
  m_linker = new LinkerUDP(this, p_linkData);
  connect(this, SIGNAL(sendReport(uint32_t, QString)), m_linker,
          SLOT(sendReport(uint32_t, QString)));
  connect(m_linker, SIGNAL(commandRecieved(uint32_t, QString)), this,
          SLOT(recieveCommand(uint32_t, QString)));
}

void PSG_Dispatcher::execute(QString iniFileName) {
  qInfo() << "executing: " << iniFileName;
  if (!iniFileName.isEmpty()) {
    m_init->readIniFile(iniFileName);
    p_fileData = m_init->getFileData();
    p_genData = m_init->getGenData();
  }
  qInfo() << "generate task:" << p_genData->durationMSec << p_genData->freq1
          << p_genData->freq2 << p_genData->freq3;
  qInfo() << "task file params:" << p_fileData->fileName
          << p_fileData->sampleRate << p_fileData->bytesPerSample;
  m_gen->setFileData(p_fileData);
  m_gen->setGenData(p_genData);
  QByteArray signalData;
  if (!m_gen->generate(&signalData)) {
    emit sendReport(static_cast<uint32_t>(BM_uState::Software_error), "");
    return;
  }
  auto signDataView = QByteArrayView(signalData);
  m_saver->setWAVData(p_fileData.data());
  qInfo() << "Try save:" << p_fileData->fileName;
  emit saveFile(p_fileData->fileName, &signDataView);
}

void PSG_Dispatcher::reportStatus() {
  emit sendReport(static_cast<uint32_t>(BM_uState::Stand_by), "");
  return;
}

void PSG_Dispatcher::quit() {
  emit sendReport(BcodeToInt(BM_uState::Quit_app), "");
  QTimer::singleShot(200, this, [] {
    qInfo() << "PGM exit code 0";
    exit(0);
  });
}

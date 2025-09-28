#include "bm_masterpsgen.h"

BM_MasterPSGen::BM_MasterPSGen(QObject *parent) : BM_MasterGeneral{parent} {
  p_wFileData = QSharedPointer<WavFileData>(new (WavFileData));
  p_genData = QSharedPointer<PSGenData>(new (PSGenData));
}

BM_MasterPSGen::~BM_MasterPSGen() {}

void BM_MasterPSGen::initialize() {
  auto guiRecIFc = p_handleMaster->get_ciRec();
  auto guiPSGIFc = p_handleMaster->get_ciRPSG();
  connect(guiPSGIFc, SIGNAL(start()), this, SLOT(slotStart()));
  connect(guiRecIFc, SIGNAL(start()), this, SLOT(slotStop()));
  // activateWatchman();
  qInfo("PSGenMaster connected to gui");
}

void BM_MasterPSGen::slotStart() {
  qInfo("slot generate acted");
  getGUIData();
  // if not activated- create handler
  if (!m_handler.isActivated) {
    createHandler();
    m_handler.isActivated = true;
    emit activate();
  } else {
    DataPtrs p;
    p.wavFileData = p_wFileData;
    p.psgenData = p_genData;
    emit setTask(p);
  }
  emit start();
}

void BM_MasterPSGen::slotStop() {
  // set close command to handler
  emit close();
  m_handler.isActivated = false;
  // #TODO check unit is closed
}

void BM_MasterPSGen::reportAccepted(uint32_t id, BM_uState s, QString arg) {
  // p_watchman->reportAccepted(id, s, arg);
  if (s != BM_uState::Processing) {
    auto message = createTextMsg(id, s, arg);
    p_handleMaster->get_ciRec()->MSGOutput(message);
    qInfo() << "Message to show on GUI:" << message;
  }
}

void BM_MasterPSGen::getGUIData() {
  QSharedPointer<RecParameters> parameters = p_handleMaster->get_rp();
  p_wFileData->bytesPerSample = parameters->bitFormat() / 8; //  bits to bytes
  p_wFileData->sampleRate = parameters->sampleRate();
  qInfo() << "File parameters: " << p_wFileData->bytesPerSample
          << p_wFileData->sampleRate;

  QSharedPointer<GenParameters> signal = p_handleMaster->get_gp();
  p_genData->durationMSec = signal->signalDuration();
  p_genData->freq1 = signal->freq1();
  p_genData->freq2 = signal->freq2();
  p_genData->freq3 = signal->freq3();
  qInfo() << "Signal data: " << p_genData->durationMSec << p_genData->freq1
          << p_genData->freq2 << p_genData->freq3;

  QSharedPointer<ProjectParameters> project = p_handleMaster->get_pp();
  p_wFileData->fileName = getFileName();
  p_wFileData->resultPath = project->folderPath();
  qInfo() << "Project data: " << p_wFileData->resultPath
          << p_wFileData->fileName;
}

void BM_MasterPSGen::createHandler() {
  // make handler and connect start / stop / msg
  m_handler.unitID =
      p_dispatcher->createPSGHandler(p_wFileData, p_genData, this);
}

QString BM_MasterPSGen::createTextMsg(uint32_t id, BM_uState s,
                                      QString arg) const {

  QString message{"PS_GEN: "};
  message += QString::number(id);
  message += " (" + BcodeToStr(s) + ") ";
  message += arg;
  return message;
}

QString BM_MasterPSGen::getFileName() const {
  QString result{"PS0"};
  result += QString::number(p_genData->durationMSec);
  result += "-0";
  result += QString::number(p_genData->freq1);
  result += "-0";
  result += QString::number(p_genData->freq2);
  result += "-0";
  result += QString::number(p_genData->freq3);
  result += ".wav";
  return result;
}

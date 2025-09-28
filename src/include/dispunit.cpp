#include "dispunit.h"

DispUnit::DispUnit(QObject *parent, QSharedPointer<ALinkData> ld)
    : QObject{parent}, p_linkData(ld) {
  initLinker();
}

void DispUnit::recieveCommand(uint32_t code, QString s) {
  qInfo() << "DISP: command recieved:" << code << " arg: " << s;
  BM_command command = intToBcom(code);
  switch (command) {
  case BM_command::Start:
    qInfo() << "DISP: Start";
    emit execute(s);
    break;
  case BM_command::Pause:
    qInfo() << "DISP: Pause";
    emit pause();
    break;
  case BM_command::Stop:
    qInfo() << "DISP: Stop - Quit";
    quit();
    break;
  case BM_command::Status:
    qInfo() << "DISP: Status check";
    emit checkStatus();
    break;
  case BM_command::SetOption1:
    qInfo() << "DISP: set Option 1";
    emit setOption1(s);
    break;
  case BM_command::SetOption2:
    qInfo() << "DISP: set Option 2";
    emit setOption2(s);
    break;
  case BM_command::SetOption3:
    qInfo() << "DISP: set Option 3";
    emit setOption3(s);
    break;
  case BM_command::SetOption4:
    qInfo() << "DISP: set Option 4";
    emit setOption4(s);
    break;
  default:
    qDebug() << "DISP: command  undefined";
    break;
  }
}

void DispUnit::recieveReport(ASR_statusCode code, QString s) {
  const std::unordered_map<ASR_statusCode, BM_uState> codeMap{
      {ASR_statusCode::Status_ok, BM_uState::Stand_by},
      {ASR_statusCode::File_saved,
       BM_uState::Positive_result}, // defines positive result of task execution
      {ASR_statusCode::Device_error, BM_uState::Device_error},
      {ASR_statusCode::Format_unsupported, BM_uState::Software_error},
      {ASR_statusCode::File_open_error, BM_uState::File_open_error}};
  BM_uState report = BM_uState::Software_error;
  try {
    report = codeMap.at(code);
  } catch (const std::out_of_range &ex) {
    qWarning() << "Wrong status code:" << ex.what();
  }
  emit sendReport(BcodeToInt(report), s);
}

void DispUnit::recieveReport(BM_uState code, QString s) {
  qInfo() << "DISP send status:" << BcodeToStr(code) << s;
  emit sendReport(BcodeToInt(code), s);
}

void DispUnit::quit() {
  emit exitUnit();
  emit sendReport(BcodeToInt(BM_uState::Quit_app), "");
  QTimer::singleShot(200, this, [] {
    qInfo() << "PGM exit code 0";
    exit(0);
  });
}

void DispUnit::initLinker() {
  m_linker = new LinkerUDP(this, p_linkData);
  connect(this, SIGNAL(sendReport(uint32_t, QString)), m_linker,
          SLOT(sendReport(uint32_t, QString)));
  connect(m_linker, SIGNAL(commandRecieved(uint32_t, QString)), this,
          SLOT(recieveCommand(uint32_t, QString)));
}

#include "sp_player.h"
#include "../include/bm_codelist.h"
#include "qaudiodevice.h"
#include <QAudioOutput>
#include <QMediaDevices>
#include <QMediaPlayer>
#include <QTimer>

SP_Player::SP_Player(QObject *parent) : ProtoUnit{parent} {
  QObject::setObjectName("SPlayer");
}

void SP_Player::setLevel(int percent) {
  m_level = percent;
  isDataChanged = true;
}

void SP_Player::setOutput(int number) {
  m_outputUnit = number;
  isDeviceSet = true;
  isDataChanged = true;
}

void SP_Player::setFilename(QString name) {
  m_fileName = name;
  isFileSet = true;
  isDataChanged = true;
}

void SP_Player::initialize() {
  qInfo() << "PLR: initialize";
  m_player = new QMediaPlayer(this);
  m_output = new QAudioOutput(this);
  qInfo() << "Try to connect player signals " << QObject::objectName();
  connect(m_player, SIGNAL(errorChanged()), this, SLOT(innerError()));
  connect(m_player, SIGNAL(playbackStateChanged(QMediaPlayer::PlaybackState)),
          this, SLOT(innerStateChg()));
  m_statusTimer = new QTimer(this);
  m_statusTimer->setInterval(100);
  connect(m_statusTimer, SIGNAL(timeout()), this, SLOT(innerStateChg()));
}

void SP_Player::slotStart() {
  qInfo() << "QPlayer slot start";
  if (!isPlrDataSet()) {
    qWarning() << "PLR: CANNOT play. Output set: " << isDeviceSet
               << " File set: " << isFileSet;
  }
  if (isDataChanged) {
    setOutput();
    playPrepare();
  }
  isDataChanged = {false};
  m_player->play();
  innerStateChg();
  m_statusTimer->start();
  checkPlayerState();
}

void SP_Player::slotPause() {
  qInfo() << "QPlayer slot pause";
  m_player->pause();
  innerStateChg();
}

void SP_Player::slotStop() {
  qInfo() << "QPlayer slot stop";
  m_player->stop();
  innerStateChg();
}

void SP_Player::slotStatusCheck() {
  qInfo() << "status requested:" << m_player->playbackState();
  if (m_player->error() == QMediaPlayer::NoError) {
    // emit signalStatus(ASR_statusCode::Status_ok, "");
    emit signalStatus(BM_uState::Stand_by, "");
    return;
  }
  innerError();
}

void SP_Player::setOpt1(QString name) {
  m_fileName = name;
  isFileSet = true;
  isDataChanged = true;
}

void SP_Player::setOpt2(QString s) {
  bool valueIsValid;
  m_outputUnit = s.toInt(&valueIsValid);
  if (!valueIsValid) {
    QString msg{"PLR: Wrong arg for source setting: " + s};
    qDebug() << msg;
    emit signalStatus(BM_uState::Software_error, msg);
    return;
  }
  isDeviceSet = true;
  isDataChanged = true;
}

void SP_Player::setOpt3(QString percent) {
  bool valueIsValid;
  m_level = percent.toInt(&valueIsValid);
  if (!valueIsValid) {
    QString msg{"PLR: Wrong arg for level setting: " + percent};
    qDebug() << msg;
    emit signalStatus(BM_uState::Software_error, msg);
    return;
  }
  isDataChanged = true;
}

void SP_Player::innerError() {
  const std::unordered_map<QMediaPlayer::Error, BM_uState> errorMap{
      {QMediaPlayer::Error::AccessDeniedError, BM_uState::File_open_error},
      {QMediaPlayer::Error::FormatError, BM_uState::Software_error},
      {QMediaPlayer::Error::NoError, BM_uState::Positive_result}};
  auto errCode = m_player->error();
  qInfo() << "QPlayer error: " << errCode;
  auto errText = m_player->errorString();
  auto statusMsg = errorMap.at(errCode);
  qDebug() << "QPlayer error: " << BcodeToStr(statusMsg) << errText;
  emit signalStatus(statusMsg, errText);
}

void SP_Player::innerStateChg() {
  auto state = m_player->playbackState();
  qInfo() << "PLR: state changed to: " << state;
  switch (state) {

  case QMediaPlayer::StoppedState:
    if (isPlaying) {
      emit signalStatus(
          BM_uState::Positive_result, // defines positive result of task
          m_player->source()
              .toLocalFile()); // execution to translate by dispatcher
      m_statusTimer->stop();
    } else {
      emit signalStatus(BM_uState::Stand_by, "PLR: stopped");
    }
    isPlaying = false;
    break;
  case QMediaPlayer::PlayingState:
    isPlaying = true;
    break;
  case QMediaPlayer::PausedState:
    emit signalStatus(BM_uState::Stand_by, "PLR: paused");
    break;
  }
}

void SP_Player::setOutput() {
  const auto deviceInfo{QMediaDevices::audioOutputs()};
  for (auto &dev : deviceInfo) {
    qInfo() << dev.description();
  }
  if (m_outputUnit > deviceInfo.size()) {
    qWarning() << "Wrong output ID:";
  }
  qInfo() << m_outputUnit << " of " << deviceInfo.size();
  auto device = deviceInfo[m_outputUnit - 1]; // in GUI list [0] is empty
  if (device.isNull()) {
    qDebug() << "Wrong device: " << device.description();
    emit signalStatus(BM_uState::Device_error, device.description());
    return;
  }
  qInfo() << "Output Device selected: " << device.description();
  m_output->setDevice(device);
}

void SP_Player::playPrepare() {
  m_player->setAudioOutput(m_output);
  setDeviceVolume();
  m_player->setSource(QUrl::fromLocalFile(m_fileName));
}

void SP_Player::setDeviceVolume() {
  m_output->setVolume(static_cast<float>(m_level) / 100.0);
}

void SP_Player::checkPlayerState() {
  auto state = m_player->playbackState();
  qInfo() << "QPlayer: " << state;
  qInfo() << "File: " << m_player->source().toLocalFile();
  qInfo() << "Level:" << m_output->volume()
          << " Device:" << m_output->device().description();
}

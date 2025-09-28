#ifndef SP_PLAYER_H
#define SP_PLAYER_H

// #include "../include/a_codelist.h"
#include "../include/protounit.h"
#include <QObject>

class QMediaPlayer;
class QAudioOutput;
class QTimer;

class SP_Player : public ProtoUnit {

public:
  explicit SP_Player(QObject *parent = nullptr);
  virtual ~SP_Player(){};

public:
  void setLevel(int percent);
  void setOutput(int number);
  void setFilename(QString name);
  void initialize() override;

public slots:
  virtual void slotStart() override; // from dispatcher
  virtual void slotPause() override;
  virtual void slotStop() override;
  virtual void slotStatusCheck() override;
  virtual void setOpt1(QString) override;
  virtual void setOpt2(QString) override;
  virtual void setOpt3(QString) override;
  virtual void setOpt4(QString) override{};
  //-------------
  virtual void innerError() override;
  virtual void innerStateChg() override;

private:
  QString m_fileName;
  int m_level; // in percent
  int m_outputUnit;
  QMediaPlayer *m_player;
  QAudioOutput *m_output;
  QTimer *m_statusTimer; // crutch while cannot connect QPlayer signals to local
                         // slots
  bool isPlaying{false};
  bool isDeviceSet{false};
  bool isFileSet{false};
  bool isDataChanged{false};
  // QAudioDevice *m_device;

private:
  bool isPlrDataSet() const { return (isFileSet && isDeviceSet); }
  void setOutput();
  void playPrepare();
  void inline setDeviceVolume();
  void inline checkPlayerState();
};

#endif // SP_PLAYER_H

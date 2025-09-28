// CLASS VD_Unit realizes protounit interface to voicedetector application
// this class implements execution policies:
// - it can work in test mode saving additional data -t- key in CL parameters
// - it can process only single task file from INI and stop after -s- key in CL
// parameters
// - it can work as standard AP-application under handler by UDPLink processing
// all input tasks

#ifndef VD_UNIT_H
#define VD_UNIT_H

#include "../include/protounit.h"
#include "vd_init.h"
#include "vd_voicedetector.h"
#include "../include/callbackproto.h"

class VD_Unit : public ProtoUnit, CallBackProto {
public:
  explicit VD_Unit(QObject *parent = nullptr);
  virtual void initialize() override; // create and initialize VD
  void setVDInit(VD_Init *p) { p_init = p; };
  virtual void acceptRESULT(const std::string &s ) override;

private:
  vd_detector::VD_VoiceDetector m_vd;
  VD_Init *p_init{nullptr}; // for ini data file read
  std::string m_taskFile;   // WAV filename for processing
  std::string m_paramFile;  // INI filename for VD setup

  bool isSingleShot{false};
  bool isDetected{false};
  bool isProcessingNow{false};
  bool isTaskSet{false};

  // ProtoUnit interface
public slots:
  virtual void slotStart() override; // set Option1 as filename to process
  virtual void slotStop() override;  // means close and quit
  virtual void slotStatusCheck() override;
  virtual void setOpt1(QString) override; // input WAV filename to process
  virtual void
      setOpt2(QString) override; // input INI filename with new VD parameters
  virtual void setOpt3(QString) override;
  virtual void setOpt4(QString) override;
  virtual void innerError() override;
  virtual void innerStateChg() override;

private:
  void setupVD(); // set all coeffs (when new INI file given)
};

#endif // VD_UNIT_H

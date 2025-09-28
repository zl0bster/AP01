#include "vd_init.h"
#include "../../import/ini.h"

VD_Init::VD_Init(QObject *parent, QStringList args) : InitUnit{parent, args} {
  p_vdData = QSharedPointer<VD_Coef_Frame>(new VD_Coef_Frame);
  p_nmData = QSharedPointer<NM_Coef_Frame>(new NM_Coef_Frame);
  p_aaData = QSharedPointer<AA_Coef_Frame>(new AA_Coef_Frame);
}

void VD_Init::initialize() {
  parseArgs(m_args);
  this->readIniFile(m_iniFile);
}

void VD_Init::readIniFile(QString iniFileName) {
  std::string fileName{iniFileName.toStdString()};
  ini::File configFile;
  try {
    qInfo() << fileName;
    configFile = ini::open(iniFileName.toStdString());
  } catch (const std::invalid_argument &e) {
    qDebug() << e.what();
    return;
  }

  //----- #todo split reading procedures to separate methods
  //----- # problem is to export INI.H realization to class description in .h
  // file

  m_testWAVfile = configFile["TestConfig"]["WAVFile"];
  qInfo() << "File to process: " << m_testWAVfile;

  p_nmData->NM_weightRefreshThreshold =
      configFile["NoiseModel"].get<float>("NM_weightRefreshThreshold");
  p_nmData->NM_noiseMixinCoef =
      configFile["NoiseModel"].get<float>("NM_noiseMixinCoef");
  p_nmData->NM_scopeLength =
      configFile["NoiseModel"].get<float>("NM_scopeLength");
  qInfo() << "NoiseModel coef: " << p_nmData->NM_weightRefreshThreshold << ", "
          << p_nmData->NM_scopeLength << ", " << p_nmData->NM_noiseMixinCoef;
  ;

  p_vdData->VD_ProbabilityThreshold =
      configFile["VoDec"].get<float>("VD_ProbabilityThreshold");
  p_vdData->VD_powerThreshold =
      configFile["VoDec"].get<float>("VD_powerThreshold");
  p_vdData->VD_vocalRangeLoFreq =
      configFile["VoDec"].get<float>("VD_vocalRangeLoFreq");
  p_vdData->VD_vocalRangeHiFreq =
      configFile["VoDec"].get<float>("VD_vocalRangeHiFreq");
  qInfo() << "Voice Detector coeff: " << p_vdData->VD_vocalRangeLoFreq << ", "
          << p_vdData->VD_vocalRangeHiFreq << ", "
          << p_vdData->VD_powerThreshold << ", "
          << p_vdData->VD_ProbabilityThreshold;

  p_aaData->AA_freqTol = configFile["ArtifAn"].get<float>("AA_freqTol");
  p_aaData->AA_pauseTimingThr =
      configFile["ArtifAn"].get<float>("AA_pauseTimingThr");
  p_aaData->AA_signalTimingThr =
      configFile["ArtifAn"].get<float>("AA_signalTimingThr");
  qInfo() << "Artifact Analizer coef: " << p_aaData->AA_freqTol << ", "
          << p_aaData->AA_signalTimingThr << ", "
          << p_aaData->AA_pauseTimingThr;
  //-----
  InitUnit::readIniFile(iniFileName);
}

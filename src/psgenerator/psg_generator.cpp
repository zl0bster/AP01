#include "psg_generator.h"
#include <QtMath>
#include <cassert>
#include <limits>

PSG_Generator::PSG_Generator(QSharedPointer<WavFileData> fd,
                             QSharedPointer<PSGenData> gd)
    : p_fileData(fd), p_genData(gd) {}

bool PSG_Generator::generate(QByteArray *b) {
  if (!areSettingsValid())
    return false;
  bool resultState = true;
  b->clear();

  assert(p_fileData->bytesPerSample == 2 || p_fileData->bytesPerSample == 4);

  size_t numFrames = p_fileData->sampleRate * p_genData->durationMSec / 1000;
  qInfo() << "Samples qty: " << numFrames;

  // vector for fading of signal at the ends
  std::vector<float> faderCoefVec(numFrames, 1.0);
  size_t faderZone = static_cast<size_t>(0.4 * (float)(numFrames));
  for (size_t i = 0; i < faderZone; ++i){
      float value = (float)(i)/(float)(faderZone);
      faderCoefVec[i]=value;
      faderCoefVec[numFrames-1-i]=value;
  }
  //  set flags for freq 2 & 3
  bool fr2 = (p_genData->freq2 > 0);
  bool fr3 = (p_genData->freq3 > 0);
  float adjustCoef = 1.0 / (1.0 + 1.0 * fr2 + 1.0 * fr3);
  // set coeffs for all freqs
  float coefFr1 = M_2_PI * p_genData->freq1 / p_fileData->sampleRate;
  float coefFr2 = M_2_PI * p_genData->freq2 / p_fileData->sampleRate;
  float coefFr3 = M_2_PI * p_genData->freq3 / p_fileData->sampleRate;
  //-------------------
  // set sequence generators for different frame type
  auto genInt = [&]() {
    auto iData = new (std::nothrow) int16_t[numFrames];
    if (iData == nullptr) {
      resultState = false;
      qDebug() << "Cannot allocate memory";
      return;
    }
    qInfo() << "generating int16 signal";
    float maxAmpl = static_cast<float>(std::numeric_limits<int16_t>::max());
    for (size_t i = 0; i < numFrames; ++i) {
      float x = qSin(coefFr1 * static_cast<float>(i % p_fileData->sampleRate));
      if (fr2)
        x += qSin(coefFr2 * static_cast<float>(i % p_fileData->sampleRate));
      if (fr3)
        x += qSin(coefFr3 * static_cast<float>(i % p_fileData->sampleRate));
      x *= adjustCoef;
      iData[i] = static_cast<int16_t>(maxAmpl * x * faderCoefVec[i]);
    }
    b->setRawData(reinterpret_cast<char *>(iData), numFrames * sizeof(int16_t));
  };

  auto genFloat = [&]() {
    auto fData = new (std::nothrow) float[numFrames];
    if (fData == nullptr) {
      resultState = false;
      qDebug() << "Cannot allocate memory";
      return;
    }
    qInfo() << "generating float signal";
    for (size_t i = 0; i < numFrames; ++i) {
      float x = qSin(coefFr1 * static_cast<float>(i % p_fileData->sampleRate));
      if (fr2)
        x += qSin(coefFr2 * static_cast<float>(i % p_fileData->sampleRate));
      if (fr3)
        x += qSin(coefFr3 * static_cast<float>(i % p_fileData->sampleRate));
      x *= adjustCoef;
      fData[i] = x * faderCoefVec[i];
    }
    b->setRawData(reinterpret_cast<char *>(fData), numFrames * sizeof(float));
  };
  //---------------------
  //  set array and fill it
  switch (p_fileData->bytesPerSample) {
  case 2:
    genInt();
    break;
  default:
    genFloat();
    break;
  }
  qInfo() << "PS raw size: " << b->size();
  qInfo() << "Result state: " << resultState;
  return resultState;
}

bool PSG_Generator::areSettingsValid() const {
  if (p_fileData.isNull() || p_genData.isNull()) {
    qDebug() << "PSGen settings are not set";
    return false;
  }
  bool isValid =
      (p_fileData->bytesPerSample == 2 || p_fileData->bytesPerSample == 4) &&
      p_fileData->sampleRate > 0 && p_genData->durationMSec > 0 &&
      p_genData->freq1 > 0;
  qInfo() << "PSGen settings validity is : " << isValid;
  return isValid;
}

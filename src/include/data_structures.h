#ifndef A_DATA_STRUCTURES_H
#define A_DATA_STRUCTURES_H

#include <QDateTime>
#include <QHostAddress>
#include <QString>
#include <QSharedPointer>

struct ALinkData {
  QHostAddress ownAddr;
  quint16 ownPort;
  QHostAddress dispAddr;
  quint16 dispPort;
  uint32_t unitID;
};

struct ARecData {
  QString projectName;
  QString chanID; // channel label
  QString resultPath;
  uint32_t sorsID; // audio source number ID
  uint32_t freqS;
  uint32_t recDurationSec; // total record time until shut down
  uint32_t chunkSec;       //  file chunk duration
  uint32_t bytesPerSample; //  1/2/3/4 => 8 - 16 - 24 - 32
};

struct WavFileData {
  QString fileName;
  QString resultPath;
  uint32_t sampleRate;
  uint32_t bytesPerSample; // record quality according Qt format
};

struct PSGenData {
  uint32_t durationMSec; // signal duration milliseconds
  uint32_t freq1;        // required
  uint32_t freq2;        // 0 if not
  uint32_t freq3;        // 0 if not
};

struct SPlayerData{
    QString fileNamePath;
    uint32_t outputID;
    uint32_t level;
};

struct DataPtrs{
    QSharedPointer<ARecData> recData;
    QSharedPointer<WavFileData> wavFileData;
    QSharedPointer<PSGenData> psgenData;
    QSharedPointer<SPlayerData> splayData;
};
#endif // A_DATA_STRUCTURES_H

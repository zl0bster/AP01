#ifndef PSG_INIT_H
#define PSG_INIT_H

#include "../include/data_structures.h"
#include "../include/initunit.h"
#include <QObject>
#include <QSharedPointer>

class PSG_Init : public InitUnit {
public:
  explicit PSG_Init(QObject *parent = nullptr, QStringList args = {});
  virtual void readIniFile(QString iniFileName) override;
  QSharedPointer<WavFileData> getFileData() const { return p_fileData; }
  QSharedPointer<PSGenData> getGenData() const { return p_genData; }

private:
  QSharedPointer<WavFileData> p_fileData;
  QSharedPointer<PSGenData> p_genData;
};

#endif // PSG_INIT_H

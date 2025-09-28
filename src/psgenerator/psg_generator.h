#ifndef PSG_GENERATOR_H
#define PSG_GENERATOR_H
#include "../include/data_structures.h"
#include <QByteArray>
#include <QSharedPointer>

class PSG_Generator {
public:
  explicit PSG_Generator(QSharedPointer<WavFileData> fd = nullptr,
                         QSharedPointer<PSGenData> gd = nullptr);

  void setFileData(QSharedPointer<WavFileData> pfd) { p_fileData = pfd; };
  void setGenData(QSharedPointer<PSGenData> pgd) { p_genData = pgd; };
  bool generate(QByteArray * b);

private:
  QSharedPointer<WavFileData> p_fileData;
  QSharedPointer<PSGenData> p_genData;

  bool areSettingsValid()const;
};

#endif // PSG_GENERATOR_H

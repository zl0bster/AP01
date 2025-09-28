#ifndef BM_HANDLERPSGENERATOR_H
#define BM_HANDLERPSGENERATOR_H

#include "bm_handlergeneral.h"
#include <QObject>

class BM_HandlerPSGenerator : public BM_HandlerGeneral {
public:
  explicit BM_HandlerPSGenerator(QObject *parent = nullptr);
  ~BM_HandlerPSGenerator() = default;
  void setWavData(QSharedPointer<WavFileData> p) { p_wFileData = p; }
  void setGenData(QSharedPointer<PSGenData> p) { p_genData = p; }
public slots:
  virtual void getTask(DataPtrs p) override {
    p_wFileData = p.wavFileData;
    p_genData = p.psgenData;
    createINIfile();
    m_taskArg = m_unitINIfile;
    qInfo ("HANDLER_GEN: data stored");
  };

protected:
  virtual void createINIfile() override;

private:
  QSharedPointer<WavFileData> p_wFileData;
  QSharedPointer<PSGenData> p_genData;
  int m_taskCounter{0};
};

#endif // BM_HANDLERPSGENERATOR_H

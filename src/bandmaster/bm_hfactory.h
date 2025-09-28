#ifndef BM_HFACTORY_H
#define BM_HFACTORY_H

#include "bm_handlerrecorder.h"
#include "bm_handlerpsgenerator.h"
#include "bm_handlersplayer.h"
#include <QObject>
#include <QString>
#include <map>

class BM_hFactory : public QObject {
  Q_OBJECT
public:
  explicit BM_hFactory(QObject *parent = nullptr);
  void initRHFactory(QString iniFile);
  BM_HandlerRecorder *getRecHandler();
  BM_HandlerPSGenerator *getGenHandler();
  BM_HandlerSPlayer * getPlrHandler();

private:
  struct BM_HandlerTypeDescription {
    QString unitTypeName;
    QString unitPGMname;
    QString unitPGMpath;
  };

  std::map<BM_HandlerType, BM_HandlerTypeDescription> m_unitDescript;
  bool isConfigured{false};
  size_t m_IDCounter;
  uint16_t m_HostPortCounter;
  uint16_t m_UnitPortCounter;
  QString m_HostIP;
  QString m_UnitIP;

private:
  void fillHandlerData( BM_HandlerType t, BM_HandlerGeneral *h, QSharedPointer<ALinkData> l);

signals:
};

#endif // BM_HFACTORY_H

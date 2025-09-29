#ifndef A_INIT_H
#define A_INIT_H

#include "../include/data_structures.h"

#include <QObject>
#include <QSharedPointer>
#include <QString>

class A_Init : public QObject {
  Q_OBJECT
public:
  explicit A_Init(QObject *parent = nullptr, QStringList args = {});
  virtual ~A_Init() = default;
  void initialize();
  QSharedPointer<ALinkData> getLinkData() const {return p_linkData;}
  QSharedPointer<ARecData> getRecData() const {return p_recData;}

  bool isPaused() const;

  signals:

private:
  QSharedPointer<ALinkData> p_linkData;
  QSharedPointer<ARecData> p_recData;
  QString iniFile;
  bool aPaused;

private:
  void readIniFile();
};

#endif // A_INIT_H

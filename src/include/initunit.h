/* Class InitUnit is a prototype for all initializing classes.
* It provides basic realization of CL-args processing/
* It provides INI UDPLink section reading also - it is general for all units.
* This class should be expanded by specific INI parameters reading method
*/
#ifndef INITUNIT_H
#define INITUNIT_H

#include "data_structures.h"
#include <QObject>
#include <QSharedPointer>

class InitUnit : public QObject {
  Q_OBJECT
public:
  explicit InitUnit(QObject *parent = nullptr, QStringList args = {});
  virtual ~InitUnit(){};
  virtual void initialize();
  virtual void readIniFile(QString iniFileName);
  bool isPaused() const { return m_paused; }
  bool isTestMode() const { return m_testModeOn; }
  QSharedPointer<ALinkData> getLinkData() const { return p_linkData; }

protected:
  virtual void parseArgs(QStringList args);

  QSharedPointer<ALinkData> p_linkData;
  QString m_iniFile;
  QStringList m_args;
  bool m_paused{true};
  bool m_testModeOn{false};
};

#endif // INITUNIT_H

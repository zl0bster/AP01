// class VD_Init overrides InitUnit to provide
// specific INI.data reading and CL-args processing
// VD_Init uses parent's realization of readINI
#ifndef VD_INIT_H
#define VD_INIT_H

#include "../include/initunit.h"
#include "vd_coef.h"

class VD_Init : public InitUnit {
public:
  explicit VD_Init(QObject *parent = nullptr, QStringList args = {});
  virtual ~VD_Init() override {}
  virtual void initialize() override;
  virtual void readIniFile(QString iniFileName) override;
  QSharedPointer<NM_Coef_Frame> getNMData() const { return p_nmData; }
  QSharedPointer<VD_Coef_Frame> getVDData() const { return p_vdData; }
  QSharedPointer<AA_Coef_Frame> getAAData() const { return p_aaData; }
  std::string getTestWAVname() const { return m_testWAVfile; }

private:
  QSharedPointer<NM_Coef_Frame> p_nmData;
  QSharedPointer<VD_Coef_Frame> p_vdData;
  QSharedPointer<AA_Coef_Frame> p_aaData;
  std::string m_testWAVfile;
};

#endif // VD_INIT_H

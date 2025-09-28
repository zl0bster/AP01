// VD_Core is thin adapter to CoreUnit for using specified
// ProtoUnit realization - VD_Unit

#ifndef VD_CORE_H
#define VD_CORE_H

#include "vd_init.h"
#include "vd_unit.h"
#include <../include/coreunit.h>

class VD_Core : public CoreUnit {
public:
  explicit VD_Core(QObject *parent, QStringList args);
  virtual void initialize() override;

private:
  VD_Unit *m_vdunit;
  VD_Init *m_vdinit;
};

#endif // VD_CORE_H

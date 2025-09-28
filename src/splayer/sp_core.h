#ifndef SP_CORE_H
#define SP_CORE_H

// #include <QObject>
#include "../include/coreunit.h"
// class InitUnit;
// class DispUnit;
class SP_Player;

class SP_Core : public CoreUnit
{
public:
    SP_Core(QObject *parent, QStringList args);

    virtual void initialize() override;

private:
//     InitUnit * m_init;
//     DispUnit * m_disp;
    SP_Player * m_plr;

};

#endif // SP_CORE_H

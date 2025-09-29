/* Class CoreUnit is the entry point of unit application realization
 * It should be adjusted by it's adaptor, connecting to specific
 * ProtoUnit-interface - class
*/
#ifndef COREUNIT_H
#define COREUNIT_H

#include <QObject>
class InitUnit;
class DispUnit;
class ProtoUnit;

class CoreUnit : public QObject
{
    Q_OBJECT
public:
    explicit CoreUnit(QObject *parent, QStringList args);
    virtual ~CoreUnit() = default;
    virtual void initialize();
    void setUnit(ProtoUnit * p){m_unit=p;};
protected:
    InitUnit * m_init;
    DispUnit * m_disp;
    ProtoUnit * m_unit;

};

#endif // COREUNIT_H

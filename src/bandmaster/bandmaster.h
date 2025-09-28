#ifndef BANDMASTER_H
#define BANDMASTER_H

#include <QObject>
#include "bandmastergui.h"

class BM_dispatcher;
class BM_MasterRecord;
class BM_MasterPSGen;
class BM_MasterSPlayer;

class BandMaster : public QObject
{
    Q_OBJECT
    BM_dispatcher * p_dispatcher;
    BM_MasterRecord * p_recorder;
    BM_MasterPSGen * p_psGen;
    BM_MasterSPlayer * p_splayer;
    BandMasterGUI * p_gui;
public:
    explicit BandMaster(QObject *parent = nullptr);// QApplication as parent for this
    ~BandMaster() {};
    void initialize(BandMasterGUI * gui);

signals:
};

#endif // BANDMASTER_H

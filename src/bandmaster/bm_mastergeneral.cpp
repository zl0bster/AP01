#include "bm_mastergeneral.h"

void BM_MasterGeneral::setP_handleMaster(
    GUIHandleMaster* newHandleMaster) {
  p_handleMaster = newHandleMaster;
}

void BM_MasterGeneral::setP_dispatcher(BM_dispatcher *newP_dispatcher) {
  p_dispatcher = newP_dispatcher;
}

BM_MasterGeneral::BM_MasterGeneral(QObject *parent) : QObject{parent} {}

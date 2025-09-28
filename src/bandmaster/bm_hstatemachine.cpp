#include "bm_hstatemachine.h"
#include "qdebug.h"
#include <cassert>

BM_hStateMachine::BM_hStateMachine(QObject *parent) : QObject{parent} {}

void BM_hStateMachine::initialState() {
  assert(!isSMacivated);
  qInfo() << "RH_SMACHINE: activated";
  isSMacivated = true;
  m_handlerState = BM_hState::Starting;
  emit initHObjects();
}

void BM_hStateMachine::requestStateChg(BM_hState s) {
  // assert(isSMacivated);
  std::map<BM_hState, fx_t> &currentStMap = BM_stMap[m_handlerState];
  auto action = currentStMap.find(s);
  if (action != currentStMap.end()) {
    if (currentStMap[s])
      currentStMap[s]();
    m_handlerState = s;
    qInfo() << "RH_SMACHINE: chg state by command to" << HCodeToStr(s);
  }
  if (m_handlerState == BM_hState::QPrsClosed) {
    emit closeHObjects();
    isSMacivated = false;
    emit SMfinished();
  }
}

void BM_hStateMachine::acceptReport(BM_uState s) {
  assert(isSMacivated);
  qInfo() << "RH_SMACHINE: report accepted" << static_cast<int>(s);
  std::map<BM_hState, fx_t> &currentStMap = BM_repMap[s];
  if (currentStMap.empty()) return;
  auto action = currentStMap.find(m_handlerState);
  if (action != currentStMap.end()) {
    if (currentStMap[m_handlerState])
      currentStMap[m_handlerState]();
    qInfo() << "RH_SMACHINE: chg state by report to"
             << HCodeToStr(m_handlerState);
  }
}

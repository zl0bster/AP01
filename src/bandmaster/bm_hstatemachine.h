#ifndef BM_HSTATEMACHINE_H
#define BM_HSTATEMACHINE_H

#include "../include/bm_codelist.h"
#include "bm_statemap.h"
#include <QObject>
#include <functional>
#include <map>

using fx_t = std::function<void()>;

class BM_hStateMachine : public QObject {
  Q_OBJECT

public:
  enum class SM_WorkMode { Process, Task };

private:
  BM_hState m_handlerState;
  bool isSMacivated{false};
  SM_WorkMode m_mode{SM_WorkMode::Process};

private:
  //------------------------
  std::map<BM_hState, std::map<BM_hState, fx_t>> BM_stMap{
      {BM_hState::Starting, {{BM_hState::Error, []() {}}}},
      {BM_hState::Idle,
       {{BM_hState::Processing, [this]() { emit startUnit(); }},
        {BM_hState::AboutToQuit, [this]() { emit closeUnit(); }},
        {BM_hState::Error, []() {}}}},
      {BM_hState::Processing,
       {{BM_hState::Idle, [this]() { emit pauseUnit(); }},
        {BM_hState::AboutToQuit, [this]() { emit closeUnit(); }},
        {BM_hState::Error, []() {}}}},
      {BM_hState::AboutToQuit, {{BM_hState::AppExit, []() {}}}},
      {BM_hState::Error, {{BM_hState::AppExit, []() {}}}},
      // {BM_hState::Error, {{BM_hState::AppExit, [this]() {emit
      // closeHObjects();}}}},
      // {BM_hState::AppExit, {{BM_hState::QPrsClosed, [this]() {emit
      // closeHObjects();}}}},
      {BM_hState::AppExit, {{BM_hState::QPrsClosed, []() {}}}},
      {BM_hState::QPrsClosed, {}}};

  std::map<BM_uState, std::map<BM_hState, fx_t>> BM_repMap{
      {BM_uState::Responce_ok, {}},
      {BM_uState::Stand_by,
       {{BM_hState::Starting, [this]() { m_handlerState = BM_hState::Idle; }},
        {BM_hState::Processing,
         [this]() { m_handlerState = BM_hState::Idle; }}}},
      {BM_uState::Processing,
       {{BM_hState::Starting,
         [this]() { m_handlerState = BM_hState::Processing; }},
        {BM_hState::Idle,
         [this]() { m_handlerState = BM_hState::Processing; }}}},
      {BM_uState::Result_saved,
       {{BM_hState::Processing,
         [this]() {
           if (m_mode == SM_WorkMode::Task)
             m_handlerState = BM_hState::Idle;
         }}}},
      {BM_uState::Negative_result,
       {{BM_hState::Processing,
        [this]() { m_handlerState = BM_hState::Idle; }}}},
      {BM_uState::Positive_result,
       {{BM_hState::Processing,
         [this]() { m_handlerState = BM_hState::Idle; }}}},
      {BM_uState::Device_error,
       {{BM_hState::Starting, [this]() { m_handlerState = BM_hState::Error; }},
        {BM_hState::Idle, [this]() { m_handlerState = BM_hState::Error; }},
        {BM_hState::Processing,
         [this]() { m_handlerState = BM_hState::Error; }}}},
      {BM_uState::Software_error,
       {{BM_hState::Starting, [this]() { m_handlerState = BM_hState::Error; }},
        {BM_hState::Idle, [this]() { m_handlerState = BM_hState::Error; }},
        {BM_hState::Processing,
         [this]() { m_handlerState = BM_hState::Error; }}}},
      {BM_uState::File_save_failed,
       {{BM_hState::Starting, [this]() { m_handlerState = BM_hState::Error; }},
        {BM_hState::Idle, [this]() { m_handlerState = BM_hState::Error; }},
        {BM_hState::Processing,
         [this]() { m_handlerState = BM_hState::Error; }}}},
      {BM_uState::File_open_error,
       {{BM_hState::Starting, [this]() { m_handlerState = BM_hState::Error; }},
        {BM_hState::Idle, [this]() { m_handlerState = BM_hState::Error; }},
        {BM_hState::Processing,
         [this]() { m_handlerState = BM_hState::Error; }}}},
      {BM_uState::Connection_problem,
       {{BM_hState::Starting, [this]() { m_handlerState = BM_hState::Error; }},
        {BM_hState::Idle, [this]() { m_handlerState = BM_hState::Error; }},
        {BM_hState::Processing,
         [this]() { m_handlerState = BM_hState::Error; }}}},
      {BM_uState::Quit_app,
       {{BM_hState::Starting,
         [this]() { m_handlerState = BM_hState::AboutToQuit; }},
        {BM_hState::Idle,
         [this]() { m_handlerState = BM_hState::AboutToQuit; }},
        {BM_hState::Processing,
         [this]() { m_handlerState = BM_hState::AboutToQuit; }},
        {BM_hState::Error,
         [this]() { m_handlerState = BM_hState::AboutToQuit; }}}},
  };
  //------------------------

public:
  explicit BM_hStateMachine(QObject *parent = nullptr);
  void initialState();
  BM_hState currentState() const { return m_handlerState; }
  bool isActive() const { return isSMacivated; }
  void setWorkMode(SM_WorkMode m) { m_mode = m; }
public slots:
  void requestStateChg(BM_hState s);
  void acceptReport(BM_uState s);

signals:
  void initHObjects();
  void startUnit();
  void pauseUnit();
  void closeUnit();
  void closeHObjects();
  void SMfinished();
};

#endif // BM_HSTATEMACHINE_H

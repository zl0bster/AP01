#ifndef BM_STATEMAP_H
#define BM_STATEMAP_H

#include <QString>

enum class BM_hState {
  Starting,
  Processing,
  Idle,
  Error,
  AboutToQuit,
  AppExit,
  QPrsClosed
};

QString inline HCodeToStr(const BM_hState c){
    static const std::unordered_map<BM_hState, QString> hStNames{
        {BM_hState::Starting, "Starting"},
        {BM_hState::Processing, "Process"},
        {BM_hState::Idle, "Idle"},
        {BM_hState::Error, "Error"},
        {BM_hState::AboutToQuit, "AboutToQuit"},
        {BM_hState::AppExit, "AppExit"},
        {BM_hState::QPrsClosed, "QProcessClosed"}
    };
    return hStNames.at(c);};

#endif // BM_STATEMAP_H

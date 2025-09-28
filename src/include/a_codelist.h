#ifndef A_CODELIST_H
#define A_CODELIST_H

#include <QString>

/* Audio Stream Recorder protocol*/

enum class ASR_statusCode { // used for component internal diagnostics
  Status_ok,
  File_saved,
  Format_unsupported,
  Connection_error,
  Device_error,
  Buffer_is_empty,
  File_open_error
};

QString inline ACodeToStr(ASR_statusCode c) {
  static const std::unordered_map<ASR_statusCode, QString> aCodeMap{
      {ASR_statusCode::Status_ok, "OK"},
      {ASR_statusCode::File_saved, "SAVED"},
      {ASR_statusCode::File_open_error, "OPEN_ERROR"},
      {ASR_statusCode::Connection_error, "CONN_ERROR"},
      {ASR_statusCode::Device_error, "DEV_ERROR"},
      {ASR_statusCode::Buffer_is_empty, "BUF_EMPTY"},
      {ASR_statusCode::Format_unsupported, "BAD_FORMAT"}};
  return QString{aCodeMap.at(c)};
};

#endif // A_CODELIST_H

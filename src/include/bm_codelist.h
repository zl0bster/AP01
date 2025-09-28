#ifndef BM_CODELIST_H
#define BM_CODELIST_H

#include <QString>
#include <cstdint>
#include <map>
enum class BM_uState { // used for handler diagnostics transfer
  Responce_ok,         // approve command acception, don't give it to core
  Processing,          // does the job
  Stand_by,            // exist but doesn't work
  Result_saved,        // continues the job
  Negative_result,     // if no valuable artifacts found in file
  Positive_result,     // task is done, arg may have value
  Connection_problem,  // error in link
  Device_error,        // sensor or other unit malfunction
  Software_error,      // component software internal error
  File_open_error,
  File_save_failed,
  Quit_app // step before close the application
};

uint32_t inline BcodeToInt(const BM_uState r) {
  return static_cast<uint32_t>(r);
}
BM_uState inline intToBcode(const uint32_t i) {
  return static_cast<BM_uState>(i);
}
QString inline BcodeToStr(const BM_uState r) {
  static const std::map<BM_uState, QString> unitStMap{
      {BM_uState::Stand_by, "IDLE"},
      {BM_uState::Processing, "PROCESS"},
      {BM_uState::Result_saved, "SAVED"},
      {BM_uState::Negative_result, "NEGATIVE"},
      {BM_uState::Positive_result, "POSITIVE"},
      {BM_uState::Device_error, "DEV ERR"},
      {BM_uState::Software_error, "SOFT ERR"},
      {BM_uState::File_open_error, "FILE OPEN ERR"},
      {BM_uState::File_save_failed, "FILE SAVE ERR"},
      {BM_uState::Connection_problem, "CONN ERR"},
      {BM_uState::Quit_app, "APP EXIT"}};
  return unitStMap.at(r);
}

enum class BM_command {
  Void_c,     // responce ok code (not used for transfer)
  Start,      // start or continue record after pause
  Pause,      // pause when recording
  Stop,       // stop record and close application
  Status,     // application status request
  SetOption1, // setting task options (signal level or filename)
  SetOption2,
  SetOption3,
  SetOption4,
  SetOption5
};

uint32_t inline BcomToInt(const BM_command c) {
  return static_cast<uint32_t>(c);
}
BM_command inline intToBcom(const uint32_t i) {
  return static_cast<BM_command>(i);
}

#endif // BM_CODELIST_H

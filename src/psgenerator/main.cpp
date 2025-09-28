#include "../include/bm_codelist.h"
#include "psg_dispatcher.h"
#include "psg_init.h"
#include <QCoreApplication>
#include <QStringList>
#include <QtLogging>
#include <stdio.h>

QtMessageHandler originalHandler = nullptr;

void logToFile(QtMsgType type, const QMessageLogContext &context,
               const QString &msg) {
  QString message = qFormatLogMessage(type, context, msg);
  static FILE *f = fopen("log_psgen.txt", "a");
  fprintf(f, "%s\n", qPrintable(message));
  fflush(f);

  if (originalHandler)
    originalHandler(type, context, msg);
}

int main(int argc, char *argv[]) {
  qSetMessagePattern("[%{time yyyyMMdd h:mm:ss.zzz}] - %{type} - "
                     "%{file}:%{function}:%{line} - "
                     "[[%{message}]]");
  originalHandler = qInstallMessageHandler(logToFile);
  QCoreApplication a(argc, argv);
  auto args = a.arguments();
  PSG_Init initializer(&a, args);
  initializer.initialize();
  PSG_Dispatcher d(&a, initializer.getFileData(), initializer.getLinkData(),
                   initializer.getGenData());
  if (!initializer.isPaused()) {
    d.recieveCommand(BcomToInt(BM_command::Start),
                     ""); // activate generation with current params
  }
  return a.exec();
}

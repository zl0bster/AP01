#include "vd_core.h"
#include <QCoreApplication>

QtMessageHandler originalHandler = nullptr;

void logToFile(QtMsgType type, const QMessageLogContext &context,
               const QString &msg) {
  QString message = qFormatLogMessage(type, context, msg);
  static FILE *f = fopen("log_vodec.txt", "w");
  fprintf(f, "%s\n", qPrintable(message));
  fflush(f);

  if (originalHandler)
    originalHandler(type, context, msg);
}
int main(int argc, char *argv[]) {
  qSetMessagePattern(
      "[%{time yyyyMMdd h:mm:ss.zzz}]  %{file}:%{function}:%{line} - "
      "[[%{message}]]");
  originalHandler = qInstallMessageHandler(logToFile);
  QCoreApplication a(argc, argv);
  VD_Core c(&a, a.arguments());
  c.initialize();
  return a.exec();
}

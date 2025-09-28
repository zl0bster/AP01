#include "a_dispatcher.h"
#include "a_init.h"
#include <QCoreApplication>
#include <QStringList>
#include <QtLogging>
#include <stdio.h>
#include <thread>
// #include <stdlib.h>

QtMessageHandler originalHandler = nullptr;

void logToFile(QtMsgType type, const QMessageLogContext &context,
               const QString &msg) {
  QString message = qFormatLogMessage(type, context, msg);

  auto id = (std::this_thread::get_id());
  std::stringstream ss;
  ss << id;
  auto filename = "log_arecorder_" + (ss.str()) + ".txt";
  static FILE *f = fopen(filename.c_str(), "w");
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
  auto args = a.arguments();
  A_Init initializer(&a, args);
  initializer.initialize();
  // qDebug()<<"MAIN: isPaused= "<< initializer.isPaused();
  A_Dispatcher d(&initializer, initializer.getLinkData(),
                 initializer.getRecData(), initializer.isPaused());

  QObject::connect(&a, SIGNAL(aboutToQuit()), &d, SLOT(stopRecord()));
  return a.exec();
}

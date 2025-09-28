#include "bandmaster.h"
#include "bandmastergui.h"

#include <QApplication>
#include <QtLogging>
#include <stdio.h>
QtMessageHandler originalHandler = nullptr;

void logToFile(QtMsgType type, const QMessageLogContext &context,
               const QString &msg) {
    QString message = qFormatLogMessage(type, context, msg);
    static FILE *f = fopen("log_bmaster.txt", "w");
    fprintf(f, "%s\n", qPrintable(message));
    fflush(f);

    if (originalHandler)
        originalHandler(type, context, msg);
}

int main(int argc, char *argv[])
{
    qSetMessagePattern(
        "[%{time yyyyMMdd h:mm:ss.zzz}] - %{type} - %{file}:%{function}:%{line} - "
        "[[%{message}]]");
    originalHandler = qInstallMessageHandler(logToFile);
    QApplication a(argc, argv);
    BandMasterGUI w;
    w.show();
    BandMaster bm;
    bm.initialize(&w);
    return a.exec();
}

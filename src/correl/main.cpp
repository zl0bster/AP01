#include "../../lib_win64/dspl.h"
#include <QCoreApplication>

int main(int argc, char *argv[]) {
  QCoreApplication a(argc, argv);

  void *hdspl;
  hdspl = dspl_load();
  if (!hdspl) {
    qWarning("libdspl loading error!");
    return -1;
  }
  dspl_info();
  dspl_free(hdspl);
  return a.exec();
}

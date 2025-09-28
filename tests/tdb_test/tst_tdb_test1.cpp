#include <QtTest>

// add necessary includes here
#include "../../src/vodec/tdbuffer.h"

class tdb_test1 : public QObject {
  Q_OBJECT

public:
  tdb_test1();
  ~tdb_test1();

private slots:
  void test_case1();

private:
};

tdb_test1::tdb_test1() {}

tdb_test1::~tdb_test1() {}

void tdb_test1::test_case1() {
  using namespace tdb;
  TDBuffer b;
  QCOMPARE_EQ(b.readyToWindowing(), false);
  b.setFs(250);
  QCOMPARE_EQ(b.fs(), 250);
  b.setWindowSize(32);
  QCOMPARE_EQ(b.windowSize(), 32);
  b.setHopValue(16);
  QCOMPARE_EQ(b.startWindowing(), false);
  b.initializeBuf(128);
  QCOMPARE_EQ(b.readyToWindowing(), true);
  QCOMPARE_EQ(b.startWindowing(), true);
}

QTEST_APPLESS_MAIN(tdb_test1)

#include "tst_tdb_test1.moc"

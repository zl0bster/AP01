#include <QCoreApplication>
#include <QtTest>

// add necessary includes here
#include "..\..\src\include\rbuffer.h"
// #include <algorithm>
#include <vector>

class RBuffer_test : public QObject {
  Q_OBJECT
public:
  RBuffer_test(QObject *parent = nullptr) : QObject(parent){};
  virtual ~RBuffer_test(){};

private slots:
  void test_case1();
};

void RBuffer_test::test_case1() {
  Rbuffer<int, 4> b;
  std::vector<int> sample{1, 2, 3, 4, 5, 6};
  std::vector<int> result{5, 6, 1, 2};
  for (auto item : sample) {
    b.pushback(item);
  }
  // int i{0};
  bool cp_result{true};
  for (size_t i = 0; i < b.size(); ++i) {
    cp_result &= (b[i] == result[i]);
  }
  QCOMPARE(cp_result, true);
}

QTEST_MAIN(RBuffer_test)

#include "../../build/debug/bm_test/moc/moc_tst_rbuffer.h"

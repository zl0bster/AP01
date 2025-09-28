#include <QCoreApplication>
#include <QtTest>

// add necessary includes here
#include "..\..\src\bandmaster\bandmastergui.h"
#include "..\..\src\include\bm_guihandles.h"

class bm_gui_test : public QObject {
  Q_OBJECT

public:
  bm_gui_test();
  ~bm_gui_test();

private slots:
  // void initTestCase();
  // void cleanupTestCase();
  void test_case1();

private:
  BandMasterGUI bg;
  GUIHandleMaster* hm;
};

bm_gui_test::bm_gui_test() {
  // bg=new BandMasterGUI;
    bg.show();
  hm = bg.getHandleMaster();
}

bm_gui_test::~bm_gui_test() {
  bg.close();
  // delete bg;
}

// void bm_gui_test::initTestCase() {}

// void bm_gui_test::cleanupTestCase() {}

void bm_gui_test::test_case1() {
  QString projectName("project1");
  bg.ui->nameEdit->setText(projectName);
  QCOMPARE(hm->get_pp()->projectName(), projectName);
}


#include "tst_bm_gui_test.moc"


QTEST_MAIN(bm_gui_test)

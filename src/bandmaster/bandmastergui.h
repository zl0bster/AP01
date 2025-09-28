#ifndef BANDMASTERGUI_H
#define BANDMASTERGUI_H

#include "..\..\build\debug\bandmaster\ui\ui_bandmastergui.h" // #TODO change folder for release when it is needed
#include "..\include\bm_guihandles.h"
// #include "bm_dispatcher.h"
#include <QColor>
#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QTimer>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui {
class BandMasterGUI;
}
QT_END_NAMESPACE

using namespace QColorConstants::Svg;

// #TODO - debug handles test case

static const QString SoundFileFILTER{"Sounds (*.wav)"};
class bm_gui_test;

class BandMasterGUI : public QMainWindow {
  Q_OBJECT

protected:
  Ui::BandMasterGUI *ui;

public:
  BandMasterGUI(QWidget *parent = nullptr);
  ~BandMasterGUI();
  friend bm_gui_test;

public slots:
  void textOutput(QString s) { ui->plainTextEdit->appendPlainText(s); }
  // --- slots from GUI ---
  void slotStop();
  void slotStart();
  void slotToggle();
  void slotChooseFolder(); // filedialogue and set text label
  void slotInpSourceChg(); // set active for start button
  void slotChoosePSFile1();// filedialogue ang set text edit
  void slotChoosePSFile2();// filedialogue ang set text edit

signals:
  // void setFolder(QString);

  //======= handles classes ========
private:
  // project settings
  class GUI_PP : public ProjectParameters {
    BandMasterGUI *m_p;

  public:
    GUI_PP(BandMasterGUI *p) : m_p(p){};
    ~GUI_PP() override{}; // = default;
    QString projectName() const override { return m_p->ui->nameEdit->text(); }
    QString folderPath() const override { return m_p->m_dirPath; };
  };

  // sources settings
  class GUI_RS : public RecSources {
    BandMasterGUI *m_p;

  public:
    GUI_RS(BandMasterGUI *p) : m_p(p){};
    ~GUI_RS() override{}; //= default
    int sourceRec1() const override {
      return m_p->ui->ch1combo->currentIndex();
    };
    int sourceRec2() const override {
      return m_p->ui->ch2combo->currentIndex();
    };
    int sourceRec3() const override {
      return m_p->ui->ch3combo->currentIndex();
    };
    int sourceRec4() const override {
      return m_p->ui->ch4combo->currentIndex();
    };
  };

  // quality settings
  class GUI_RP : public RecParameters {
    BandMasterGUI *m_p;

  public:
    GUI_RP(BandMasterGUI *p) : m_p(p){};
    ~GUI_RP() override{}; //= default;
    int bitFormat() const override {
      return m_p->ui->bitFormatBox->currentText().toInt();
    }
    int sampleRate() const override {
      return m_p->ui->sampleRateBox->currentText().toInt();
    }
    int fileDuration() const override {
      return m_p->ui->fileDurationSpinBox->value();
    }
    int recTime() const override { return m_p->ui->timerSpinBox->value(); }
  };

  class GUI_GP : public GenParameters {
      BandMasterGUI *m_p;

  public:
      GUI_GP(BandMasterGUI *p) : m_p(p){};
      ~GUI_GP() override{};
      // GenParameters interface
  public:
      virtual int signalDuration() const override {
          return m_p->ui->pulseDurationSpinBox->value();
      };
      virtual int freq1() const override {
          return m_p->ui->PSFreqSpinBox1->value();
      };
      virtual int freq2() const override {
          return m_p->ui->PSFreqSpinBox2->value();
      };
      virtual int freq3() const override {
          return m_p->ui->PSFreqSpinBox3->value();
      };
  };

  class GUI_PD : public PlayerData {
      BandMasterGUI *m_p;

  public:
      GUI_PD(BandMasterGUI *p) : m_p(p){};
      ~GUI_PD() override{};
      // PlayerData interface
  public:
      virtual bool isFile1active() const override {
          return m_p->ui->PSigCheckBox1->isChecked();
      };
      virtual bool isFile2active() const override {
          return m_p->ui->PSigCheckBox2->isChecked();
      };
      virtual int playDevice() const override {
          return m_p->ui->outputComboBox->currentIndex();
      };
      virtual QString psfile1() const override {
          return m_p->ui->PSFileLineEdit1->text();
      };
      virtual int psperiod1() const override {
          return m_p->ui->PSPeriodSpinBox1->value();
      };
      virtual int pslevel1() const override {
          return m_p->ui->PSLevelSpinBox1->value();
      };
      virtual QString psfile2() const override {
          return m_p->ui->PSFileLineEdit2->text();
      };
      virtual int psperiod2() const override {
          return m_p->ui->PSPeriodSpinBox2->value();
      };
      virtual int pslevel2() const override {
          return m_p->ui->PSLevelSpinBox2->value();
      };
  };

  // command interface settings
  class GUI_CIRec : public CommandIntarface {
    BandMasterGUI *m_p;

  public:
    GUI_CIRec(BandMasterGUI *p) : m_p(p){};
    ~GUI_CIRec() override{}; //= default;
    void MSGOutput(QString text) override { m_p->textOutput(text); }
  };

  class GUI_CIGen : public CommandIntarface {
      BandMasterGUI *m_p;

  public:
      GUI_CIGen(BandMasterGUI *p) : m_p(p){};
      ~GUI_CIGen() override{}; //= default;
      void MSGOutput(QString text) override { m_p->textOutput(text); }
  };

  //==================================================
  // ---- handles infrastructure to be initialized ------
  GUIHandleMaster *m_handleMaster;
  GUIHandles m_handles;
  void setupHandles();
  auto getCIRec() { return static_cast<GUI_CIRec *>(m_handles.p_ciRec); }

  // ----------- give GUI handles --------------
public:
  GUIHandleMaster *getHandleMaster() const { return m_handleMaster; }
  //==================================================

private:
  const QVector<QString> sampleRates = {"16000", "22050", "32000",
                                        "44100", "48000", "96000"};
  const QVector<QString> bitFormats = {"16", "24", "32"};
  QString m_dirPath;
  bool m_isProcessNow{false};

  //--------------
  void setupComboFreq(QComboBox *item);
  void setupComboBit(QComboBox *item);
  void setupComboOutputList(QComboBox *item);
  void setupComboInputList(QComboBox *item);
  void setupRecSection();
  void setupPSGenSection();
  void setupPSPlayerSection();

protected:
  virtual void resizeEvent(QResizeEvent *event);
};
#endif // BANDMASTERGUI_H

//--------------------

// const std::map<BM_uState, QColor> stateColorMap{
//     {BM_uState::Stand_by, darkcyan},
//     {BM_uState::Processing, green},
//     {BM_uState::Result_saved, darkgreen},
//     {BM_uState::Device_error, orange},
//     {BM_uState::Software_error, red},
//     {BM_uState::File_open_error, orange},
//     {BM_uState::File_save_failed, orange},
//     {BM_uState::Connection_problem, orange},
//     {BM_uState::Quit_app, black}};
//-------------------------------

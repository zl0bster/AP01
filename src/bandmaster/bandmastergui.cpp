#include "bandmastergui.h"
#include "ui_bandmastergui.h"
#include <QAudioDevice>
#include <QDebug>
#include <QFileDialog>
#include <QPalette>
#include <QTimer>
#include <QtMultimedia/QMediaDevices>

BandMasterGUI::BandMasterGUI(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::BandMasterGUI) {
  ui->setupUi(this);
  this->setWindowTitle("MultiChannelRecorder");
  setupRecSection();
  setupHandles();
  setupPSGenSection();
  setupPSPlayerSection();
}

BandMasterGUI::~BandMasterGUI() {
  delete ui;
}

void BandMasterGUI::slotStop() {
  // adjust view
  m_isProcessNow = false;
  ui->startButton->setText(tr("Start"));
  ui->paramFrame->setEnabled(true);
  ui->sourcesFrame->setEnabled(true);
  ui->PSGenFrame->setEnabled(true);
  emit getCIRec()->stop();
}

void BandMasterGUI::slotStart() {
  textOutput("GUI: activate units");
  qInfo() << "GUI: start process";
  // adjust view
  m_isProcessNow = true;
  ui->startButton->setText(tr("Stop"));
  ui->paramFrame->setEnabled(false);
  ui->sourcesFrame->setEnabled(false);
  ui->PSGenFrame->setEnabled(false);
  emit getCIRec()->start();
}

void BandMasterGUI::slotToggle() {
  if (!m_isProcessNow) {
    slotStart();
  } else {
    slotStop();
  }
}

void BandMasterGUI::slotChooseFolder() {
  QString tmp = QFileDialog::getExistingDirectory(this, tr("Choose folder:"));
  if (!tmp.isEmpty()) {
    m_dirPath = tmp + "/";
    ui->folderPathLabel->setText(m_dirPath);
  }
}

void BandMasterGUI::slotInpSourceChg() {
  bool isReadyToStart =
      ui->ch1combo->currentIndex() || ui->ch2combo->currentIndex() ||
      ui->ch3combo->currentIndex() || ui->ch4combo->currentIndex();
  ui->startButton->setDisabled(!isReadyToStart);
}

void BandMasterGUI::slotChoosePSFile1() {
  QString tmp = QFileDialog::getOpenFileName(this, tr("Choose Probe Signal 1"),
                                             m_dirPath, SoundFileFILTER);
  if (!tmp.isEmpty()) {
    ui->PSFileLineEdit1->setText(tmp);
  }
}

void BandMasterGUI::slotChoosePSFile2() {
  QString tmp = QFileDialog::getOpenFileName(this, tr("Choose Probe Signal 2"),
                                             m_dirPath, SoundFileFILTER);
  if (!tmp.isEmpty()) {
    ui->PSFileLineEdit2->setText(tmp);
  }
}

void BandMasterGUI::setupComboFreq(QComboBox *item) {
  for (const auto &val : sampleRates) {
    item->addItem(val);
  }
}

void BandMasterGUI::setupComboBit(QComboBox *item) {
  for (const auto &val : bitFormats) {
    item->addItem(val);
  }
}

void BandMasterGUI::setupComboOutputList(QComboBox *item) {
  const auto deviceInfos = QMediaDevices::audioOutputs();
  item->addItem("---");
  for (const QAudioDevice &devInfo : deviceInfos) {
    item->addItem(devInfo.description());
  }
}

void BandMasterGUI::setupComboInputList(QComboBox *item) {
  const auto deviceInfos = QMediaDevices::audioInputs();
  item->addItem("---");
  for (const QAudioDevice &devInfo : deviceInfos) {
    item->addItem(devInfo.description());
  }
}

void BandMasterGUI::setupRecSection() {
  setupComboInputList(ui->ch1combo);
  setupComboInputList(ui->ch2combo);
  setupComboInputList(ui->ch3combo);
  setupComboInputList(ui->ch4combo);
  ui->ch1combo->setCurrentIndex(1); // set 1st source as default
  setupComboBit(ui->bitFormatBox);
  setupComboFreq(ui->sampleRateBox);
  slotInpSourceChg();
  ui->nameEdit->setText("Rec");
  ui->startButton->setText(tr("Start"));
  ui->startButton->setDefault(true);
  ui->ch1combo->activated(1);
  ui->outputComboBox->activated(1);
  QDir dir;
  m_dirPath = dir.absolutePath();
  ui->folderPathLabel->setText(m_dirPath);
  ui->timerSpinBox->setValue(14);
  ui->fileDurationSpinBox->setValue(6);
}

void BandMasterGUI::setupPSGenSection() {
  // connect button clicked to CI start signal
  connect(ui->PSGen_pushButton, SIGNAL(clicked()), m_handles.p_ciPSG,
          SIGNAL(start()));
}

void BandMasterGUI::setupPSPlayerSection() {
  // fill output device
  // set 1st source by default
  setupComboOutputList(ui->outputComboBox);
  ui->outputComboBox->activated(1);
  connect(ui->PSFileSelectButton1, SIGNAL(clicked()), this,
          SLOT(slotChoosePSFile1()));
  connect(ui->PSFileSelectButton2, SIGNAL(clicked()), this,
          SLOT(slotChoosePSFile2()));
  ui->PSLevelSpinBox1->setValue(75);
  ui->PSLevelSpinBox2->setValue(75);

  // make fields inactive without checkbox toggled
  connect(ui->PSigCheckBox1, SIGNAL (toggled(bool)), ui->PSFileLineEdit1, SLOT (setEnabled(bool)));
  connect(ui->PSigCheckBox1, SIGNAL (toggled(bool)), ui->PSFileSelectButton1, SLOT (setEnabled(bool)));
  connect(ui->PSigCheckBox1, SIGNAL (toggled(bool)), ui->PSLevelSpinBox1, SLOT (setEnabled(bool)));
  connect(ui->PSigCheckBox1, SIGNAL (toggled(bool)), ui->PSPeriodSpinBox1, SLOT (setEnabled(bool)));
  connect(ui->PSigCheckBox1, SIGNAL (toggled(bool)), ui->PSLevelHorizontalSlider1, SLOT (setEnabled(bool)));
  ui->PSigCheckBox1->toggled(false);

  connect(ui->PSigCheckBox2, SIGNAL (toggled(bool)), ui->PSFileLineEdit2, SLOT (setEnabled(bool)));
  connect(ui->PSigCheckBox2, SIGNAL (toggled(bool)), ui->PSFileSelectButton2, SLOT (setEnabled(bool)));
  connect(ui->PSigCheckBox2, SIGNAL (toggled(bool)), ui->PSLevelSpinBox2, SLOT (setEnabled(bool)));
  connect(ui->PSigCheckBox2, SIGNAL (toggled(bool)), ui->PSPeriodSpinBox2, SLOT (setEnabled(bool)));
  connect(ui->PSigCheckBox2, SIGNAL (toggled(bool)), ui->PSLevelHorizontalSlider2, SLOT (setEnabled(bool)));
  ui->PSigCheckBox2->toggled(false);
}

void BandMasterGUI::setupHandles() {
  m_handles.p_pp.reset(new GUI_PP(this));
  m_handles.p_rs.reset(new GUI_RS(this));
  m_handles.p_rp.reset(new GUI_RP(this));
  m_handles.p_gp.reset(new GUI_GP(this));
  m_handles.p_pd.reset(new GUI_PD(this));
  m_handles.p_ciRec = new GUI_CIRec(this);
  m_handles.p_ciPSG = new GUI_CIGen(this);
  m_handleMaster = new GUIHandleMaster(&m_handles);
  qInfo() << "GUI handles activated";
}

void BandMasterGUI::resizeEvent(QResizeEvent *event) {
  QString statLine{"Window size: "};
  QSize currentSize = event->size();
  statLine.append(QString::number(currentSize.width()));
  statLine.append(" * ");
  statLine.append(QString::number(currentSize.height()));
  ui->statusbar->showMessage(statLine);
}

// void BandMasterGUI::acceptReport(char c) {
//   // set label color
//   // QComboBox *w = chanStateObjMap.at(c);
//   // QPalette p = w->palette();
//   // QPalette p = w->);
//   // p.setColor(w->backgroundRole(), stateColorMap.at(bmd->getUState(c)));
//   // w->setPalette(p);
//   // w->QComboBox::setEditText(unitStMap.at(bmd->getUState(c)));
// }

#include "bm_handlerpsgenerator.h"
#include "../../import/ini.h"
#include <QFile>

BM_HandlerPSGenerator::BM_HandlerPSGenerator(QObject *parent)
    : BM_HandlerGeneral{parent} {}

void BM_HandlerPSGenerator::createINIfile() {
  assert(!p_linkData.isNull());
  assert(!p_wFileData.isNull());
  assert(!p_genData.isNull());
  m_unitChan = QString::number(m_taskCounter);
  ++m_taskCounter;
  QString iniName = makeINIfileName();
  std::string iniFileName =
      m_unitPath.toStdString() + "\\" + iniName.toStdString();
  {
    QFile file(QString::fromStdString(iniFileName));
    if (!file.exists()) {
      file.open(QFile::OpenModeFlag::WriteOnly);
      file.close();
    }
  }
  // debugOutput("create INI file"+ QString::fromStdString(iniFileName));
  ini::File iFile = ini::open(iniFileName);
  if (!iFile.has_section("LinkConfig"))
    iFile.add_section("LinkConfig");
  if (!iFile.has_section("FileConfig"))
    iFile.add_section("FileConfig");
  if (!iFile.has_section("GenConfig"))
      iFile.add_section("GenConfig");
  iFile["LinkConfig"]["ownAddr"] = p_linkData->ownAddr.toString().toStdString();
  iFile["LinkConfig"]["ownPort"] = std::to_string(p_linkData->ownPort);
  iFile["LinkConfig"]["dispAddr"] =
      p_linkData->dispAddr.toString().toStdString();
  iFile["LinkConfig"]["dispPort"] = std::to_string(p_linkData->dispPort);
  iFile["LinkConfig"]["unitID"] = std::to_string(p_linkData->unitID);

  iFile["FileConfig"]["fileName"] = p_wFileData->fileName.toStdString();
  iFile["FileConfig"]["resultPath"] = p_wFileData->resultPath.toStdString();
  iFile["FileConfig"]["sampleRate"] = std::to_string(p_wFileData->sampleRate);
  iFile["FileConfig"]["bytesPerSample"] =
      std::to_string(p_wFileData->bytesPerSample);

  iFile["GenConfig"]["durationMSec"] = std::to_string(p_genData->durationMSec);
  iFile["GenConfig"]["freq1"] = std::to_string(p_genData->freq1);
  iFile["GenConfig"]["freq2"] = std::to_string(p_genData->freq2);
  iFile["GenConfig"]["freq3"] = std::to_string(p_genData->freq3);
  iFile.write(iniFileName);
  m_unitINIfile = QString::fromStdString(iniFileName);
  qInfo() << "unit INI file created: " + m_unitINIfile;
}

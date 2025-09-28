#include "bm_handlerrecorder.h"
#include "../../import/ini.h"
#include <QFile>

BM_HandlerRecorder::BM_HandlerRecorder(QObject *parent)
    : BM_HandlerGeneral{parent} {}

void BM_HandlerRecorder::createINIfile() {
  assert(!p_linkData.isNull());
  assert(!p_recData.isNull());
  m_unitINIfile = makeINIfileName();
  std::string iniFileName =
      m_unitPath.toStdString() + "\\" + m_unitINIfile.toStdString();
  qInfo()<<m_unitID << "Header create: "<<iniFileName;
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
  if (!iFile.has_section("RecConfig"))
    iFile.add_section("RecConfig");
  iFile["LinkConfig"]["ownAddr"] = p_linkData->ownAddr.toString().toStdString();
  iFile["LinkConfig"]["ownPort"] = std::to_string(p_linkData->ownPort);
  iFile["LinkConfig"]["dispAddr"] =
      p_linkData->dispAddr.toString().toStdString();
  iFile["LinkConfig"]["dispPort"] = std::to_string(p_linkData->dispPort);
  iFile["LinkConfig"]["unitID"] = std::to_string(p_linkData->unitID);
  iFile["RecConfig"]["projectName"] = p_recData->projectName.toStdString();
  iFile["RecConfig"]["chanID"] = p_recData->chanID.toStdString();
  iFile["RecConfig"]["sorsID"] = std::to_string(p_recData->sorsID);
  iFile["RecConfig"]["freqS"] = std::to_string(p_recData->freqS);
  iFile["RecConfig"]["recDurationSec"] =
      std::to_string(p_recData->recDurationSec);
  iFile["RecConfig"]["chunkSec"] = std::to_string(p_recData->chunkSec);
  iFile["RecConfig"]["bytesPerSample"] =
      std::to_string(p_recData->bytesPerSample);
  iFile["RecConfig"]["resultPath"] = p_recData->resultPath.toStdString();

  iFile.write(iniFileName);
  m_unitINIfile = QString::fromStdString(iniFileName);
  qInfo() << "unit INI file created: " + m_unitINIfile;
}

void BM_HandlerRecorder::setFields()
{
    m_unitChan=p_recData->chanID;
}

#include "bm_handlersplayer.h"
#include "../../import/ini.h"
#include <QFile>

BM_HandlerSPlayer::BM_HandlerSPlayer(QObject *parent)
    : BM_HandlerGeneral{parent} {}

void BM_HandlerSPlayer::getTask(DataPtrs p) {
  qInfo() << "SLOT GetTask running";
  p_playData = p.splayData;
  sendParameters();
}

void BM_HandlerSPlayer::createINIfile() {
  assert(!p_linkData.isNull());
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

  iFile["LinkConfig"]["ownAddr"] = p_linkData->ownAddr.toString().toStdString();
  iFile["LinkConfig"]["ownPort"] = std::to_string(p_linkData->ownPort);
  iFile["LinkConfig"]["dispAddr"] =
      p_linkData->dispAddr.toString().toStdString();
  iFile["LinkConfig"]["dispPort"] = std::to_string(p_linkData->dispPort);
  iFile["LinkConfig"]["unitID"] = std::to_string(p_linkData->unitID);

  iFile.write(iniFileName);
  m_unitINIfile = QString::fromStdString(iniFileName);
  qInfo() << "unit INI file created: " + m_unitINIfile;
}

void BM_HandlerSPlayer::sendParameters() {
  qInfo() << "Send params to player";
  m_taskArg = p_playData->fileNamePath;
  if (!(ifQProcessOk() && ifLinkerOk())) {
    qWarning() << "HLR: CANNOT set params. Process OK: " << ifQProcessOk()
               << " Linker OK: " << ifLinkerOk();
    return;
  }
  p_linker->sendReport(BcomToInt(BM_command::SetOption1),
                       p_playData->fileNamePath);
  p_linker->sendReport(BcomToInt(BM_command::SetOption2),
                       QString::number(p_playData->outputID));
  p_linker->sendReport(BcomToInt(BM_command::SetOption3),
                       QString::number(p_playData->level));
}

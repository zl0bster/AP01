#ifndef BM_HANDLERSPLAYER_H
#define BM_HANDLERSPLAYER_H

#include "bm_handlergeneral.h"
#include <QObject>

class BM_HandlerSPlayer : public BM_HandlerGeneral {
public:
  explicit BM_HandlerSPlayer(QObject *parent = nullptr);

  // BM_HandlerGeneral interface
public slots:
  virtual void getTask(DataPtrs) override;

protected:
  virtual void createINIfile() override;

private:
  QSharedPointer<SPlayerData> p_playData;

  void sendParameters();
};

#endif // BM_HANDLERSPLAYER_H

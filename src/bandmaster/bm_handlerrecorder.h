#ifndef BM_HEADERRECORDER_H
#define BM_HEADERRECORDER_H

#include "bm_handlergeneral.h"
#include <QObject>

class BM_HandlerRecorder : public BM_HandlerGeneral {
  Q_OBJECT

public:
  explicit BM_HandlerRecorder(QObject *parent = nullptr);
  ~BM_HandlerRecorder() = default;
  void setRecorder(QSharedPointer<ARecData> p) {
    p_recData = p;
    setFields();
  }
public slots:
  virtual void getTask(DataPtrs p) override {
    p_recData = p.recData;
    setFields();
    createINIfile();
    m_taskArg = m_unitINIfile;
    qInfo ("HANDLER_REC: data stored");
  };

protected:
  virtual void createINIfile() override;

private:
  QSharedPointer<ARecData> p_recData;
  void setFields();
};

#endif // BM_HEADERRECORDER_H

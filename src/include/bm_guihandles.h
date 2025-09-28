#ifndef BM_GUIHANDLES_H
#define BM_GUIHANDLES_H
#include <QObject>
#include <QSharedPointer>
#include <QString>

class ProjectParameters {
public:
  virtual ~ProjectParameters(){};
  virtual QString projectName() const = 0;
  virtual QString folderPath() const = 0;
};

class RecSources {
public:
  virtual ~RecSources(){};
  virtual int sourceRec1() const = 0; // 0 if not, actual pos - 1 in OS order.
  virtual int sourceRec2() const = 0; // 0 if not, actual pos - 1 in OS order.
  virtual int sourceRec3() const = 0; // 0 if not, actual pos - 1 in OS order.
  virtual int sourceRec4() const = 0; // 0 if not, actual pos - 1 in OS order.
};

class RecParameters {
public:
  virtual ~RecParameters(){};
  virtual int bitFormat() const = 0;    // 16 - 24 - 32 bit
  virtual int sampleRate() const = 0;   // Hz
  virtual int fileDuration() const = 0; // sec
  virtual int recTime() const = 0;      // sec - 0 for unlimited
};

class GenParameters {
public:
  virtual ~GenParameters(){};
  virtual int signalDuration() const = 0; // ms
  virtual int freq1() const = 0;          // Hz
  virtual int freq2() const = 0;          // Hz
  virtual int freq3() const = 0;          // Hz
};

class PlayerData {
public:
  virtual ~PlayerData(){};
  virtual int playDevice() const = 0; // 0 if not, actual pos - 1 in OS order.
  virtual bool isFile1active() const = 0;
  virtual bool isFile2active() const = 0;
  virtual QString psfile1() const = 0; // path + name
  virtual int psperiod1() const = 0;   // sec
  virtual int pslevel1() const = 0;    // %
  virtual QString psfile2() const = 0; // path + name
  virtual int psperiod2() const = 0;   // sec
  virtual int pslevel2() const = 0;    // %
};

class CommandIntarface : public QObject {
  Q_OBJECT
public:
  virtual ~CommandIntarface(){};
public slots:
  virtual void MSGOutput(QString text) = 0;
signals:
  void start();
  void stop();
  void pause();
  // void generate();
};

//---------------- use inside GUI -----------------
struct GUIHandles {
  QSharedPointer<ProjectParameters> p_pp;
  QSharedPointer<RecSources> p_rs;
  QSharedPointer<RecParameters> p_rp;
  QSharedPointer<GenParameters> p_gp;
  QSharedPointer<PlayerData> p_pd;
  CommandIntarface *p_ciRec;
  CommandIntarface *p_ciPSG;
  CommandIntarface *p_ciPla;
};
//------------------

class GUIHandleMaster {
private:
  GUIHandles *m_h;

public:
  explicit GUIHandleMaster(GUIHandles *p) : m_h(p){};
  QSharedPointer<ProjectParameters> get_pp() const { return m_h->p_pp; }
  QSharedPointer<RecSources> get_rs() const { return m_h->p_rs; }
  QSharedPointer<RecParameters> get_rp() const { return m_h->p_rp; }
  QSharedPointer<GenParameters> get_gp() const { return m_h->p_gp; }
  QSharedPointer<PlayerData> get_pd() const { return m_h->p_pd; }
  CommandIntarface *get_ciRec() const { return m_h->p_ciRec; }
  CommandIntarface *get_ciRPSG() const { return m_h->p_ciPSG; }
  CommandIntarface *get_ciPla() const { return m_h->p_ciPla; }
  // #TODO GenParameters and PlayerData should be realized in future iteration
};

#endif // BM_GUIHANDLES_H

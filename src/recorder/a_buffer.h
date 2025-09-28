#ifndef A_BUFFER_H
#define A_BUFFER_H

#include <QByteArray>
#include <QByteArrayView>
#include <QIODevice>
#include <QObject>

const qsizetype nBuffs{2};

class A_Buffer : public QIODevice {
  Q_OBJECT
public:
  explicit A_Buffer(QObject *parent = nullptr, quint64 bufferSize = 4096);
signals:
  void dataIsReadyToSave(const QByteArrayView*);
public slots:
  void slotStart(void);
  void slotStop(void);

protected:
  qint64 readData(char *data, qint64 maxSize) override;
  qint64 writeData(const char *data, qint64 maxSize) override;

private:
  QByteArray *m_b[nBuffs];
  qsizetype m_currentBuf{0};
  qint64 m_nBufToSave{-1};
  quint64 m_bufferSize;
  bool m_recStarted{false};
  QByteArrayView *m_SaveView;
};

#endif // A_BUFFER_H

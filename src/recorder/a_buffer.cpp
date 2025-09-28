#include "a_buffer.h"
#include "qdebug.h"

#include <cassert>

A_Buffer::A_Buffer(QObject *parent, quint64 bufferSize)
    : QIODevice{parent}, m_bufferSize(bufferSize) {
  m_recStarted = false;
  for (qsizetype i = 0; i < nBuffs; ++i) {
    m_b[i] = new QByteArray;
    // m_b[i]->reserve(bufferSize);
  }
  m_SaveView = new QByteArrayView;
}

void A_Buffer::slotStart() {
  assert(!m_recStarted);
  assert(nBuffs > 1);
  for (qsizetype i = 0; i < nBuffs; ++i) {
    m_b[i]->clear();
    m_b[i]->reserve(m_bufferSize);
    qDebug() << "BUFF: " << i << " capacity =" << m_b[i]->capacity()
             << " size =" << m_b[i]->size();
  }
  m_currentBuf = 0;
  m_nBufToSave = -1;
  m_recStarted = true;
}

void A_Buffer::slotStop() {
  // assert(m_recStarted);
  if (!m_b[m_currentBuf]->isEmpty()) {
    *m_SaveView = *m_b[m_currentBuf];
    emit dataIsReadyToSave(m_SaveView);
  }
  m_recStarted = false;
}

qint64 A_Buffer::readData(char *data, qint64 maxSize) {
  Q_UNUSED(data);
  Q_UNUSED(maxSize);
  return -1;
}

qint64 A_Buffer::writeData(const char *data, qint64 maxSize) {
  assert(m_recStarted);
  qsizetype restPlace =
      m_b[m_currentBuf]->capacity() - m_b[m_currentBuf]->size();
  // qDebug() << "BUFFER: " << m_currentBuf
  //          << " capacity =" << m_b[m_currentBuf]->capacity()
  //          << " rest place =" << restPlace;
  if (restPlace > maxSize) {
    m_b[m_currentBuf]->append(data, maxSize);
    return maxSize;
  }
  m_b[m_currentBuf]->append(data, restPlace);
  m_nBufToSave = m_currentBuf;

  // switch to next buffer to save data of filled one
  ++m_currentBuf;
  m_currentBuf %= nBuffs;

  m_b[m_currentBuf]->clear();
  m_b[m_currentBuf]->reserve(m_bufferSize);
  m_b[m_currentBuf]->append(data + restPlace, maxSize - restPlace);

  *m_SaveView = *m_b[m_nBufToSave];
  qDebug() << "BUFFER: chunk to save size _" << m_SaveView->size() << " of "
           << m_b[m_nBufToSave]->size();
  emit dataIsReadyToSave(m_SaveView);
  return maxSize;
}

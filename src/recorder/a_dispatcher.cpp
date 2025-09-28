#include "a_dispatcher.h"
#include "../include/bm_codelist.h"
#include <QDateTime>
// #include <iostream>

A_Dispatcher::A_Dispatcher(QObject *parent, QSharedPointer<ALinkData> ld,
                           QSharedPointer<ARecData> rd, bool isPaused)
    : p_linkData(ld), p_recData(rd) {
    this->setParent(parent);

    // prepare initializing data
    // recorder -> buffer -> file data type = <float>
    // create and tune workers
    fillWavData();
    initBuffer();
    initRecorder();
    initSaver();
    initLinker();

    // component must shut down if no commands recieved while delay of this timer
    m_delayOff = 30000;
    if (p_recData->recDurationSec > 0)
        m_delayOff = p_recData->recDurationSec * 1000;
    m_autoFinish = new QTimer(this);
    m_autoFinish->setSingleShot(true);
    m_autoFinish->setInterval(m_delayOff);
    connect(m_autoFinish, &QTimer::timeout, this, &A_Dispatcher::stopRecord);
    m_autoFinish->start();
    QTimer::singleShot(m_delayOff / 2, this, &A_Dispatcher::statusRequest);
    // auto start delay to start event loop
    if (!isPaused) {
        QTimer::singleShot(100, this, &A_Dispatcher::startRecord);
        qInfo() << "auto start option is activated";
    } else
        qInfo() << "standby mode started";
    // move workers to separate threads
    // connect workers by signal-slot
    // data transfer Buffer (pointer) -> Dispatcher (pointer + filename) ->
    // FileSaver
    // start  record if this mode defined in commandline
}

A_Dispatcher::~A_Dispatcher() {
    delete m_linker;
    t_saver.quit();
    if (!t_saver.wait(100)) {
        qDebug() << "terminate A_Saver thread";
        t_saver.terminate();
    } else
        qDebug() << "A_Saver thread stopped correctly";
    delete m_saver;
    delete m_rec;
    delete m_buf;
    // delete m_WAVData;
    delete m_WFData;
    qDebug() << "destructor finished";
}

void A_Dispatcher::recieveCommand(uint32_t code, QString s) {
    Q_UNUSED(s);
    qInfo() << "command recieved:" << code;
    // refresh timer if connection is alive
    m_autoFinish->start(m_delayOff);
    BM_command command = intToBcom(code);
    switch (command) {
    case BM_command::Start:
        startRecord();
        break;
    case BM_command::Pause:
        pauseRecord();
        break;
    case BM_command::Stop:
        stopRecord();
        break;
    case BM_command::Status:
        statusRequest();
        break;
    default:
        break;
    }
}

void A_Dispatcher::recieveReport(ASR_statusCode code, QString s) {
    qInfo() << "internal report:" << static_cast<int>(code) << s;
    switch (code) {
    case ASR_statusCode::File_saved:
        emit sendReport(BcodeToInt(BM_uState::Result_saved), s);
        break;
    case ASR_statusCode::File_open_error:
        emit sendReport(BcodeToInt(BM_uState::File_save_failed), s);
        stopRecord();
        break;
    case ASR_statusCode::Status_ok:
        emit sendReport(BcodeToInt(BM_uState::Processing), s);
        break;
    case ASR_statusCode::Device_error:
        emit sendReport(BcodeToInt(BM_uState::Device_error), s);
        stopRecord();
        break;
    case ASR_statusCode::Connection_error:
        emit sendReport(BcodeToInt(BM_uState::Connection_problem), s);
        // stopRecord();
        break;
    case ASR_statusCode::Format_unsupported:
        emit sendReport(BcodeToInt(BM_uState::Software_error), s);
        stopRecord();
        break;
    case ASR_statusCode::Buffer_is_empty:
        emit sendReport(BcodeToInt(BM_uState::Software_error), s);
        stopRecord();
        break;
    default:
        emit sendReport(BcodeToInt(BM_uState::Software_error), s);
        stopRecord();
        break;
    }
}

void A_Dispatcher::saveBufToFile(const QByteArrayView *buf) {
    QString fName = m_WFData->fileName;
    fName = getFNameVsTime(fName);
    qInfo() << "file saving requested: " << fName;
    emit saveFile(fName, buf);
}

void A_Dispatcher::startRecord() {
    qInfo() << "Command START accepted";
    switch (m_state) {
    case A_DispState::Running:
        return;
    case A_DispState::StandBy:
        m_state = A_DispState::Running;
        emit signalStart();
        break;
    case A_DispState::StoppedToQuit:
    default:
        break;
    }
}

void A_Dispatcher::pauseRecord() { // unused temporarry
    switch (m_state) {
    case A_DispState::Running:
        m_state = A_DispState::StandBy;
        emit signalStop();
        break;
    case A_DispState::StandBy:
        return;
    case A_DispState::StoppedToQuit:
    default:
        break;
    }
}

void A_Dispatcher::stopRecord() {
    qInfo() << "Command STOP accepted";
    switch (m_state) {
    case A_DispState::Running:
    case A_DispState::StandBy:
        m_state = A_DispState::StoppedToQuit;
        emit sendReport(BcodeToInt(BM_uState::Quit_app), "");
        emit signalStop();
        break;
    case A_DispState::StoppedToQuit:
    default:
        break;
    }
    QTimer::singleShot(1000, this, [] { qInfo()<<"PGM exit code 0";exit(0); });
}

void A_Dispatcher::statusRequest() {
    qInfo() << "status request accepted";
    switch (m_state) {
    case A_DispState::StandBy:
        emit sendReport(BcodeToInt(BM_uState::Stand_by),"");
        return;
    case A_DispState::StoppedToQuit:
        emit sendReport(BcodeToInt(BM_uState::Quit_app),"");
        return;
    default:
        break;
    }
    // emit checkStatus(); // check recorder state
    m_rec->slotStatusCheck();
    ifLinkerOk(); // check Linker state
}

void A_Dispatcher::initBuffer() {
    qsizetype bufSize =
        p_recData->chunkSec * p_recData->freqS * p_recData->bytesPerSample;
    qInfo() << "chunk duration =" << p_recData->chunkSec;
    qInfo() << "sample rate=" << p_recData->freqS;
    qInfo() << "buf size=" << bufSize;
    m_buf = new A_Buffer(this, bufSize);
    // connect workers by signal-slot
    connect(this, SIGNAL(signalStart()), m_buf, SLOT(slotStart()));
    connect(this, SIGNAL(signalStop()), m_buf, SLOT(slotStop()));
    connect(m_buf, &A_Buffer::dataIsReadyToSave, this,
            &A_Dispatcher::saveBufToFile);
}

void A_Dispatcher::initSaver() {
    m_saver = new SaveWAV;
    m_saver->setWAVData(m_WFData);
    // move worker to separate thread
    m_saver->moveToThread(&t_saver);
    // connect workers by signal-slot
    connect(m_saver, SIGNAL(signalStatus(ASR_statusCode, QString)), this,
            SLOT(recieveReport(ASR_statusCode, QString)));
    connect(this, SIGNAL(saveFile(QString, const QByteArrayView *)), m_saver,
            SLOT(slotSaveFile(QString, const QByteArrayView *)));
    t_saver.start();
}

void A_Dispatcher::initRecorder() {
    m_rec = new A_Recorder(this, m_buf);
    m_rec->setSampleRate(p_recData->freqS);
    m_rec->setSource(p_recData->sorsID);
    m_rec->setFormat(p_recData->bytesPerSample);
    // connect workers by signal-slot
    connect(this, SIGNAL(signalStart()), m_rec, SLOT(slotStart()));
    connect(this, SIGNAL(signalStop()), m_rec, SLOT(slotStop()));
    connect(this, SIGNAL(checkStatus()), m_rec, SLOT(slotStatusCheck()));
    connect(m_rec, SIGNAL(signalStatus(ASR_statusCode, QString)), this,
            SLOT(recieveReport(ASR_statusCode, QString)));
}

void A_Dispatcher::initLinker() {
    m_linker = new LinkerUDP(this, p_linkData);
    connect(this, SIGNAL(sendReport(uint32_t, QString)), m_linker,
            SLOT(sendReport(uint32_t, QString)));
    connect(m_linker, SIGNAL(commandRecieved(uint32_t, QString)), this,
            SLOT(recieveCommand(uint32_t, QString)));
}

QString A_Dispatcher::getFNameVsTime(const QString &fn) const {
    QDateTime d = QDateTime::currentDateTimeUtc();
    QString ds = d.toString(Qt::ISODate);
    ds.replace(":", "-");
    return fn + ds + ".wav";
}

void A_Dispatcher::fillWavData() {
    m_WFData = new WavFileData();
    m_WFData->fileName = p_recData->projectName + "_" + p_recData->chanID + "_";
    m_WFData->sampleRate = p_recData->freqS;
    m_WFData->bytesPerSample = p_recData->bytesPerSample;
}

bool A_Dispatcher::ifLinkerOk() {
    bool result = true;
    size_t currentErrMsgCount = m_linker->errorMSGsCount();
    qInfo() << "Linker errors count:" << m_linker->errorMSGsCount();
    if (currentErrMsgCount > 1) {
        if (currentErrMsgCount > m_errMSGsCount) {
            result = false; // minor alarm
            if (currentErrMsgCount >= LIMIT_LINK_ERROR) {
                recieveReport(ASR_statusCode::Connection_error,
                              QString::number(currentErrMsgCount) +
                                  " errors"); // major alarm
            }
        }
    }
    m_errMSGsCount = currentErrMsgCount;
    return result;
}

// bool A_Dispatcher::ifRecorderOk() {}

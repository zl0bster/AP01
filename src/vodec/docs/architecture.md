# Архитектура подпроекта vodec

## Содержание
1. [Детальная архитектура системы](#детальная-архитектура-системы)
2. [Алгоритмическая архитектура](#алгоритмическая-архитектура)
3. [Структуры данных и интерфейсы](#структуры-данных-и-интерфейсы)
4. [Архитектура экспорта данных](#архитектура-экспорта-данных)
5. [Конфигурационная архитектура](#конфигурационная-архитектура)
6. [Архитектура обработки ошибок](#архитектура-обработки-ошибок)
7. [Производительность и масштабируемость](#производительность-и-масштабируемость)
8. [Интеграционная архитектура](#интеграционная-архитектура)
9. [Архитектура тестирования](#архитектура-тестирования)
10. [Диаграммы и схемы](#диаграммы-и-схемы)

---

## 1. Детальная архитектура системы

### Общая архитектура компонентов

```mermaid
graph TB
    subgraph "Application Layer"
        MAIN[main.cpp]
        VD_CORE[VD_Core]
    end
    
    subgraph "Core Framework"
        CORE_UNIT[CoreUnit]
        INIT_UNIT[InitUnit]
        PROTO_UNIT[ProtoUnit]
        DISP_UNIT[DispUnit]
    end
    
    subgraph "VD Implementation"
        VD_UNIT[VD_Unit]
        VD_INIT[VD_Init]
    end
    
    subgraph "Signal Processing"
        VD_DETECTOR[VD_VoiceDetector]
        VD_ANALYZER[VD_ArtifactAnalizer]
    end
    
    subgraph "SP Libraries"
        TD_BUFFER[TDBuffer]
        FD_BUFFER[FDBuffer]
        VOCAL_DET[VocalDetector]
        NOISE_MODEL[N_Model]
    end
    
    MAIN --> VD_CORE
    VD_CORE --> CORE_UNIT
    VD_CORE --> VD_UNIT
    VD_CORE --> VD_INIT
    
    VD_UNIT --> PROTO_UNIT
    VD_INIT --> INIT_UNIT
    
    VD_UNIT --> VD_DETECTOR
    VD_DETECTOR --> VD_ANALYZER
    VD_DETECTOR --> TD_BUFFER
    VD_DETECTOR --> FD_BUFFER
    VD_DETECTOR --> VOCAL_DET
    VD_DETECTOR --> NOISE_MODEL
```

### Иерархия классов

```mermaid
classDiagram
    class CoreUnit {
        +QObject* parent
        +QStringList args
        +InitUnit* m_init
        +DispUnit* m_disp
        +ProtoUnit* m_unit
        +initialize()
    }
    
    class VD_Core {
        +VD_Unit* m_vdunit
        +VD_Init* m_vdinit
        +initialize()
    }
    
    class ProtoUnit {
        <<interface>>
        +initialize()
        +slotStart()
        +slotStop()
        +slotStatusCheck()
        +setOpt1(QString)
        +setOpt2(QString)
        +signalStatus()
    }
    
    class VD_Unit {
        +vd_detector::VD_VoiceDetector m_vd
        +VD_Init* p_init
        +std::string m_taskFile
        +std::string m_paramFile
        +bool isSingleShot
        +bool isDetected
        +initialize()
        +slotStart()
        +slotStop()
        +setOpt1(QString)
        +setOpt2(QString)
    }
    
    class VD_VoiceDetector {
        +TDBuffer m_tdInputBuf
        +TDBuffer m_tdOutputBuf
        +N_Model m_nmodel
        +VocalDetector m_vd
        +FDBuffer m_fdBuf
        +VD_ArtifactAnalizer m_aa
        +VDATA_T m_fs
        +bool m_isArtifactDetected
        +bool m_export_data
        +processFile(std::string)
        +setupDetector()
        +setupNoiseMod()
        +setupArtAn()
    }
    
    CoreUnit <|-- VD_Core
    ProtoUnit <|-- VD_Unit
    VD_Core --> VD_Unit
    VD_Core --> VD_Init
    VD_Unit --> VD_VoiceDetector
```

### Жизненный цикл обработки

```mermaid
sequenceDiagram
    participant M as main()
    participant C as VD_Core
    participant U as VD_Unit
    participant D as VD_VoiceDetector
    participant A as VD_ArtifactAnalizer
    
    M->>C: initialize()
    C->>U: new VD_Unit()
    C->>U: initialize()
    U->>D: new VD_VoiceDetector()
    U->>D: setupVD()
    
    Note over U: Ready for processing
    
    U->>D: processFile(wavFile)
    D->>D: loadFile()
    D->>D: applyWindow()
    D->>D: FFT transform()
    D->>D: analyzeVocalRange()
    D->>D: updateNoiseModel()
    D->>D: detectArtifacts()
    D->>A: processSignature()
    
    alt Artifact detected
        A->>D: isSentenceDetected() = true
        D->>D: createWAVdata()
        D->>D: saveFile()
        D->>U: acceptRESULT()
    end
    
    D->>U: return processing result
    U->>C: signalStatus()
```

---

## 2. Алгоритмическая архитектура

### Поток обработки сигналов

```mermaid
graph TD
    A[WAV File Input] --> B[Load to TDBuffer]
    B --> C["Set Window Parameters<br/>Size: 2048, Hop: 1024"]
    C --> D[Apply Hann Window]
    D --> E["FFT Transform<br/>Frame Size: 1024"]
    E --> F["Copy to FDBuffer<br/>Ring Buffer: 128 frames"]
    
    F --> G[VocalDetector Analysis]
    G --> H{Artifact Detected?}
    
    H -->|No| I[Update Noise Model]
    H -->|Yes| J[Generate Artifact Signature]
    
    I --> K[Switch to Next Window]
    J --> L[ArtifactAnalizer Processing]
    
    L --> M{Sentence Complete?}
    M -->|No| K
    M -->|Yes| N[Create Output WAV]
    
    N --> O[Save Artifact File]
    O --> P[Emit Result Callback]
    P --> K
    
    K --> Q{More Windows?}
    Q -->|Yes| D
    Q -->|No| R[Processing Complete]
    
    style A fill:#e1f5fe
    style R fill:#c8e6c9
    style H fill:#fff3e0
    style M fill:#fff3e0
```

### Алгоритм детекции голоса

```mermaid
graph TD
    A[Input: FD Frame] --> B[Calculate Magnitude Spectrum]
    B --> C["Extract Vocal Range<br/>60-400 Hz"]
    C --> D[Calculate Frame Power]
    D --> E[Calculate Vocal Range Power]
    
    E --> F[Check Frame Power Criteria]
    F --> G[Check Noise Power Criteria]
    G --> H[Detect F0 Fundamental]
    
    F --> I[Probability += 0.05]
    G --> J[Probability += 0.05]
    H --> K[Probability += 0.05]
    
    I --> L[Total Probability]
    J --> L
    K --> L
    
    L --> M{Probability >= Threshold?}
    M -->|Yes| N[Artifact Detected]
    M -->|No| O[No Artifact]
    
    N --> P[Generate Signature]
    O --> Q[Continue Processing]
    
    style N fill:#ffcdd2
    style O fill:#c8e6c9
    style M fill:#fff3e0
```

### Алгоритм анализа артефактов

```mermaid
stateDiagram
    [*] --> Clear
    
    Clear --> Started : Magnitude & Probability OK
    Started --> Continues : Signal Timing OK
    Started --> Clear : Signal Timing NOT OK
    
    Continues --> Interrupted : Criteria NOT met
    Interrupted --> Continues : Probability OK
    Interrupted --> Finished : Pause Timing OK
    
    Finished --> Clear : Reset for next sequence
    
    note right of Clear
        SEQUENCE_STATE::Clear
        Waiting for artifact start
    end note
    
    note right of Started
        SEQUENCE_STATE::Started
        Artifact sequence begun
    end note
    
    note right of Continues
        SEQUENCE_STATE::Continues
        Active artifact sequence
    end note
    
    note right of Interrupted
        SEQUENCE_STATE::Interrupted
        Sequence paused
    end note
    
    note right of Finished
        SEQUENCE_STATE::Finished
        Complete sentence detected
    end note
```

---

## 3. Структуры данных и интерфейсы

### Основные структуры данных

```cpp
// Структуры коэффициентов
struct VD_Coef_Frame {
    float VD_vocalRangeLoFreq;     // 60 Hz - нижняя граница голосового диапазона
    float VD_vocalRangeHiFreq;     // 400 Hz - верхняя граница голосового диапазона
    float VD_ProbabilityThreshold; // 0.3 - порог вероятности детекции
    float VD_powerThreshold;       // 0.15 - порог мощности сигнала
};

struct NM_Coef_Frame {
    float NM_scopeLength;            // 10.0 - длина области для модели шума
    float NM_weightRefreshThreshold; // 0.4 - порог обновления весов
    float NM_noiseMixinCoef;         // 0.01 - коэффициент смешивания шума
};

struct AA_Coef_Frame {
    unsigned AA_signalTimingThr; // 5 - порог времени сигнала (в кадрах)
    unsigned AA_pauseTimingThr;  // 10 - порог времени паузы (в кадрах)
    float AA_freqTol;            // 0.1 - толерантность частоты
};
```

### Структура подписи артефакта

```cpp
template<typename T, size_t FSize>
struct ArtifactSignature {
    long frameID;                              // ID кадра
    T probability;                             // Вероятность [0...1.0]
    T framePower;                              // Мощность кадра
    std::array<FrData<T>, FSize> signature;    // Частотные данные
};

template<typename T>
struct FrData {
    T freqHz;    // Частота в Гц
    T magn;      // Магнитуда
};
```

### Интерфейсы компонентов

```mermaid
graph LR
    subgraph "Interface Layer"
        CALLBACK[CallBackProto]
        PROTO[ProtoUnit]
        INIT[InitUnit]
    end
    
    subgraph "Implementation"
        VD_UNIT[VD_Unit]
        VD_INIT[VD_Init]
    end
    
    CALLBACK -.->|implements| VD_UNIT
    PROTO -.->|implements| VD_UNIT
    INIT -.->|implements| VD_INIT
    
    VD_UNIT -->|uses| VD_INIT
```

---

## 4. Архитектура экспорта данных

### Механизм экспорта

```mermaid
graph TD
    A[VD_VoiceDetector] --> B{m_export_data == true?}
    B -->|No| C[Skip Export]
    B -->|Yes| D[Export FDBuffer to NPY]
    
    D --> E[Export FDBuffer to TIFF]
    E --> F[Export SIGNATURE_T to NPY]
    F --> G[Export SIGNATURE_T to TIFF]
    
    D --> H["FDBuffer NPY Format:<br/>numpy array with frame_id, freq_bin, magnitude_real, magnitude_imag"]
    E --> I["FDBuffer TIFF Format:<br/>image data with frequency domain visualization"]
    F --> J["SIGNATURE_T NPY Format:<br/>numpy array with frame_id, probability, frame_power, freq_hz, magnitude"]
    G --> K["SIGNATURE_T TIFF Format:<br/>image data with signatures visualization"]
    
    style B fill:#fff3e0
    style G fill:#e8f5e8
    style H fill:#e8f5e8
    style I fill:#e8f5e8
```

### Структура экспортируемых файлов

```mermaid
graph TB
    subgraph "Export Files Structure"
        A[input_file.wav] --> B[input_file_fdbuffer.npy]
        A --> C[input_file_fdbuffer.tiff]
        A --> D[input_file_signatures.npy]
        A --> E[input_file_signatures.tiff]
        A --> F["input_file_1.wav - detected artifact"]
        A --> G["input_file_2.wav - detected artifact"]
    end
    
    subgraph "NPY FDBuffer Format"
        B --> H["numpy array with shape (frames, freq_bins, 2)<br/>containing frame_id, freq_bin, magnitude_real, magnitude_imag<br/>Data type: float32"]
    end
    
    subgraph "TIFF FDBuffer Format"
        C --> I["TIFF image with frequency domain visualization<br/>containing magnitude spectrum as image layers<br/>with time-frequency representation"]
    end
    
    subgraph "NPY Signatures Format"
        D --> J["numpy array with shape (signatures, 5)<br/>containing frame_id, probability, frame_power, freq_hz, magnitude<br/>Data type: float32"]
    end
    
    subgraph "TIFF Signatures Format"
        E --> K["TIFF image with signatures visualization<br/>containing frameID, probability,<br/>framePower, and signature data as image layers"]
    end
```

---

## 5. Конфигурационная архитектура

### Структура конфигурационного файла

```mermaid
graph TD
    A[INI Configuration File] --> B[TestConfig Section]
    A --> C[VoDec Section]
    A --> D[NoiseModel Section]
    A --> E[ArtifAn Section]
    
    B --> F[WAVFile = path/to/test.wav]
    
    C --> G[VD_ProbabilityThreshold = 0.3]
    C --> H[VD_powerThreshold = 0.15]
    C --> I[VD_vocalRangeLoFreq = 60.0]
    C --> J[VD_vocalRangeHiFreq = 400.0]
    
    D --> K[NM_weightRefreshThreshold = 0.4]
    D --> L[NM_noiseMixinCoef = 0.01]
    D --> M[NM_scopeLength = 10.0]
    
    E --> N[AA_freqTol = 0.1]
    E --> O[AA_signalTimingThr = 5]
    E --> P[AA_pauseTimingThr = 10]
```

### Парсинг и валидация

```mermaid
sequenceDiagram
    participant I as VD_Init
    participant P as ini::File
    participant V as VD_Coef_Frame
    participant N as NM_Coef_Frame
    participant A as AA_Coef_Frame
    
    I->>P: open(iniFileName)
    P-->>I: configFile
    
    I->>P: configFile["TestConfig"]["WAVFile"]
    P-->>I: m_testWAVfile
    
    I->>P: configFile["VoDec"].get<float>("VD_ProbabilityThreshold")
    P-->>I: 0.3
    I->>V: p_vdData->VD_ProbabilityThreshold = 0.3
    
    I->>P: configFile["NoiseModel"].get<float>("NM_weightRefreshThreshold")
    P-->>I: 0.4
    I->>N: p_nmData->NM_weightRefreshThreshold = 0.4
    
    I->>P: configFile["ArtifAn"].get<float>("AA_freqTol")
    P-->>I: 0.1
    I->>A: p_aaData->AA_freqTol = 0.1
    
    Note over I: Configuration loaded and validated
```

---

## 6. Архитектура обработки ошибок

### Иерархия обработки ошибок

```mermaid
graph TD
    A[Error Occurs] --> B{Error Type}
    
    B -->|File I/O| C[File Error Handler]
    B -->|Configuration| D[Config Error Handler]
    B -->|Processing| E[Processing Error Handler]
    B -->|Memory| F[Memory Error Handler]
    
    C --> G[Log Error]
    D --> G
    E --> G
    F --> G
    
    G --> H{Recoverable?}
    H -->|Yes| I[Attempt Recovery]
    H -->|No| J[Signal Fatal Error]
    
    I --> K[Continue Processing]
    I --> L[Fallback to Defaults]
    
    J --> M[Emit Error Signal]
    M --> N[Graceful Shutdown]
    
    style A fill:#ffcdd2
    style J fill:#f8bbd9
    style K fill:#c8e6c9
```

### Обработка ошибок по уровням

| Уровень | Тип ошибки | Обработка |
|---------|------------|-----------|
| **File I/O** | Не удается открыть WAV файл | Логирование, возврат false |
| **Configuration** | Неверный параметр в INI | Логирование, использование значения по умолчанию |
| **Processing** | Ошибка FFT | Логирование, пропуск кадра |
| **Memory** | Недостаточно памяти | Логирование, сигнал критической ошибки |

---

## 7. Производительность и масштабируемость

### Анализ узких мест

```mermaid
graph TD
    A[Performance Analysis] --> B[CPU Intensive Operations]
    A --> C[Memory Usage]
    A --> D[I/O Operations]
    
    B --> E["FFT Transformations<br/>O(n log n) per frame"]
    B --> F["Vocal Range Analysis<br/>O(n) per frame"]
    B --> G["Noise Model Updates<br/>O(n) per frame"]
    
    C --> H["TDBuffer: 2048 samples × 4 bytes"]
    C --> I["FDBuffer: 1024 complex × 128 frames"]
    C --> J["Artifact Buffer: 80 signatures"]
    
    D --> K["WAV File Loading<br/>Sequential read"]
    D --> L["Export File Writing<br/>Batch write"]
    
    style E fill:#ffcdd2
    style F fill:#fff3e0
    style G fill:#fff3e0
```

### Профиль производительности

| Операция | Время (мс) | Память (MB) | Оптимизация |
|----------|------------|-------------|-------------|
| **FFT Transform** | 0.5 | 0.008 | SIMD инструкции |
| **Vocal Detection** | 0.1 | 0.004 | Векторизация |
| **Noise Model** | 0.05 | 0.008 | Кэширование |
| **File I/O** | 10-100 | 0.5-5 | Асинхронный I/O |

---

## 8. Интеграционная архитектура

### Взаимодействие с BandMaster

```mermaid
sequenceDiagram
    participant BM as BandMaster
    participant UDP as UDP Link
    participant VD as VD_Unit
    participant SP as Signal Processing
    
    BM->>UDP: Send Task (WAV file + config)
    UDP->>VD: setOpt1(wavFile)
    UDP->>VD: setOpt2(configFile)
    UDP->>VD: slotStart()
    
    VD->>SP: processFile(wavFile)
    SP-->>VD: Processing Complete
    
    alt Artifact Found
        VD->>SP: Save Artifact WAV
        SP-->>VD: File Saved
        VD->>UDP: acceptRESULT(filename)
        UDP->>BM: Result Notification
    end
    
    VD->>UDP: signalStatus(Result_saved, message)
    UDP->>BM: Status Update
```

### UDP протокол

```mermaid
graph LR
    subgraph "UDP Message Types"
        A[Start Task] --> B[WAV File Path]
        A --> C[Config File Path]
        D[Status Update] --> E[Processing State]
        D --> F[Result File Path]
        G[Error Report] --> H[Error Code]
        G --> I[Error Message]
    end
    
    subgraph "Message Format"
        J["MessageType:StartTask<br/>WavFile:path/to/file.wav<br/>ConfigFile:path/to/config.ini"]
        K["MessageType:StatusUpdate<br/>State:Processing<br/>Progress:50%"]
        L["MessageType:Result<br/>File:path/to/result.wav<br/>Artifacts:2"]
    end
```

---

## 9. Архитектура тестирования

### Стратегия тестирования

```mermaid
graph TD
    A[Testing Strategy] --> B[Unit Tests]
    A --> C[Integration Tests]
    A --> D[Performance Tests]
    
    B --> E[VD_VoiceDetector Tests]
    B --> F[VD_ArtifactAnalizer Tests]
    B --> G[Configuration Tests]
    
    C --> H[End-to-End Processing]
    C --> I[UDP Communication]
    C --> J[File I/O Operations]
    
    D --> K[Processing Speed]
    D --> L[Memory Usage]
    D --> M[Concurrent Processing]
    
    style E fill:#e8f5e8
    style F fill:#e8f5e8
    style G fill:#e8f5e8
```

### Тестовые данные

| Тип теста | Входные данные | Ожидаемый результат |
|-----------|----------------|-------------------|
| **Clean Voice** | Чистый голосовой сигнал | 1 артефакт, высокая вероятность |
| **Noisy Voice** | Голос с шумом | 1 артефакт, средняя вероятность |
| **Music** | Музыкальный сигнал | 0 артефактов |
| **Silence** | Тишина | 0 артефактов |
| **Mixed** | Смешанный сигнал | Множественные артефакты |

---

## 10. Диаграммы и схемы

### UML диаграмма классов (детальная)

```mermaid
classDiagram
    class VD_Core {
        -VD_Unit* m_vdunit
        -VD_Init* m_vdinit
        +VD_Core(QObject*, QStringList)
        +initialize() void
    }
    
    class VD_Unit {
        -vd_detector::VD_VoiceDetector m_vd
        -VD_Init* p_init
        -std::string m_taskFile
        -std::string m_paramFile
        -bool isSingleShot
        -bool isDetected
        -bool isProcessingNow
        -bool isTaskSet
        +initialize() void
        +setVDInit(VD_Init*) void
        +acceptRESULT(std::string) void
        +slotStart() void
        +slotStop() void
        +slotStatusCheck() void
        +setOpt1(QString) void
        +setOpt2(QString) void
        -setupVD() void
    }
    
    class VD_VoiceDetector {
        -TDBuffer m_tdInputBuf
        -TDBuffer m_tdOutputBuf
        -N_Model m_nmodel
        -VocalDetector m_vd
        -FDBuffer m_fdBuf
        -VD_ArtifactAnalizer m_aa
        -VDATA_T m_fs
        -bool m_isArtifactDetected
        -bool m_export_data
        -std::string m_sampleFileName
        -std::string m_voiceFileName
        -std::string m_processingFileName
        -CallBackProto* p_resultSaved
        +processFile(std::string) bool
        +isArtifactDetected() bool
        +setupDetector(VD_Coef_Frame) void
        +setupNoiseMod(NM_Coef_Frame) void
        +setupArtAn(VD_Coef_Frame, AA_Coef_Frame) void
        +setResultPtr(CallBackProto*) void
        -createWAVdata(RESULT_LIST_T) void
        -generateFileNames() void
    }
    
    VD_Core --> VD_Unit : contains
    VD_Core --> VD_Init : contains
    VD_Unit --> VD_VoiceDetector : uses
```

### Схема потоков данных

```mermaid
graph LR
    subgraph "Input"
        A[WAV File]
        B[Configuration]
    end
    
    subgraph "Processing Pipeline"
        C[TDBuffer] --> D[FFT]
        D --> E[FDBuffer]
        E --> F[VocalDetector]
        F --> G[ArtifactAnalizer]
        G --> H[Output WAV]
    end
    
    subgraph "Export"
        E --> I[NPY Export]
        E --> J[TIFF Export]
        G --> K[NPY Export]
        G --> L[TIFF Export]
    end
    
    subgraph "Feedback"
        K[Noise Model] --> F
    end
    
    A --> C
    B --> F
    F --> K
    
    style A fill:#e1f5fe
    style H fill:#c8e6c9
    style I fill:#fff3e0
    style J fill:#fff3e0
```

---

*Документ создан: [Дата]*
*Версия: 1.0*
*Автор: [Имя]*

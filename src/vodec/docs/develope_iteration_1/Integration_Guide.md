# Инструкция по интеграции: Экспорт спектрограммы

> **Связанные документы:**
> - [Техническое задание](Technical_Specification.md) - требования и критерии приемки
> - [Документ проектирования](Design_Document.md) - архитектура и принципы решения
> - [План тестирования](Testing_Plan.md) - стратегия и процедуры тестирования
> - [Архитектурное видение проекта](../../../docs/project_vision.md) - общий контекст системы AP01

## 1. Подготовка зависимостей

### Создание структуры папок
```
import/
├── libnpy/
│   └── npy.h
└── tinytiff/
    └── tinytiff.h

src/vodec/
└── spectrogram_exporter.h
```

### Загрузка библиотек

Детальное обоснование выбора библиотек и критерии выбора описаны в [документе проектирования](Design_Document.md#2-выбор-библиотек).

#### libnpy (для NPY формата)
```bash
# Скачать npy.h из репозитория:
# https://github.com/llohse/libnpy
# Разместить в import/libnpy/npy.h
```

#### tinytiff (для TIFF формата)
```bash
# Скачать tinytiff.h из репозитория:
# https://github.com/gegles/tinytiff
# Разместить в import/tinytiff/tinytiff.h
```

### Проверка зависимостей
- Убедиться, что файлы загружены корректно
- Проверить совместимость с C++20
- Убедиться в отсутствии дополнительных зависимостей

## 2. Изменения в коде

### Шаг 1: Создание SpectrogramExporter

Создать файл `src/vodec/spectrogram_exporter.h`:

```cpp
#ifndef SPECTROGRAM_EXPORTER_H
#define SPECTROGRAM_EXPORTER_H

#ifdef DEBUG_SPECTROGRAM_SAVE
#include <string>
#include <vector>
#include "../../import/libnpy/npy.h"
#include "../../import/tinytiff/tinytiff.h"

class SpectrogramExporter {
private:
    std::string m_outputDir;
    std::string m_prefix;
    bool m_enabled;
    
    std::string generateTimestamp();
    std::vector<uint8_t> normalizeData(const std::vector<float>& data);
    
public:
    SpectrogramExporter(const std::string& outputDir = ".", const std::string& prefix = "spectrogram");
    void exportToNPY(const std::vector<float>& data, 
                     size_t width, size_t height,
                     const std::string& timestamp = "");
    void exportToTIFF(const std::vector<float>& data, 
                      size_t width, size_t height,
                      const std::string& timestamp = "");
    void setEnabled(bool enabled);
    void setOutputDir(const std::string& dir);
    void setPrefix(const std::string& prefix);
};

#endif // DEBUG_SPECTROGRAM_SAVE
#endif // SPECTROGRAM_EXPORTER_H
```

### Шаг 2: Реализация SpectrogramExporter

Создать файл `src/vodec/spectrogram_exporter.cpp`:

```cpp
#ifdef DEBUG_SPECTROGRAM_SAVE
#include "spectrogram_exporter.h"
#include <chrono>
#include <iomanip>
#include <sstream>
#include <algorithm>

SpectrogramExporter::SpectrogramExporter(const std::string& outputDir, const std::string& prefix) 
    : m_outputDir(outputDir), m_prefix(prefix), m_enabled(true) {
}

std::string SpectrogramExporter::generateTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time_t = std::chrono::system_clock::to_time_t(now);
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
        now.time_since_epoch()) % 1000;
    
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time_t), "%Y%m%d_%H%M%S");
    ss << "_" << std::setfill('0') << std::setw(3) << ms.count();
    return ss.str();
}

std::vector<uint8_t> SpectrogramExporter::normalizeData(const std::vector<float>& data) {
    if (data.empty()) return {};
    
    auto minmax = std::minmax_element(data.begin(), data.end());
    float minVal = *minmax.first;
    float maxVal = *minmax.second;
    
    std::vector<uint8_t> normalized(data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        float normalized_val = (data[i] - minVal) / (maxVal - minVal);
        normalized[i] = static_cast<uint8_t>(normalized_val * 255);
    }
    
    return normalized;
}

void SpectrogramExporter::exportToNPY(const std::vector<float>& data,
                                       size_t width, size_t height,
                                       const std::string& timestamp) {
    if (!m_enabled || data.empty()) return;
    
    std::string ts = timestamp.empty() ? generateTimestamp() : timestamp;
    std::string fileName = m_outputDir + "/" + m_prefix + "_" + ts + ".npy";
    
    try {
        // Экспорт в NPY (сырые данные)
        npy::save(fileName, data, {height, width});
        qInfo() << "Spectrogram NPY exported:" << QString::fromStdString(fileName);
    } catch (const std::exception& e) {
        qWarning() << "Failed to export NPY:" << e.what();
    }
}

void SpectrogramExporter::exportToTIFF(const std::vector<float>& data,
                                       size_t width, size_t height,
                                       const std::string& timestamp) {
    if (!m_enabled || data.empty()) return;
    
    std::string ts = timestamp.empty() ? generateTimestamp() : timestamp;
    std::string fileName = m_outputDir + "/" + m_prefix + "_" + ts + ".tiff";
    
    try {
        // Экспорт в TIFF (нормализованные данные)
        auto normalized = normalizeData(data);
        tinytiff::write(fileName.c_str(), width, height, normalized.data());
        qInfo() << "Spectrogram TIFF exported:" << QString::fromStdString(fileName);
    } catch (const std::exception& e) {
        qWarning() << "Failed to export TIFF:" << e.what();
    }
}

void SpectrogramExporter::setEnabled(bool enabled) {
    m_enabled = enabled;
}

void SpectrogramExporter::setOutputDir(const std::string& dir) {
    m_outputDir = dir;
}

void SpectrogramExporter::setPrefix(const std::string& prefix) {
    m_prefix = prefix;
}

#endif // DEBUG_SPECTROGRAM_SAVE
```

### Шаг 3: Интеграция в VD_VoiceDetector

В файле `src/vodec/vd_voicedetector.h`:

```cpp
// Добавить в private секцию
#ifdef DEBUG_SPECTROGRAM_SAVE
    #include "spectrogram_exporter.h"
    SpectrogramExporter m_spectrogramExporter;
#endif
```

В файле `src/vodec/vd_voicedetector.cpp`:

```cpp
// В конструкторе
#ifdef DEBUG_SPECTROGRAM_SAVE
    m_spectrogramExporter.setOutputDir(".");
#endif

// В методе обработки данных (после получения FDBuffer)
#ifdef DEBUG_SPECTROGRAM_SAVE
    if (m_export_data) {
        // Преобразование FDBuffer в std::vector<float>
        std::vector<float> spectrogramData;
        // ... код преобразования данных ...
        
        std::string timestamp = generateTimestamp();
        m_spectrogramExporter.exportToNPY(spectrogramData, 
                                         FDFrameSize, FDNumFramesInBuf, 
                                         timestamp);
        m_spectrogramExporter.exportToTIFF(spectrogramData, 
                                          FDFrameSize, FDNumFramesInBuf, 
                                          timestamp);
    }
#endif
```

## 3. Настройка сборки

### Изменения в vodec.pro

```pro
# Добавить в vodec.pro
DEFINES += DEBUG_SPECTROGRAM_SAVE  # только для debug сборки

# Добавить пути к заголовочным файлам
INCLUDEPATH += ../../import/libnpy
INCLUDEPATH += ../../import/tinytiff

# Добавить исходные файлы
SOURCES += spectrogram_exporter.cpp
```

### Условная компиляция

Убедиться, что весь код экспорта обернут в:
```cpp
#ifdef DEBUG_SPECTROGRAM_SAVE
    // код экспорта
#endif
```

## 4. Конфигурация

### Добавление в INI файл

В секцию `[VoDec]` добавить:
```ini
[VoDec]
# ... существующие параметры ...
export_spectrogram = true
spectrogram_output_dir = ./spectrograms
```

### Чтение конфигурации

В `vd_init.cpp` добавить:
```cpp
#ifdef DEBUG_SPECTROGRAM_SAVE
    bool exportSpectrogram = ini.getBool("VoDec", "export_spectrogram", false);
    std::string outputDir = ini.getString("VoDec", "spectrogram_output_dir", ".");
    
    if (exportSpectrogram) {
        m_voicedetector->setSpectrogramExport(true, outputDir);
    }
#endif
```

## 5. Проверка работоспособности

> **Примечание:** Детальная стратегия тестирования и автоматизированные тесты описаны в [плане тестирования](Testing_Plan.md).

### Шаг 1: Компиляция
```bash
cd src/vodec
qmake vodec.pro
make
```

### Шаг 2: Тестирование экспорта
```bash
# Запуск с тестовым файлом
./vodec -t test_audio.wav

# Проверка создания файлов
ls -la spectrogram_*.npy spectrogram_*.tiff
```

### Шаг 3: Верификация данных

#### Проверка NPY файла (Python)
```python
import numpy as np
data = np.load('spectrogram_20241201_143022.npy')
print(f"Shape: {data.shape}")
print(f"Data type: {data.dtype}")
print(f"Min/Max: {data.min()}/{data.max()}")
```

#### Проверка TIFF файла
- Открыть в любом просмотрщике изображений
- Убедиться в корректном отображении спектрограммы

### Шаг 4: Проверка условной компиляции
```bash
# Сборка без флага DEBUG_SPECTROGRAM_SAVE
# Убедиться, что код экспорта не включен в release сборку
```

---

*Документ создан: [Дата]*
*Версия: 1.0*
*Статус: Черновик*

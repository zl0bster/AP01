# Документ проектирования: Экспорт спектрограммы

> **Связанные документы:**
> - [Техническое задание](Technical_Specification.md) - требования и критерии приемки
> - [Архитектурное видение проекта](../../../docs/project_vision.md) - общий контекст системы AP01
> - [Инструкция по интеграции](Integration_Guide.md) - пошаговая реализация
> - [План тестирования](Testing_Plan.md) - стратегия и процедуры тестирования

## 1. Архитектурный обзор

### Общая концепция
Добавление функциональности экспорта спектрограммы как дополнительного модуля, интегрированного в существующую архитектуру vodec без нарушения основного потока обработки данных. Данное решение основано на требованиях, изложенных в [техническом задании](Technical_Specification.md).

### Принципы проектирования
- **Минимальное вмешательство**: Изменения только в точках получения данных спектрограммы
- **Условная компиляция**: Весь код экспорта изолирован директивой `#ifdef DEBUG_SPECTROGRAM_SAVE`
- **Инкапсуляция**: Логика экспорта вынесена в отдельный класс/модуль
- **Обратная совместимость**: Существующий API остается неизменным
- **Изолированная разработка**: Все изменения выполняются в отдельной ветке `dev_spectrogram_export`

### Место в архитектуре

Данное решение интегрируется в существующую архитектуру модуля vodec, которая является частью общей системы AP01. Подробная архитектура системы описана в [архитектурном видении проекта](../../../docs/project_vision.md).

```
VD_VoiceDetector (существующий)
    ↓ получает FDBuffer данные
SpectrogramExporter (новый)
    ↓ экспортирует в NPY/TIFF
FileSystem (система)
```

## 2. Выбор библиотек

### Критерии выбора
- **Header-only**: Не требует компиляции отдельных модулей
- **C++20 совместимость**: Работа с современным стандартом C++
- **Минимальные зависимости**: Простота интеграции
- **Активная поддержка**: Стабильные, поддерживаемые библиотеки

### Рекомендуемые библиотеки

#### Для NPY формата
- **libnpy**: Современная header-only библиотека для работы с NPY файлами
- **Альтернатива**: cnpy (устаревшая, но стабильная)

#### Для TIFF формата  
- **tinytiff**: Специализированная header-only библиотека для TIFF файлов
- **Альтернатива**: stb_image_write.h (универсальная для изображений)

### Обоснование выбора
- **libnpy + tinytiff**: Минимальный набор зависимостей
- **Простота интеграции**: Один заголовочный файл на библиотеку
- **Широкая поддержка**: Популярные, проверенные решения

## 3. Структура кода

### Вариант 1: Отдельный класс (Рекомендуемый)
```cpp
class SpectrogramExporter {
private:
    std::string m_outputDir;
    std::string m_prefix;
    bool m_enabled;
    
public:
    SpectrogramExporter(const std::string& outputDir = ".", const std::string& prefix = "spectrogram");
    void exportToNPY(const FDBuffer& data, const std::string& timestamp);
    void exportToTIFF(const FDBuffer& data, const std::string& timestamp);
    void setEnabled(bool enabled);
    void setPrefix(const std::string& prefix);
};
```

### Вариант 2: Статические функции
```cpp
namespace SpectrogramExport {
    void exportToNPY(const FDBuffer& data, const std::string& filename);
    void exportToTIFF(const FDBuffer& data, const std::string& filename);
    std::string generateTimestamp();
}
```

### Рекомендация
**Вариант 1 (отдельный класс)** - лучше для:
- Инкапсуляции состояния (настройки, директория)
- Будущего расширения функциональности
- Тестирования и изоляции зависимостей

## 4. Процесс экспорта

### Алгоритм экспорта
```
1. Получение данных FDBuffer из VD_VoiceDetector
2. Генерация уникального timestamp
3. Вызов exportToNPY():
   - Сохранение сырых данных в NPY формат
   - Обработка ошибок записи
4. Вызов exportToTIFF():
   - Нормализация данных для TIFF (0-255)
   - Сохранение нормализованных данных в TIFF формат
   - Обработка ошибок записи
```

### Нормализация данных
```cpp
// Для TIFF: преобразование float [0,1] → uint8 [0,255]
uint8_t normalizeToUint8(float value, float min, float max) {
    return static_cast<uint8_t>((value - min) / (max - min) * 255);
}
```

### Обработка ошибок
- Проверка доступности директории записи
- Валидация размера данных перед записью
- Логирование ошибок через существующую систему логирования vodec

## 5. Интеграция с существующей системой

### Точки интеграции
1. **VD_VoiceDetector::processFrame()** - вызов экспорта после обработки кадра
2. **VD_Init** - инициализация экспортера при старте
3. **Система сборки** - добавление флага `DEBUG_SPECTROGRAM_SAVE`

### Изменения в существующем коде
```cpp
// В VD_VoiceDetector.h
#ifdef DEBUG_SPECTROGRAM_SAVE
    #include "spectrogram_exporter.h"
    SpectrogramExporter m_spectrogramExporter;
#endif

// В VD_VoiceDetector::processFrame()
#ifdef DEBUG_SPECTROGRAM_SAVE
    if (m_export_data) {
        std::string timestamp = generateTimestamp();
        m_spectrogramExporter.exportToNPY(fdBuffer, timestamp);
        m_spectrogramExporter.exportToTIFF(fdBuffer, timestamp);
    }
#endif
```

### Настройка системы сборки
```pro
# В vodec.pro
DEFINES += DEBUG_SPECTROGRAM_SAVE  # только для debug сборки
INCLUDEPATH += ../../import/libnpy
INCLUDEPATH += ../../import/tinytiff
```

### Конфигурация
- **Директория экспорта**: Настраивается через параметр в INI файле
- **Префикс файлов**: Настраивается через метод `setPrefix()` (по умолчанию "spectrogram")
- **Включение/отключение**: Флаг `m_export_data` в существующей конфигурации
- **Формат имен файлов**: `{prefix}_YYYYMMDD_HHMMSS.npy/tiff`

Детальная инструкция по настройке и интеграции описана в [инструкции по интеграции](Integration_Guide.md).

---

*Документ создан: [Дата]*
*Версия: 1.0*
*Статус: Черновик*

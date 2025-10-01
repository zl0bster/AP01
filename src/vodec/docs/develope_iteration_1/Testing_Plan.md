# План тестирования: Экспорт спектрограммы

## 1. Стратегия тестирования

### Принципы тестирования
- **Простота**: Минимальная инфраструктура тестирования
- **Практичность**: Фокус на реальных сценариях использования
- **Автоматизация**: Где возможно, автоматические проверки
- **Верификация**: Проверка как функциональности, так и качества данных

### Уровни тестирования
1. **Модульные тесты**: Изолированное тестирование SpectrogramExporter
2. **Интеграционные тесты**: Тестирование в контексте vodec модуля
3. **Верификация данных**: Проверка корректности выходных файлов
4. **Системные тесты**: Полный цикл обработки с экспортом

### Инструменты тестирования
- **Qt Test Framework**: Для модульных тестов
- **Python/NumPy**: Для верификации NPY файлов
- **Стандартные утилиты**: Для проверки TIFF файлов
- **Ручное тестирование**: Для визуальной проверки

## 2. Модульные тесты

### Тест 1: Создание и инициализация SpectrogramExporter
```cpp
// tst_spectrogram_exporter.cpp
void TestSpectrogramExporter::testInitialization() {
    SpectrogramExporter exporter("./test_output");
    QVERIFY(exporter.isEnabled());
    QCOMPARE(exporter.getOutputDir(), "./test_output");
}
```

### Тест 2: Генерация timestamp
```cpp
void TestSpectrogramExporter::testTimestampGeneration() {
    SpectrogramExporter exporter;
    std::string ts1 = exporter.generateTimestamp();
    std::string ts2 = exporter.generateTimestamp();
    
    QVERIFY(!ts1.empty());
    QVERIFY(!ts2.empty());
    QVERIFY(ts1 != ts2); // Уникальность
}
```

### Тест 3: Нормализация данных
```cpp
void TestSpectrogramExporter::testDataNormalization() {
    std::vector<float> testData = {0.0f, 0.5f, 1.0f, 0.25f, 0.75f};
    auto normalized = exporter.normalizeData(testData);
    
    QCOMPARE(normalized.size(), testData.size());
    QCOMPARE(normalized[0], 0);    // min -> 0
    QCOMPARE(normalized[2], 255);  // max -> 255
    QVERIFY(normalized[1] == 127); // 0.5 -> 127
}
```

### Тест 4: Экспорт с отключенным флагом
```cpp
void TestSpectrogramExporter::testDisabledExport() {
    SpectrogramExporter exporter;
    exporter.setEnabled(false);
    
    std::vector<float> testData = {0.1f, 0.2f, 0.3f};
    // Не должно создавать файлы
    exporter.exportSpectrogram(testData, 1, 3);
    
    // Проверка, что файлы не созданы
    QVERIFY(!QFile::exists("./spectrogram_*.npy"));
    QVERIFY(!QFile::exists("./spectrogram_*.tiff"));
}
```

## 3. Интеграционные тесты

### Тест 1: Интеграция с VD_VoiceDetector
```cpp
// В существующем тесте vodec
void TestVoDecIntegration::testSpectrogramExport() {
    // Настройка тестового окружения
    VD_VoiceDetector detector;
    detector.setExportData(true);
    
    // Загрузка тестового WAV файла
    detector.loadTestFile("test_audio.wav");
    
    // Обработка одного кадра
    detector.processFrame();
    
    // Проверка создания файлов
    QVERIFY(QDir().entryList({"spectrogram_*.npy"}).size() > 0);
    QVERIFY(QDir().entryList({"spectrogram_*.tiff"}).size() > 0);
}
```

### Тест 2: Условная компиляция
```cpp
void TestConditionalCompilation::testDebugFlag() {
    // Тест с включенным флагом
    #ifdef DEBUG_SPECTROGRAM_SAVE
        QVERIFY(true); // Код должен компилироваться
    #else
        QSKIP("DEBUG_SPECTROGRAM_SAVE not defined");
    #endif
}
```

### Тест 3: Обработка ошибок
```cpp
void TestErrorHandling::testInvalidDirectory() {
    SpectrogramExporter exporter("/nonexistent/directory");
    
    std::vector<float> testData = {0.1f, 0.2f, 0.3f};
    // Не должно вызывать исключений
    QVERIFY_NO_THROW(exporter.exportSpectrogram(testData, 1, 3));
}
```

## 4. Верификация данных

### Тест 1: Проверка NPY файла
```python
# verify_npy.py
import numpy as np
import os
import glob

def test_npy_file():
    npy_files = glob.glob("spectrogram_*.npy")
    assert len(npy_files) > 0, "No NPY files found"
    
    for npy_file in npy_files:
        data = np.load(npy_file)
        
        # Проверка структуры данных
        assert data.ndim == 2, f"Expected 2D array, got {data.ndim}D"
        assert data.dtype == np.float32, f"Expected float32, got {data.dtype}"
        
        # Проверка диапазона значений
        assert np.isfinite(data).all(), "Data contains NaN or Inf values"
        assert data.min() >= 0, "Data contains negative values"
        
        print(f"✓ {npy_file}: shape={data.shape}, range=[{data.min():.3f}, {data.max():.3f}]")

if __name__ == "__main__":
    test_npy_file()
```

### Тест 2: Проверка TIFF файла
```bash
# verify_tiff.sh
#!/bin/bash

tiff_files=(spectrogram_*.tiff)
if [ ${#tiff_files[@]} -eq 0 ]; then
    echo "❌ No TIFF files found"
    exit 1
fi

for tiff_file in "${tiff_files[@]}"; do
    # Проверка, что файл существует и не пустой
    if [ ! -s "$tiff_file" ]; then
        echo "❌ $tiff_file is empty or doesn't exist"
        exit 1
    fi
    
    # Проверка, что это валидный TIFF файл
    if ! file "$tiff_file" | grep -q "TIFF"; then
        echo "❌ $tiff_file is not a valid TIFF file"
        exit 1
    fi
    
    echo "✓ $tiff_file is valid"
done
```

### Тест 3: Сравнение данных NPY и TIFF
```python
# compare_formats.py
import numpy as np
from PIL import Image
import glob

def compare_npy_tiff():
    npy_files = glob.glob("spectrogram_*.npy")
    tiff_files = glob.glob("spectrogram_*.tiff")
    
    for npy_file in npy_files:
        base_name = npy_file.replace('.npy', '')
        tiff_file = base_name + '.tiff'
        
        if tiff_file not in tiff_files:
            print(f"❌ No corresponding TIFF file for {npy_file}")
            continue
            
        # Загрузка данных
        npy_data = np.load(npy_file)
        tiff_data = np.array(Image.open(tiff_file))
        
        # Проверка размеров
        assert npy_data.shape == tiff_data.shape, f"Shape mismatch: {npy_data.shape} vs {tiff_data.shape}"
        
        # Проверка, что TIFF является нормализованной версией NPY
        npy_normalized = ((npy_data - npy_data.min()) / (npy_data.max() - npy_data.min()) * 255).astype(np.uint8)
        
        # Допустимая погрешность при нормализации
        diff = np.abs(tiff_data.astype(float) - npy_normalized.astype(float))
        max_diff = np.max(diff)
        
        assert max_diff < 2, f"Normalization error too large: {max_diff}"
        print(f"✓ {base_name}: formats match (max diff: {max_diff:.2f})")

if __name__ == "__main__":
    compare_npy_tiff()
```

## 5. Критерии успеха

### Функциональные критерии
- [ ] **Создание файлов**: NPY и TIFF файлы создаются при включенном флаге
- [ ] **Уникальность имен**: Временные метки в именах файлов уникальны
- [ ] **Условная компиляция**: Код не включается при отключенном флаге
- [ ] **Обработка ошибок**: Приложение не падает при ошибках записи

### Качественные критерии
- [ ] **Корректность NPY**: Данные загружаются в NumPy без ошибок
- [ ] **Корректность TIFF**: Файлы открываются в стандартных просмотрщиках
- [ ] **Соответствие данных**: TIFF является нормализованной версией NPY
- [ ] **Производительность**: Экспорт не замедляет основную обработку более чем на 10%

### Критерии интеграции
- [ ] **Совместимость API**: Существующий API vodec не изменен
- [ ] **Конфигурация**: Настройки читаются из INI файла
- [ ] **Логирование**: Ошибки и успешные операции логируются
- [ ] **Сборка**: Проект компилируется в debug и release режимах

### Автоматизация тестирования
```bash
# run_tests.sh
#!/bin/bash

echo "Running spectrogram export tests..."

# Компиляция тестов
qmake && make

# Запуск модульных тестов
./tst_spectrogram_exporter

# Запуск интеграционных тестов
./vodec -t test_audio.wav

# Верификация данных
python3 verify_npy.py
bash verify_tiff.sh
python3 compare_formats.py

echo "All tests completed!"
```

---

*Документ создан: [Дата]*
*Версия: 1.0*
*Статус: Черновик*

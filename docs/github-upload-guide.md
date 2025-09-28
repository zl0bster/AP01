# Загрузка проекта AP01 в GitHub

## Пошаговая инструкция

### Шаг 1: Создание репозитория на GitHub

1. **Войдите в GitHub**
   - Откройте https://github.com
   - Войдите в свой аккаунт

2. **Создайте новый репозиторий**
   - Нажмите зеленую кнопку "New" или "+" → "New repository"
   - Заполните форму:
     - **Repository name**: `AP01` (или любое другое имя)
     - **Description**: `Audio Processing System - Modular audio processing with Qt`
     - **Visibility**: 
       - ✅ Public (для открытого доступа)
       - 🔒 Private (только для вас и приглашенных)
     - **Initialize repository**: 
       - ❌ НЕ ставьте галочки (README, .gitignore, license)
       - У нас уже есть эти файлы

3. **Нажмите "Create repository"**

### Шаг 2: Подключение локального репозитория к GitHub

После создания репозитория GitHub покажет инструкции. Выберите вариант **"push an existing repository from the command line"**.

Выполните команды в терминале:

```bash
# Добавить удаленный репозиторий (замените YOUR_USERNAME на ваш GitHub username)
git remote add origin https://github.com/YOUR_USERNAME/AP01.git

# Переименовать основную ветку в main (современный стандарт GitHub)
git branch -M main

# Отправить код на GitHub
git push -u origin main
```

### Шаг 3: Проверка загрузки

1. Обновите страницу репозитория на GitHub
2. Убедитесь, что все файлы загружены
3. Проверьте, что README.md отображается на главной странице

## Альтернативные способы

### Через GitHub CLI (если установлен)

```bash
# Создать репозиторий и загрузить код одной командой
gh repo create AP01 --public --source=. --remote=origin --push
```

### Через SSH (если настроен SSH ключ)

```bash
# Добавить SSH remote
git remote add origin git@github.com:YOUR_USERNAME/AP01.git

# Отправить код
git push -u origin main
```

## Настройки репозитория

### После загрузки рекомендуется настроить:

1. **Описание репозитория**
   - Нажмите ⚙️ рядом с "About"
   - Добавьте описание: "Modular audio processing system with Qt Framework"

2. **Темы (Topics)**
   - Добавьте теги: `qt`, `cpp`, `audio-processing`, `dsp`, `signal-processing`

3. **README отображение**
   - README.md автоматически отобразится на главной странице
   - Можно добавить логотип или диаграммы

4. **Issues и Wiki**
   - Включите Issues для багрепортов и предложений
   - Включите Wiki для расширенной документации

## Структура репозитория на GitHub

После загрузки ваш репозиторий будет содержать:

```
AP01/
├── .gitignore              # Исключения для Git
├── README.md               # Описание проекта
├── ap01.pro                # Основной файл проекта Qt
├── common.pri              # Общие настройки сборки
├── app.pri                 # Настройки приложений
├── docs/                   # Документация
│   ├── vision.md           # Архитектурное видение
│   ├── git-setup.md        # Настройка Git
│   └── github-upload-guide.md
├── src/                    # Исходный код
│   ├── bandmaster/         # Главный модуль
│   ├── recorder/           # Модуль записи
│   ├── psgenerator/        # Генератор сигналов
│   ├── splayer/            # Плеер
│   ├── vodec/              # Детектор голоса
│   ├── correl/             # Корреляционный анализ
│   └── include/            # Общие заголовки
├── lib_win64/              # Библиотеки для Windows
├── import/                 # Внешние библиотеки
└── tests/                  # Тесты
```

## Дальнейшая работа

### Клонирование репозитория (для других разработчиков)

```bash
git clone https://github.com/YOUR_USERNAME/AP01.git
cd AP01
```

### Обновление кода на GitHub

```bash
# Добавить изменения
git add .

# Сделать коммит
git commit -m "Описание изменений"

# Отправить на GitHub
git push origin main
```

### Создание релизов

1. Перейдите в раздел "Releases" на GitHub
2. Нажмите "Create a new release"
3. Создайте тег (например, v1.0.0)
4. Добавьте описание изменений
5. Прикрепите скомпилированные файлы (опционально)

## Полезные ссылки

- [GitHub Docs](https://docs.github.com/)
- [Git Handbook](https://guides.github.com/introduction/git-handbook/)
- [Qt Documentation](https://doc.qt.io/)

---

*После загрузки проекта на GitHub вы сможете делиться кодом, принимать участие в разработке и использовать все возможности платформы для совместной работы.*

# Настройка Git для проекта AP01

## Подготовка проекта для работы с Git

Проект AP01 теперь готов для работы с системой контроля версий Git. Выполнены следующие шаги:

### 1. Создание .gitignore
Создан файл `.gitignore` который исключает из версионирования:
- Временные файлы сборки (`build/`, `*.o`, `*.obj`)
- Сгенерированные Qt файлы (`moc_*`, `ui_*`, `qrc_*`)
- Исполняемые файлы (`*.exe`, `*.dll`)
- Логи и временные файлы (`*.log`, `*.tmp`)
- Аудиофайлы (`*.wav`, `*.mp3`)
- IDE файлы (`.vscode/`, `.idea/`)
- Системные файлы (`.DS_Store`, `Thumbs.db`)

### 2. Инициализация репозитория
```bash
git init
```

### 3. Первый коммит
Добавлены все исходные файлы проекта:
- Исходный код всех модулей
- Конфигурационные файлы сборки
- Документация
- Внешние библиотеки
- Тестовые файлы

## Рекомендуемые настройки Git

### Настройка пользователя
```bash
git config user.name "Ваше Имя"
git config user.email "your.email@example.com"
```

### Настройка окончаний строк (для Windows)
```bash
git config core.autocrlf true
```

### Настройка редактора
```bash
git config core.editor "code --wait"  # для VS Code
# или
git config core.editor "notepad"      # для Notepad
```

## Рабочий процесс с Git

### Основные команды
```bash
# Проверка статуса
git status

# Добавление файлов
git add .                    # все файлы
git add src/bandmaster/      # конкретная директория
git add *.cpp               # по маске

# Коммит изменений
git commit -m "Описание изменений"

# Просмотр истории
git log --oneline
git log --graph --oneline --all

# Просмотр изменений
git diff                    # незафиксированные изменения
git diff --cached          # изменения в индексе
git diff HEAD~1            # с предыдущим коммитом
```

### Работа с ветками
```bash
# Создание новой ветки
git branch feature/new-module
git checkout feature/new-module
# или одной командой:
git checkout -b feature/new-module

# Переключение между ветками
git checkout master
git checkout feature/new-module

# Слияние веток
git checkout master
git merge feature/new-module

# Удаление ветки
git branch -d feature/new-module
```

## Структура коммитов

### Рекомендуемый формат сообщений
```
<тип>: <краткое описание>

<подробное описание при необходимости>

<ссылки на задачи/issues>
```

### Типы коммитов
- `feat`: новая функциональность
- `fix`: исправление ошибки
- `docs`: изменения в документации
- `style`: форматирование кода
- `refactor`: рефакторинг без изменения функциональности
- `test`: добавление или изменение тестов
- `build`: изменения в системе сборки
- `ci`: изменения в CI/CD

### Примеры хороших коммитов
```bash
git commit -m "feat: add voice detection algorithm to VoDec module"
git commit -m "fix: resolve UDP connection timeout in LinkerUDP"
git commit -m "docs: update README with build instructions for Linux"
git commit -m "refactor: extract common audio processing functions"
```

## Работа с удаленными репозиториями

### Добавление удаленного репозитория
```bash
# GitHub
git remote add origin https://github.com/username/AP01.git

# GitLab
git remote add origin https://gitlab.com/username/AP01.git

# Локальный сервер
git remote add origin git@server:/path/to/repo.git
```

### Отправка изменений
```bash
# Первая отправка
git push -u origin master

# Последующие отправки
git push
```

### Получение изменений
```bash
git pull origin master
```

## Игнорирование файлов

### Временное игнорирование отслеживаемых файлов
```bash
# Прекратить отслеживание без удаления
git rm --cached filename

# Прекратить отслеживание директории
git rm -r --cached directory/
```

### Локальное игнорирование
Создайте файл `.git/info/exclude` для локальных исключений, которые не должны попасть в общий `.gitignore`.

## Полезные алиасы

Добавьте в `~/.gitconfig`:
```ini
[alias]
    st = status
    co = checkout
    br = branch
    ci = commit
    unstage = reset HEAD --
    last = log -1 HEAD
    visual = !gitk
    tree = log --graph --pretty=format:'%Cred%h%Creset -%C(yellow)%d%Creset %s %Cgreen(%cr) %C(bold blue)<%an>%Creset' --abbrev-commit
```

## Восстановление и откат

### Отмена изменений
```bash
# Отмена изменений в рабочей директории
git checkout -- filename

# Отмена всех изменений
git checkout -- .

# Отмена коммита (создает новый коммит)
git revert HEAD

# Сброс к предыдущему коммиту (ОСТОРОЖНО!)
git reset --hard HEAD~1
```

## Решение проблем

### Конфликты слияния
1. Откройте файлы с конфликтами
2. Найдите маркеры `<<<<<<<`, `=======`, `>>>>>>>`
3. Разрешите конфликты вручную
4. Добавьте файлы: `git add .`
5. Завершите слияние: `git commit`

### Случайно добавленные файлы
```bash
# Удалить из индекса, оставить в рабочей директории
git reset HEAD filename

# Удалить из репозитория, но оставить локально
git rm --cached filename
```

---

*Этот документ содержит основные рекомендации по работе с Git в проекте AP01. Для более подробной информации обращайтесь к официальной документации Git.*

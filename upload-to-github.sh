#!/bin/bash

echo "==============================================="
echo "    Загрузка проекта AP01 в GitHub"
echo "==============================================="
echo

echo "Шаг 1: Переименование ветки в main"
git branch -M main

echo
echo "Шаг 2: Добавление удаленного репозитория"
echo "ВНИМАНИЕ: Замените YOUR_USERNAME на ваш GitHub username!"
echo
read -p "Введите ваш GitHub username: " username

git remote add origin https://github.com/${username}/AP01.git

echo
echo "Шаг 3: Отправка кода на GitHub"
git push -u origin main

echo
echo "==============================================="
echo "    Загрузка завершена!"
echo "==============================================="
echo
echo "Ваш репозиторий доступен по адресу:"
echo "https://github.com/${username}/AP01"
echo

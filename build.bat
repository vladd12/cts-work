@echo off
:: Настраиваем окружение для Visual Studio
call "C:\Program Files\Microsoft Visual Studio\2022\Preview\VC\Auxiliary\Build\vcvarsx86_amd64.bat"

:: Переходим в директорию проекта
cd C:\Users\Julia\Downloads\1211test\misis2023f-22-4-chvikov_m_e-main

:: Создаем директорию для сборки
if not exist build mkdir build
cd build

:: Конфигурируем проект
cmake -G "Visual Studio 17 2022" -A x64 ..

:: Компилируем
cmake --build . --config Release

:: Пауза чтобы увидеть результат
pause
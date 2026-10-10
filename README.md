# cpp-hardware-info-tool

Низкоуровневая утилита сбора информации об аппаратной части ПК на C++17 под Windows.

## Возможности
- Чтение регистров процессора через интринсики `__cpuid` / `__cpuidex`.
- Детектирование точной модели процессора (Brand string), архитектуры и количества физических/логических ядер.
- Проверка поддержки ключевых наборов инструкций: **SSE, SSE2, SSE3, SSSE3, SSE4.1, SSE4.2, AVX, AVX2, AVX-512, FMA, AES, RDRAND**.
- Сбор данных об оперативной памяти через `GlobalMemoryStatusEx` (Всего ОЗУ, свободно, процент загрузки).
- Инспекция логических дисков (`GetLogicalDrives`, `GetDiskFreeSpaceExW`): тип накопителя (SSD/HDD/Removable) и свободное пространство.

## Сборка
```bash
g++ -O2 -std=c++17 src/main.cpp -o releases/cpp-hardware-info-tool.exe
```

## Использование
```bash
./releases/cpp-hardware-info-tool.exe
```

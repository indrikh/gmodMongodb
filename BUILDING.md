# Инструкция по сборке модуля gmodMongodb

Данная инструкция описывает процесс сборки бинарного модуля gmodMongodb для различных платформ.

## Требования

Для сборки модуля вам потребуется:

- CMake 3.5 или выше
- Компилятор с поддержкой C++14
- MongoDB C++ Driver (libmongocxx, libbsoncxx)
- Git

## Подготовка библиотек MongoDB

Сначала необходимо скачать и установить MongoDB C++ Driver. Модуль использует драйвер MongoDB для взаимодействия с базой данных.

### Установка MongoDB C++ Driver

#### Windows

1. Скачайте предкомпилированные библиотеки с официального сайта MongoDB или соберите их из исходников
2. Создайте в директории проекта папку `MongoDB` со следующей структурой:
   ```
   MongoDB/
   ├── include/
   │   ├── bsoncxx/
   │   └── mongocxx/
   ├── lib/windows/           # 32-битные библиотеки
   │   ├── bsoncxx.lib
   │   └── mongocxx.lib
   └── lib64/windows/         # 64-битные библиотеки
       ├── bsoncxx.lib
       └── mongocxx.lib
   ```

#### Linux

1. Установите MongoDB C++ Driver через пакетный менеджер или соберите из исходников
2. Создайте в директории проекта папку `MongoDB` со следующей структурой:
   ```
   MongoDB/
   ├── include/
   │   ├── bsoncxx/
   │   └── mongocxx/
   ├── lib/linux/             # 32-битные библиотеки
   │   ├── libbsoncxx.a
   │   └── libmongocxx.a
   └── lib64/linux/           # 64-битные библиотеки
       ├── libbsoncxx.a
       └── libmongocxx.a
   ```

## Сборка модуля

### Windows

1. Клонируйте репозиторий:
   ```bat
   git clone https://github.com/indrikh/gmodMongodb.git
   cd gmodMongodb
   ```

2. Создайте директорию для сборки и запустите CMake:
   ```bat
   mkdir build
   cd build
   ```

3. Для 32-битной версии:
   ```bat
   cmake -G "Visual Studio 17 2022" -A Win32 ..
   cmake --build . --config Release
   ```

4. Для 64-битной версии:
   ```bat
   cmake -G "Visual Studio 17 2022" -A x64 ..
   cmake --build . --config Release
   ```

### Linux

1. Клонируйте репозиторий:
   ```bash
   git clone https://github.com/indrikh/gmodMongodb.git
   cd gmodMongodb
   ```

2. Создайте директорию для сборки и запустите CMake:
   ```bash
   mkdir build
   cd build
   ```

3. Для 32-битной версии:
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_CXX_FLAGS=-m32 -DCMAKE_C_FLAGS=-m32 ..
   make
   ```

4. Для 64-битной версии:
   ```bash
   cmake -DCMAKE_BUILD_TYPE=Release ..
   make
   ```

## Установка модуля

После сборки скопируйте соответствующий бинарный файл в директорию `garrysmod/lua/bin/` вашего сервера Garry's Mod:

- Windows 32-bit: `gmodmongodb_win32.dll`
- Windows 64-bit: `gmodmongodb_win64.dll`
- Linux 32-bit: `gmodmongodb_linux.dll`
- Linux 64-bit: `gmodmongodb_linux64.dll`

## Проблемы и решения

### Ошибка "Библиотека не найдена"

Если при запуске возникает ошибка о ненайденной библиотеке:

1. Для Windows: убедитесь, что DLL-файлы MongoDB C++ Driver находятся в пути системы или рядом с бинарным файлом модуля
2. Для Linux: убедитесь, что все зависимости установлены:
   ```bash
   ldd gmodmongodb_linux64.dll
   ```

### Ошибка "Неправильная версия библиотеки"

Убедитесь, что используете совместимую версию MongoDB C++ Driver. Рекомендуется версия 3.6.0 или выше.

## Отладка

Для отладки модуля можно использовать встроенные отладочные функции:

```lua
print("Allocated objects:", mongodb.objectCount())
print("Allocation count:", mongodb.allocationCount())
print("Deallocation count:", mongodb.deallocationCount())
```

## Дополнительная информация

Для получения дополнительной информации о работе с модулем см. примеры в директории `examples/`.

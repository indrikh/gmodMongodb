# gmodMongodb

Модуль для Garry's Mod, обеспечивающий интерфейс взаимодействия с MongoDB.

## Особенности

- Асинхронные запросы к MongoDB
- Поддержка CRUD операций (создание, чтение, обновление, удаление)
- Полная поддержка BSON документов
- Callback-интерфейс в стиле MySQLOO
- Поддержка Windows и Linux (32 и 64 бит)

## Установка

1. Скачайте последнюю версию из [релизов](https://github.com/indrikh/gmodMongodb/releases)
2. Распакуйте бинарные файлы в папку `garrysmod/lua/bin/`

## Использование

```lua
require("gmodmongodb")

-- Создание подключения
local mongodb = mongodb.connect("mongodb://localhost:27017", "mydatabase")

-- Проверка соединения
function mongodb:onConnected()
  print("Подключение к MongoDB установлено!")
  
  -- Работа с коллекцией
  local collection = self:getCollection("players")
  
  -- Вставка документа
  local doc = collection:insert({
    steamid = "STEAM_0:1:12345",
    name = "Player",
    score = 100
  })
  
  doc:onSuccess(function()
    print("Документ успешно вставлен!")
  end)
  
  -- Поиск документов
  local query = collection:find({score = {["$gt"] = 50}})
  
  query:onSuccess(function(results)
    for k, v in pairs(results) do
      print(v.name .. ": " .. v.score)
    end
  end)
end

function mongodb:onConnectionFailed(err)
  print("Ошибка подключения к MongoDB: " .. err)
end
```

## Компиляция из исходников

Для компиляции необходимы:

- CMake 3.5+
- Компилятор с поддержкой C++14
- MongoDB C++ Driver (libmongocxx, libbsoncxx)

```bash
mkdir build && cd build
cmake ..
make
```

## Лицензия

MIT License
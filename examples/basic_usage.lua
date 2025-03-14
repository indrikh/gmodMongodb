-- Пример использования модуля MongoDB для Garry's Mod
-- Требуется загруженный бинарный модуль gmodmongodb

-- Загружаем модуль
require("gmodmongodb")

-- Создаем подключение к MongoDB
local mongo = mongodb.connect("mongodb://localhost:27017", "gmod_test")

-- Обработчик успешного подключения
function mongo:onConnected()
    print("Подключение к MongoDB успешно установлено!")
    
    -- Получаем коллекцию players
    local players = self:getCollection("players")
    
    -- Вставляем документ в коллекцию
    local insertQuery = players:insert([[
        {
            "steamid": "STEAM_0:1:12345678",
            "name": "TestPlayer",
            "score": 100,
            "inventory": ["weapon_pistol", "weapon_smg1"],
            "lastLogin": { "$date": "2025-03-14T12:00:00Z" },
            "settings": {
                "volume": 0.8,
                "sensitivity": 3.5,
                "notifications": true
            }
        }
    ]])
    
    -- Обработчик успешной вставки
    insertQuery:onSuccess(function()
        print("Документ успешно добавлен!")
        print("ID документа: " .. insertQuery:getLastInsertId())
        
        -- Ищем документы с определенными критериями
        local findQuery = players:find([[
            { "score": { "$gte": 50 } }
        ]])
        
        -- Настраиваем поиск
        findQuery:setSort([[ { "score": -1 } ]]) -- Сортировка по убыванию счета
        findQuery:setLimit(10) -- Ограничиваем выборку 10 документами
        
        -- Обработчик успешного поиска
        findQuery:onSuccess(function()
            local results = findQuery:getData()
            print("Найдено " .. #results .. " игроков с высоким счетом:")
            
            for i, player in ipairs(results) do
                print(i .. ". " .. player.name .. " - " .. player.score .. " очков")
            end
            
            -- Обновляем документ
            local updateQuery = players:update([[
                { "steamid": "STEAM_0:1:12345678" }
            ]], [[
                { "$set": { "score": 150 }, "$push": { "inventory": "item_healthkit" } }
            ]])
            
            -- Обработчик успешного обновления
            updateQuery:onSuccess(function()
                print("Документ успешно обновлен!")
                print("Обновлено документов: " .. updateQuery:getModifiedCount())
                
                -- Подсчитываем игроков с высоким счетом
                local countQuery = players:count([[ { "score": { "$gt": 100 } } ]])
                
                countQuery:onSuccess(function()
                    print("Игроков с более чем 100 очками: " .. countQuery:getCount())
                    
                    -- Удаляем тестовый документ
                    local deleteQuery = players:remove([[ { "steamid": "STEAM_0:1:12345678" } ]])
                    
                    deleteQuery:onSuccess(function()
                        print("Тестовый документ успешно удален!")
                        
                        -- Отключаемся от базы данных
                        mongo:disconnect()
                    end)
                    
                    -- Запускаем запрос удаления
                    deleteQuery:start()
                end)
                
                -- Запускаем запрос подсчета
                countQuery:start()
            end)
            
            -- Запускаем запрос обновления
            updateQuery:start()
        end)
        
        -- Запускаем запрос поиска
        findQuery:start()
    end)
    
    -- Запускаем запрос вставки
    insertQuery:start()
end

-- Обработчик ошибки подключения
function mongo:onConnectionFailed(err)
    print("Ошибка подключения к MongoDB: " .. err)
end

-- Обработчик отключения от базы данных
function mongo:onDisconnected()
    print("Соединение с MongoDB разорвано")
end

print("Подключение к MongoDB...")

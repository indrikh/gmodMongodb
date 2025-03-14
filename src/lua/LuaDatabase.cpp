#include "LuaDatabase.h"
#include "LuaCollection.h"
#include <iostream>

// Создание метатаблицы для класса базы данных
void LuaDatabase::createMetaTable(ILuaBase* LUA) {
    // Если метатаблица уже создана, не создаем её снова
    if (metaTables.find("Database") != metaTables.end()) {
        return;
    }

    // Создаем метатаблицу
    LUA->CreateTable();

    // Наследуем от базового объекта UserData
    LUA->ReferencePush(metaTables["UserData"]);
    LUA->SetField(-2, "__index");

    // Устанавливаем тип
    LUA->PushString("Database");
    LUA->SetField(-2, "__type");

    // Создаем таблицу методов
    LUA->CreateTable();

    // Добавляем методы
    LUA->PushCFunction(getCollection);
    LUA->SetField(-2, "getCollection");

    LUA->PushCFunction(escape);
    LUA->SetField(-2, "escape");

    LUA->PushCFunction(setCharacterSet);
    LUA->SetField(-2, "setCharacterSet");

    LUA->PushCFunction(wait);
    LUA->SetField(-2, "wait");

    LUA->PushCFunction(serverInfo);
    LUA->SetField(-2, "serverInfo");

    LUA->PushCFunction(hostInfo);
    LUA->SetField(-2, "hostInfo");

    LUA->PushCFunction(status);
    LUA->SetField(-2, "status");

    LUA->PushCFunction(setAutoReconnect);
    LUA->SetField(-2, "setAutoReconnect");

    LUA->PushCFunction(connect);
    LUA->SetField(-2, "connect");

    LUA->PushCFunction(disconnect);
    LUA->SetField(-2, "disconnect");

    LUA->PushCFunction(setConnectTimeout);
    LUA->SetField(-2, "setConnectTimeout");

    LUA->PushCFunction(query);
    LUA->SetField(-2, "query");

    LUA->PushCFunction(ping);
    LUA->SetField(-2, "ping");

    LUA->PushCFunction(queueSize);
    LUA->SetField(-2, "queueSize");

    LUA->PushCFunction(abortAllQueries);
    LUA->SetField(-2, "abortAllQueries");

    // Устанавливаем таблицу __index
    LUA->SetField(-2, "__index");

    // Сохраняем метатаблицу
    metaTables["Database"] = LUA->ReferenceCreate();
    ++referenceCreatedCount;
}

// Создание слабой таблицы для хранения ссылок на базы данных
void LuaDatabase::createWeakTable(ILuaBase* LUA) {
    LUA->CreateTable();
    LUA->CreateTable();
    LUA->PushString("v");
    LUA->SetField(-2, "__mode");
    LUA->SetMetaTable(-2);
    LUA->SetField(-2, "DBs");
}

// Выполнение всех think-хуков для всех баз данных
void LuaDatabase::runAllThinkHooks(ILuaBase* LUA) {
    LUA->GetField(-1, "DBs");
    LUA->PushNil();

    while (LUA->Next(-2)) {
        LuaDatabase* db = (LuaDatabase*)LUA->GetUserdata(-1);
        
        if (db && db->isValid()) {
            db->think(LUA);
        }
        
        LUA->Pop();
    }
    
    LUA->Pop();
}

// Обработка событий базы данных
void LuaDatabase::think(ILuaBase* LUA) {
    if (!m_database || !isValid()) {
        return;
    }

    // Проверяем состояние соединения
    auto finishedQueries = m_database->takeFinishedQueries();
    
    for (auto& pair : finishedQueries) {
        auto query = pair.first;
        auto success = query->isSuccess();
        
        if (success && query->getSuccessCallback() != 0) {
            LUA->ReferencePush(query->getSuccessCallback());
            LUA->Call(0, 0);
        } else if (!success && query->getFailCallback() != 0) {
            LUA->ReferencePush(query->getFailCallback());
            LUA->PushString(query->error().c_str());
            LUA->Call(1, 0);
        }
    }

    // Проверяем состояние подключения
    if (!m_dbCallbackRan && m_database->isConnectionDone()) {
        m_dbCallbackRan = true;

        LUA->ReferencePush(m_tableReference);
        
        if (m_database->connectionSuccessful()) {
            LUA->GetField(-1, "onConnected");
            
            if (LUA->IsType(-1, Type::Function)) {
                LUA->Push(1);
                LUA->Call(1, 0);
            } else {
                LUA->Pop();
            }
        } else {
            LUA->GetField(-1, "onConnectionFailed");
            
            if (LUA->IsType(-1, Type::Function)) {
                LUA->Push(1);
                LUA->PushString(m_database->connectionError().c_str());
                LUA->Call(2, 0);
            } else {
                LUA->Pop();
            }
        }
        
        LUA->Pop();
    }

    // Проверяем отключение
    if (m_database->wasDisconnected() && m_hasOnDisconnected) {
        LUA->ReferencePush(m_tableReference);
        LUA->GetField(-1, "onDisconnected");
        
        if (LUA->IsType(-1, Type::Function)) {
            LUA->Push(1);
            LUA->Call(1, 0);
        }
        
        LUA->Pop(2);
    }
}

// Обработка уничтожения объекта базы данных
void LuaDatabase::onDestroyedByLua(ILuaBase* LUA) {
    // Освобождаем ссылки
    if (m_tableReference != 0) {
        LUA->ReferenceFree(m_tableReference);
        m_tableReference = 0;
        ++referenceFreedCount;
    }
    
    // Отключаем БД
    if (m_database) {
        m_database->disconnect(false);
        m_database = nullptr;
    }
}

// Создание экземпляра базы данных
int LuaDatabase::create(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    
    // Проверяем аргументы
    if (!LUA->IsType(1, Type::String) || !LUA->IsType(2, Type::String)) {
        LUA->ThrowError("Expected mongodb.connect(uri, database)");
        return 0;
    }
    
    // Получаем параметры подключения
    const char* uri = LUA->GetString(1);
    const char* dbName = LUA->GetString(2);
    
    // Создаем объект базы данных
    std::shared_ptr<Database> database = Database::createDatabase(uri, dbName);
    
    // Создаем Lua-объект
    LuaDatabase* luaDb = (LuaDatabase*)LUA->NewUserdata(sizeof(LuaDatabase));
    new (luaDb) LuaDatabase(database);
    
    // Устанавливаем метатаблицу
    LUA->ReferencePush(metaTables["Database"]);
    LUA->SetMetaTable(-2);
    
    // Создаем таблицу для хранения методов базы данных
    LUA->CreateTable();
    luaDb->m_tableReference = LUA->ReferenceCreate();
    ++referenceCreatedCount;
    
    // Добавляем базу данных в слабую таблицу
    LUA->PushSpecial(SPECIAL_GLOB);
    LUA->GetField(-1, "mongodb");
    LUA->GetField(-1, "DBs");
    LUA->Push(-4);
    
    // Генерируем уникальный ключ
    char buffer[32];
    snprintf(buffer, sizeof(buffer), "%p", luaDb);
    LUA->PushString(buffer);
    
    LUA->PushUserdata(luaDb);
    LUA->SetTable(-3);
    LUA->Pop(3);
    
    // Запускаем подключение
    database->connect();
    
    return 1;
}

// Получение коллекции из базы данных
int LuaDatabase::getCollection(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    if (!LUA->IsType(2, Type::String)) {
        LUA->ThrowError("Expected database:getCollection(collectionName)");
        return 0;
    }
    
    const char* collectionName = LUA->GetString(2);
    std::shared_ptr<Collection> collection = luaDb->m_database->getCollection(collectionName);
    
    // Создаем Lua-объект коллекции
    LuaCollection* luaCol = (LuaCollection*)LUA->NewUserdata(sizeof(LuaCollection));
    new (luaCol) LuaCollection(collection);
    
    // Устанавливаем метатаблицу
    LUA->ReferencePush(metaTables["Collection"]);
    LUA->SetMetaTable(-2);
    
    return 1;
}

// Экранирование строки
int LuaDatabase::escape(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    if (!LUA->IsType(2, Type::String)) {
        LUA->ThrowError("Expected database:escape(string)");
        return 0;
    }
    
    const char* str = LUA->GetString(2);
    LUA->PushString(luaDb->m_database->escape(str).c_str());
    
    return 1;
}

// Установка набора символов
int LuaDatabase::setCharacterSet(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    if (!LUA->IsType(2, Type::String)) {
        LUA->ThrowError("Expected database:setCharacterSet(charset)");
        return 0;
    }
    
    const char* charset = LUA->GetString(2);
    bool success = luaDb->m_database->setCharacterSet(charset);
    LUA->PushBool(success);
    
    return 1;
}

// Ожидание выполнения запросов
int LuaDatabase::wait(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    luaDb->m_database->wait();
    return 0;
}

// Получение информации о сервере
int LuaDatabase::serverInfo(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    LUA->PushString(luaDb->m_database->serverInfo().c_str());
    return 1;
}

// Получение информации о хосте
int LuaDatabase::hostInfo(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    LUA->PushString("");  // В MongoDB нет прямого аналога hostInfo
    return 1;
}

// Получение статуса соединения
int LuaDatabase::status(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    LUA->PushNumber(luaDb->m_database->status());
    return 1;
}

// Установка автоматического переподключения
int LuaDatabase::setAutoReconnect(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    if (!LUA->IsType(2, Type::Bool)) {
        LUA->ThrowError("Expected database:setAutoReconnect(bool)");
        return 0;
    }
    
    bool autoReconnect = LUA->GetBool(2);
    luaDb->m_database->setShouldAutoReconnect(autoReconnect);
    
    return 0;
}

// Подключение к базе данных
int LuaDatabase::connect(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    luaDb->m_database->connect();
    luaDb->m_dbCallbackRan = false;
    
    return 0;
}

// Отключение от базы данных
int LuaDatabase::disconnect(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    bool wait = false;
    if (LUA->IsType(2, Type::Bool)) {
        wait = LUA->GetBool(2);
    }
    
    luaDb->m_database->disconnect(wait);
    
    return 0;
}

// Установка таймаута подключения
int LuaDatabase::setConnectTimeout(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    if (!LUA->IsType(2, Type::Number)) {
        LUA->ThrowError("Expected database:setConnectTimeout(timeout)");
        return 0;
    }
    
    unsigned int timeout = LUA->GetNumber(2);
    luaDb->m_database->setConnectTimeout(timeout);
    
    return 0;
}

// Создание запроса (не используется напрямую в MongoDB)
int LuaDatabase::query(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LUA->ThrowError("Direct query is not supported in MongoDB. Use collection operations instead.");
    return 0;
}

// Проверка соединения
int LuaDatabase::ping(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    bool success = luaDb->m_database->ping();
    LUA->PushBool(success);
    
    return 1;
}

// Получение размера очереди запросов
int LuaDatabase::queueSize(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    LUA->PushNumber(luaDb->m_database->queueSize());
    return 1;
}

// Отмена всех запросов
int LuaDatabase::abortAllQueries(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaDatabase* luaDb = (LuaDatabase*)LUA->GetUserdata(1);
    
    if (!luaDb || !luaDb->isValid()) {
        LUA->ThrowError("Trying to use an invalid database");
        return 0;
    }
    
    luaDb->m_database->abortAllQueries();
    return 0;
}
#include "LuaIQuery.h"

// Конструктор
LuaIQuery::LuaIQuery(std::shared_ptr<IQuery> query) : LuaObject("IQuery"), m_query(std::move(query)) {
}

// Проверка валидности запроса
bool LuaIQuery::isValid() const {
    return m_query != nullptr;
}

// Обработка уничтожения запроса в Lua
void LuaIQuery::onDestroyedByLua(ILuaBase* LUA) {
    m_query = nullptr;
}

// Создание метатаблицы для типа запроса
void LuaIQuery::createMetaTable(ILuaBase* LUA) {
    // Если метатаблица уже создана, не создаем её снова
    if (metaTables.find("IQuery") != metaTables.end()) {
        return;
    }

    // Создаем метатаблицу
    LUA->CreateTable();

    // Наследуем от базового объекта UserData
    LUA->ReferencePush(metaTables["UserData"]);
    LUA->SetField(-2, "__index");

    // Записываем тип
    LUA->PushString("IQuery");
    LUA->SetField(-2, "__type");

    // Устанавливаем общие методы для всех запросов
    LUA->CreateTable();

    // Добавляем методы
    LUA->PushCFunction(start);
    LUA->SetField(-2, "start");

    LUA->PushCFunction(wait);
    LUA->SetField(-2, "wait");

    LUA->PushCFunction(error);
    LUA->SetField(-2, "error");

    LUA->PushCFunction(status);
    LUA->SetField(-2, "status");

    LUA->PushCFunction(abort);
    LUA->SetField(-2, "abort");

    LUA->PushCFunction(isRunning);
    LUA->SetField(-2, "isRunning");

    LUA->PushCFunction(isSuccess);
    LUA->SetField(-2, "isSuccess");

    LUA->PushCFunction(affectedRows);
    LUA->SetField(-2, "affectedRows");

    LUA->PushCFunction(setOption);
    LUA->SetField(-2, "setOption");

    LUA->PushCFunction(setTimeout);
    LUA->SetField(-2, "setTimeout");

    LUA->PushCFunction(onSuccess);
    LUA->SetField(-2, "onSuccess");

    LUA->PushCFunction(onError);
    LUA->SetField(-2, "onError");

    LUA->PushCFunction(onAbort);
    LUA->SetField(-2, "onAbort");

    LUA->PushCFunction(onData);
    LUA->SetField(-2, "onData");

    // Устанавливаем таблицу __index
    LUA->SetField(-2, "__index");

    // Сохраняем метатаблицу
    metaTables["IQuery"] = LUA->ReferenceCreate();
    ++referenceCreatedCount;
}

// Запуск запроса
int LuaIQuery::start(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    obj->getQuery()->start();
    return 0;
}

// Ожидание выполнения запроса
int LuaIQuery::wait(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    bool success = obj->getQuery()->wait();
    LUA->PushBool(success);
    return 1;
}

// Получение ошибки запроса
int LuaIQuery::error(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    LUA->PushString(obj->getQuery()->error().c_str());
    return 1;
}

// Получение статуса запроса
int LuaIQuery::status(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    LUA->PushNumber(obj->getQuery()->status());
    return 1;
}

// Прерывание запроса
int LuaIQuery::abort(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    obj->getQuery()->abort();
    return 0;
}

// Проверка выполнения запроса
int LuaIQuery::isRunning(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    LUA->PushBool(obj->getQuery()->isRunning());
    return 1;
}

// Проверка успешности запроса
int LuaIQuery::isSuccess(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    LUA->PushBool(obj->getQuery()->isSuccess());
    return 1;
}

// Получение количества затронутых строк
int LuaIQuery::affectedRows(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    LUA->PushNumber(obj->getQuery()->affectedRows());
    return 1;
}

// Установка опций запроса
int LuaIQuery::setOption(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    if (LUA->IsType(2, Type::Number)) {
        obj->getQuery()->setOption(LUA->GetNumber(2));
    }

    // Возвращаем сам объект для цепочки вызовов
    LUA->Push(1);
    return 1;
}

// Установка таймаута запроса
int LuaIQuery::setTimeout(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    if (LUA->IsType(2, Type::Number)) {
        obj->getQuery()->setTimeout(LUA->GetNumber(2));
    }

    // Возвращаем сам объект для цепочки вызовов
    LUA->Push(1);
    return 1;
}

// Установка callback'а на успешное выполнение
int LuaIQuery::onSuccess(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    if (LUA->IsType(2, Type::Function)) {
        LUA->Push(2);
        obj->getQuery()->setSuccessCallback(LUA->ReferenceCreate());
        ++referenceCreatedCount;
    }

    // Возвращаем сам объект для цепочки вызовов
    LUA->Push(1);
    return 1;
}

// Установка callback'а на ошибку
int LuaIQuery::onError(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    if (LUA->IsType(2, Type::Function)) {
        LUA->Push(2);
        obj->getQuery()->setFailCallback(LUA->ReferenceCreate());
        ++referenceCreatedCount;
    }

    // Возвращаем сам объект для цепочки вызовов
    LUA->Push(1);
    return 1;
}

// Установка callback'а на прерывание
int LuaIQuery::onAbort(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    if (LUA->IsType(2, Type::Function)) {
        LUA->Push(2);
        obj->getQuery()->setAbortCallback(LUA->ReferenceCreate());
        ++referenceCreatedCount;
    }

    // Возвращаем сам объект для цепочки вызовов
    LUA->Push(1);
    return 1;
}

// Установка callback'а на получение данных
int LuaIQuery::onData(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaIQuery* obj = (LuaIQuery*)LUA->GetUserdata(1);

    if (!obj || !obj->isValid()) {
        LUA->ThrowError("Trying to use an invalid query");
        return 0;
    }

    if (LUA->IsType(2, Type::Function)) {
        LUA->Push(2);
        obj->getQuery()->setDataCallback(LUA->ReferenceCreate());
        ++referenceCreatedCount;
    }

    // Возвращаем сам объект для цепочки вызовов
    LUA->Push(1);
    return 1;
}

// Метод lastInsertId хоть и является общим, но зависит от специфики запроса
int LuaIQuery::lastInsertId(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LUA->PushString("");
    return 1;
}
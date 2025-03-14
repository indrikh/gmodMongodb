#include "GarrysMod/Lua/Interface.h"
#include "../mongodb/Database.h"
#include <iostream>
#include <fstream>
#include "LuaObject.h"
#include "LuaDatabase.h"
#include "LuaCollection.h"
#include "LuaIQuery.h"
#include "LuaFindQuery.h"
#include "LuaInsertQuery.h"
#include "LuaUpdateQuery.h"
#include "LuaDeleteQuery.h"
#include "LuaAggregateQuery.h"
#include "LuaCountQuery.h"

#define GMODMONGODB_VERSION "1"
#define GMODMONGODB_MINOR_VERSION "0"

using namespace GarrysMod::Lua;

GMOD_MODULE_CLOSE() {
    return 0;
}

/* Возвращает количество текущих аллоцированных объектов.
 */
LUA_FUNCTION(objectCount) {
    LUA->PushNumber((double) (LuaObject::allocationCount - LuaObject::deallocationCount));
    return 1;
}

static void runInTimer(ILuaBase *LUA, double delay, CFunc func) {
    LUA->PushSpecial(SPECIAL_GLOB);
    LUA->GetField(-1, "timer");
    //На случай, если кто-то удалит библиотеку timer
    if (LUA->IsType(-1, Type::Nil)) {
        LUA->Pop(2);
        return;
    }
    LUA->GetField(-1, "Simple");
    LUA->PushNumber(delay);
    LUA->PushCFunction(func);
    LUA->Call(2, 0);
    LUA->Pop(2);
}

static void printMessage(ILuaBase *LUA, const char *str, int r, int g, int b) {
    LUA->PushSpecial(SPECIAL_GLOB);
    LUA->GetField(-1, "Color");
    LUA->PushNumber(r);
    LUA->PushNumber(g);
    LUA->PushNumber(b);
    LUA->Call(3, 1);
    int ref = LUA->ReferenceCreate();
    LUA->GetField(-1, "MsgC");
    LUA->ReferencePush(ref);
    LUA->PushString(str);
    LUA->Call(2, 0);
    LUA->Pop();
    LUA->ReferenceFree(ref);
}

LUA_FUNCTION(allocationCount) {
    LUA->PushNumber(LuaObject::allocationCount);
    return 1;
}

LUA_FUNCTION(deallocationCount) {
    LUA->PushNumber(LuaObject::deallocationCount);
    return 1;
}

LUA_FUNCTION(referenceCreatedCount) {
    LUA->PushNumber((double) LuaObject::referenceCreatedCount);
    return 1;
}

LUA_FUNCTION(referenceFreedCount) {
    LUA->PushNumber((double) LuaObject::referenceFreedCount);
    return 1;
}

LUA_FUNCTION(mongodbThink) {
    LUA->PushSpecial(SPECIAL_GLOB);
    LUA->GetField(-1, "mongodb");
    if (LUA->IsType(-1, Type::Nil)) {
        LUA->Pop(2); //nil, Global
        return 0;
    }
    LuaDatabase::runAllThinkHooks(LUA);
    LUA->Pop(2); //nil, Global
    return 0;
}

GMOD_MODULE_OPEN() {
    //Создание метатаблиц
    LuaObject::createUserDataMetaTable(LUA);
    LuaDatabase::createMetaTable(LUA);
    LuaCollection::createMetaTable(LUA);
    LuaIQuery::createMetaTable(LUA);
    LuaFindQuery::createMetaTable(LUA);
    LuaInsertQuery::createMetaTable(LUA);
    LuaUpdateQuery::createMetaTable(LUA);
    LuaDeleteQuery::createMetaTable(LUA);
    LuaAggregateQuery::createMetaTable(LUA);
    LuaCountQuery::createMetaTable(LUA);

    //Добавление хука для обработки событий
    LUA->PushSpecial(SPECIAL_GLOB);
    LUA->GetField(-1, "hook");
    LUA->GetField(-1, "Add");
    LUA->PushString("Think");
    LUA->PushString("__MongoDBThinkHook");
    LUA->PushCFunction(mongodbThink);
    LUA->Call(3, 0);
    LUA->Pop();
    LUA->Pop();

    //Создание глобальной таблицы модуля
    LUA->PushSpecial(SPECIAL_GLOB);
    LUA->CreateTable(); //mongodb

    //Установка версий
    LUA->PushString(GMODMONGODB_VERSION);
    LUA->SetField(-2, "VERSION");
    LUA->PushString(GMODMONGODB_MINOR_VERSION);
    LUA->SetField(-2, "MINOR_VERSION");

    //Определение констант для статусов базы данных
    LUA->PushNumber(MONGODB_CONNECTED);
    LUA->SetField(-2, "DATABASE_CONNECTED");
    LUA->PushNumber(MONGODB_CONNECTING);
    LUA->SetField(-2, "DATABASE_CONNECTING");
    LUA->PushNumber(MONGODB_NOT_CONNECTED);
    LUA->SetField(-2, "DATABASE_NOT_CONNECTED");
    LUA->PushNumber(MONGODB_CONNECTION_FAILED);
    LUA->SetField(-2, "DATABASE_CONNECTION_FAILED");

    //Определение констант для статусов запросов
    LUA->PushNumber(QUERY_NOT_RUNNING);
    LUA->SetField(-2, "QUERY_NOT_RUNNING");
    LUA->PushNumber(QUERY_RUNNING);
    LUA->SetField(-2, "QUERY_RUNNING");
    LUA->PushNumber(QUERY_COMPLETE);
    LUA->SetField(-2, "QUERY_COMPLETE");
    LUA->PushNumber(QUERY_ABORTED);
    LUA->SetField(-2, "QUERY_ABORTED");
    LUA->PushNumber(QUERY_WAITING);
    LUA->SetField(-2, "QUERY_WAITING");
    LUA->PushNumber(QUERY_ERROR);
    LUA->SetField(-2, "QUERY_ERROR");

    //Определение констант для опций запросов
    LUA->PushNumber(OPTION_DEFAULT);
    LUA->SetField(-2, "OPTION_DEFAULT");
    LUA->PushNumber(OPTION_NO_CURSOR_TIMEOUT);
    LUA->SetField(-2, "OPTION_NO_CURSOR_TIMEOUT");
    LUA->PushNumber(OPTION_AWAIT_DATA);
    LUA->SetField(-2, "OPTION_AWAIT_DATA");
    LUA->PushNumber(OPTION_BATCH_SIZE);
    LUA->SetField(-2, "OPTION_BATCH_SIZE");
    LUA->PushNumber(OPTION_TAILABLE_CURSOR);
    LUA->SetField(-2, "OPTION_TAILABLE_CURSOR");

    //Регистрация основной функции подключения
    LUA->PushCFunction(LuaDatabase::create);
    LUA->SetField(-2, "connect");

    //Отладочные функции
    LUA->PushCFunction(objectCount);
    LUA->SetField(-2, "objectCount");
    LUA->PushCFunction(allocationCount);
    LUA->SetField(-2, "allocationCount");
    LUA->PushCFunction(deallocationCount);
    LUA->SetField(-2, "deallocationCount");
    LUA->PushCFunction(referenceFreedCount);
    LUA->SetField(-2, "referenceFreedCount");
    LUA->PushCFunction(referenceCreatedCount);
    LUA->SetField(-2, "referenceCreatedCount");

    //Создание слабой таблицы для хранения баз данных
    LuaDatabase::createWeakTable(LUA);

    //Установка глобальной таблицы модуля
    LUA->SetField(-2, "mongodb");
    LUA->Pop();

    //Вывод информации при загрузке
    printMessage(LUA, "MongoDB module for Garry's Mod successfully loaded.\n", 0, 255, 0);
    printMessage(LUA, "Version: ", 0, 255, 0);
    printMessage(LUA, GMODMONGODB_VERSION, 255, 255, 255);
    printMessage(LUA, ".", 0, 255, 0);
    printMessage(LUA, GMODMONGODB_MINOR_VERSION, 255, 255, 255);
    printMessage(LUA, "\n", 0, 255, 0);

    return 1;
}
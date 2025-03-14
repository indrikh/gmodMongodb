#ifndef GMODMONGODB_LUAINSERTQUERY_H
#define GMODMONGODB_LUAINSERTQUERY_H

#include "../mongodb/InsertQuery.h"
#include "LuaIQuery.h"
#include <memory>

// Класс для запросов вставки в Lua
class LuaInsertQuery : public LuaIQuery {
public:
    // Создание метатаблицы для типа
    static void createMetaTable(ILuaBase *LUA);

    // Создание экземпляра класса
    static int create(lua_State *L);

    // Конструктор
    explicit LuaInsertQuery(std::shared_ptr<InsertQuery> query);

    // Получение указателя на запрос
    std::shared_ptr<InsertQuery> getInsertQuery() const {
        return std::static_pointer_cast<InsertQuery>(m_query);
    }

    // Lua-методы
    static int addDocument(lua_State *L);
    static int getLastInsertId(lua_State *L);
    static int getInsertedIds(lua_State *L);
};

#endif // GMODMONGODB_LUAINSERTQUERY_H
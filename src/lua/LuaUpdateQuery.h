#ifndef GMODMONGODB_LUAUPDATEQUERY_H
#define GMODMONGODB_LUAUPDATEQUERY_H

#include "../mongodb/UpdateQuery.h"
#include "LuaIQuery.h"
#include <memory>

// Класс для запросов обновления в Lua
class LuaUpdateQuery : public LuaIQuery {
public:
    // Создание метатаблицы для типа
    static void createMetaTable(ILuaBase *LUA);

    // Создание экземпляра класса
    static int create(lua_State *L);

    // Конструктор
    explicit LuaUpdateQuery(std::shared_ptr<UpdateQuery> query);

    // Получение указателя на запрос
    std::shared_ptr<UpdateQuery> getUpdateQuery() const {
        return std::static_pointer_cast<UpdateQuery>(m_query);
    }

    // Lua-методы
    static int setFilter(lua_State *L);
    static int setUpdate(lua_State *L);
    static int setMulti(lua_State *L);
    static int setUpsert(lua_State *L);
    static int getModifiedCount(lua_State *L);
    static int getMatchedCount(lua_State *L);
    static int getUpsertedCount(lua_State *L);
    static int getUpsertedId(lua_State *L);
};

#endif // GMODMONGODB_LUAUPDATEQUERY_H
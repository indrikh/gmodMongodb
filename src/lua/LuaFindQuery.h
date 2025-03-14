#ifndef GMODMONGODB_LUAFINDQUERY_H
#define GMODMONGODB_LUAFINDQUERY_H

#include "../mongodb/FindQuery.h"
#include "LuaIQuery.h"
#include <memory>

// Класс для запросов поиска в Lua
class LuaFindQuery : public LuaIQuery {
public:
    // Создание метатаблицы для типа
    static void createMetaTable(ILuaBase *LUA);

    // Создание экземпляра класса
    static int create(lua_State *L);

    // Конструктор
    explicit LuaFindQuery(std::shared_ptr<FindQuery> query);

    // Получение указателя на запрос
    std::shared_ptr<FindQuery> getFindQuery() const {
        return std::static_pointer_cast<FindQuery>(m_query);
    }

    // Lua-методы
    static int setFilter(lua_State *L);
    static int setSort(lua_State *L);
    static int setProjection(lua_State *L);
    static int setLimit(lua_State *L);
    static int setSkip(lua_State *L);
    static int getData(lua_State *L);
};

#endif // GMODMONGODB_LUAFINDQUERY_H
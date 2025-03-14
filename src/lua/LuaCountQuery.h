#ifndef GMODMONGODB_LUACOUNTQUERY_H
#define GMODMONGODB_LUACOUNTQUERY_H

#include "../mongodb/CountQuery.h"
#include "LuaIQuery.h"
#include <memory>

// Класс для запросов подсчета в Lua
class LuaCountQuery : public LuaIQuery {
public:
    // Создание метатаблицы для типа
    static void createMetaTable(ILuaBase *LUA);

    // Создание экземпляра класса
    static int create(lua_State *L);

    // Конструктор
    explicit LuaCountQuery(std::shared_ptr<CountQuery> query);

    // Получение указателя на запрос
    std::shared_ptr<CountQuery> getCountQuery() const {
        return std::static_pointer_cast<CountQuery>(m_query);
    }

    // Lua-методы
    static int setFilter(lua_State *L);
    static int getCount(lua_State *L);
};

#endif // GMODMONGODB_LUACOUNTQUERY_H
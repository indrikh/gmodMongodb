#ifndef GMODMONGODB_LUADELETEQUERY_H
#define GMODMONGODB_LUADELETEQUERY_H

#include "../mongodb/DeleteQuery.h"
#include "LuaIQuery.h"
#include <memory>

// Класс для запросов удаления в Lua
class LuaDeleteQuery : public LuaIQuery {
public:
    // Создание метатаблицы для типа
    static void createMetaTable(ILuaBase *LUA);

    // Создание экземпляра класса
    static int create(lua_State *L);

    // Конструктор
    explicit LuaDeleteQuery(std::shared_ptr<DeleteQuery> query);

    // Получение указателя на запрос
    std::shared_ptr<DeleteQuery> getDeleteQuery() const {
        return std::static_pointer_cast<DeleteQuery>(m_query);
    }

    // Lua-методы
    static int setFilter(lua_State *L);
    static int setMulti(lua_State *L);
    static int getDeletedCount(lua_State *L);
};

#endif // GMODMONGODB_LUADELETEQUERY_H
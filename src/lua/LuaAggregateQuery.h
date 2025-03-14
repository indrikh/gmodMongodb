#ifndef GMODMONGODB_LUAAGGREGATEQUERY_H
#define GMODMONGODB_LUAAGGREGATEQUERY_H

#include "../mongodb/AggregateQuery.h"
#include "LuaIQuery.h"
#include <memory>

// Класс для запросов агрегации в Lua
class LuaAggregateQuery : public LuaIQuery {
public:
    // Создание метатаблицы для типа
    static void createMetaTable(ILuaBase *LUA);

    // Создание экземпляра класса
    static int create(lua_State *L);

    // Конструктор
    explicit LuaAggregateQuery(std::shared_ptr<AggregateQuery> query);

    // Получение указателя на запрос
    std::shared_ptr<AggregateQuery> getAggregateQuery() const {
        return std::static_pointer_cast<AggregateQuery>(m_query);
    }

    // Lua-методы
    static int setPipeline(lua_State *L);
    static int addStage(lua_State *L);
    static int getData(lua_State *L);
};

#endif // GMODMONGODB_LUAAGGREGATEQUERY_H
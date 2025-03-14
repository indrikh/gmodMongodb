#ifndef GMODMONGODB_LUAIQUERY_H
#define GMODMONGODB_LUAIQUERY_H

#include "../mongodb/IQuery.h"
#include "LuaObject.h"
#include <memory>

// Базовый класс для всех типов запросов в Lua
class LuaIQuery : public LuaObject {
public:
    // Создание метатаблицы для типа
    static void createMetaTable(ILuaBase *LUA);

    // Обработка уничтожения объекта в Lua
    void onDestroyedByLua(ILuaBase *LUA) override;

    // Проверка валидности объекта
    bool isValid() const override;

    // Конструктор
    explicit LuaIQuery(std::shared_ptr<IQuery> query);

    // Получение указателя на запрос
    std::shared_ptr<IQuery> getQuery() const {
        return m_query;
    }

    // Lua-методы для всех типов запросов
    static int start(lua_State *L);
    static int wait(lua_State *L);
    static int error(lua_State *L);
    static int status(lua_State *L);
    static int abort(lua_State *L);
    static int isRunning(lua_State *L);
    static int isSuccess(lua_State *L);
    static int affectedRows(lua_State *L);
    static int lastInsertId(lua_State *L);
    static int setOption(lua_State *L);
    static int setTimeout(lua_State *L);
    static int onSuccess(lua_State *L);
    static int onError(lua_State *L);
    static int onAbort(lua_State *L);
    static int onData(lua_State *L);

protected:
    // Указатель на запрос
    std::shared_ptr<IQuery> m_query;
};

#endif // GMODMONGODB_LUAIQUERY_H
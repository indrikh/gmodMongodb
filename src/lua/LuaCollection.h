#ifndef GMODMONGODB_LUACOLLECTION_H
#define GMODMONGODB_LUACOLLECTION_H

#include "../mongodb/Collection.h"
#include "LuaObject.h"
#include <memory>

// Класс представления коллекции MongoDB в Lua
class LuaCollection : public LuaObject {
public:
    // Создание метатаблицы для типа
    static void createMetaTable(ILuaBase *LUA);

    // Создание экземпляра класса
    static int create(lua_State *L);

    // Обработка уничтожения объекта в Lua
    void onDestroyedByLua(ILuaBase *LUA) override;

    // Конструктор
    explicit LuaCollection(std::shared_ptr<Collection> collection) : LuaObject("Collection"),
                                                                    m_collection(std::move(collection)) {
    }

    // Получение внутреннего объекта коллекции
    std::shared_ptr<Collection> getCollection() const {
        return m_collection;
    }

    // Lua-методы
    static int find(lua_State *L);
    static int insert(lua_State *L);
    static int update(lua_State *L);
    static int remove(lua_State *L);
    static int count(lua_State *L);
    static int aggregate(lua_State *L);
    static int createIndex(lua_State *L);
    static int dropIndex(lua_State *L);
    static int getName(lua_State *L);

private:
    // Указатель на коллекцию
    std::shared_ptr<Collection> m_collection;
};

#endif // GMODMONGODB_LUACOLLECTION_H
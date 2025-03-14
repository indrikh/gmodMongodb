#ifndef GMODMONGODB_LUADATABASE_H
#define GMODMONGODB_LUADATABASE_H

#include "../mongodb/Database.h"
#include <utility>
#include "LuaObject.h"

// Класс для представления MongoDB базы данных в Lua
class LuaDatabase : public LuaObject {
public:
    // Создание метатаблицы для типа
    static void createMetaTable(ILuaBase *LUA);

    // Создание экземпляра базы данных из Lua
    static int create(lua_State *L);

    // Функция для обработки событий базы данных
    void think(ILuaBase *LUA);

    // Ссылка на таблицу с колбэками
    int m_tableReference = 0;

    // Флаг наличия колбэка на отключение
    bool m_hasOnDisconnected = false;

    // Указатель на базу данных
    std::shared_ptr<Database> m_database;

    // Флаг выполнения колбэка подключения
    bool m_dbCallbackRan = false;

    // Обработка уничтожения объекта в Lua
    void onDestroyedByLua(ILuaBase *LUA) override;

    // Конструктор
    explicit LuaDatabase(std::shared_ptr<Database> database) : LuaObject("Database"),
                                                               m_database(std::move(database)) {
    }

    // Создание слабой таблицы для хранения ссылок на базы данных
    static void createWeakTable(ILuaBase *LUA);

    // Выполнение всех think-хуков для всех баз данных
    static void runAllThinkHooks(ILuaBase *LUA);

    // Lua-методы
    static int getCollection(lua_State *L);
    static int escape(lua_State *L);
    static int setCharacterSet(lua_State *L);
    static int wait(lua_State *L);
    static int serverInfo(lua_State *L);
    static int hostInfo(lua_State *L);
    static int status(lua_State *L);
    static int setAutoReconnect(lua_State *L);
    static int connect(lua_State *L);
    static int disconnect(lua_State *L);
    static int setConnectTimeout(lua_State *L);
    static int query(lua_State *L);
    static int ping(lua_State *L);
    static int queueSize(lua_State *L);
    static int abortAllQueries(lua_State *L);
};

#endif // GMODMONGODB_LUADATABASE_H
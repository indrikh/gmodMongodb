#ifndef GMODMONGODB_LUAOBJECT_H
#define GMODMONGODB_LUAOBJECT_H

#include "GarrysMod/Lua/Interface.h"
#include <string>
#include <atomic>
#include <unordered_map>

using namespace GarrysMod::Lua;

// Базовый класс для всех объектов, доступных из Lua
class LuaObject {
public:
    // Конструктор
    explicit LuaObject(const char* name);

    // Виртуальный деструктор
    virtual ~LuaObject();

    // Вызывается, когда объект удаляется в Lua
    virtual void onDestroyedByLua(ILuaBase* LUA);

    // Вызывается для проверки валидности объекта
    virtual bool isValid() const;

    // Получение имени типа объекта
    const char* getName() const;

    // Создание метатаблицы для типа объекта
    static void createUserDataMetaTable(ILuaBase* LUA);

    // Счетчики для отладки
    static std::atomic<unsigned int> allocationCount;
    static std::atomic<unsigned int> deallocationCount;
    static std::atomic<unsigned int> referenceCreatedCount;
    static std::atomic<unsigned int> referenceFreedCount;

protected:
    // Имя типа объекта
    const char* m_name;

    // Карта метатаблиц для разных типов объектов
    static std::unordered_map<std::string, int> metaTables;

    // Функция для освобождения объекта из Lua
    static int gcLuaObject(lua_State* L);

    // Функция для получения имени типа объекта из Lua
    static int toStringLuaObject(lua_State* L);

    // Функция для проверки валидности объекта из Lua
    static int isValidLuaObject(lua_State* L);
};

#endif // GMODMONGODB_LUAOBJECT_H
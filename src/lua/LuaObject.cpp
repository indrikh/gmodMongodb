#include "LuaObject.h"

// Инициализация статических переменных
std::atomic<unsigned int> LuaObject::allocationCount{0};
std::atomic<unsigned int> LuaObject::deallocationCount{0};
std::atomic<unsigned int> LuaObject::referenceCreatedCount{0};
std::atomic<unsigned int> LuaObject::referenceFreedCount{0};
std::unordered_map<std::string, int> LuaObject::metaTables;

// Конструктор
LuaObject::LuaObject(const char* name) : m_name(name) {
    ++allocationCount;
}

// Деструктор
LuaObject::~LuaObject() {
    ++deallocationCount;
}

// Функция, вызываемая, когда объект уничтожается в Lua
void LuaObject::onDestroyedByLua(ILuaBase* LUA) {
    // Базовая реализация ничего не делает
}

// Проверка валидности объекта
bool LuaObject::isValid() const {
    return true;
}

// Получение имени типа объекта
const char* LuaObject::getName() const {
    return m_name;
}

// Garbage Collector Lua-объекта
int LuaObject::gcLuaObject(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaObject* obj = (LuaObject*)LUA->GetUserdata(1);
    
    if (obj) {
        obj->onDestroyedByLua(LUA);
        obj->~LuaObject();
    }
    
    return 0;
}

// __tostring для Lua-объекта
int LuaObject::toStringLuaObject(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaObject* obj = (LuaObject*)LUA->GetUserdata(1);
    
    if (obj) {
        const char* name = obj->getName();
        char buffer[256];
        snprintf(buffer, sizeof(buffer), "%s: %p", name, obj);
        LUA->PushString(buffer);
    } else {
        LUA->PushString("Invalid Object");
    }
    
    return 1;
}

// isvalid для Lua-объекта
int LuaObject::isValidLuaObject(lua_State* L) {
    ILuaBase* LUA = L->luabase;
    LuaObject* obj = (LuaObject*)LUA->GetUserdata(1);
    
    if (obj) {
        LUA->PushBool(obj->isValid());
    } else {
        LUA->PushBool(false);
    }
    
    return 1;
}

// Создание метатаблицы для Lua-userdata объектов
void LuaObject::createUserDataMetaTable(ILuaBase* LUA) {
    // Метатаблица для UserData (базовый класс)
    LUA->CreateTable();
    
    LUA->PushCFunction(gcLuaObject);
    LUA->SetField(-2, "__gc");
    
    LUA->PushCFunction(toStringLuaObject);
    LUA->SetField(-2, "__tostring");
    
    LUA->PushCFunction(isValidLuaObject);
    LUA->SetField(-2, "IsValid");
    
    LUA->PushString("Userdata");
    LUA->SetField(-2, "__type");
    
    // Создаем таблицу __index
    LUA->CreateTable();
    
    LUA->PushCFunction(isValidLuaObject);
    LUA->SetField(-2, "IsValid");
    
    // Устанавливаем таблицу __index
    LUA->SetField(-2, "__index");
    
    // Сохраняем метатаблицу для дальнейшего использования
    metaTables["UserData"] = LUA->ReferenceCreate();
    ++referenceCreatedCount;
}

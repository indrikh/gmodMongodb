#ifndef GARRYSMOD_LUA_INTERFACE_H
#define GARRYSMOD_LUA_INTERFACE_H

#include <stddef.h>

struct lua_State;

namespace GarrysMod {
    namespace Lua {
        typedef int (*CFunc)(lua_State* L);

        //
        // Access to the Lua interface
        //
        class ILuaBase {
        public:
            // Functions for interacting with Lua states (pushing, getting, etc.)
            virtual int Top() = 0;
            virtual void Push(int amount) = 0;
            virtual void Pop(int amount) = 0;
            virtual void GetTable(int index) = 0;
            virtual void GetField(int index, const char* key) = 0;
            virtual void SetField(int index, const char* key) = 0;
            virtual void CreateTable() = 0;
            virtual void SetTable(int index) = 0;
            virtual void SetMetaTable(int index) = 0;
            virtual bool GetMetaTable(int index) = 0;
            virtual void Call(int args, int results) = 0;
            virtual int PCall(int args, int results, int errorFunc) = 0;
            virtual int Equal(int index1, int index2) = 0;
            virtual int RawEqual(int index1, int index2) = 0;
            virtual void Insert(int index) = 0;
            virtual void Remove(int index) = 0;
            virtual int Next(int index) = 0;
            virtual void* NewUserdata(unsigned int size) = 0;
            virtual void ThrowError(const char* message) = 0;
            virtual void CheckType(int index, int type) = 0;
            virtual void ArgError(int index, const char* message) = 0;
            virtual void RawGet(int index) = 0;
            virtual void RawSet(int index) = 0;

            // Push functions
            virtual const char* GetString(int index = -1, unsigned int* size = nullptr) = 0;
            virtual double GetNumber(int index = -1) = 0;
            virtual bool GetBool(int index = -1) = 0;
            virtual void* GetUserData(int index = -1) = 0;

            // Pushes
            virtual void PushNil() = 0;
            virtual void PushString(const char* val, unsigned int len = 0) = 0;
            virtual void PushNumber(double val) = 0;
            virtual void PushBool(bool val) = 0;
            virtual void PushCFunction(CFunc val) = 0;
            virtual void PushUserdata(void*) = 0;

            // References
            virtual int ReferenceCreate() = 0;
            virtual void ReferenceFree(int reference) = 0;
            virtual void ReferencePush(int reference) = 0;

            // Type checks
            virtual int IsType(int index, int type) = 0;
            virtual int GetType(int index) = 0;
            virtual const char* GetTypeName(int type) = 0;

            // Useful functions
            virtual void CreateMetaTableType(const char* name, int type) = 0;

            // State controlling functions
            virtual void ObjLen(int index) = 0;
            virtual void GetUserType(int index, int* userType) = 0;
            virtual bool PushMetaTable(int index) = 0;
            virtual void PushSpecial(int type) = 0;
            virtual bool SetUserType(int index, int type) = 0;

            // Lua state management
            virtual void SetState(lua_State* state) = 0;
            virtual lua_State* GetState() = 0;
        };

        // Enumerations for Lua types
        enum {
            Type_Invalid = -1,
            Type_Nil,
            Type_Bool,
            Type_LightUserData,
            Type_Number,
            Type_String,
            Type_Table,
            Type_Function,
            Type_UserData,
            Type_Thread,

            Type_Entity = 100,
            Type_Vector,
            Type_Angle,
            Type_PhysObj,
            Type_Save,
            Type_Restore,
            Type_DamageInfo,
            Type_EffectData,
            Type_MoveData,
            Type_RecipientFilter,
            Type_UserCmd,
            Type_ScriptedVehicle,
            Type_Material,
            Type_Panel,
            Type_Particle,
            Type_ParticleEmitter,
            Type_Texture,
            Type_UserMsg,
            Type_ConVar,
            Type_IMesh,
            Type_Matrix,
            Type_Sound,
            Type_PixelVisHandle,
            Type_DLight,
            Type_Video,
            Type_File,
            Type_Player = 200,
            Type_Entity_NPC,
            Type_Weapon,
            Type_Vehicle,
            Type_NPC
        };

        // Special Lua push types
        enum {
            SPECIAL_GLOB,      // Global table
            SPECIAL_ENV,       // Environment table
            SPECIAL_REG        // Registry table
        };
    }
}

// Helper macros
#define LUA_FUNCTION(name) int name(lua_State* state)

// Create a userdata type reference to be used with ILuaBase functions.
inline int LuaReferenceCreate(GarrysMod::Lua::ILuaBase* lua) {
    lua->PushNil();
    return lua->ReferenceCreate();
}

// Free a reference created with LuaReferenceCreate.
inline void LuaReferenceFree(GarrysMod::Lua::ILuaBase* lua, int ref) {
    lua->ReferenceFree(ref);
}

#endif // GARRYSMOD_LUA_INTERFACE_H
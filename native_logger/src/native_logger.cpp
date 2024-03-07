#define EXTENSION_NAME NativeLogger
#define LIB_NAME "NativeLogger"
#define MODULE_NAME "native_logger"
#define DLIB_LOG_DOMAIN LIB_NAME

#include <dmsdk/sdk.h>
#include "native_logger.h"

static int log(lua_State* L) {
    int log_level = luaL_checkinteger(L, 1);
    const char* message = luaL_checkstring(L, 2);

#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID) || defined(__EMSCRIPTEN__)
    NativeLogger_LogInternal(log_level, message);
#endif
    return 0;
}

static const luaL_reg Module_methods[] =
{
    {"log", log},
    {0, 0}
};

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);
    luaL_register(L, MODULE_NAME, Module_methods);
    lua_pop(L, 1);
    assert(top == lua_gettop(L));
}

dmExtension::Result InitializeNativeLogger(dmExtension::Params* params)
{
    LuaInit(params->m_L);
    return dmExtension::RESULT_OK;
}


DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, 0, 0, InitializeNativeLogger, 0, 0, 0)

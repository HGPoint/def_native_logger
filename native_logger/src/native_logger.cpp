#define EXTENSION_NAME NativeLogger
#define LIB_NAME "NativeLogger"
#define MODULE_NAME "native_logger"
#define DLIB_LOG_DOMAIN LIB_NAME

#include <dmsdk/sdk.h>
#include <dmsdk/dlib/log.h>
#include "native_logger.h"

static int log(lua_State* L) {
    int log_level = luaL_checkinteger(L, 1);
    const char* message = luaL_checkstring(L, 2);
    
#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID) || defined(__EMSCRIPTEN__)
    NativeLogger_LogInternal(log_level, message);
#endif
    return 0;
}

static int set_engine_log_level(lua_State* L) {
    int log_level = luaL_checkinteger(L, 1);
    
    switch (log_level)
    {
        case 0:  dmLogSetLevel(LOG_SEVERITY_DEBUG);      break;
        case 1:  dmLogSetLevel(LOG_SEVERITY_USER_DEBUG); break;
        case 2:  dmLogSetLevel(LOG_SEVERITY_INFO); 		 break;
        case 3:  dmLogSetLevel(LOG_SEVERITY_WARNING); 	 break;
        case 4:  dmLogSetLevel(LOG_SEVERITY_ERROR); 	 break;
        case 5:  dmLogSetLevel(LOG_SEVERITY_FATAL); 	 break;
        default:
            assert(0);
            break;
    }
    
    return 0;
}

static const luaL_reg Module_methods[] =
{
    {"log", log},
    {"set_engine_log_level", set_engine_log_level},
    {0, 0}
};

void InternalLogListener(LogSeverity severity, const char* domain, const char* formatted_string)
{
#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID) || defined(__EMSCRIPTEN__)
    int log_level = 0;
    switch (severity)
    {
        case LOG_SEVERITY_DEBUG:      log_level = 1; break;
        case LOG_SEVERITY_USER_DEBUG: log_level = 1; break;
        case LOG_SEVERITY_INFO:       log_level = 2; break;
        case LOG_SEVERITY_WARNING:    log_level = 3; break;
        case LOG_SEVERITY_ERROR:      log_level = 4; break;
        case LOG_SEVERITY_FATAL:      log_level = 5; break;
        default:
            assert(0);
            break;
    }
    NativeLogger_LogInternal(log_level, formatted_string);
#endif
}

static void LuaInit(lua_State* L)
{
    int top = lua_gettop(L);
    luaL_register(L, MODULE_NAME, Module_methods);
    lua_pop(L, 1);
    assert(top == lua_gettop(L));
    dmLogRegisterListener(InternalLogListener);
}

dmExtension::Result InitializeNativeLogger(dmExtension::Params* params)
{
    LuaInit(params->m_L);
    return dmExtension::RESULT_OK;
}


DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, 0, 0, InitializeNativeLogger, 0, 0, 0)

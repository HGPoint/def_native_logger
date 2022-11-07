#define EXTENSION_NAME NativeLogger
#define LIB_NAME "NativeLogger"
#define MODULE_NAME "native_logger"
#define DLIB_LOG_DOMAIN LIB_NAME

#include <dmsdk/sdk.h>
#include <dmsdk/dlib/log.h>
#include "native_logger.h"
#include "native_logger_utils.h"
#include <stdlib.h> // free


struct NativeLoggerModule
{
    NativeLoggerModule()
    {
        memset(this, 0, sizeof(*this));
    }

    bool IsInitialized() { return m_Initialized; }

    void Initialize()
    {
        dmNativeLogger::QueueCreate(&m_CommandQueue);
        m_Initialized = true;
    }

    void Finalize()
    {
        dmNativeLogger::QueueDestroy(&m_CommandQueue);
        m_Initialized = false;
    }

    bool m_Initialized;
    dmScript::LuaCallbackInfo *m_Listener;
    dmNativeLogger::CommandQueue m_CommandQueue;
};

static NativeLoggerModule g_NativeLoggerModule;

static int log(lua_State *L)
{
    int log_level = luaL_checkinteger(L, 1);
    const char *message = luaL_checkstring(L, 2);

#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID) || defined(__EMSCRIPTEN__)
    NativeLogger_LogInternal(log_level, message);
#endif
    return 0;
}

static int set_engine_log_level(lua_State *L)
{
    int log_level = luaL_checkinteger(L, 1);

    switch (log_level)
    {
    case 0:
        dmLogSetLevel(LOG_SEVERITY_DEBUG);
        break;
    case 1:
        dmLogSetLevel(LOG_SEVERITY_USER_DEBUG);
        break;
    case 2:
        dmLogSetLevel(LOG_SEVERITY_INFO);
        break;
    case 3:
        dmLogSetLevel(LOG_SEVERITY_WARNING);
        break;
    case 4:
        dmLogSetLevel(LOG_SEVERITY_ERROR);
        break;
    case 5:
        dmLogSetLevel(LOG_SEVERITY_FATAL);
        break;
    default:
        assert(0);
        break;
    }

    return 0;
}

static int set_native_log_listener(lua_State *L)
{
    DM_LUA_STACK_CHECK(L, 0);

    if (g_NativeLoggerModule.m_Listener)
        dmScript::DestroyCallback(g_NativeLoggerModule.m_Listener);

    g_NativeLoggerModule.m_Listener = dmScript::CreateCallback(L, 1);
    return 0;
}

static const luaL_reg Module_methods[] =
    {
        {"log", log},
        {"set_engine_log_level", set_engine_log_level},
        {"set_listener", set_native_log_listener},
        {0, 0}
    };



static void addMsgToQ(int log_level,const char *domain,const char *formatted_string){
    dmNativeLogger::Command cmd;
    cmd.m_Severity = log_level;
    cmd.m_Domain =strdup(domain);
    cmd.m_Message = strdup(formatted_string);
    dmNativeLogger::QueuePush(&g_NativeLoggerModule.m_CommandQueue, &cmd);
}
void InternalLogListener(LogSeverity severity, const char *domain, const char *formatted_string)
{
#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID) || defined(__EMSCRIPTEN__)
    int log_level = 0;
    switch (severity)
    {
    case LOG_SEVERITY_DEBUG:
        log_level = 1;
        break;
    case LOG_SEVERITY_USER_DEBUG:
        log_level = 1;
        break;
    case LOG_SEVERITY_INFO:
        log_level = 2;
        break;
    case LOG_SEVERITY_WARNING:
        log_level = 3;
        break;
    case LOG_SEVERITY_ERROR:
        log_level = 4;
        break;
    case LOG_SEVERITY_FATAL:
        log_level = 5;
        break;
    default:
        assert(0);
        break;
    }
    NativeLogger_LogInternal(log_level, formatted_string);
    addMsgToQ(log_level, domain, formatted_string);
#endif
}

static void LuaInit(lua_State *L)
{
    int top = lua_gettop(L);
    luaL_register(L, MODULE_NAME, Module_methods);
    lua_pop(L, 1);
    assert(top == lua_gettop(L));
    dmLogRegisterListener(InternalLogListener);
}

dmExtension::Result InitializeNativeLogger(dmExtension::Params *params)
{
    LuaInit(params->m_L);
    return dmExtension::RESULT_OK;
}

static void SendToLua(const dmNativeLogger::Command *cmd)
{
    if (!dmScript::IsCallbackValid(g_NativeLoggerModule.m_Listener))
    {
        return;
    }

    lua_State *L = dmScript::GetCallbackLuaContext(g_NativeLoggerModule.m_Listener);
    DM_LUA_STACK_CHECK(L, 0);

    if (!dmScript::SetupCallback(g_NativeLoggerModule.m_Listener))
    {
        return;
    }

    lua_pushnumber(L, cmd->m_Severity);
    lua_pushstring(L, cmd->m_Domain);
    lua_pushstring(L, cmd->m_Message);
    int ret = dmScript::PCall(L, 4, 0);
    (void)ret;

    dmScript::TeardownCallback(g_NativeLoggerModule.m_Listener);
}

void HandleCommand(dmNativeLogger::Command *cmd)
{
    SendToLua(cmd);

    if (cmd->m_Domain)
        free((void *)cmd->m_Domain);
    if (cmd->m_Message)
        free((void *)cmd->m_Message);
}


static dmExtension::Result AppInitializeNativeLogger(dmExtension::AppParams *params)
{
    if (!g_NativeLoggerModule.IsInitialized())
    {
        g_NativeLoggerModule.Initialize();
    }
    return dmExtension::RESULT_OK;
}

static dmExtension::Result AppFinalizeNativeLogger(dmExtension::AppParams *params)
{
    if (g_NativeLoggerModule.IsInitialized())
    {
        g_NativeLoggerModule.Finalize();
    }
    return dmExtension::RESULT_OK;
}

static dmExtension::Result UpdateNativeLogger(dmExtension::Params *params)
{
    if (g_NativeLoggerModule.m_Listener)
    {
        dmNativeLogger::QueueFlush(&g_NativeLoggerModule.m_CommandQueue, HandleCommand);
    }
    return dmExtension::RESULT_OK;
}

static dmExtension::Result FinalizeNativeLogger(dmExtension::Params *params)
{
    if (g_NativeLoggerModule.m_Listener)
    {
        dmScript::DestroyCallback(g_NativeLoggerModule.m_Listener);
        g_NativeLoggerModule.m_Listener = 0;
    }
    return dmExtension::RESULT_OK;
}

DM_DECLARE_EXTENSION(EXTENSION_NAME, LIB_NAME, AppInitializeNativeLogger, AppFinalizeNativeLogger, InitializeNativeLogger, UpdateNativeLogger, 0, 0)

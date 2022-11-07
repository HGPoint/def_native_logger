#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID) || defined(__EMSCRIPTEN__)
#include <dmsdk/sdk.h>
#include <stdlib.h> // free

#include "native_logger_utils.h"

static void PushError(lua_State* L, const char* error)
{
    // Could be extended with error codes etc
    if (error != 0) {
        lua_newtable(L);
        lua_pushstring(L, "error");
        lua_pushstring(L, error);
        lua_rawset(L, -3);
    } else {
        lua_pushnil(L);
    }
}

void dmNativeLogger::QueueCreate(CommandQueue* queue)
{
    queue->m_Mutex = dmMutex::New();
}

void dmNativeLogger::QueueDestroy(CommandQueue* queue)
{
    {
        DM_MUTEX_SCOPED_LOCK(queue->m_Mutex);
        queue->m_Commands.SetSize(0);
    }
    dmMutex::Delete(queue->m_Mutex);
}

void dmNativeLogger::QueuePush(CommandQueue* queue, Command* cmd)
{
    DM_MUTEX_SCOPED_LOCK(queue->m_Mutex);

    if(queue->m_Commands.Full())
    {
        queue->m_Commands.OffsetCapacity(2);
    }
    queue->m_Commands.Push(*cmd);
}

void dmNativeLogger::QueueFlush(CommandQueue* queue, CommandFn fn)
{
    assert(fn != 0);
    if (queue->m_Commands.Empty())
    {
        return;
    }

    dmArray<Command> tmp;
    {
        DM_MUTEX_SCOPED_LOCK(queue->m_Mutex);
        tmp.Swap(queue->m_Commands);
    }

    for(uint32_t i = 0; i != tmp.Size(); ++i)
    {
        fn(&tmp[i]);
    }
}

#endif // DM_PLATFORM_ANDROID || DM_PLATFORM_IOS

#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID) || defined(__EMSCRIPTEN__)
#ifndef DM_PUSH_UTILS
#define DM_PUSH_UTILS

#include <dmsdk/sdk.h>

namespace dmNativeLogger
{
	struct Command
	{
	    Command()
	    {
	        memset(this, 0, sizeof(Command));
	    }
	    int32_t  	m_Severity;
	    const char* m_Domain;
	    const char* m_Message;
	};

	struct CommandQueue
	{
	    dmArray<Command> m_Commands;
	    dmMutex::HMutex      m_Mutex;
	};

	typedef void (*CommandFn)(Command* cmd);

	void QueueCreate(CommandQueue* queue);
	void QueueDestroy(CommandQueue* queue);
	void QueuePush(CommandQueue* queue, Command* cmd);
	void QueueFlush(CommandQueue* queue, CommandFn fn);
}

#endif
#endif // DM_PLATFORM_ANDROID || DM_PLATFORM_IOS || __EMSCRIPTEN__

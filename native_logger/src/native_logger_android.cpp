#if defined(DM_PLATFORM_ANDROID)

#include <android/log.h>

static android_LogPriority ToAndroidPriority(int severity)
{
    switch (severity)
    {
        case 0:
            return ANDROID_LOG_DEBUG;
            
        case 1:
            return ANDROID_LOG_DEBUG;

        case 2:
            return ANDROID_LOG_INFO;

        case 3:
            return ANDROID_LOG_WARN;

        case 4:
            return ANDROID_LOG_ERROR;

        case 5:
            return ANDROID_LOG_FATAL;

        default:
            return ANDROID_LOG_ERROR;
    }
}

void NativeLogger_LogInternal(int severity, const char* message)
{
    __android_log_print(ToAndroidPriority(severity), "defold", "%s", message);
}
#endif
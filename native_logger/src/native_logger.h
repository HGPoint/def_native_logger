#pragma once
#if defined(DM_PLATFORM_IOS) || defined(DM_PLATFORM_ANDROID) || defined(__EMSCRIPTEN__)
    void NativeLogger_LogInternal(int severity, const char* message);
#endif
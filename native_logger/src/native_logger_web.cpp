#if defined(__EMSCRIPTEN__)

#include <emscripten/emscripten.h>

void NativeLogger_LogInternal(int severity, const char* message)
{
    EM_ASM({
        try {
            if ($1 != 4 && $1 != 5)
                console.log(UTF8ToString($0));
            else
                console.error(UTF8ToString($0));
        } catch (err){
            console.log(err)
        }
    }, message, severity);
}
#endif
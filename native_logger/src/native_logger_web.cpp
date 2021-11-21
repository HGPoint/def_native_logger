#if defined(__EMSCRIPTEN__)

#include <emscripten/emscripten.h>

void NativeLogger_LogInternal(int severity, const char* message)
{
    EM_ASM({
        try {
            if ($1 == 1)
                console.debug(UTF8ToString($0));
            else if($1 == 3)
                console.warn(UTF8ToString($0));
            else if($1 == 4 || $1 == 5)
                console.error(UTF8ToString($0));
            else
                console.log(UTF8ToString($0));
        } catch (err){
            console.log(err)
        }
    }, message, severity);
}
#endif
# def_native_logger
Defold log enable for release builds. Works on Android, iOS, HTML5(Emscripten)
# Using Lua: 
native_logger.log(int log_level, string message) - prints message to platform console
- "DEBUG" = 0
- "USER_DEBUG" = 1
- "INFO" = 2
- "WARN" = 3
- "ERROR" = 4
- "FATAL" = 5

native_logger.set_engine_log_level(int log_level) - set engine severity level
(
    https://github.com/defold/defold/blob/ddd4f963f77f1e3c9552132ac915042e5a491923/engine/dlib/src/dlib/log.cpp#L531
    https://github.com/defold/defold/blob/ddd4f963f77f1e3c9552132ac915042e5a491923/engine/dlib/src/dmsdk/dlib/log.h#L133
)

- "DEBUG" = 0
- "USER_DEBUG" = 1
- "INFO" = 2
- "WARN" = 3
- "ERROR" = 4
- "FATAL" = 5
# Example Lua:
 native_logger.log(1, "Test debug message")

# PS
    I think there is no difference between DEBUG and USER_DEBUG severity in this case, but i'll leave it here for full compatibility

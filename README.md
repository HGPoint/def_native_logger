# def_native_logger
Defold log enable for release builds. Works on Android, iOS, HTML5(Emscripten)
# Using Lua: 
native_logger.log(int log_level, string message)
- "DEBUG" = 1
- "INFO" = 2
- "WARN" = 3
- "ERROR" = 4
- "FATAL" = 5
# Example Lua:
 native_logger.log(1, "Test debug message")

import ctypes
logger_dll = ctypes.CDLL('D:/C++_Project//Logger/build/windows/x64/release/Logger.dll')
logger_dll.logger_enable_console_output.restype = None
logger_dll.logger_enable_file_output.restype = None
logger_dll.logger_enable_file_output.argtypes = [ctypes.c_char_p, ctypes.c_int]
logger_dll.logger_info.restype = None
logger_dll.logger_info.argtypes = [ctypes.c_char_p]
logger_dll.logger_set_level.restype = None
logger_dll.logger_set_level.argtypes = [ctypes.c_int]

# 调用 DLL 函数
logger_dll.logger_enable_console_output()
logger_dll.logger_enable_file_output(b"logs/async_log.txt", 1)  # 1 是追加模式
logger_dll.logger_set_level(2)  # 2 代表 spdlog::level::info
logger_dll.logger_info(b"This is an info message from Python!")
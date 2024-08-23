#include "../include/Logger.h"
int main() {
    Logger& logger = Logger::getInstance();

    // 启用控制台输出
    logger.enableConsoleOutput();

    // 启用文件输出，使用追加模式
    logger.enableFileOutput("logs/async_log.txt", Logger::FileMode::Append);

    logger.setLevel(spdlog::level::info);

    logger.info("This is an info message with async logging.");
    logger.warn("This is a warning message.");
    logger.error("This is an error message.");

    return 0;
}



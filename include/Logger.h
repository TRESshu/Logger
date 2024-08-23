#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/async.h>
#include <spdlog/async_logger.h>
#include <spdlog/details/thread_pool.h>
#include <memory>
#include <vector>
#include <mutex>
#include <string>
#include <filesystem>  // 确保包含此头文件
namespace fs = std::filesystem;
#ifdef _WIN32  // Windows 平台
    #ifdef LOGGER_DLL_EXPORTS
        #define LOGGER_API __declspec(dllexport)
    #else
        #define LOGGER_API __declspec(dllimport)
    #endif
#else  //Linux
    #define LOGGER_API __attribute__((visibility("default")))
#endif
class Logger
{
public:
    enum class FileMode {
        Overwrite,   // 覆盖模式
        Append,      // 追加模式
        Rolling,     // 按大小滚动
        Daily        // 按日期滚动
    };

    static Logger& getInstance()
    {
        static Logger instance;
        return instance;
    }

    void setLevel(spdlog::level::level_enum level)
    {
        logger_->set_level(level);
    }

    template<typename... Args>
    void info(const char* fmt, const Args&... args)
    {
        logger_->info(fmt, args...);
    }

    template<typename... Args>
    void warn(const char* fmt, const Args&... args)
    {
        logger_->warn(fmt, args...);
    }

    template<typename... Args>
    void error(const char* fmt, const Args&... args)
    {
        logger_->error(fmt, args...);
    }
    //启用控制台
    void enableConsoleOutput()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!console_sink_)
        {
            console_sink_ = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
            sinks_.push_back(console_sink_);
            updateLogger();
        }
    }
    //禁用控制台
    void disableConsoleOutput()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (console_sink_)
        {
            sinks_.erase(std::remove(sinks_.begin(), sinks_.end(), console_sink_), sinks_.end());
            console_sink_.reset();
            updateLogger();
        }
    }
    //启用本地文件
    void enableFileOutput(const std::string& filePath, FileMode mode = FileMode::Overwrite, size_t max_size = 1048576, size_t max_files = 3, int hour = 0, int minute = 0)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!file_sink_)
        {
            switch (mode)
            {
            case FileMode::Overwrite:
                file_sink_ = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filePath, true);
                break;
            case FileMode::Append:
                file_sink_ = std::make_shared<spdlog::sinks::basic_file_sink_mt>(filePath, false);
                break;
            case FileMode::Rolling:
                file_sink_ = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(filePath, max_size, max_files);
                break;
            case FileMode::Daily:
                file_sink_ = std::make_shared<spdlog::sinks::daily_file_sink_mt>(filePath, hour, minute);
                break;
            }
            sinks_.push_back(file_sink_);
            updateLogger();
        }
    }
    //禁用本地文件
    void disableFileOutput()
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (file_sink_)
        {
            sinks_.erase(std::remove(sinks_.begin(), sinks_.end(), file_sink_), sinks_.end());
            file_sink_.reset();
            updateLogger();
        }
    }

private:
    std::shared_ptr<spdlog::logger> logger_;
    std::vector<spdlog::sink_ptr> sinks_;
    std::shared_ptr<spdlog::sinks::stdout_color_sink_mt> console_sink_;
    std::shared_ptr<spdlog::sinks::sink> file_sink_;  // 改为通用的 sink 类型
    std::shared_ptr<spdlog::details::thread_pool> thread_pool_;
    std::mutex mutex_;

    Logger()
    {
        // 初始化线程池
        thread_pool_ = std::make_shared<spdlog::details::thread_pool>(8192, 1);

        // 初始创建 logger，但不添加任何 sink，用户需要手动启用输出
        updateLogger();
        logger_->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v");
        logger_->set_level(spdlog::level::info);
    }

    void updateLogger()
    {
        if (!logger_)
        {
            logger_ = std::make_shared<spdlog::async_logger>("logger", sinks_.begin(), sinks_.end(), thread_pool_, spdlog::async_overflow_policy::block);
        }
        else
        {
            logger_->sinks() = sinks_;
        }
    }

    Logger(const Logger&) = delete;
    Logger& operator=(const Logger&) = delete;
};
//导出链接函数
#ifdef BUILDING_DLL
extern "C" {

LOGGER_API void logger_enable_console_output() {
    Logger::getInstance().enableConsoleOutput();
}

LOGGER_API void logger_enable_file_output(const char* file_path, int mode) {
    Logger::getInstance().enableFileOutput(file_path, static_cast<Logger::FileMode>(mode));
}

LOGGER_API void logger_info(const char* message) {
    Logger::getInstance().info(message);
}

LOGGER_API void logger_set_level(int level) {
    Logger::getInstance().setLevel(static_cast<spdlog::level::level_enum>(level));
}

}
#endif
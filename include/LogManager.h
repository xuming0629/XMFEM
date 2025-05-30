#ifndef LOGMANAGER_H
#define LOGMANAGER_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>
#include <chrono>
#include <ctime>
#include <mutex>
#include <iomanip>

namespace fs = std::filesystem;

// 定义日志等级
enum class LogLevel
{
    DEBUG, // 调试信息
    INFO,  // 普通信息
    WARN,  // 警告信息
    ERROR  // 错误信息
};

// 日志管理器单例类
class LogManager
{
public:
    // 获取单例实例
    static LogManager &getInstance()
    {
        static LogManager instance;
        return instance;
    }

    // 禁止拷贝构造和赋值
    LogManager(const LogManager &) = delete;
    LogManager &operator=(const LogManager &) = delete;

    // 配置日志参数
    void configure(const std::string &logDir = "logs",                     // 日志目录
                   const std::string &filenamePattern = "logs_{time}.log", // 日志文件名格式，{time}会替换成日期
                   LogLevel level = LogLevel::INFO,                        // 日志等级
                   std::size_t maxSize = 10 * 1024 * 1024,                 // 最大文件大小，默认10MB
                   int backupCount = 6)
    { // 最大保留备份数
        std::lock_guard<std::mutex> lock(m_mutex);
        if (m_configured)
            return; // 防止重复配置
        m_configured = true;

        m_logDir = logDir;
        m_filenamePattern = filenamePattern;
        m_logLevel = level;
        m_maxSize = maxSize;
        m_backupCount = backupCount;

        // 如果日志目录不存在则创建
        if (!fs::exists(m_logDir))
        {
            fs::create_directories(m_logDir);
        }

        openLogFile(); // 打开日志文件
    }

    // 写入日志
    void log(LogLevel level, const std::string &message, const char *file, int line)
    {
        if (level < m_logLevel)
            return; // 如果日志等级不满足，直接返回

        std::lock_guard<std::mutex> lock(m_mutex);

        rotateIfNeeded(); // 检查是否需要日志轮转

        // 获取日志等级字符串
        std::string levelStr = getLevelString(level);
        // 获取当前时间字符串
        std::string timeStr = getCurrentTimeString();

        // 写入日志文件
        m_logFile << timeStr << " [" << levelStr << "] "
                  << fs::path(file).filename() << ":" << line
                  << " - " << message << std::endl;

        // 控制台彩色输出
        std::string colorCode = getColorCode(level);
        std::string resetCode = "\033[0m";

        std::cout << colorCode
                  << timeStr << " [" << levelStr << "] "
                  << fs::path(file).filename() << ":" << line
                  << " - " << message
                  << resetCode << std::endl;
    }

private:
    // 私有构造函数，保证单例模式
    LogManager() : m_configured(false) {}

    // 获取日志等级对应的终端颜色代码
    std::string getColorCode(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return "\033[36m"; // 青色
        case LogLevel::INFO:
            return "\033[32m"; // 绿色
        case LogLevel::WARN:
            return "\033[33m"; // 黄色
        case LogLevel::ERROR:
            return "\033[31m"; // 红色
        }
        return "\033[0m"; // 默认颜色
    }

    // 打开日志文件
    void openLogFile()
    {
        std::string timeStr = getCurrentTimeStringForFilename(); // 获取当前日期字符串
        std::string filename = m_filenamePattern;
        size_t pos = filename.find("{time}");
        if (pos != std::string::npos)
        {
            filename.replace(pos, 6, timeStr); // 替换文件名中的{time}
        }

        m_logFilePath = fs::path(m_logDir) / filename;
        m_logFile.open(m_logFilePath, std::ios::app); // 追加方式打开
    }

    // 判断文件大小，必要时执行日志轮转
    void rotateIfNeeded()
    {
        if (fs::exists(m_logFilePath) &&
            fs::file_size(m_logFilePath) >= m_maxSize)
        {

            m_logFile.close();

            // 依次重命名旧文件，从最大备份编号往回推
            for (int i = m_backupCount - 1; i >= 0; --i)
            {
                std::string oldName = m_logFilePath.string() + "." + std::to_string(i);
                std::string newName = m_logFilePath.string() + "." + std::to_string(i + 1);
                if (fs::exists(oldName))
                {
                    fs::rename(oldName, newName);
                }
            }

            // 当前日志文件改名为 .0
            fs::rename(m_logFilePath, m_logFilePath.string() + ".0");

            openLogFile(); // 新建日志文件
        }
    }

    // 获取日志等级对应的字符串
    std::string getLevelString(LogLevel level)
    {
        switch (level)
        {
        case LogLevel::DEBUG:
            return "DEBUG";
        case LogLevel::INFO:
            return "INFO";
        case LogLevel::WARN:
            return "WARN";
        case LogLevel::ERROR:
            return "ERROR";
        }
        return "UNKNOWN";
    }

    // 获取当前时间字符串，格式：2025-05-29 20:15:03.123
    std::string getCurrentTimeString()
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
                      now.time_since_epoch()) %
                  1000;

        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S")
            << "." << std::setfill('0') << std::setw(3) << ms.count();
        return oss.str();
    }

    // 获取当前日期字符串，供文件名使用，格式：2025-05-29
    std::string getCurrentTimeStringForFilename()
    {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::ostringstream oss;
        oss << std::put_time(std::localtime(&time), "%Y-%m-%d");
        return oss.str();
    }

    // 成员变量定义
    bool m_configured;             // 是否已配置
    std::string m_logDir;          // 日志目录
    std::string m_filenamePattern; // 日志文件名格式
    LogLevel m_logLevel;           // 日志等级
    std::size_t m_maxSize;         // 单个日志最大大小
    int m_backupCount;             // 备份日志个数
    std::ofstream m_logFile;       // 日志文件输出流
    fs::path m_logFilePath;        // 当前日志文件路径
    std::mutex m_mutex;            // 互斥锁，线程安全
};

// 定义日志宏，自动记录当前文件名和行号
#define LOG_DEBUG(msg) LogManager::getInstance().log(LogLevel::DEBUG, msg, __FILE__, __LINE__)
#define LOG_INFO(msg) LogManager::getInstance().log(LogLevel::INFO, msg, __FILE__, __LINE__)
#define LOG_WARN(msg) LogManager::getInstance().log(LogLevel::WARN, msg, __FILE__, __LINE__)
#define LOG_ERROR(msg) LogManager::getInstance().log(LogLevel::ERROR, msg, __FILE__, __LINE__)

#endif // LOGMANAGER_H

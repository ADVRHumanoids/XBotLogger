#include <XBotInterface/RtLog.hpp>
#include <XBotInterface/Thread.h>

#define RT_LOG_RESET   "\033[0m"
#define RT_LOG_BLACK   "\033[30m"      /* Black */
#define RT_LOG_RED     "\033[31m"      /* Red */
#define RT_LOG_GREEN   "\033[32m"      /* Green */
#define RT_LOG_YELLOW  "\033[33m"      /* Yellow */
#define RT_LOG_BLUE    "\033[34m"      /* Blue */
#define RT_LOG_MAGENTA "\033[35m"      /* Magenta */
#define RT_LOG_CYAN    "\033[36m"      /* Cyan */
#define RT_LOG_WHITE   "\033[37m"      /* White */

#ifndef DPRINTF

#ifdef __XENO__
    #include <rtdk.h>
    #define DPRINTF rt_printf
#else
    #include <stdio.h>
    #define DPRINTF printf
#endif

#endif


// #define MT_SAFE

#ifdef MT_SAFE
#define SET_LOCK_GUARD(x) std::lock_guard<Mutex> guard(x);
#else
#define SET_LOCK_GUARD(x)
#endif

#include <pthread.h>

namespace XBot {
    
    LoggerClass Logger::_logger("");
    
        

    std::ostream& Logger::error(Logger::Severity s)
    {
        return _logger.error(s);
    }

    void Logger::error(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        _logger.__error(Logger::Severity::HIGH, fmt, args);
        
        va_end(args);
    }

    void Logger::error(Logger::Severity s, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        _logger.__error(s, fmt, args);
        
        va_end(args);
    }
    
    void Logger::success(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        _logger.__success(Logger::Severity::LOW, fmt, args);
        
        va_end(args);
    }

    void Logger::success(Logger::Severity s, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        _logger.__success(s, fmt, args);
        
        va_end(args);
    }
    
    void Logger::warning(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        _logger.__warning(Logger::Severity::MID, fmt, args);
        
        va_end(args);
    }

    void Logger::warning(Logger::Severity s, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        _logger.__warning(s, fmt, args);
        
        va_end(args);
    }


    Endl& Logger::endl()
    {
        return _logger.endl();
    }

    std::ostream& Logger::info(Logger::Severity s)
    {
        return _logger.info(s);
    }
    
    
    
    void Logger::info(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        _logger.__info(Logger::Severity::LOW, fmt, args);
        
        va_end(args);
    }

    void Logger::info(Logger::Severity s, const char* fmt, ...)
    {
            va_list args;
            va_start(args, fmt);
            
            _logger.__info(s, fmt, args);
            
            va_end(args);
    }


    std::ostream& Logger::log()
    {
        return _logger.log();
    }


    void Logger::SetVerbosityLevel(Logger::Severity s)
    {
        _logger.setVerbosityLevel(s);
    }

    std::ostream& Logger::success(Logger::Severity s)
    {
        return _logger.success(s);
    }

    std::ostream& Logger::warning(Logger::Severity s)
    {
        return _logger.warning(s);
    }

    Logger::Severity Logger::GetVerbosityLevel()
    {
        return _logger.getVerbosityLevel();
    }

    
    std::ostream& bold_on(std::ostream& os)
    {
        return os << "\e[1m";
    }

    std::ostream& bold_off(std::ostream& os)
    {
        return os << "\e[0m";
    }
    
    std::ostream& color_green(std::ostream& os)
    {
        return os << RT_LOG_GREEN;
    }
    
    std::ostream& color_red(std::ostream& os)
    {
        return os << RT_LOG_RED;
    }
    
    std::ostream& color_yellow(std::ostream& os)
    {
        return os << RT_LOG_YELLOW;
    }
    
    std::ostream& color_reset(std::ostream& os)
    {
        return os << RT_LOG_RESET;
    }
    
    
    
    
    
    
    
    
    
    
    /* LoggerClass impl */
    
    LoggerClass::LoggerClass(std::string name):
        _endl(*this),
        _name(name),
        _mutex(new XBot::Mutex(XBot::Mutex::Type::RECURSIVE))
    {
        if(_name != ""){
            _name_tag = " (" + name + ")";
        }
        
        _sink.open(_buffer);
    }
    
    XBot::LoggerClass::~LoggerClass()
    {
        
        _sink.close();
        
        if((int)_verbosity_level <= (int)Logger::Severity::LOW){
            std::cout << __func__ << std::endl;
        }
    }

    
    void LoggerClass::init_sink()
    {
        memset(_buffer, 0, BUFFER_SIZE);
        _sink.seekp(0);
    }

    
    void operator<< ( std::ostream& os, Endl& endl )
    {
        endl.print();
    }
    
    void Endl::print()
    {
        _logger_handle.print();
    }
    
    
    std::ostream& LoggerClass::log()
    {
        SET_LOCK_GUARD(*_mutex)
        
        if(_sink.tellp() == 0){
            memset(_buffer, 0, BUFFER_SIZE);
        }
        
        return _sink;
    }
    
    std::ostream& LoggerClass::info(Logger::Severity s) 
    {
        SET_LOCK_GUARD(*_mutex)
        
        _severity = s;
        
        init_sink();
        _sink << bold_on << "[info" << _name_tag << "] " << bold_off;
        return _sink;
    };
    
    void XBot::LoggerClass::__fmt_print(const char* fmt, va_list args)
    {
        int pos = _sink.tellp();
        int nchars = vsnprintf(&_buffer[pos], (BUFFER_SIZE - pos), fmt, args);
        
        _sink.seekp(std::min(pos + nchars, BUFFER_SIZE - 1));
        
        print();
    }

    
    void LoggerClass::info(Logger::Severity s, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        __info(s, fmt, args);
        
        va_end(args);
    }
    
    void LoggerClass::info(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        __info(Logger::Severity::LOW, fmt, args);
        
        va_end(args);
        
    }
    
    void XBot::LoggerClass::__info(Logger::Severity s, const char* fmt, va_list args)
    {
        SET_LOCK_GUARD(*_mutex)
        
        info(s);
        
        __fmt_print(fmt, args);
    }

        
    std::ostream& LoggerClass::error(Logger::Severity s) 
    {
        SET_LOCK_GUARD(*_mutex)
        
        _severity = s;
        
        init_sink();
        _sink << bold_on << color_red << "[error" << _name_tag << "] " << bold_off << color_red;
        return _sink;
    };
    
    void LoggerClass::error(Logger::Severity s, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        __error(s, fmt, args);
        
        va_end(args);
    }
    
    void LoggerClass::error(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        __error(Logger::Severity::HIGH, fmt, args);
        
        va_end(args);
        
    }
    
    void XBot::LoggerClass::__error(Logger::Severity s, const char* fmt, va_list args)
    {
        SET_LOCK_GUARD(*_mutex)
        
        error(s);
        
        __fmt_print(fmt, args);
    }
    
    std::ostream& LoggerClass::warning(Logger::Severity s) 
    {
        SET_LOCK_GUARD(*_mutex)
        
        _severity = s;
        
        init_sink();
        _sink << bold_on << color_yellow << "[warning" << _name_tag << "] " << bold_off << color_yellow;
        return _sink;
    };
    
    void LoggerClass::warning(Logger::Severity s, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        __warning(s, fmt, args);
        
        va_end(args);
    }
    
    void LoggerClass::warning(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        __warning(Logger::Severity::MID, fmt, args);
        
        va_end(args);
        
    }
    
    void XBot::LoggerClass::__warning(Logger::Severity s, const char* fmt, va_list args)
    {
        warning(s);
        
        __fmt_print(fmt, args);
    }
    
    std::ostream& LoggerClass::success(Logger::Severity s) 
    {
        SET_LOCK_GUARD(*_mutex)
        
        _severity = s;
        
        init_sink();
        _sink << bold_on << color_green << "[success" << _name_tag << "] " << bold_off << color_green;
        return _sink;
    };
    
    void LoggerClass::success(Logger::Severity s, const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        __success(s, fmt, args);
        
        va_end(args);
    }
    
    void LoggerClass::success(const char* fmt, ...)
    {
        va_list args;
        va_start(args, fmt);
        
        __success(Logger::Severity::LOW, fmt, args);
        
        va_end(args);
        
    }
    
    
    void XBot::LoggerClass::__success(Logger::Severity s, const char* fmt, va_list args)
    {
        success(s);
        
        __fmt_print(fmt, args);
    }


    
    Endl& LoggerClass::endl() { return _endl; }
    

    void LoggerClass::setVerbosityLevel(Logger::Severity s)
    {
        _verbosity_level = s;
    }
    
    Logger::Severity LoggerClass::getVerbosityLevel() const
    {   
        
        return _verbosity_level;

    }
        
    Endl::Endl(LoggerClass& logger_handle):
        _logger_handle(logger_handle)
    {

    }
    
    
    void LoggerClass::print()
    {
        SET_LOCK_GUARD(*_mutex)
        
          _sink << color_reset;
        
        
        if( (int)_severity >= (int)_verbosity_level ){
            
            print_internal();

        }
        
        _severity = Logger::Severity::HIGH;
        
    }
    
    
    inline void LoggerClass::print_internal()
    {
        DPRINTF("%s\n", _buffer);
        
#if !defined __XENO__ && !defined __COBALT__ 
        fflush(stdout);
#endif
    }


    
}
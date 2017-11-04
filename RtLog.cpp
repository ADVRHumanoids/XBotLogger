#include <XBotInterface/RtLog.hpp>

namespace XBot {
    
    char Logger::_buffer[Logger::BUFFER_SIZE];
    Logger::IoStream Logger::_sink;
    Endl Logger::_endl;
    Logger::Severity Logger::_severity = Logger::Severity::HIGH;
    Logger::Severity Logger::_verbosity_level = Logger::Severity::LOW;
    bool Logger::_debug_only = false;
    
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
    
    void Logger::init_sink()
    {
        memset(_buffer, 0, BUFFER_SIZE);
        _sink.open(_buffer);
    }

    
    void operator<< ( std::ostream& os, Endl& endl )
    {
        endl.print();
    }
    
    void Endl::print()
    {
        Logger::print();
    }
    
    
    std::ostream& Logger::log(Logger::Severity s)
    {
        _severity = s;
        _debug_only = false;
        
        init_sink();
        return _sink;
    }
    
    std::ostream& Logger::info(Logger::Severity s) 
    {
        _severity = s;
        _debug_only = false;
        
        init_sink();
        _sink << bold_on << "[INFO] " << bold_off;
        return _sink;
    };
    
    std::ostream& Logger::error(Logger::Severity s) 
    {
        _severity = s;
        _debug_only = false;
        
        init_sink();
        _sink << bold_on << color_red << "[ERROR] " << bold_off << color_red;
        return _sink;
    };
    
    std::ostream& Logger::warning(Logger::Severity s) 
    {
        _severity = s;
        _debug_only = false;
        
        init_sink();
        _sink << bold_on << color_yellow << "[WARNING] " << bold_off << color_yellow;
        return _sink;
    };
    
    std::ostream& Logger::success(Logger::Severity s) 
    {
        _severity = s;
        _debug_only = false;
        
        init_sink();
        _sink << bold_on << color_green << "[OK] " << bold_off << color_green;
        return _sink;
    };
    
    std::ostream& Logger::debug(Logger::Severity s)
    {
        _severity = s;
        _debug_only = true;
        
        init_sink();
        _sink << "[DEBUG] ";
        return _sink;
    }

    
    Endl& Logger::endl() { return _endl; }
    

    void Logger::SetVerbosityLevel(Logger::Severity s)
    {
        _verbosity_level = s;
    }
    
}
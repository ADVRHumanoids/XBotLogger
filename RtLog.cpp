#include <XBotInterface/RtLog.hpp>

namespace XBot {
    
    LoggerClass Logger::_logger("");
    
        
    std::ostream& Logger::debug(Logger::Severity s)
    {
        return _logger.debug(s);
    }

    std::ostream& Logger::error(Logger::Severity s)
    {
        return _logger.error(s);
    }


    Endl& Logger::endl()
    {
        return _logger.endl();
    }

    std::ostream& Logger::info(Logger::Severity s)
    {
        return _logger.info(s);
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
        _name(name)
    {
        if(_name != ""){
            _name_tag = " (" + name + ")";
        }
        
    }
    
    void LoggerClass::init_sink()
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
        _logger_handle.print();
    }
    
    
    std::ostream& LoggerClass::log()
    {
        if(!_sink.is_open()){
            init_sink();
        }
        return _sink;
    }
    
    std::ostream& LoggerClass::info(Logger::Severity s) 
    {
        _severity = s;
        
        init_sink();
        _sink << bold_on << "[info" << _name_tag << "] " << bold_off;
        return _sink;
    };
    
    std::ostream& LoggerClass::error(Logger::Severity s) 
    {
        _severity = s;
        
        init_sink();
        _sink << bold_on << color_red << "[error" << _name_tag << "] " << bold_off << color_red;
        return _sink;
    };
    
    std::ostream& LoggerClass::warning(Logger::Severity s) 
    {
        _severity = s;
        
        init_sink();
        _sink << bold_on << color_yellow << "[warning" << _name_tag << "] " << bold_off << color_yellow;
        return _sink;
    };
    
    std::ostream& LoggerClass::success(Logger::Severity s) 
    {
        _severity = s;
        
        init_sink();
        _sink << bold_on << color_green << "[success" << _name_tag << "] " << bold_off << color_green;
        return _sink;
    };
    
    std::ostream& LoggerClass::debug(Logger::Severity s)
    {
        _severity = s;
        
        init_sink();
        _sink << "[debug" << _name_tag << "] ";
        return _sink;
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

        if( (int)_severity >= (int)_verbosity_level ){
            
            _sink << color_reset;
            
            print_internal();
            
        }
        
        _severity = Logger::Severity::HIGH;
        _sink.close();
        
    }


    
}
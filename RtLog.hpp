/*
 * Copyright (C) 2017 IIT-ADVR
 * Author: Arturo Laurenzi, Luca Muratore
 * email:  arturo.laurenzi@iit.it, luca.muratore@iit.it
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>
*/

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <iostream>

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

#include <pthread.h>


namespace XBot { namespace experimental {
    
    /* Modifiers */
    std::ostream& bold_on(std::ostream& os);

    std::ostream& bold_off(std::ostream& os);
    
    std::ostream& color_green(std::ostream& os);
    
    std::ostream& color_red(std::ostream& os);
    
    std::ostream& color_yellow(std::ostream& os);
    
    std::ostream& color_reset(std::ostream& os);
    
    /**
     * @brief Forward declaration for Logger
     * 
     */
    class Logger;
    
    
    
    /**
     * @brief Class handling the flushing of log messages to console.
     * 
     */
    class Endl {
        
    public:
        
        friend class Logger;
        
        friend void operator<< ( std::ostream& os, Endl& endl );
        
    private:
        
        Endl() = default;
        
        void print();
        
    };
    
    
    
    class Logger {
        
    public:
        
        enum class Severity { LOW = 0, MID = 1, HIGH = 2 };
        
        friend class Endl;
        
        static std::ostream& log(Severity s = Severity::LOW);
        
        static std::ostream& info(Severity s = Severity::LOW);
        
        static std::ostream& error(Severity s = Severity::HIGH);
        
        static std::ostream& warning(Severity s = Severity::MID);
        
        static std::ostream& success(Severity s = Severity::LOW);
        
        static Endl& endl();
        
        static void SetVerbosityLevel(Severity s);
        
    protected:
        
    private:
        
        typedef boost::iostreams::stream<boost::iostreams::array_sink> IoStream;
        
        Logger() = delete;
        
        static void print();
        
        static const int BUFFER_SIZE = 4096;
        
        static char _buffer[BUFFER_SIZE];
        
        static IoStream _sink;
        
        static Endl _endl;
        
        static Severity _severity;
        static Severity _verbosity_level;
        
    };
    
    char Logger::_buffer[Logger::BUFFER_SIZE];
    Logger::IoStream Logger::_sink;
    Endl Logger::_endl;
    Logger::Severity Logger::_severity = Logger::Severity::HIGH;
    Logger::Severity Logger::_verbosity_level = Logger::Severity::LOW;
    
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
    
    void operator<< ( std::ostream& os, Endl& endl )
    {
        endl.print();
    }
    
    void Endl::print()
    {
        Logger::print();
    }
    
    void Logger::print()
    {

        if( (int)_severity >= (int)_verbosity_level ){
            
            _sink << color_reset;
            DPRINTF("%s\n", _buffer);
            
        }
        
        _sink.close();
    }
    
    
    std::ostream& Logger::log(Logger::Severity s)
    {
        _severity = s;
        
        memset(_buffer, 0, BUFFER_SIZE);
        _sink.open(_buffer);
        return _sink;
    }
    
    std::ostream& Logger::info(Logger::Severity s) 
    {
        _severity = s;
        
        memset(_buffer, 0, BUFFER_SIZE);
        _sink.open(_buffer);
        _sink << bold_on << "[INFO] " << bold_off;
        return _sink;
    };
    
    std::ostream& Logger::error(Logger::Severity s) 
    {
        _severity = s;
        
        memset(_buffer, 0, BUFFER_SIZE);
        _sink.open(_buffer);
        _sink << bold_on << color_red << "[ERROR] " << bold_off << color_red;
        return _sink;
    };
    
    std::ostream& Logger::warning(Logger::Severity s) 
    {
        _severity = s;
        
        memset(_buffer, 0, BUFFER_SIZE);
        _sink.open(_buffer);
        _sink << bold_on << color_yellow << "[WARNING] " << bold_off << color_yellow;
        return _sink;
    };
    
    std::ostream& Logger::success(Logger::Severity s) 
    {
        _severity = s;
        
        memset(_buffer, 0, BUFFER_SIZE);
        _sink.open(_buffer);
        _sink << bold_on << color_green << "[OK] " << bold_off << color_green;
        return _sink;
    };
    
    Endl& Logger::endl() { return _endl; }
    

    void Logger::SetVerbosityLevel(Logger::Severity s)
    {
        _verbosity_level = s;
    }

    
    
    
    
    
} }
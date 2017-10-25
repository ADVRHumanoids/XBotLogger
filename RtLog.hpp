#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <iostream>

#define RESET   "\033[0m"
#define BLACK   "\033[30m"      /* Black */
#define RED     "\033[31m"      /* Red */
#define GREEN   "\033[32m"      /* Green */
#define YELLOW  "\033[33m"      /* Yellow */
#define BLUE    "\033[34m"      /* Blue */
#define MAGENTA "\033[35m"      /* Magenta */
#define CYAN    "\033[36m"      /* Cyan */
#define WHITE   "\033[37m"      /* White */

#ifndef DPRINTF

#ifdef __XENO__
    #include <rtdk.h>
    #define DPRINTF rt_printf
#else
    #include <stdio.h>
    #define DPRINTF printf
#endif

#endif


namespace XBot { namespace experimental {
    
    class Logger;
    
    class Endl {
        
    public:
        
        friend class Logger;
        
        friend void operator<< ( std::ostream& os, Endl& endl );
        
    private:
        
        Endl(Logger& logger): _logger(logger) {}
        
        void print();
        
        Logger& _logger;
        
    };
    
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
        return os << GREEN;
    }
    
    std::ostream& color_red(std::ostream& os)
    {
        return os << RED;
    }
    
    std::ostream& color_yellow(std::ostream& os)
    {
        return os << YELLOW;
    }
    
    std::ostream& color_reset(std::ostream& os)
    {
        return os << RESET;
    }
    
    class Logger {
        
    public:
        
        typedef boost::iostreams::stream<boost::iostreams::array_sink> IoStream;
        
        friend class Endl;
        
        Logger(): 
            _endl(*this) 
        {
           
        }
        
        std::ostream& info() 
        {
            memset(_buffer, 0, BUFFER_SIZE);
            _sink.open(_buffer);
            _sink << bold_on << "[INFO] " << bold_off;
            return _sink;
        };
        
        std::ostream& error() 
        {
            memset(_buffer, 0, BUFFER_SIZE);
            _sink.open(_buffer);
            _sink << bold_on << color_red << "[ERROR] " << bold_off << color_red;
            return _sink;
        };
        
        std::ostream& warning() 
        {
            memset(_buffer, 0, BUFFER_SIZE);
            _sink.open(_buffer);
            _sink << bold_on << color_yellow << "[WARNING] " << bold_off << color_yellow;
            return _sink;
        };
        
        std::ostream& success() 
        {
            memset(_buffer, 0, BUFFER_SIZE);
            _sink.open(_buffer);
            _sink << bold_on << color_green << "[OK] " << bold_off << color_green;
            return _sink;
        };
        
        Endl& endl() { return _endl; }
        
    protected:
        
    private:
        
        void print();
        
        static const int BUFFER_SIZE = 4096;
        
        char _buffer[BUFFER_SIZE];
        
        IoStream _sink;
        
        Endl _endl;
        
    };
    
    void operator<< ( std::ostream& os, Endl& endl )
    {
        auto& l_ios = dynamic_cast<Logger::IoStream&>(os);
        endl.print();
        l_ios.close();
    }
    
    void Endl::print()
    {
        _logger.print();
    }
    
    void Logger::print()
    {
        _sink << color_reset;
        DPRINTF("%s\n", _buffer);
    }

    
    
    
    
    
} }
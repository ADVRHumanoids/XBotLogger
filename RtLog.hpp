/*
 * Copyright (C) 2017 IIT-ADVR
 * Author: Arturo Laurenzi
 * email:  arturo.laurenzi@iit.it
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

#ifndef __XBOT_RT_LOGGER_HPP__
#define __XBOT_RT_LOGGER_HPP__

#include <boost/iostreams/stream.hpp>
#include <boost/iostreams/device/array.hpp>
#include <iostream>


namespace XBot { 
    
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
    class LoggerClass;
    
    
    
    /**
     * @brief Class handling the flushing of log messages to console.
     * 
     */
    class Endl {
        
    public:
        
        friend class LoggerClass;
        
        friend void operator<< ( std::ostream& os, Endl& endl );
        
    private:
        
        Endl(LoggerClass& logger_handle);
        
        LoggerClass& _logger_handle;
        
        void print();
        
    };
    
    
    
    class Logger {
        
    public:
        
        enum class Severity { DEBUG = -1, LOW = 0, MID = 1, HIGH = 2, FATAL = 3 };
        
        /**
         * @brief Writes to the internal stream with no special formatting and without changing
         * the severity level (which defaults to HIGH). 
         * 
         * @return Reference to std::ostream (it enables to leverage the same interface as std::cout / cerr)
         */
        static std::ostream& log();
        
        /**
         * @brief Logs an information message (with bold [INFO] header).
         * 
         * @param s Message severity. Defaults to LOW.
         * @return Reference to std::ostream (it enables to leverage the same interface as std::cout / cerr)
         */
        static std::ostream& info(Logger::Severity s = Logger::Severity::LOW);
        
        /**
         * @brief Logs an error message (in red, with bold [ERROR] header).
         * 
         * @param s Message severity. Defaults to HIGH.
         * @return Reference to std::ostream (it enables to leverage the same interface as std::cout / cerr)
         */
        static std::ostream& error(Logger::Severity s = Logger::Severity::HIGH);
        
        /**
         * @brief Logs a warning message (in yellow, with bold [WARNING] header).
         * 
         * @param s Message severity. Defaults to MEDIUM.
         * @return Reference to std::ostream (it enables to leverage the same interface as std::cout / cerr)
         */
        static std::ostream& warning(Logger::Severity s = Logger::Severity::MID);
        
        /**
         * @brief Logs a success message (in green, with bold [OK] header).
         * 
         * @param s Message severity. Defaults to LOW.
         * @return Reference to std::ostream (it enables to leverage the same interface as std::cout / cerr)
         */
        static std::ostream& success(Logger::Severity s = Logger::Severity::LOW);
        
        /**
         * @brief Logs a message that is printed only in debug mode (or whenever NDEBUG is not defined)
         * 
         * @param s Message severity. Defaults to LOW.
         * @return Reference to std::ostream (it enables to leverage the same interface as std::cout / cerr)
         */
        static std::ostream& debug(Logger::Severity s = Logger::Severity::DEBUG);
        
        /**
         * @brief Closes the message and prints to screen.
         */
        static Endl& endl();
        
        /**
         * @brief Sets the global verbosity level, i.e. the minimum severity that a message must have
         * in order to actually be printed.
         */
        static void SetVerbosityLevel(Logger::Severity s);
        
        /**
         * @brief Sets the global verbosity level, i.e. the minimum severity that a message must have
         * in order to actually be printed.
         */
        static Logger::Severity GetVerbosityLevel();

        
    protected:
        
    private:
        
        Logger() = delete;
        
        static LoggerClass _logger;
        
    };
    
    
    /**
     * @brief Logger class.
     * 
     */
    class LoggerClass {
        
    public:
        
        friend class Endl;
        
        LoggerClass(std::string logger_name);
        
        /**
         * @brief Writes to the internal stream with no special formatting and without changing
         * the severity level (which defaults to HIGH). 
         * 
         * @return Reference to std::ostream (it enables to leverage the same interface as std::cout / cerr)
         */
        std::ostream& log();
        
        /**
         * @brief Logs an information message (with bold [INFO] header).
         * 
         * @param s Message severity. Defaults to LOW.
         * @return Reference to std::ostream (it enables to leverage the same interface as std::cout / cerr)
         */
        std::ostream& info(Logger::Severity s = Logger::Severity::LOW);
        
        /**
         * @brief Logs an error message (in red, with bold [ERROR] header).
         * 
         * @param s Message severity. Defaults to HIGH.
         * @return Reference to std::ostream (it enables to leverage the same interface as std::cout / cerr)
         */
        std::ostream& error(Logger::Severity s = Logger::Severity::HIGH);
        
        /**
         * @brief Logs a warning message (in yellow, with bold [WARNING] header).
         * 
         * @param s Message severity. Defaults to MEDIUM.
         * @return Reference to std::ostream (it enables to leverage the same interface as std::cout / cerr)
         */
        std::ostream& warning(Logger::Severity s = Logger::Severity::MID);
        
        /**
         * @brief Logs a success message (in green, with bold [OK] header).
         * 
         * @param s Message severity. Defaults to LOW.
         * @return Reference to std::ostream (it enables to leverage the same interface as std::cout / cerr)
         */
        std::ostream& success(Logger::Severity s = Logger::Severity::LOW);
        
        /**
         * @brief Logs a message that is printed only in debug mode (or whenever NDEBUG is not defined)
         * 
         * @param s Message severity. Defaults to LOW.
         * @return Reference to std::ostream (it enables to leverage the same interface as std::cout / cerr)
         */
        std::ostream& debug(Logger::Severity s = Logger::Severity::LOW);
        
        /**
         * @brief Closes the message and prints to screen.
         */
        Endl& endl();
        
        /**
         * @brief Sets the global verbosity level, i.e. the minimum severity that a message must have
         * in order to actually be printed.
         */
        void setVerbosityLevel(Logger::Severity s);
        
        /**
         * @brief Sets the global verbosity level, i.e. the minimum severity that a message must have
         * in order to actually be printed.
         */
        Logger::Severity getVerbosityLevel() const;
        
    private:
        
        typedef boost::iostreams::stream<boost::iostreams::array_sink> IoStream;
        
        void print();
        
        void print_internal();
        
        void init_sink();
        
        static const int BUFFER_SIZE = 4096;
        
        char _buffer[BUFFER_SIZE];
        
        IoStream _sink;
        
        Endl _endl;
        
        std::string _name, _name_tag;
        Logger::Severity _severity;
        Logger::Severity _verbosity_level;
        
        
    };
    
    

    
    


} 


#endif
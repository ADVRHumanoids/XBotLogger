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


#ifndef __XBOT_LOGGER__
#define __XBOT_LOGGER__

#include <ostream>
#include <iostream>
#include <sstream>

#include <spdlog/spdlog.h>

#define ASYNC_QUEUE_SIZE_BIT 65536

namespace XBot {

class SSLogger;

/**
 * @brief Logger severity level
 *
 */
enum class LoggerLevel {
    INFO,
    WARNING,
    ERROR
};

/**
 * @brief Endl abstraction to control the print of the SSLogger
 *
 */
class LoggerEndl {

public:

    LoggerEndl ( SSLogger * sslogger ) : _ss_logger ( sslogger ) {}
    friend void operator<< ( std::ostream& os, LoggerEndl& log );

private:

    void print ( std::ostream& os );
    SSLogger * _ss_logger;

};

/////////////////////////////////////////////////////////////////////////////////////
// LoggerEndl << operator : print the os using the different Logger severity level //
/////////////////////////////////////////////////////////////////////////////////////

void operator<< ( std::ostream& os, LoggerEndl& log ) {
    log.print ( os );
}


/**
 * @brief String Stream Logger
 *
 */
class SSLogger  {

    // LoggerEndl can access SSLogger private print
    friend LoggerEndl;

public:

    /**
     * @brief SSLogger constructor
     *
     *
     */
    SSLogger ( const std::string& logger_name,
               const std::string& log_filename,
               int log_file_size,
               int log_max_files )
        : _endl ( this ) {

        // by default use the async mode to be RT safe
        size_t q_size = ASYNC_QUEUE_SIZE_BIT;
        spdlog::set_async_mode ( q_size );

        _console = spdlog::stdout_color_mt ( "console" );
        _rotating_logger = spdlog::rotating_logger_mt ( logger_name, log_filename, log_file_size, log_max_files );
    }

    void setSeverityLevel ( XBot::LoggerLevel severity ) {
        _severity = severity;
    }

    std::stringstream& operator<< ( std::stringstream& ss ) {
        _ss << ss.str();
        return _ss;
    }

    std::stringstream& operator<< ( const char * text ) {
        _ss << std::string ( text );
        return _ss;
    }

    void setHeader ( const std::string& header ) {
        _header = header;
    }

    std::stringstream& log() {
        // put the header in the stringstream
        _ss << _header;
        // give it back to <<
        return _ss;
    }

    LoggerEndl& endl() {
        return _endl;
    }

    friend void operator<< ( std::ostream& ss, SSLogger& log );
    friend void operator<< ( std::ostream& ss, LoggerEndl& log );

    // TBD ask why we needed this
//     SSLogger& operator<< ( LoggerEndl e ) {
//         std::cout << _ss.str() << std::endl;
//         return *this;
//     }

private:

    void print ( std::ostream& os ) {

        // from ostream to stringstream
        std::stringstream ss;
        ss << os.rdbuf();

        // chose the right severity level
        switch ( _severity ) {
        case LoggerLevel::INFO : {
            _console->info ( ss.str() );
            _rotating_logger->info ( ss.str() );
            break;
        }
        case LoggerLevel::WARNING : {
            _console->warn ( ss.str() );
            _rotating_logger->warn ( ss.str() );
            break;
        }
        case LoggerLevel::ERROR : {
            _console->error ( ss.str() );
            _rotating_logger->error ( ss.str() );
            break;
        }
        default : {
            break;
        }
        }
    }


private:

    std::stringstream _ss;
    std::string _header;

    LoggerEndl _endl;
    LoggerLevel _severity;

    std::shared_ptr<spdlog::logger> _console;
    std::shared_ptr<spdlog::logger> _rotating_logger;


};

// now that SSLogger is completely defined we can implment the print()
void LoggerEndl::print ( std::ostream& ss ) {
    _ss_logger->print ( ss );
}


/**
 * @brief XBot Logger
 *
 */
class Logger {

public:

    /**
     * @brief XBot Logger constructor: you can specify the logger name, the path for the log,
     * the file_size and the maximum number of file to use for the log.
     *
     * By default it creates the log in /tmp/XBotLog, using 5Mb and a maximum of 5 files.
     *
     */
    Logger ( const std::string& logger_name,
             const std::string& log_filename = "/tmp/XBotLog",
             int log_file_size = 1048576 * 5,
             int log_max_files = 5 )
        : _ss_logger ( logger_name, log_filename, log_file_size, log_max_files ) {}

    /**
    * @brief Info log
    *
    * @return XBot::SSLogger&
    */
    SSLogger& info() {
        _ss_logger.setSeverityLevel ( XBot::LoggerLevel::INFO );
        return _ss_logger;
    }

    /**
     * @brief Warning log
     *
     * @return XBot::SSLogger&
     */
    SSLogger& warning() {
        _ss_logger.setSeverityLevel ( XBot::LoggerLevel::WARNING );
        return _ss_logger;
    }

    /**
     * @brief Error log
     *
     * @return XBot::SSLogger&
     */
    SSLogger& error() {
        _ss_logger.setSeverityLevel ( XBot::LoggerLevel::ERROR );
        return _ss_logger;
    }

    /**
     * @brief Smart endl for XBot Logger
     *
     * @return XBot::LoggerEndl&
     */
    LoggerEndl& endl() {
        return _ss_logger.endl();
    }

private:

    SSLogger _ss_logger;

};


}

#endif //__XBOT_LOGGER__

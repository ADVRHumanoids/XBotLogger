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

#include <unordered_map>

#include <Eigen/Dense>

#include <spdlog/spdlog.h>

#define ASYNC_QUEUE_SIZE_BIT 65536

#define M_PREALLOCATION_SIZE 8192

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
        : _logger_name(logger_name),
          _log_filename(log_filename),
          _log_file_size(log_file_size),
          _log_max_files(log_max_files),
          _endl ( this )
    {
    }

    void setSeverityLevel ( XBot::LoggerLevel severity ) {
        _severity = severity;
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

    friend void operator<< ( std::ostream& ss, LoggerEndl& log );

protected:

    virtual void info_internal( const std::string& s) = 0;
    virtual void warning_internal( const std::string& s) = 0;
    virtual void error_internal( const std::string& s) = 0;

    std::string _logger_name;
    std::string _log_filename;
    int _log_file_size;
    int _log_max_files;

private:

    void print ( std::ostream& os ) {

        // from ostream to stringstream
        std::stringstream ss;
        ss << os.rdbuf();

        // chose the right severity level
        switch ( _severity ) {
            case LoggerLevel::INFO : {
                info_internal( ss.str() );
                break;
            }
            case LoggerLevel::WARNING : {
                warning_internal( ss.str() );
                break;
            }
            case LoggerLevel::ERROR : {
                error_internal( ss.str() );
                break;
            }
            default : {
                break;
            }
        }

        // empty the stringstream
        _ss.str(std::string());
    }

    std::stringstream _ss;
    std::string _header;

    LoggerEndl _endl;
    LoggerLevel _severity;

};

class SSFileLogger : public SSLogger {

public:
     /**
     * @brief SSLogger constructor
     *
     *
     */
     SSFileLogger ( const std::string& logger_name,
                 const std::string& log_filename,
                 int log_file_size,
                 int log_max_files ) :
        SSLogger(logger_name,
                 log_filename,
                 log_file_size,
                 log_max_files)
    {

    }


};

class SPDFileLogger : public SSFileLogger {

public:
     /**
     * @brief SSLogger constructor
     *
     *
     */
     SPDFileLogger ( const std::string& logger_name,
                     const std::string& log_filename,
                     int log_file_size,
                     int log_max_files ) :
        SSFileLogger( logger_name,
                      log_filename,
                      log_file_size,
                      log_max_files)
    {

        // by default use the async mode to be RT safe
        size_t q_size = ASYNC_QUEUE_SIZE_BIT;
        spdlog::set_async_mode ( q_size );

        // rotating file logger
        _rotating_logger = spdlog::rotating_logger_mt ( logger_name, log_filename, log_file_size, log_max_files );
    }

protected:

    virtual void info_internal( const std::string& s) {
        _rotating_logger->info ( s );
    }

    virtual void warning_internal( const std::string& s) {
        _rotating_logger->warn ( s );
    }

    virtual void error_internal( const std::string& s) {
        _rotating_logger->error ( s );
    }


private:

    std::shared_ptr<spdlog::logger> _rotating_logger;

};

class SSConsoleLogger : public SSLogger {

public:
     /**
     * @brief SSLogger constructor
     *
     *
     */
     SSConsoleLogger ( const std::string& logger_name ) :
        SSLogger(logger_name,
                 "/tmp/XBotConsoleLog",
                 1048576 * 5,
                 5)
    {

    }

};

class SPDConsoleLogger : public SSConsoleLogger {

public:
     /**
     * @brief SSLogger constructor
     *
     *
     */
     SPDConsoleLogger ( const std::string& logger_name) :
        SSConsoleLogger( logger_name)
    {
        // console logger
        _console = spdlog::stdout_color_mt ( "console" );
        // rotating file logger
        _rotating_logger = spdlog::rotating_logger_mt ( logger_name, _log_filename, _log_file_size, _log_max_files );
    }

protected:

    virtual void info_internal( const std::string& s) {
        _console->info ( s );
        _rotating_logger->info ( s );
    }

    virtual void warning_internal( const std::string& s) {
        _console->warn ( s );
        _rotating_logger->warn ( s );
    }

    virtual void error_internal( const std::string& s) {
        _console->error ( s );
        _rotating_logger->error ( s );
    }


private:

    std::shared_ptr<spdlog::logger> _console;
    std::shared_ptr<spdlog::logger> _rotating_logger;

};


class Logger {

public:


    /**
    * @brief Info log
    *
    * @return XBot::SSLogger&
    */
    virtual std::stringstream& info() {
        _ss_logger->setSeverityLevel ( XBot::LoggerLevel::INFO );
        return _ss_logger->log();
    }

    /**
     * @brief Warning log
     *
     * @return XBot::SSLogger&
     */
    virtual std::stringstream& warning() {
        _ss_logger->setSeverityLevel ( XBot::LoggerLevel::WARNING );
        return _ss_logger->log();
    }

    /**
     * @brief Error log
     *
     * @return XBot::SSLogger&
     */
    virtual std::stringstream& error() {
        _ss_logger->setSeverityLevel ( XBot::LoggerLevel::ERROR );
        return _ss_logger->log();
    }

    /**
     * @brief Smart endl for XBot Logger
     *
     * @return XBot::LoggerEndl&
     */
    LoggerEndl& endl() {
        return _ss_logger->endl();
    }

protected:

     std::shared_ptr<SSLogger> _ss_logger;

};

class FileLogger : public Logger {

public:

    typedef std::shared_ptr<FileLogger> Ptr;

    static Ptr getLogger( const std::string& logger_name,
                 const std::string& log_filename,
                 int log_file_size = 1048576 * 5,
                 int log_max_files = 5 ){

        if(_instance_map.count(logger_name)) return _instance_map.at(logger_name);
        else{
            Ptr ptr(new FileLogger(logger_name, log_filename, log_file_size, log_max_files));
            _instance_map[logger_name] = ptr;
            return ptr;
        }

    }



private:

    FileLogger ( const std::string& logger_name,
                 const std::string& log_filename,
                 int log_file_size = 1048576 * 5,
                 int log_max_files = 5 )
    {
        _ss_logger = std::make_shared<SPDFileLogger>(logger_name, log_filename, log_file_size, log_max_files);
    }

    static std::map<std::string, Ptr> _instance_map;

};

std::map<std::string, FileLogger::Ptr> FileLogger::_instance_map;

class ConsoleLogger : public Logger {

public:

    typedef std::shared_ptr<ConsoleLogger> Ptr;

    static Ptr getLogger(const std::string& logger_name = "console_logger"){
        if(_instance) return _instance;
        else{
            _instance = Ptr( new ConsoleLogger(logger_name) );
            return _instance;
        }
    }

private:

    /**
     * @brief XBot Logger constructor: you can specify the logger name, the path for the log,
     * the file_size and the maximum number of file to use for the log.
     *
     * By default it creates the log using file /tmp/XBotLog with 5Mb and a maximum of 5 files.
     *
     */

    ConsoleLogger ( const std::string& logger_name)
    {
        _ss_logger = std::make_shared<SPDConsoleLogger>(logger_name);
    }

    static Ptr _instance;


};

ConsoleLogger::Ptr ConsoleLogger::_instance;


class MatLogger {

public:

    typedef std::shared_ptr<MatLogger> Ptr;

    static Ptr getLogger( const std::string& logger_name,
                 const std::string& log_filename);

    template < typename Derived >
    void add( const std::string& var_name, const Eigen::MatrixBase<Derived>& var) {
        std::stringstream ss;
        int current_cout = 0;
        auto it = _var_count_map.find(var_name);
        if(it != _var_count_map.end()) {
            it->second++;
            current_cout = it->second;
        }
        else {
            // create the var in the _var_count_map
            _var_count_map[var_name] = 1;
            current_cout = 1;

            // set the var _var_dim_map
            _var_dim_map[var_name] = std::make_pair<int,int>(var.rows(), var.cols());

            // name of the variable
            ss << var_name;

            //check if matrix or vector and allocated it
            if(var.cols() == 1) {
                // NOTE only column vecotrs
                ss << "(" << var.rows() << "," << M_PREALLOCATION_SIZE << ") = 0;" << std::endl;
            }
            else {
                // NOTE matrix is a cube
                ss << "(" << var.rows() << "," << var.cols() << "," << M_PREALLOCATION_SIZE << ") = 0;" << std::endl;
            }

        }

        if(_var_dim_map.at(var_name).first != var.rows() ||
           _var_dim_map.at(var_name).second != var.cols() ){
            // NOTE do it on the logger
            std::cerr << "Error in " << __func__ << " : provided Matrix/Vector has mismatching dimensions" << std::endl;
            return;
        }

        //check if matrix or vector and fill it
        if(var.cols() == 1) {
            // NOTE only column vectors
            ss << var_name << "(:," << current_cout << ") = " << var.format(_mat_fmt) << ";" << std::endl;
        }
        else {
            // NOTE matrix is a cube
            ss << var_name << "(:,:," << current_cout << ") = " << var.format(_mat_fmt) << ";" << std::endl;
        }

        add_internal(ss.str() );

    }

    virtual void add( const std::string& var_name, float var) {
        _scalar_var[0] = var;
        add(var_name, _scalar_var);
    }

    virtual ~MatLogger() {
        // TBD how to do it?
//         std::cout << "~~~~~~~MatLogger" << std::endl;
//         close();
    }

protected:

    virtual void add_internal( const std::string& var_string) = 0;

    MatLogger ( const std::string& logger_name,
                const std::string& log_filename ) :
        _logger_name(logger_name),
        _log_filename(log_filename),
        _scalar_var(1),
        _mat_fmt(Eigen::FullPrecision, 0, ", ", ";\n", "", "", "[", "]")
    {

    }
private:

    void close() {
        for(auto m_var_count : _var_count_map) {
            std::stringstream ss;
            if(_var_dim_map.at(m_var_count.first).first == 1) {
                // NOTE only column vecotrs
                ss << m_var_count.first << "=" << m_var_count.first << "(:,1:" << m_var_count.second << ");" << std::endl;
            }
            else {
                // NOTE matrix is a cube
                ss << m_var_count.first << "=" << m_var_count.first << "(:,:,1:" << m_var_count.second << ");" << std::endl;
            }

            add_internal(ss.str());
        }
    }

    Eigen::IOFormat _mat_fmt;

    std::string _logger_name;
    std::string _log_filename;

    std::string _var_string;

    Eigen::VectorXd _scalar_var;

    std::unordered_map<std::string, int> _var_count_map;
    std::unordered_map<std::string, std::pair<int,int>> _var_dim_map;

    static std::map<std::string, Ptr> _instance_map;


};

class SPDMatLogger : public MatLogger {

public:
     /**
     * @brief SSLogger constructor
     *
     *
     */
     SPDMatLogger ( const std::string& logger_name,
                      const std::string& log_filename ) :
        MatLogger(logger_name, log_filename)
    {

        // by default use the async mode to be RT safe
        size_t q_size = ASYNC_QUEUE_SIZE_BIT;
        spdlog::set_async_mode ( q_size );

        // set m pattern
        spdlog::set_pattern("%v");

        // rotating file logger
        _simple_logger = spdlog::basic_logger_mt ( logger_name, log_filename );
    }

    virtual ~SPDMatLogger(){
//         std::cout << "~~~~~~~SPDMatLogger" << std::endl;
    }

protected:

    void add_internal( const std::string& var_string) {
        _simple_logger->info(var_string);
    }

private:

    std::shared_ptr<spdlog::logger> _simple_logger;

};

// now that SSLogger is completely defined we can implment the print()
void LoggerEndl::print ( std::ostream& ss ) {
    _ss_logger->print ( ss );
}

std::map<std::string, MatLogger::Ptr> MatLogger::_instance_map;

MatLogger::Ptr MatLogger::getLogger(const std::string& logger_name, const std::string& log_filename)
{

    if(_instance_map.count(logger_name)) return _instance_map.at(logger_name);
    else{
        Ptr ptr(new SPDMatLogger(logger_name, log_filename));
        _instance_map[logger_name] = ptr;
        return ptr;
    }

}



}

#endif //__XBOT_LOGGER__

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

#include <signal.h>

#include <time.h>

#include <matio.h>

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
                 "/tmp/console_log",
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


#define DEFAULT_BUFFER_SIZE 60000

class MatLogger {



protected: enum class VariableType { Scalar, Vector, Matrix };

protected: struct VariableInfo{

    int interleave = 1;
    int count = 0;
    VariableType type;
    Eigen::MatrixXd data;
    int rows, cols;
    int buffer_capacity;
    int head = 0, tail = 0;
    bool empty = true;

    void rearrange()
    {
        if( empty ){
            data.resize(0,0);
            return;
        }

        if( tail > head ){
            data.conservativeResize(data.rows(), cols*(tail-head));
            return;
        }

        for( int i = 0; i < (buffer_capacity-tail)*cols; i++ )
            circshift();
    }

private:

    void circshift(){
        for( int i = 1; i < data.cols(); i++){
            data.col(i).swap(data.col(0));
        }
    }

};

public:

    typedef std::shared_ptr<MatLogger> Ptr;

    static Ptr getLogger(std::string filename)
    {
        if( _instances.count(filename) ){
            return _instances.at(filename);
        }
        else{
            _instances[filename] = Ptr(new MatLogger(filename));
            return _instances.at(filename);
        }
    }


    bool createScalarVariable(std::string name, int interleave = 1, int buffer_size = DEFAULT_BUFFER_SIZE)
    {
        if(_var_idx_map.count(name)){
            return false;
        }

        _var_idx_map[name] = VariableInfo();

        VariableInfo& varinfo = _var_idx_map.at(name);

        varinfo.interleave = interleave;
        varinfo.count = -1;
        varinfo.type = VariableType::Scalar;
        varinfo.data = Eigen::VectorXd::Zero(buffer_size);
        varinfo.rows = 1;
        varinfo.cols = 1;
        varinfo.buffer_capacity = buffer_size;


        return true;

    }

    bool createVectorVariable(std::string name, int size, int interleave = 1, int buffer_size = DEFAULT_BUFFER_SIZE)
    {
        if(_var_idx_map.count(name)){
            return false;
        }

        _var_idx_map[name] = VariableInfo();

        VariableInfo& varinfo = _var_idx_map.at(name);

        varinfo.interleave = interleave;
        varinfo.count = 0;
        varinfo.type = VariableType::Vector;
        varinfo.data = Eigen::MatrixXd::Zero(size, buffer_size);
        varinfo.rows = size;
        varinfo.cols = 1;
        varinfo.buffer_capacity = buffer_size;


        return true;
    }

    bool createMatrixVariable(std::string name, int rows, int cols, int interleave = 1, int buffer_size = DEFAULT_BUFFER_SIZE)
    {
        if(_var_idx_map.count(name)){
            return false;
        }

        _var_idx_map[name] = VariableInfo();

        VariableInfo& varinfo = _var_idx_map.at(name);

        varinfo.interleave = interleave;
        varinfo.count = -1;
        varinfo.type = VariableType::Matrix;
        varinfo.data = Eigen::MatrixXd::Zero(rows, cols*buffer_size);
        varinfo.rows = rows;
        varinfo.cols = cols;
        varinfo.buffer_capacity = buffer_size;


        return true;
    }

    template <typename Derived>
    bool add(const std::string& name, const Eigen::MatrixBase<Derived>& data)
    {
        auto it = _var_idx_map.find(name);

        if( it == _var_idx_map.end() ){
            _clog->warning() << " in " << __func__ << "! Variable with name " << name << " has NOT been created yet!" << _clog->endl();
            return false;
        }

        VariableInfo& varinfo = it->second;

        if( data.rows() != varinfo.rows || data.cols() != varinfo.cols ){
            _clog->warning() << " in " << __func__ << "! Provided data has unmatching dimensions!" << _clog->endl();
            return false;
        }


        varinfo.count = (varinfo.count + 1) % varinfo.interleave;

        if( varinfo.count != 0 ){
            return true;
        }

        // if buffer is not empty and head = tail, increment head since we are going to overwrite an element
        if( !varinfo.empty && varinfo.head == varinfo.tail ){
            varinfo.head = (varinfo.head + 1) % varinfo.buffer_capacity;
        }

        // write to tail position
        varinfo.data.block(0,varinfo.tail*varinfo.cols,varinfo.rows,varinfo.cols) = data;
        varinfo.empty = false;

        // increment tail position
        varinfo.tail = (varinfo.tail + 1) % varinfo.buffer_capacity;

    }

    bool add(const std::string& name, double data)
    {
        Eigen::Matrix<double, 1, 1> eigen_data;
        eigen_data(0) = data;
        return add(name, eigen_data);
    }

    void flush(){

        if(_flushed) return;

        _flushed = true;

        std::cout << "Dumping data to mat file " << _file_name << std::endl;

        mat_t * mat_file = Mat_CreateVer(_file_name.c_str(), nullptr, MAT_FT_MAT73);

        if(!mat_file){
            _clog->error() << "ERROR creating MAT file!" << _clog->endl();
        }

        for( auto& pair : _var_idx_map ){

            VariableInfo& varinfo = pair.second;

            varinfo.rearrange();

            int n_dims = 2;
            std::size_t dims[3];

            if( varinfo.type == VariableType::Matrix ){
                n_dims = 3;
                dims[0] = varinfo.rows;
                dims[1] = varinfo.cols;
                dims[2] = varinfo.data.cols()/varinfo.cols;
            }
            else{
                dims[0] = varinfo.data.rows();
                dims[1] = varinfo.data.cols();
            }

            matvar_t * mat_var = Mat_VarCreate(pair.first.c_str(),
                                               MAT_C_DOUBLE,
                                               MAT_T_DOUBLE,
                                               n_dims,
                                               dims,
                                               (void *)varinfo.data.data(),
                                               0 );

            Mat_VarWrite(mat_file, mat_var, MAT_COMPRESSION_ZLIB);
            Mat_VarFree(mat_var);

        }



    }


    ~MatLogger(){
        flush();
    }

protected:

    MatLogger(std::string file_name):
        _clog(ConsoleLogger::getLogger()),
        _flushed(false)
    {
        // retrieve time
        time_t rawtime;
        struct tm * timeinfo;
        char buffer [80];

        std::time (&rawtime);
        timeinfo = localtime (&rawtime);

        strftime(buffer,80,"__%Y_%m_%d__%H_%M_%S.mat",timeinfo);
        puts (buffer);

        // rotating file logger
        std::string file_name_extended;
        file_name_extended = file_name+std::string(buffer);

        _file_name = file_name_extended;
    }

    std::unordered_map<std::string, VariableInfo> _var_idx_map;
    std::string _file_name;

private:

    static std::unordered_map<std::string, Ptr> _instances;
    ConsoleLogger::Ptr _clog;
    bool _flushed;

};



}

#endif //__XBOT_LOGGER__

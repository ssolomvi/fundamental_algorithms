#pragma once
#ifndef LOGGER_BASE_CLASS_H
#define LOGGER_BASE_CLASS_H

#include <iostream>
#include <exception>
#include "C:\fund-algths\lab5\vcpkg\installed\x86-windows\include\rapidjson\document.h"
#include "C:\fund-algths\lab5\vcpkg\installed\x86-windows\include\rapidjson\istreamwrapper.h"

#include <fstream>
#include <map>
#include <string>
#include <chrono>

class logger_builder;

class Logger
{
public:
    enum class Severity
    {
        trace = 0,
        debug,
        information,
        warning,
        error,
        critical
    };

    class severity_exception final : public std::exception
    {
    private:
        std::string _message;

    public:
        severity_exception(std::string const & message)
                : _message(message)
        {

        }

        char const * what() const noexcept override
        {
            return _message.c_str();
        }

    };

    static std::string severity_to_string_logger(Severity s)
    {
        switch (s) {
            case Severity::trace:
                return "[ trace ] ";
            case Severity::debug:
                return "[ debug ] ";
            case Severity::information:
                return "[ information ] ";
            case Severity::warning:
                return "[ warning ] ";
            case Severity::error:
                return "[ error ] ";
            case Severity::critical:
                return "[ critical ] ";
            default:
                throw severity_exception("invalid Severity value used");
        }
    }

    static Logger::Severity from_string_to_severity_parse(const std::string& s)
    {
        if (s == "trace") {
            return Logger::Severity::trace;
        }
        if (s == "debug") {
            return Logger::Severity::debug;
        }
        if (s == "information") {
            return Logger::Severity::information;
        }
        if (s == "warning") {
            return Logger::Severity::warning;
        }
        if (s == "error") {
            return Logger::Severity::error;
        }
        if (s == "critical") {
            return Logger::Severity::critical;
        } else {
            throw Logger::severity_exception("invalid Severity string passed");
        }
    }

    virtual Logger const * log(std::string const & target, Severity level) const = 0;

    virtual ~Logger() = default;
};

class Logger_impl : public Logger
{
private:
    std::map<std::string, Severity> this_logger_streams;
    static std::map<std::string, std::pair<std::ofstream*, size_t> > _all_loggers_streams;

    explicit Logger_impl(std::map<std::string, Severity> const & info_to_construct_from)
            : this_logger_streams(info_to_construct_from)
    {
        std::pair<std::ofstream *, size_t> stream_pair;
        for (auto & this_logger_stream: this_logger_streams) {
            auto iterator = _all_loggers_streams.find(this_logger_stream.first);
            if (iterator == _all_loggers_streams.end()) {
                std::ofstream *new_stream = nullptr;
                if (this_logger_stream.first != "console") {
                    new_stream = new std::ofstream();
                    new_stream->open(this_logger_stream.first);
                }
                _all_loggers_streams.insert(std::make_pair(this_logger_stream.first, std::make_pair(new_stream, 1)));
            }
            else {
                iterator->second.second++;
            }
        }
    }

public:
    Logger_impl(Logger_impl const& log) = delete;        // copying
    Logger_impl& operator=(Logger_impl const&) = delete; // assignment

    friend class logger_builder_impl;
    // static logger_builder builder();
    ~Logger_impl() {
        std::pair<std::ofstream *, size_t> stream_pair;
        for (auto & this_logger_stream: this_logger_streams) {
            if ((stream_pair = Logger_impl::_all_loggers_streams[this_logger_stream.first]).second == 1) {
                if (stream_pair.first != nullptr) {
                    stream_pair.first->flush();
                    stream_pair.first->close();
                }
                Logger_impl::_all_loggers_streams.erase(this_logger_stream.first);
            } else {
                Logger_impl::_all_loggers_streams[this_logger_stream.first].second--;
            }
        }
    }

    Logger const* log(std::string const & target, Severity level) const override
    {
        for (const auto & this_loggers_stream : this_logger_streams) {
            if (this_loggers_stream.second <= level) {
                time_t raw_time;
                struct tm * time_info;
                char curr_time [80];

                time ( &raw_time );
                time_info = localtime (&raw_time );
                strftime (curr_time, 80, "[ %d/%m/%Y %H:%M:%S ]", time_info);

                if (this_loggers_stream.first == "console") {
                    std::cout << curr_time << severity_to_string_logger(level) << target << std::endl;
                } else {
                    (*Logger_impl::_all_loggers_streams[this_loggers_stream.first].first) << curr_time << severity_to_string_logger(
                            level) << target << std::endl;
                }
            }
        }
        return this;
    }

};

std::map<std::string, std::pair<std::ofstream*, size_t>> Logger_impl::_all_loggers_streams = std::map<std::string, std::pair<std::ofstream*, size_t>>();

class logger_builder
{
public:
    virtual logger_builder *with_stream(std::string const &, Logger::Severity) = 0;

    [[nodiscard]] virtual Logger *build() const = 0;

    virtual ~logger_builder() {  }

    virtual Logger *config_from_json(const std::string&) = 0;
};

class logger_builder_impl final : public logger_builder
{
private:
    std::map<std::string, Logger::Severity> _under_construction_logger_setup;
public:
    logger_builder *with_stream(const std::string &stream, Logger::Severity severity) override
    {
        _under_construction_logger_setup[stream] = severity;
        return this;
    }

    [[nodiscard]] Logger *build() const override
    {
        return new Logger_impl(_under_construction_logger_setup);
    }

    Logger *config_from_json(const std::string& filename) override;
};

/*
Logger *logger_builder_impl::config_from_json(const std::string &filename) {
    std::ifstream config_file_ifs(filename);
    if (!(config_file_ifs.is_open())) {
        throw std::runtime_error("meow: .json configuration file not opened");
    }

    rapidjson::IStreamWrapper config_file_isw(config_file_ifs);
    rapidjson::Document doc;
    doc.ParseStream(config_file_isw);

    if (doc.HasParseError()) {
        config_file_ifs.close();
        throw std::runtime_error("Error parsing configuration file");
    }

    // building an object of Logger
    // logger_builder *builder = new logger_builder_impl();
    Logger* log;
    const rapidjson::Value& streams = doc["_all_loggers_streams"];
    rapidjson::Value::ConstValueIterator iter;
    for (iter = streams.Begin(); iter != streams.End(); ++iter) {
        this->with_stream(iter->GetObject()["name"].GetString(),
                          Logger::from_string_to_severity_parse(iter->GetObject()["Severity"].GetString()));
    }

    log = this->build();
    config_file_ifs.close();
    return log;
}
*/

Logger *logger_builder_impl::config_from_json(const std::string &filename) {
    std::ifstream config_file_ifs(filename);
    if (!(config_file_ifs.is_open())) {
        throw std::runtime_error("Configuration file not opened");
    }

    rapidjson::IStreamWrapper config_file_isw(config_file_ifs);
    rapidjson::Document doc;
    doc.ParseStream(config_file_isw);

    if (doc.HasParseError()) {
        config_file_ifs.close();
        throw std::runtime_error("Error parsing configuration file");
    }

    // building an object of logger
    // logger_builder *builder = new logger_builder_impl();
    Logger* log;
    const rapidjson::Value& streams = doc["streams"];
    rapidjson::Value::ConstValueIterator iter;
    for (iter = streams.Begin(); iter != streams.End(); ++iter) {
        this->with_stream(iter->GetObject()["name"].GetString(),
                          Logger::from_string_to_severity_parse(iter->GetObject()["severity"].GetString()));
    }

    log = this->build();
    config_file_ifs.close();
    return log;
}

#endif //LOGGER_BASE_CLASS_H

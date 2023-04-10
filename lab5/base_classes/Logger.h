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

class Logger {
public:
    enum class Severity {
        trace = 0,
        debug,
        information,
        warning,
        error,
        critical
    };

    class severity_exception final : public std::exception {
    private:
        std::string _message;

    public:
        severity_exception(std::string const &message)
                : _message(message) {

        }

        char const *what() const noexcept override {
            return _message.c_str();
        }

    };

    static std::string severity_to_string_logger(Severity s);

    static Logger::Severity from_string_to_severity_parse(const std::string &s);

    virtual Logger const *log(std::string const &target, Severity level) const = 0;

    virtual ~Logger() = default;
};

class Logger_impl : public Logger {
private:
    std::map<std::string, Severity> this_logger_streams;
    static std::map<std::string, std::pair<std::ofstream *, size_t> > _all_loggers_streams;

    explicit Logger_impl(std::map<std::string, Severity> const &info_to_construct_from);

public:
    Logger_impl(Logger_impl const &log) = delete;        // copying
    Logger_impl &operator=(Logger_impl const &) = delete; // assignment

    friend class logger_builder_impl;

    // static logger_builder builder();
    ~Logger_impl();

    Logger const *log(std::string const &target, Severity level) const override;
};


class logger_builder {
public:
    virtual logger_builder *with_stream(std::string const &, Logger::Severity) = 0;

    [[nodiscard]] virtual Logger *build() const = 0;

    virtual ~logger_builder() {}

    virtual Logger *config_from_json(const std::string &) = 0;
};

class logger_builder_impl final : public logger_builder {
private:
    std::map<std::string, Logger::Severity> _under_construction_logger_setup;
public:
    logger_builder *with_stream(const std::string &stream, Logger::Severity severity) override;

    [[nodiscard]] Logger *build() const override;

    Logger *config_from_json(const std::string &filename) override;
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

#endif //LOGGER_BASE_CLASS_H

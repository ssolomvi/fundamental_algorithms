#include <iostream>
#include <exception>
#include ".../rapidjson/document.h"
#include ".../rapidjson/istreamwrapper.h"

#include <fstream>
#include <map>
#include <string>
#include <chrono>

class logger_builder;

class logger
{
public:
    enum class severity
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

    static std::string severity_to_string_logger(severity s)
    {
        switch (s) {
            case severity::trace:
                return "[ trace ] ";
            case severity::debug:
                return "[ debug ] ";
            case severity::information:
                return "[ information ] ";
            case severity::warning:
                return "[ warning ] ";
            case severity::error:
                return "[ error ] ";
            case severity::critical:
                return "[ critical ] ";
            default:
                throw severity_exception("invalid severity value used");
        }
    }

    static logger::severity from_string_to_severity_parse(const std::string& s)
    {
        if (s == "trace") {
            return logger::severity::trace;
        }
        if (s == "debug") {
            return logger::severity::debug;
        }
        if (s == "information") {
            return logger::severity::information;
        }
        if (s == "warning") {
            return logger::severity::warning;
        }
        if (s == "error") {
            return logger::severity::error;
        }
        if (s == "critical") {
            return logger::severity::critical;
        } else {
            throw logger::severity_exception("invalid severity string passed");
        }
    }

    virtual logger const * log(std::string const & target, severity level) const = 0;

    virtual ~logger() = default;
};

class logger_impl final : public logger
{
private:
    explicit logger_impl(std::map<std::string, severity> const & info_to_construct_from)
            : this_logger_streams(info_to_construct_from)
    {
        std::pair<std::ofstream *, size_t> stream_pair;
        for (auto & this_logger_stream: this_logger_streams) {
            auto iterator = streams.find(this_logger_stream.first);
            if (iterator == streams.end()) {
                std::ofstream *new_stream = nullptr;
                if (this_logger_stream.first != "console") {
                    new_stream = new std::ofstream();
                    new_stream->open(this_logger_stream.first);
                }
                streams.insert(std::make_pair(this_logger_stream.first, std::make_pair(new_stream, 1)));
            }
            else {
                iterator->second.second++;
            }
        }
    }

    std::map<std::string, severity> this_logger_streams;
    static std::map<std::string, std::pair<std::ofstream*, size_t> > streams;
public:
    logger_impl(logger_impl const& log) = delete;        // copying
    logger_impl& operator=(logger_impl const&) = delete; // assignment

    friend class logger_builder_impl;
    // static logger_builder builder();
    ~logger_impl() {
        std::pair<std::ofstream *, size_t> stream_pair;
        for (auto & this_logger_stream: this_logger_streams) {
            if ((stream_pair = logger_impl::streams[this_logger_stream.first]).second == 1) {
                if (stream_pair.first != nullptr) {
                    stream_pair.first->flush();
                    stream_pair.first->close();
                }
                logger_impl::streams.erase(this_logger_stream.first);
            } else {
                logger_impl::streams[this_logger_stream.first].second--;
            }
        }
    }

    logger const* log(std::string const & target, severity level) const override
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
                    (*logger_impl::streams[this_loggers_stream.first].first) << curr_time << severity_to_string_logger(
                            level) << target << std::endl;
                }
            }
        }
        return this;
    }

};

std::map<std::string, std::pair<std::ofstream*, size_t>> logger_impl::streams = std::map<std::string, std::pair<std::ofstream*, size_t>>();

class logger_builder
{
public:
    virtual logger_builder *with_stream(std::string const &, logger::severity) = 0;

    [[nodiscard]] virtual logger *build() const = 0;

    virtual ~logger_builder() {  }

    virtual logger *config_from_json(const std::string&) = 0;
};

class logger_builder_impl final : public logger_builder
{
private:
    std::map<std::string, logger::severity> _under_construction_logger_setup;
public:
    logger_builder *with_stream(const std::string &stream, logger::severity severity) override
    {
        _under_construction_logger_setup[stream] = severity;
        return this;
    }

    [[nodiscard]] logger *build() const override
    {
        return new logger_impl(_under_construction_logger_setup);
    }

    logger *config_from_json(const std::string& filename) override;
};

logger *logger_builder_impl::config_from_json(const std::string &filename) {
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
    logger* log;
    const rapidjson::Value& streams = doc["streams"];
    rapidjson::Value::ConstValueIterator iter;
    for (iter = streams.Begin(); iter != streams.End(); ++iter) {
        this->with_stream(iter->GetObject()["name"].GetString(),
                             logger::from_string_to_severity_parse(iter->GetObject()["severity"].GetString()));
    }

    log = this->build();
    config_file_ifs.close();
    return log;
}

int main() {

    logger_builder *builder = new logger_builder_impl(), *builder2 = new logger_builder_impl();
    logger *log = builder
            ->with_stream("1.txt", logger::severity::trace)
            ->with_stream("console", logger::severity::debug)
            ->with_stream("2.txt", logger::severity::information)
            ->with_stream("3.txt", logger::severity::critical)
            ->build();

    logger *log2;
    try {
        log2 = builder2->config_from_json("C:/yandex/json_parser/1.json");
//        log2 = config_from_json("1.json");
    }
    catch (const std::exception& e) {
        std::cerr << "Caught: " << e.what() << std::endl;
        delete log;
        delete builder;
        delete builder2;
        return -1;
    }

    log->log("information kek from logger1", logger::severity::information);
    log->log("trace kek from logger1", logger::severity::trace);

    log2->log("here is some error info from logger2", logger::severity::error);
    log2->log("here is some information about program from logger2", logger::severity::information);


    delete log;
    delete builder;
    delete builder2;
    delete log2;
    std::cout << "Program done\n" << std::endl;
    return 0;
}


#include "Logger.h"

std::string Logger::severity_to_string_logger(Severity s) {
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

Logger::Severity Logger::from_string_to_severity_parse(const std::string &s) {
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

Logger_impl::~Logger_impl() {
    std::pair<std::ofstream *, size_t> stream_pair;
    for (auto &this_logger_stream: this_logger_streams) {
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

Logger_impl::Logger_impl(std::map<std::string, Severity> const &info_to_construct_from)
        : this_logger_streams(info_to_construct_from) {
    std::pair<std::ofstream *, size_t> stream_pair;
    for (auto &this_logger_stream: this_logger_streams) {
        auto iterator = _all_loggers_streams.find(this_logger_stream.first);
        if (iterator == _all_loggers_streams.end()) {
            std::ofstream *new_stream = nullptr;
            if (this_logger_stream.first != "console") {
                new_stream = new std::ofstream();
                new_stream->open(this_logger_stream.first);
                // todo: if new_stream is not opened, throw a message
                *new_stream << "<--- " << severity_to_string_logger(this_logger_stream.second) << " --->" << std::endl;
            }
            _all_loggers_streams.insert(std::make_pair(this_logger_stream.first, std::make_pair(new_stream, 1)));
        } else {
            iterator->second.second++;
        }
    }
}

Logger const *Logger_impl::log(std::string const &target, Severity level) const {
    for (const auto &this_loggers_stream: this_logger_streams) {
        if (this_loggers_stream.second <= level) {
            time_t raw_time;
            struct tm *time_info;
            char curr_time[80];

            time(&raw_time);
            time_info = localtime(&raw_time);
            strftime(curr_time, 80, "[ %d/%m/%Y %H:%M:%S ]", time_info);

            if (this_loggers_stream.first == "console") {
                std::cout << curr_time << severity_to_string_logger(level) << target << std::endl;
            } else {
                (*Logger_impl::_all_loggers_streams[this_loggers_stream.first].first) << curr_time
                                                                                      << severity_to_string_logger(
                                                                                              level) << target
                                                                                      << std::endl;
            }
        }
    }
    return this;
}

std::map<std::string, std::pair<std::ofstream *, size_t>> Logger_impl::_all_loggers_streams =
        std::map<std::string, std::pair<std::ofstream *, size_t>>();

logger_builder *logger_builder_impl::with_stream(const std::string &stream, Logger::Severity severity) {
    _under_construction_logger_setup[stream] = severity;
    return this;
}

Logger *logger_builder_impl::build() const {
    return new Logger_impl(_under_construction_logger_setup);
}

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
    Logger *log;
    const rapidjson::Value &streams = doc["streams"];
    rapidjson::Value::ConstValueIterator iter;
    for (iter = streams.Begin(); iter != streams.End(); ++iter) {
        this->with_stream(iter->GetObject()["name"].GetString(),
                          Logger::from_string_to_severity_parse(iter->GetObject()["severity"].GetString()));
    }

    log = this->build();
    config_file_ifs.close();
    return log;
}

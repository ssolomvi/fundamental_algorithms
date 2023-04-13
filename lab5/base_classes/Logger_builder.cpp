#include "Logger_builder.h"

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
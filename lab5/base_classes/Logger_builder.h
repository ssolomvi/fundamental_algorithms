#ifndef LAB5_LOGGER_BUILDER_H
#define LAB5_LOGGER_BUILDER_H

#include "Logger.h"

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



#endif //LAB5_LOGGER_BUILDER_H

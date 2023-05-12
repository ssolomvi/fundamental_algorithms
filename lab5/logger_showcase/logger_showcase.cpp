#include "../base_classes/logger.h"

int main() {

    logger_builder *builder = new logger_builder_impl();
    Logger *log = builder
            ->with_stream("1.txt", Logger::Severity::trace)
            ->with_stream("console", Logger::Severity::debug)
            ->with_stream("2.txt", Logger::Severity::information)
            ->with_stream("3.txt", Logger::Severity::critical)
            ->build();

    Logger *log2;
    logger_builder *builder2 = new logger_builder_impl();
    try {
        log2 = builder2->config_from_json("C:/fund-algths/lab5/logger_showcase/1.json");
    }
    catch (const std::exception& e) {
        std::cerr << "Caught: " << e.what() << std::endl;
        delete log;
        delete builder;
        delete builder2;
        return -1;
    }

    log->log("information kek from logger1", Logger::Severity::information);
    log->log("trace kek from logger1", Logger::Severity::trace);

    log2->log("here is some error info from logger2", Logger::Severity::error);
    log2->log("here is some information about program from logger2", Logger::Severity::information);


    delete log;
    delete builder;
    delete builder2;
    delete log2;
    std::cout << "Program done\n" << std::endl;
    return 0;
}


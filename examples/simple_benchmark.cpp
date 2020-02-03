#include <iostream>
#include <chrono>

#include "clipp.h"
#include "templatedb/db.hpp"
#include "templatedb/operation.hpp"

typedef struct _configuration
{
    std::string fname;
    std::string wlname;
} config;


config cmdline_settings(int argc, char * argv[])
{
    using namespace clipp;
    config cfg;
    auto cli = (
        (required("-f", "--file") % "(input data file)" & value("file", cfg.fname)),
        (required("-w", "--work") % "(input workload file)" & value("workload", cfg.wlname))
    );

    if (!parse(argc, argv, cli)) {
        std::cout << make_man_page(cli, "simple_benchmark");
        exit(EXIT_FAILURE);
    }

    return cfg;
}


int main(int argc, char * argv[])
{
    config cfg = cmdline_settings(argc, argv);

    templatedb::DB db;
    if (!db.load_from_file(cfg.fname))
    {
        fprintf(stderr, "Unable to load DB\n");
        exit(EXIT_FAILURE);
    }

    std::vector<templatedb::Operation> ops = templatedb::Operation::ops_from_file(cfg.wlname);

    auto start = std::chrono::high_resolution_clock::now();
    for (auto op : ops)
    {
        db.execute_op(op);
    }
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start).count();

    printf("Workload Time %d us\n", duration);

    return 0;
}
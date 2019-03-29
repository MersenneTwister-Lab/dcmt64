/**
 * @file options.hpp
 *
 * @brief command line options and keep search options.
 *
 * @author Mutsuo Saito
 * @author Makoto Matsumoto (Hiroshima University)
 * @author Takuji Nishimura (Yamagata University)
 *
 * Copyright (C) 2019 Mutsuo Saito, Makoto Matsumoto,
 * Takuji Nishimura and Hiroshima University.
 * All rights reserved.
 *
 * The MIT License is applied to this software, see
 * LICENSE
 */
#include "options.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <getopt.h>
#include <errno.h>

namespace {
    void output_help(std::string& pgm);
}

/**
 * command line option parser
 * @param opt a structure to keep the result of parsing
 * @param argc number of command line arguments
 * @param argv command line arguments
 * @param start default start value
 * @return command line options have error, or not
 */
bool parse_opt(options& opt, int argc, char **argv) {
    using namespace std;
    opt.verbose = false;
    opt.mexp = 0;
    opt.count = 1;
    opt.seed = 1;
    opt.outfilename = "";
    opt.logfilename = "";
    opt.fixedPOS = -1;
    opt.id = -1;
    opt.seq = -1;
    opt.logcount = -1;
    opt.max_defect = -1;
    int c;
    bool error = false;
    string pgm = argv[0];
    static struct option longopts[] = {
        {"verbose", no_argument, NULL, 'v'},
        {"file", required_argument, NULL, 'f'},
        {"logfile", required_argument, NULL, 'L'},
        {"id", required_argument, NULL, 'I'},
        {"start-seq", required_argument, NULL, 'S'},
        {"count", required_argument, NULL, 'c'},
        {"log-count", required_argument, NULL, 'C'},
        {"seed", required_argument, NULL, 's'},
        {"mexp", required_argument, NULL, 'm'},
        {"fixed-pos", required_argument, NULL, 'X'},
        {"max-defect", required_argument, NULL, 'M'},
        {NULL, 0, NULL, 0}};
    errno = 0;
    for (;;) {
        c = getopt_long(argc, argv, "vs:f:c:C:m:M:X:S:I:", longopts, NULL);
        if (error) {
            break;
        }
        if (c == -1) {
            break;
        }
        switch (c) {
        case 's':
            opt.seed = strtoull(optarg, NULL, 0);
            if (errno) {
                error = true;
                cerr << "seed must be a number" << endl;
            }
            break;
        case 'I':
            opt.id = strtoull(optarg, NULL, 0);
            if (errno) {
                error = true;
                cerr << "id must be a number" << endl;
            }
            break;
        case 'S':
            opt.seq = strtoull(optarg, NULL, 0);
            if (errno) {
                error = true;
                cerr << "start-seq must be a number" << endl;
            }
            break;
        case 'm':
            opt.mexp = strtoull(optarg, NULL, 0);
            if (errno) {
                error = true;
                cerr << "mexp must be a number" << endl;
            }
            break;
        case 'M':
            opt.max_defect = strtoull(optarg, NULL, 0);
            if (errno) {
                error = true;
                cerr << "max_defect must be a number" << endl;
            }
            break;
        case 'X':
            opt.fixedPOS = strtoull(optarg, NULL, 0);
            if (errno) {
                error = true;
                cerr << "fixed pos must be a number" << endl;
            }
            break;
        case 'v':
            opt.verbose = true;
            break;
        case 'f':
            opt.outfilename = optarg;
            break;
        case 'L':
            opt.logfilename = optarg;
            break;
        case 'c':
            opt.count = strtoll(optarg, NULL, 10);
            if (errno) {
                error = true;
                cerr << "count must be a number" << endl;
            }
            break;
        case 'C':
            opt.logcount = strtoll(optarg, NULL, 10);
            if (errno) {
                error = true;
                cerr << "logcount must be a number" << endl;
            }
            break;
        case '?':
        default:
            error = true;
            break;
        }
    }
    if (opt.id < 0 || opt.id >= INT64_C(0x100000000)) {
        cerr << "id must be 0 <= id < 2^32-1" << endl;
        error = true;
    }
    if (opt.logcount <= 0) {
        opt.logcount = opt.mexp / 2;
    }
    if (opt.max_defect < 0) {
        opt.max_defect = opt.mexp * 64;
    }
    if (opt.mexp > 0 && opt.fixedPOS > 0) {
        int size = opt.mexp / 64 + 1;
        if (opt.fixedPOS < 1 || opt.fixedPOS >= size) {
            cerr << "fixed-pos must be 1 <= fixed-pos < "
                 << dec << size << endl;
            error = true;
        }
    }
    if (error) {
        output_help(pgm);
        return false;
    }
    static const int allowed_mexp[] = {521, 607, 1279,
                                       2203, 2281, 3217, 4253,
                                       4423, 9689, 9941, 11213, 19937,
                                       -1};
    bool found = false;
    for (int i = 0; allowed_mexp[i] > 0; i++) {
        if (opt.mexp == allowed_mexp[i]) {
            found = true;
            break;
        }
    }
    if (! found) {
        error = true;
        cerr << "mexp must be one of ";
        for (int i = 0; allowed_mexp[i] > 0; i++) {
            cerr << dec << allowed_mexp[i] << " ";
        }
        cerr << endl;
    }
    if (error) {
        output_help(pgm);
        return false;
    }
    return true;
}

namespace {
/**
 * showing help message
 * @param pgm program name
 */
    void output_help(std::string& pgm) {
        using namespace std;
        cerr << "usage:" << endl;
        cerr << pgm
             << " -m mexp"
             << " -I id"
             << " [-s seed] [-v] [-c count]"
             << " [-f outputfile]"
             << " [-l logfile]"
             << " [-S start_seq]"
             << " [-C log_count]"
             << " [-F fixed_pos]"
             << " [-M max_defect]"
             << endl;
        static string help_string1 = "\n"
            "--mexp, -m mexp      mersenne exponent.\n"
            "--id, -I id          start id. The first id.\n"
            "--seed, -s seed      seed of randomness.\n"
            "--verbose, -v        Verbose mode. Output parameters, calculation time, etc.\n"
            "--count, -c count    Output count. The number of parameters to be outputted.\n"
            "--file, -f filename  Parameters are outputted to this file. without this\n"
            "--logfile, -f log    Logs are outputted to this file. without this\n"
            "                     option, parameters are outputted to standard output.\n"
            "--start-seq, -S seq  start seq. seq will be count ***down***.\n"
            "--log-count count    log output interval.\n"
            "--fixed-pos          fix the parameter pos to given value.\n"
            "--max-defect max     total dimensiton defect larger than max will be skipped.\n"
            ;
        cerr << help_string1 << endl;
    }
}

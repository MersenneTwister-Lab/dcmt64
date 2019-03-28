#pragma once
#ifndef OPTIONS_HPP
#define OPTIONS_HPP
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
#include <stdint.h>
#include <inttypes.h>
//#include <time.h>
#include <string>
//#include <sstream>
#include <getopt.h>
#include <errno.h>

class options {
public:
    int mexp;                   // mersenne exponent (required)
    int id;                     // id (required)
    bool verbose;               // verbose mode (optional)
    long seq;                   // start seq no (optional) -1 means use default
    int fixedPOS;               // fix pos parameter -1 means not fix
    uint64_t seed;              // if pos is fixed not used
    std::string outfilename;    // parameter output file
    std::string logfilename;    // log output file
    long count;                 // number of parameters you want to get
};

namespace {

    bool parse_opt(options& opt, int argc, char **argv);
    void output_help(std::string& pgm);

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
            {"seed", required_argument, NULL, 's'},
            {"mexp", required_argument, NULL, 'm'},
            {"fixed-pos", required_argument, NULL, 'X'},
            {NULL, 0, NULL, 0}};
        errno = 0;
        for (;;) {
            c = getopt_long(argc, argv, "vs:f:c:m:X:S:I:", longopts, NULL);
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
            case '?':
            default:
                error = true;
                break;
            }
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

/**
 * showing help message
 * @param pgm program name
 */
    void output_help(std::string& pgm) {
        using namespace std;
        cerr << "usage:" << endl;
        cerr << pgm
             << " [-s seed] [-v] [-c count]"
             << " [-f outputfile]"
             << " [-l logfile]"
             << " [-S start_seq]"
             << " -I id"
             << " --mexp mexp"
             << endl;
        static string help_string1 = "\n"
            "--verbose, -v        Verbose mode. Output parameters, calculation time, etc.\n"
            "--file, -f filename  Parameters are outputted to this file. without this\n"
            "                     option, parameters are outputted to standard output.\n"
            "--count, -c count    Output count. The number of parameters to be outputted.\n"
            "--id, -I id          start id. The first id.\n"
            "--start-seq, -S id   start seq. seq will be count ***down***.\n"
            "--seed, -s seed      seed of randomness.\n"
            "--fixed-pos          fix the parameter pos1 to given value.\n"
            "--mexp mexp          mersenne exponent.\n"
            ;
        cerr << help_string1 << endl;
    }
}
#endif // OPTIONS_HPP

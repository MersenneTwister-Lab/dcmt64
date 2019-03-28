/**
 * @file dcmt64.cpp
 *
 * @brief The main function of parameter generator of 64 bit Mersenne Twister.
 *
 * The functions in this file are simple. They parse the command line
 * options and call all_in_one function which does almost all things.
 * Users can change this file so that it fits to their applications
 * and OS.
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
#include <time.h>
#include <fcntl.h>
#include <unistd.h>
#include <string>
#include <sstream>
#include <fstream>
#include <MTToolBox/AlgorithmRecursionSearch.hpp>
#include <MTToolBox/AlgorithmEquidistribution.hpp>
#include <MTToolBox/MersenneTwister.hpp>
#include <MTToolBox/MersenneTwister.hpp>
#include <NTL/GF2X.h>
#include <getopt.h>
#include "mt64Search.hpp"
#include "search.h"
#include "options.h"

using namespace std;
using namespace MTToolBox;
using namespace NTL;

/**
 * parse command line option, and search parameters
 * @param argc number of arguments
 * @param argv value of arguments
 * @return 0 if this ends normally
 */
int main(int argc, char** argv) {
    options opt;
    bool parse = parse_opt(opt, argc, argv);
    if (!parse) {
        return -1;
    }
    ofstream ofs;
    ofstream log;
    ostream *os;
    ostream *ls;
    if (!opt.outfilename.empty()) {
        ofs.open(opt.outfilename.c_str());
        if (!ofs) {
            cerr << "can't open file:" << opt.outfilename << endl;
            return -1;
        }
        os = &ofs;
    } else {
        os = &cout;
    }
    if (!opt.logfilename.empty()) {
        log.open(opt.logfilename.c_str());
        if (!log) {
            cerr << "can't open file:" << opt.logfilename << endl;
            return -1;
        }
        ls = &log;
    } else {
        ls = os;
    }
    return search(opt, *os, *ls, opt.count);
}

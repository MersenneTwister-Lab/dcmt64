/**
 * @file dcmt64mpi.cpp
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
#include "mpicontrol.hpp"
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
    MPIControl mpi(&argc, &argv);
    options opt;
    bool parse = parse_opt(opt, argc, argv);
    if (!parse) {
        return -1;
    }
    // MPI
    char buff[200];
    opt.id = opt.id + mpi.getRank();
    //if (opt.mexp == 19937) {
    //    opt.fixedPOS = 156;
    //    opt.max_defect = 8000; // mt19937-64 is 7820
    //}
    //opt.seq; count down, -1 means uint32_t max
    //opt.count;
    //opt.logcount;
    if (!opt.outfilename.empty()) {
        sprintf(buff, ".s%04ld-%03d.txt", opt.seed, mpi.getRank());
        opt.outfilename += buff;
    }
    if (!opt.logfilename.empty()) {
        sprintf(buff, ".s%04ld-%03d.log", opt.seed, mpi.getRank());
        opt.logfilename += buff;
    }
    // MPI end
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
    //return best_search(opt, *os, *ls, opt.count); bug? or too slow?
}

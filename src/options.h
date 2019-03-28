#pragma once
#ifndef OPTIONS_H
#define OPTIONS_H
/**
 * @file options.h
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
#include <string>

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
    long logcount;              // count for log output
};

bool parse_opt(options& opt, int argc, char **argv);
#endif // OPTIONS_H

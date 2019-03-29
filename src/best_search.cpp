/**
 * @file best_search.cpp
 *
 * @brief search function of parameter generator of 64 bit Mersenne Twister.
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
#include <iostream>
#include <iomanip>
#include <time.h>
#include <string>
//#include <sstream>
//#include <MTToolBox/AlgorithmRecursionAndTempering.hpp>
#include <MTToolBox/AlgorithmBestBits.hpp>
#include <MTToolBox/AlgorithmRecursionSearch.hpp>
#include <MTToolBox/AlgorithmEquidistribution.hpp>
#include <MTToolBox/AlgorithmPartialBitPattern.hpp>
//#include <MTToolBox/MersenneTwister.hpp>
#include "MixedSequence.hpp"
//#include <NTL/GF2X.h>
//#include "options.hpp"
#include "mt64Search.hpp"
#include "search.h"

using namespace std;
using namespace MTToolBox;
using namespace NTL;

namespace {
    int best_search_main(options& opt, ostream& os, ostream& log, int count);
}

/**
 * search parameters using all_in_one function in the file search_all.hpp
 * @param opt command line options
 * @param count number of parameters user requested
 * @return 0 if this ends normally
 */
int best_search(options& opt, ostream& os, ostream& log, int count) {
    try {
        return best_search_main(opt, os, log, count);
    } catch (underflow_error &e) {
        log << "# search end: sequence has wasted out." << endl;
        return 0;
    }
}

namespace {
    int best_search_main(options& opt, ostream& os, ostream& log, int count) {
        uint32_t seq = 0;
        seq = ~seq;
        if (opt.seq > 0) {
            seq = opt.seq;
        }
        MixedSequence mx(seq, opt.seed, 0);
        mt64 g(opt.mexp, opt.id);
        static const int shifts[] = {17, 37};
        //limit_v 何ビットテンパリングするか とりあえず 15のまま
        AlgorithmBestBits<uint64_t> besttmp(64, shifts, 2, 15);
        if (opt.verbose) {
            time_t t = time(NULL);
            log << "#search start id = " << opt.id << " at " << ctime(&t) << endl;
            log << "#seed = " << dec << opt.seed
                << ", seq = " << seq << endl;
        }
        if (opt.fixedPOS > 0) {
            g.setFixedPOS(opt.fixedPOS);
        }
        g.setTmpIdx(-1);
        AlgorithmRecursionSearch<uint64_t> ars(g, mx);
        long cnt = 0;
        os << "# " << g.getHeaderString() << ", delta"
           << endl;
        while (cnt < count) {
            if (ars.start(opt.logcount)) {
                log << "# search found: " << dec << g.getID()
                    << ", " << g.getSEQ()
                    << "; tempering search start..." << endl;
                besttmp(g, false);
                AlgorithmEquidistribution<uint64_t> equi(g, 64, opt.mexp);
                int veq[64];
                int delta = equi.get_all_equidist(veq);
                if (delta > opt.max_defect) {
                    log << "# search skipped: " << dec << g.getID()
                        << ", " << g.getSEQ()
                        << "; dd = " << delta << endl;
                    continue;
                }
                os << g.getParamString();
                os << "," << dec << delta << endl;
#if defined(DEBUG)
                for (int j = 0; j < 64; j++) {
                    cout << "k(" << dec << (j + 1) << ") = " << dec << veq[j];
                    cout << "\td(" << dec << (j + 1) << ") = " << dec
                         << (opt.mexp / (j + 1) - veq[j]) << endl;
                }
#endif
                cnt++;
            } else {
                log << "# search not found: " << dec << g.getID()
                    << ", " << g.getSEQ() << endl;
            }
        }
        if (opt.verbose) {
            time_t t = time(NULL);
            log << "search end at " << ctime(&t) << endl;
        }
        return 0;
    }
}

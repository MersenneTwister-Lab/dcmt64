#include "mt64Search.hpp"
#include "mt64Rec1Search.hpp"
#include "mt64Rec2Search.hpp"
#include <MTToolBox/AlgorithmEquidistribution.hpp>
#include <MTToolBox/AlgorithmReducibleRecursionSearch.hpp>
#include <MTToolBox/period.hpp>
#include <NTL/GF2X.h>
#include <errno.h>
#include <stdlib.h>
#include <getopt.h>

using namespace MTToolBox;
using namespace std;

class options {
public:
    bool verbose;
    bool reverse;
    bool period;
    int rectype;
    uint64_t seed;
    mt64_param params;
    mt64rec2_param paramsrec2;
    mt64rec1_param paramsrec1;
};

namespace {
    bool parse_opt(options& opt, int argc, char **argv);
    void output_help(string& pgm);
    template<class G>
    void search_tempering(G& mt, options& opt, int mexp, bool verbose);
    template<class G>
    bool check_period(G& mt);
}


int main(int argc, char * argv[])
{
    options opt;
    if (!parse_opt(opt, argc, argv)) {
        return -1;
    }
    if (opt.rectype == 1) {
        mt64rec1 mt(opt.paramsrec1);
        if (opt.verbose) {
            cout << opt.paramsrec1.get_debug_string() << endl;
        }
        search_tempering(mt, opt, opt.paramsrec1.mexp, opt.verbose);
    } else if (opt.rectype == 2) {
        mt64rec2 mt(opt.paramsrec2);
        if (opt.verbose) {
            cout << opt.paramsrec2.get_debug_string() << endl;
        }
        search_tempering(mt, opt, opt.paramsrec2.mexp, opt.verbose);
    } else if (opt.rectype == 3) {
        mt64 mt(opt.params);
        if (opt.verbose) {
            cout << opt.params.get_debug_string() << endl;
        }
        search_tempering(mt, opt, opt.params.mexp, opt.verbose);
    }
    return 0;
}

namespace {
    /**
     * command line option parser
     * @param opt a structure to keep the result of parsing
     * @param argc number of command line arguments
     * @param argv command line arguments
     * @param start default start value
     * @return command line options have error, or not
     */
    bool parse_opt(options& opt, int argc, char **argv) {
        opt.verbose = false;
        opt.period = false;
        opt.rectype = 3;
        opt.seed = 0;
        int c;
        bool error = false;
        string pgm = argv[0];
        static struct option longopts[] = {
            {"verbose", no_argument, NULL, 'v'},
            {"period", no_argument, NULL, 'p'},
            {"rectype", required_argument, NULL, 'r'},
            {"seed", required_argument, NULL, 's'},
            {NULL, 0, NULL, 0}};
        errno = 0;
        for (;;) {
            c = getopt_long(argc, argv, "vps:r:", longopts, NULL);
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
            case 'r':
                opt.rectype = strtoull(optarg, NULL, 0);
                if (errno || opt.rectype < 1 || opt.rectype > 3) {
                    error = true;
                    cerr << "rectype must be 1, 2, 3" << endl;
                }
                break;
            case 'v':
                opt.verbose = true;
                break;
            case 'p':
                opt.period = true;
                break;
            case '?':
            default:
                error = true;
                break;
            }
        }
        argc -= optind;
        argv += optind;
        if (argc < 1) {
            error = true;
        } else if (opt.rectype == 1) {
            char * para = argv[0];
            opt.paramsrec1.mexp = strtoul(para, &para, 10);
            para++;
            opt.paramsrec1.id = strtoul(para, &para, 10);
            para++;
            opt.paramsrec1.pos1 = strtoul(para, &para, 10);
            para++;
            opt.paramsrec1.mat = strtoull(para, &para, 16);
            para++;
            opt.paramsrec1.tsh0 = strtoul(para, &para, 10);
            para++;
            opt.paramsrec1.tsh1 = strtoul(para, &para, 10);
            para++;
            opt.paramsrec1.tsh2 = strtoul(para, &para, 10);
            para++;
            opt.paramsrec1.tsh3 = strtoul(para, &para, 10);
            para++;
            opt.paramsrec1.tmsk0 = strtoull(para, &para, 16);
            para++;
            opt.paramsrec1.tmsk1 = strtoull(para, &para, 16);
            para++;
            opt.paramsrec1.tmsk2 = strtoull(para, &para, 16);
        } else if (opt.rectype == 2) {
            char * para = argv[0];
            opt.paramsrec2.mexp = strtoul(para, &para, 10);
            para++;
            opt.paramsrec2.id = strtoul(para, &para, 10);
            para++;
            opt.paramsrec2.pos1 = strtoul(para, &para, 10);
            para++;
            opt.paramsrec2.pos2 = strtoul(para, &para, 10);
            para++;
            opt.paramsrec2.pos3 = strtoul(para, &para, 10);
            para++;
            opt.paramsrec2.mat = strtoull(para, &para, 16);
            para++;
            opt.paramsrec2.tsh0 = strtoul(para, &para, 10);
            para++;
            opt.paramsrec2.tsh1 = strtoul(para, &para, 10);
            para++;
            opt.paramsrec2.tsh2 = strtoul(para, &para, 10);
            para++;
            opt.paramsrec2.tsh3 = strtoul(para, &para, 10);
            para++;
            opt.paramsrec2.tmsk1 = strtoull(para, &para, 16);
            para++;
            opt.paramsrec2.tmsk2 = strtoull(para, &para, 16);
        } else if (opt.rectype == 3) {
            char * para = argv[0];
            opt.params.mexp = strtoul(para, &para, 10);
            para++;
            opt.params.id = strtoul(para, &para, 10);
            para++;
            opt.params.pos = strtoul(para, &para, 10);
            para++;
            opt.params.mat = strtoull(para, &para, 16);
            para++;
            opt.params.tmsk1 = strtoull(para, &para, 16);
            para++;
            opt.params.tmsk2 = strtoull(para, &para, 16);
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
    void output_help(string& pgm)
    {
        cerr << "usage:" << endl;
        cerr << pgm
             << " [-v] [-s seed] [-p] [-r rectype] params" << endl;
        cerr << " params in rectype 1 :\n"
             << "mexp,id,pos1,mat,tsh0,tsh1,tsh2,tsh3,tmsk0,tmsk1,tmsk2"
             << endl;
        cerr << " params in rectype 2 :\n"
             << " mexp,id,pos1,pos2,pos3,mat,tsh0,tsh1,tsh2,tsh3,tmsk1,tmsk2"
             << endl;
        cerr << " params in rectype 3 :\n"
             << " mexp,id,pos,mat,tmsk1,tmsk2" << endl;

        static string help_string1 = "\n"
            "--verbose, -v        Verbose mode. Output detailed information.\n"
            "--period, -p         period chek only.\n"
            "--seed, -s seed      seed for generation.\n"
            ;
        cerr << help_string1 << endl;
    }

    template<class G>
    bool check_period(G& mt)
    {
        GF2X poly;
        minpoly<uint64_t>(poly, mt);
        cout << "deg(poly) = " << dec << deg(poly) << endl;
        if (deg(poly) != mt.getMexp()) {
            cout << "deg(poly) is not mexp. NG." << endl;
            return false;
        }
        if (isPrime(poly)) {
            cout << "poly is prime. OK." << endl;
            return true;
        } else {
            cout << "poly is not prime. NG." << endl;
            return false;
        }
    }

    template<class G>
    void search_tempering(G& mt, options& opt, int mexp, bool verbose)
    {
        // 64 - 17 = 47
        // 64 - 37 = 27
        typedef AlgorithmPartialBitPattern<uint64_t, 64, 1, 47, 5> stsl1;
        typedef AlgorithmPartialBitPattern<uint64_t, 64, 1, 27, 5> stsl2;

        stsl1 apbp1;
        stsl2 apbp2;
        mt.setTmpIdx(0);
        apbp1(mt, false);
        mt.setTmpIdx(1);
        apbp2(mt, false);
        AlgorithmEquidistribution<uint64_t> equi(mt, 64, opt.mexp);
        int veq[64];
        int delta = equi.get_all_equidist(veq);
        os << g.getParamString();
        os << "," << dec << delta << endl;
        for (int j = 0; j < 64; j++) {
            cout << "k(" << dec << (j + 1) << ") = " << dec << veq[j];
            cout << "\td(" << dec << (j + 1) << ") = " << dec
                 << (opt.mexp / (j + 1) - veq[j]) << endl;
        }
    }

}

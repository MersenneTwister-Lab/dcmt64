#include "mt64Search.hpp"
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
    uint64_t seed;
    mt64_param params;
};

namespace {
    bool parse_opt(options& opt, int argc, char **argv);
    void output_help(string& pgm);
}

int main(int argc, char * argv[])
{
    options opt;
    if (!parse_opt(opt, argc, argv)) {
        return -1;
    }
    mt64 mt(opt.params);
    mt.seed(opt.seed);
    int delta = 0;
    int veq[64];
    AlgorithmEquidistribution<uint64_t> equi(mt, 64, opt.params.mexp);
    delta = equi.get_all_equidist(veq);
    cout << mt.getParamString();
    cout << dec << delta << endl;
    if (opt.verbose) {
        cout << "64bit dimension of equidistribution at v-bit accuracy k(v)"
             << endl;
        for (int j = 0; j < 64; j++) {
            cout << "k(" << dec << (j + 1) << ") = " << dec << veq[j];
            cout << "\td(" << dec << (j + 1) << ") = " << dec
                 << (opt.params.mexp / (j + 1) - veq[j]) << endl;
        }
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
        opt.seed = 0;
        int c;
        bool error = false;
        string pgm = argv[0];
        static struct option longopts[] = {
            {"verbose", no_argument, NULL, 'v'},
            {"seed", required_argument, NULL, 's'},
            {NULL, 0, NULL, 0}};
        errno = 0;
        for (;;) {
            c = getopt_long(argc, argv, "vs:", longopts, NULL);
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
            case 'v':
                opt.verbose = true;
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
        } else {
            char * para = argv[0];
            opt.params.mexp = strtoul(para, &para, 10);
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
             << " [-v] [-r]"
             << " mexp,pos,mat,tmsk1,tmsk2"
             << endl;
        static string help_string1 = "\n"
            "--verbose, -v        Verbose mode. Output detailed information.\n"
            ;
        cerr << help_string1 << endl;
    }
}

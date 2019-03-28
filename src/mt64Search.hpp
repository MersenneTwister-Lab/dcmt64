#ifndef MT64SEARCH_HPP
#define MT64SEARCH_HPP
/**
 * @file mt64Search.hpp
 *
 * @brief 64 bit Mersenne Twister
 * this class is used by dcmt64.
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

#include <iostream>
#include <iomanip>
#include <cstdlib>
#include <sstream>
#include <MTToolBox/ReducibleGenerator.hpp>
#include <MTToolBox/MersenneTwister64.hpp>
#include <MTToolBox/AlgorithmPartialBitPattern.hpp>
#include <MTToolBox/util.hpp>

namespace MTToolBox {
    using namespace NTL;
    using namespace std;

    typedef AlgorithmPartialBitPattern<uint64_t, 64, 1, 47, 5> stsl1;
    typedef AlgorithmPartialBitPattern<uint64_t, 64, 1, 27, 5> stsl2;
    /**
     * @class mt64_param
     * @brief a class keeping parameters of mt64
     *
     * This class keeps parameters of mt64, and has some
     * method for outputting parameters.
     */
    class mt64_param {
    public:
        int mexp;
        int pos;
        uint32_t id;
        uint32_t seq;
        uint64_t mat;
        uint64_t tmsk1;
        uint64_t tmsk2;

        mt64_param() {
            mexp = 0;
            id = 0;
            seq = 0;
            pos = 0;
            mat = 0;
            tmsk1 = 0;
            tmsk2 = 0;
        }

        mt64_param(const mt64_param& src) {
            mexp = src.mexp;
            id = src.id;
            seq = src.seq;
            pos = src.pos;
            mat = src.mat;
            tmsk1 = src.tmsk1;
            tmsk2 = src.tmsk2;
        }
        /**
         * This method is used in output.hpp.
         * @return header line of output.
         */
        const string get_header() const {
            return "mexp, id, pos, mat, tmsk1, tmsk2";
        }

        /**
         * This method is used in output.hpp.
         * @return string of parameters
         */
        const string get_string() const {
            stringstream ss;
            ss << dec << mexp << ",";
            ss << dec << id << ",";
            ss << dec << pos << ",";
            ss << hex << setw(16) << setfill('0') << mat << ",";
            ss << hex << setw(16) << setfill('0') << tmsk1 << ",";
            ss << hex << setw(16) << setfill('0') << tmsk2;
            string s;
            ss >> s;
            return s;
        }

        /**
         * This method is used for DEBUG.
         * @return string of parameters.
         */
        const string get_debug_string() const {
            stringstream ss;
            ss << "mexp:" << dec << mexp << endl;
            ss << "id:" << dec << id << endl;
            ss << "pos:" << dec << pos << endl;
            ss << "mat:" << hex << setw(16) << setfill('0') << mat << endl;
            ss << "tmsk1:" << hex << setw(16) << setfill('0') << tmsk1 << endl;
            ss << "tmsk2:" << hex << setw(16) << setfill('0') << tmsk2 << endl;
            string s;
            ss >> s;
            return s;
        }
    };

    /**
     * @class mt64
     * @brief DSFMT generator class used for dynamic creation
     *
     * This class is one of the main class of DSFMT dynamic creator.
     * This class is designed to be called from programs in MTToolBox,
     * but is not a subclass of some abstract class.
     * Instead, this class is passed to them as template parameters.
     */
    class mt64 : public TemperingCalculatable<uint64_t> {
    public:
        /**
         * Constructor by mexp.
         * @param mexp Mersenne Exponent
         */
        mt64(int mexp, int id) {
            size = mexp / 64 + 1;
            state = new uint64_t[size];
            param.mexp = mexp;
            param.id = id;
            param.pos = 0;
            param.mat = 0;
            param.tmsk1 = 0;
            param.tmsk2 = 0;
            index = 0;
            fixedPOS = -1;
            reverse_bit_flag = false;
            make_mask(mexp);
        }

        ~mt64() {
            delete[] state;
        }

        /**
         * The copy constructor.
         * @param src The origin of copy.
         */
        mt64(const mt64& src) : param(src.param) {
            size = src.size;
            state = new uint64_t[size];
            for (int i = 0; i < size; i++) {
                state[i] = src.state[i];
            }
            index = src.index;
            fixedPOS = src.fixedPOS;
            reverse_bit_flag = src.reverse_bit_flag;
            lower_mask = src.lower_mask;
            upper_mask = src.upper_mask;
        }

        /**
         * Constructor by parameter.
         * @param src_param
         */
        mt64(const mt64_param& src_param) :
            TemperingCalculatable<uint64_t>(), param(src_param) {
            size = src_param.mexp / 64 + 1;
            state = new uint64_t[size];
            for (int i = 0; i < size; i++) {
                state[i] = 0;
            }
            index = 0;
            fixedPOS = -1;
            reverse_bit_flag = false;
            make_mask(src_param.mexp);
        }

        mt64 * clone() const {
            return new mt64(*this);
        }

        /**
         * This method initialize internal state.
         * @param seed seed for initialization
         */
        void seed(uint64_t seed) {
            state[0] = seed;
            for (int i = 1; i < size; i++) {
                state[i] = UINT64_C(6364136223846793005)
                    * (state[i-1] ^ (state[i-1] >> 62)) + i;
            }
            index = size - 1;
        }

        /**
         * Important state transition function.
         */
        void next_state() {
            index = (index + 1) % size;
            uint64_t x = (state[index] & upper_mask)
                | (state[(index + 1) % size] & lower_mask);
            state[index] = state[(index + param.pos) % size] ^ (x >> 1);
            if (x & 1) {
                state[index] ^= param.mat;
            }
        }

        /**
         * Tempering
         */
        uint64_t temper() {
            uint64_t x = state[index];
            x ^= (x >> 29) & tmsk0;
            x ^= (x << tsl1) & param.tmsk1;
            x ^= (x << tsl2) & param.tmsk2;
            x ^= (x >> 43);
            return x;
        }
        /**
         * Important method, generate new random number
         * @return new pseudo random number
         */
        uint64_t generate() {
            next_state();
            return temper();
        }

        /**
         * This method is called by the functions in simple_shortest_basis.hpp
         * This method returns \b bit_len bits of MSB of generated numbers
         * If reverse_bit_flag is set, bits are taken from LSB
         * @param bit_len bit length from MSB or LSB
         * @return generated numbers of bit_len
         */
        uint64_t generate(int bit_len) {
            uint64_t w;
            if (reverse_bit_flag) {
                w = reverse_bit(generate());
            } else {
                w = generate();
            }
            w = generate();
            uint64_t mask = 0;
            mask = ~mask;
            mask = mask << (64 - bit_len);
            return w  & mask;
        }

        /**
         * make parameters from given sequential number and
         * internal id
         * @param num sequential number
         */
        void setUpParam(ParameterGenerator& mt) {
            if (fixedPOS > 0) {
                param.pos = fixedPOS;
            } else {
                param.pos = mt.getUint32() % (size - 1) + 1;
            }
            uint32_t work = mt.getUint32();
            uint32_t wmat1 = (work & 0xffff0000) | (param.id & 0xffff);
            uint32_t wmat2 = (work & 0xffff) | (param.id & 0xffff0000);
            wmat1 ^= wmat1 >> 19;
            wmat2 ^= wmat2 << 18;
            param.mat = wmat1;
            param.mat = param.mat << 32;
            param.mat = param.mat | wmat2;
            param.tmsk1 = 0;
            param.tmsk2 = 0;
        }

        void setZero() {
            for (int i = 0; i < size; i++) {
                state[i] = 0;
            }
            index = 0;
        }

        /**
         * This method is called by the functions in the file
         * simple_shortest_basis.hpp
         * @return true if all elements of state is zero
         */
        bool isZero() const {
            for (int i = 0; i < size; i++) {
                if (state[i] != 0) {
                    return false;
                }
            }
            return true;
        }

        /**
         * This method is called by functions in the file
         * simple_shortest_basis.hpp addition of internal state as
         * GF(2) vector is possible when state transition function and
         * output function is GF(2)-linear.
         * @param that DSFMT generator added to this generator
         */
        void add(EquidistributionCalculatable<uint64_t>& other) {
            mt64 *that = dynamic_cast<mt64 *>(&other);
            if (that == 0) {
                throw std::invalid_argument(
                    "the adder should have same type as the addee.");
            }
            this->add(that);
        }

        void add(const mt64 * that) {
            for (int i = 0; i < size; i++) {
                state[(i + index) % size]
                    ^= that->state[(i + that->index) % size];
            }
        }

        int getMexp() const {
            return param.mexp;
        }

        int bitSize() const {
            return param.mexp;
        }

        const std::string getHeaderString() {
            return param.get_header();
        }

        const std::string getParamString() {
            return param.get_string();
        }

        /**
         * This method is called by the functions in search_temper.hpp
         * to calculate the equidistribution properties from LSB
         */
        void set_reverse_bit() {
            reverse_bit_flag = true;
        }

        /**
         * This method is called by the functions in search_temper.hpp
         * to reset the reverse_bit_flag
         */
        void reset_reverse_bit() {
            reverse_bit_flag = false;
        }

        bool equals(const mt64& that) {
            for (int i = 0; i < size; i++) {
                if (state[(index + i) % size]
                    != that.state[(that.index + i) % size]) {
                    return false;
                }
            }
            return true;
        }

        void d_p() {
            cout << "index = " << dec << index << endl;
            for (int i = 0; i < size; i++) {
                cout << setfill('0') << setw(16) << hex << state[i] << endl;
            }
            cout << endl;
        }

        void setFixedPOS(int value) {
            fixedPOS = value;
        }

        /**
         * This method is called by functions in the file search_temper.hpp
         * Do not remove this.
         */
        void setUpTempering() {
        }

        /**
         * This method is called by functions in the file search_temper.hpp
         * @param mask available bits of pattern
         * @param pattern bit pattern
         * @param src_bit only 0 is allowed
         */
        void setTemperingPattern(uint64_t mask, uint64_t pattern, int src_bit) {
            if (src_bit == 0) {
                param.tmsk1 &= ~mask;
                param.tmsk1 |= pattern & mask;
            } else {
                param.tmsk2 &= ~mask;
                param.tmsk2 |= pattern & mask;
            }
        }
        void setReverseOutput() {
            reverse_bit_flag = true;
        }
        void resetReverseOutput() {
            reverse_bit_flag = false;
        }
        bool isReverseOutput() {
            return reverse_bit_flag;
        }
        int getID() {
            return param.id;
        }
        int getSEQ() {
            return param.seq;
        }
    private:
        void make_mask(int mexp) {
            int bit = mexp % 64;
            lower_mask = 0;
            lower_mask = ~lower_mask;
            lower_mask = lower_mask >> bit;
            upper_mask = ~lower_mask;
        }
        mt64& operator=(const mt64&) {
            throw std::logic_error("can't assign");
        }
        static const uint64_t tmsk0 = UINT64_C(0x5555555555555555);
        enum {tsl1 = 17, tsl2 = 37};
        int fixedPOS;
        int size;
        int index;
        uint64_t * state;
        mt64_param param;
        uint64_t upper_mask;
        uint64_t lower_mask;
        bool reverse_bit_flag;
    };
}

#endif // MT64SEARCH_HPP

#ifndef MT64REC2SEARCH_HPP
#define MT64REC2SEARCH_HPP
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
#include <MTToolBox/TemperingCalculatable.hpp>
#include <MTToolBox/util.hpp>

namespace MTToolBox {
    using namespace NTL;
    using namespace std;

    /**
     * @class mt64_param
     * @brief a class keeping parameters of mt64
     *
     * This class keeps parameters of mt64, and has some
     * method for outputting parameters.
     */
    class mt64rec2_param {
    public:
        int mexp;
        int pos1;
        int pos2;
        int pos3;
        int tsh0;
        int tsh1;
        int tsh2;
        int tsh3;
        uint32_t id;
        uint32_t seq;
        uint64_t mat;
        uint64_t tmsk0;
        uint64_t tmsk1;
        uint64_t tmsk2;
        static const uint64_t tmsk0ff = UINT64_C(0xffffffffffffffff);
        static const uint64_t tmsk055 = UINT64_C(0x5555555555555555);

        mt64rec2_param() {
            mexp = 0;
            id = 0;
            seq = 0;
            pos1 = 0;
            pos2 = 0;
            pos3 = 0;
            mat = 0;
            // rec 2
            tmsk0 = tmsk0ff;
            tmsk1 = 0;
            tmsk2 = 0;
            // rec 2
            tsh0 = 26;
            tsh1 = 17;
            tsh2 = 33;
            tsh3 = 39;
        }

        mt64rec2_param(const mt64rec2_param& src) {
            mexp = src.mexp;
            id = src.id;
            seq = src.seq;
            pos1 = src.pos1;
            pos2 = src.pos2;
            pos2 = src.pos2;
            mat = src.mat;
            tmsk0 = src.tmsk0;
            tmsk1 = src.tmsk1;
            tmsk2 = src.tmsk2;
            tsh0 = src.tsh0;
            tsh1 = src.tsh1;
            tsh2 = src.tsh2;
            tsh3 = src.tsh3;
        }
        /**
         * This method is used in output.hpp.
         * @return header line of output.
         */
        const string get_header() const {
            return "mexp, id, pos1, pos2, pos3, mat, tsh0, tsh1, tsh2, tsh3,"
                " tmsk0, tmsk1, tmsk2";
        }

        /**
         * This method is used in output.hpp.
         * @return string of parameters
         */
        const string get_string() const {
            stringstream ss;
            ss << dec << mexp << ",";
            ss << dec << id << ",";
            ss << dec << pos1 << ",";
            ss << dec << pos2 << ",";
            ss << dec << pos3 << ",";
            ss << hex << setw(16) << setfill('0') << mat << ",";
            ss << dec << setw(0) << tsh0 << ",";
            ss << dec << setw(0) << tsh1 << ",";
            ss << dec << setw(0) << tsh2 << ",";
            ss << dec << setw(0) << tsh3 << ",";
            ss << hex << setw(16) << setfill('0') << tmsk0 << ",";
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
            ss << "pos1:" << dec << pos1 << endl;
            ss << "pos2:" << dec << pos2 << endl;
            ss << "pos3:" << dec << pos3 << endl;
            ss << "mat:" << hex << setw(16) << setfill('0') << mat << endl;
            ss << "tsh0:" << dec << setw(0) << tsh0 << endl;
            ss << "tsh1:" << dec << setw(0) << tsh1 << endl;
            ss << "tsh2:" << dec << setw(0) << tsh2 << endl;
            ss << "tsh3:" << dec << setw(0) << tsh3 << endl;
            ss << "tmsk0:" << hex << setw(16) << setfill('0') << tmsk0 << endl;
            ss << "tmsk1:" << hex << setw(16) << setfill('0') << tmsk1 << endl;
            ss << "tmsk2:" << hex << setw(16) << setfill('0') << tmsk2 << endl;
            string s;
            ss >> s;
            return s;
        }
    };

    /**
     * @class mt64rec2
     * @brief DSFMT generator class used for dynamic creation
     *
     * This class is one of the main class of DSFMT dynamic creator.
     * This class is designed to be called from programs in MTToolBox,
     * but is not a subclass of some abstract class.
     * Instead, this class is passed to them as template parameters.
     */
    class mt64rec2 : public TemperingCalculatable<uint64_t> {
    public:
        /**
         * Constructor by mexp.
         * @param mexp Mersenne Exponent
         */
        mt64rec2(int mexp, int id) {
            size = mexp / 64 + 1;
            state = new uint64_t[size];
            param.mexp = mexp;
            param.id = id;
            //param.pos = 0;
            //param.mat = 0;
            //param.tmsk1 = 0;
            //param.tmsk2 = 0;
            index = 0;
            //fixedPOS = -1;
            reverse_bit_flag = false;
            make_mask(mexp);
        }

        ~mt64rec2() {
            delete[] state;
        }

        /**
         * The copy constructor.
         * @param src The origin of copy.
         */
        mt64rec2(const mt64rec2& src) : param(src.param) {
            size = src.size;
            state = new uint64_t[size];
            for (int i = 0; i < size; i++) {
                state[i] = src.state[i];
            }
            index = src.index;
            //fixedPOS = src.fixedPOS;
            reverse_bit_flag = src.reverse_bit_flag;
            lower_mask = src.lower_mask;
            upper_mask = src.upper_mask;
        }

        /**
         * Constructor by parameter.
         * @param src_param
         */
        mt64rec2(const mt64rec2_param& src_param) :
            TemperingCalculatable<uint64_t>(), param(src_param) {
            size = src_param.mexp / 64 + 1;
            state = new uint64_t[size];
            for (int i = 0; i < size; i++) {
                state[i] = 0;
            }
            index = 0;
            //fixedPOS = -1;
            reverse_bit_flag = false;
            make_mask(src_param.mexp);
        }

        mt64rec2 * clone() const {
            return new mt64rec2(*this);
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
            state[index] = state[(index + param.pos1) % size]
                ^ state[(index + param.pos2) % size]
                ^ state[(index + param.pos3) % size]
                ^ (x >> 1);
            if (x & 1) {
                state[index] ^= param.mat;
            }
        }

        /**
         * Tempering
         */
        uint64_t temper() {
            uint64_t x = state[index];
            x ^= (x >> param.tsh0) & param.tmsk0;
            x ^= (x << param.tsh1) & param.tmsk1;
            x ^= (x << param.tsh2) & param.tmsk2;
            x ^= (x >> param.tsh3);
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
        void setUpParam(ParameterGenerator& mix) {
            param.pos1 = mix.getUint64() % (size - 1) + 1;
            param.pos2 = mix.getUint64() % (size - 1) + 1;
            while (param.pos1 == param.pos2) {
                param.pos2 = mix.getUint64() % (size - 1) + 1;
            }
            param.pos3 = mix.getUint64() % (size - 1) + 1;
            while (param.pos1 == param.pos3 || param.pos2 == param.pos3) {
                param.pos3 = mix.getUint64() % (size - 1) + 1;
            }
            uint32_t seq = mix.getUint32();
            param.seq = seq;
            seq = seq ^ (seq << 15) ^ (seq << 23);
            uint32_t wmat1 = (seq & 0xffff0000) | (param.id & 0xffff);
            uint32_t wmat2 = (seq & 0xffff) | (param.id & 0xffff0000);
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
            mt64rec2 *that = dynamic_cast<mt64rec2 *>(&other);
            if (that == 0) {
                throw std::invalid_argument(
                    "the adder should have same type as the addee.");
            }
            this->add(that);
        }

        void add(const mt64rec2 * that) {
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

        bool equals(const mt64rec2& that) {
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
            (void)value;
            //fixedPOS = value;
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
            if (tmpidx < 0) { // Algorithm BestBits
                if (src_bit == 0) {
                    param.tmsk1 &= ~mask;
                    param.tmsk1 |= pattern & mask;
                } else {
                    param.tmsk2 &= ~mask;
                    param.tmsk2 |= pattern & mask;
                }
            } else if (tmpidx == 0) { // Algorithm Partial Bit pattern
                param.tmsk1 &= ~mask;
                param.tmsk1 |= pattern & mask;
            } else if (tmpidx == 1) { // Algorithm Partial Bit pattern
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
        uint32_t getID() {
            return param.id;
        }
        uint32_t getSEQ() {
            return param.seq;
        }
        void setTmpIdx(int idx) {
            tmpidx = idx;
        }
    private:
        void make_mask(int mexp) {
            int bit = mexp % 64;
            lower_mask = 0;
            lower_mask = ~lower_mask;
            lower_mask = lower_mask >> bit;
            upper_mask = ~lower_mask;
        }
        mt64rec2& operator=(const mt64rec2&) {
            throw std::logic_error("can't assign");
        }
        //int fixedPOS;
        int size;
        int index;
        int tmpidx;
        uint64_t * state;
        mt64rec2_param param;
        uint64_t upper_mask;
        uint64_t lower_mask;
        bool reverse_bit_flag;
    };
}

#endif // MT64REC2SEARCH_HPP

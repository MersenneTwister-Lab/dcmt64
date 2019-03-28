#pragma once
#ifndef MIXEDSEQUENCE_HPP
#define MIXEDSEQUENCE_HPP

#include <MTToolBox/MersenneTwister.hpp>
#include <MTToolBox/Sequential.hpp>

namespace MTToolBox {

    class MixedSequence : public ParameterGenerator {
    public:
        MixedSequence(uint32_t first, uint64_t seed, uint32_t mask) :
            mt(seed), sq(mask, first) {
        }

        uint64_t getUint64() {
            return mt.getUint32();
        }

        uint32_t getUint32() {
            return sq.getUint32();
        }

        void seed(uint64_t value) {
            mt.seed(value);
        }
    private:
        MersenneTwister mt;
        Sequential<uint32_t> sq;
    };
}
#endif // MIXEDSEQUENCE_HPP

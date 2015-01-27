#ifndef __DNA_H__
#define __DNA_H__

#include <random>
#include <cstddef>

#include "constants.hpp"

template <class URNG>
struct Dna
{
    Dna(const Dna& lhs, const Dna& rhs, URNG& urng) :
        _data(c_dnaLength)
    {
        std::bernoulli_distribution startDistrib(0.5);
        std::bernoulli_distribution mutatDistrib(c_mutatP);
        std::bernoulli_distribution crossDistrib(c_crossP);

        const Dna * ptr = (startDistrib(urng)) ? &lhs : &rhs;

        for (std::size_t i = 0; i < c_dnaLength; i++)
        {
            if (crossDistrib(urng))
            {
                ptr = (ptr == &rhs) ? &lhs : &rhs;
            }

            _data[i] = mutatDistrib(urng) ? 1 ^ ptr->_data[i] : ptr->_data[i];
        }
    }

    Dna(URNG& urng) : 
        _data(c_dnaLength)
    {
        std::uniform_int_distribution<int> dnaDistrib(0, 1);

        for (std::size_t i = 0; i < c_dnaLength; i++)
        {
            _data[i] = dnaDistrib(urng);
        }
    }

    std::vector<int> _data;
};

#endif
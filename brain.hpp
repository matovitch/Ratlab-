#ifndef __BRAIN_H__
#define __BRAIN_H__

#include "rat.hpp"

template <template <class> class Derived, class URNG>
struct Brain
{
    virtual Move operator() (const Rat<Derived, URNG>& rat, URNG& urng) const = 0;
};

#endif
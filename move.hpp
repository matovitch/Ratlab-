#ifndef __MOVE_H__
#define __MOVE_H__

struct Move
{
    template<class URNG>
    void getRandom(int range, URNG& urng)
    {
        std::uniform_int_distribution<int> rangeP(-range, range);
        x = rangeP(urng);
        y = rangeP(urng);
    }    

    int x;
    int y; 
};

#endif
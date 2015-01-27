#ifndef __SIMULATION_H__
#define __SIMULATION_H__

#include <list>
#include <queue>
#include <vector>
#include <random>
#include <cstddef>
#include <cstdlib>
#include <iostream>

#include "rat.hpp"
#include "map.hpp"
#include "constants.hpp"

template <template <class> class BRAIN, class URNG>
class Simulation
{
public:

    Simulation()
    {
        _Rat::_map = &_map;

        std::random_device rd;
        _urng.seed(rd());
    }

    void run()
    {
        for (std::size_t i = 0; i < c_nMaps; i++)
        {
            godCreatedEarthAndRats();

            for (std::size_t j = 0; j < c_nTurns; j++)
            {
                live();
                die();
                haveSex();

                /*if (j % 500 == 0)
                {
                    std::system("clear");
                    std::cout << _map.build_view(_rats);    // display
                }*/
            }
        }
    }

private:

    typedef Rat<BRAIN, URNG> _Rat;

    void godCreatedEarthAndRats()
    {
        _goals = 1;

        _map.build(_urng);
        _rats.clear();

        for (std::size_t j = 0; j < c_initNRats; j++)
        {
            _rats.push_back(_Rat(_urng));
        }
    }

    void live()
    {
        for (typename std::list<_Rat>::iterator rat = _rats.begin(); rat != _rats.end(); rat++) 
        {
            rat->move(_urng);
        }
    }

    void die()
    {
        for (typename std::list<_Rat>::iterator rat = _rats.begin(); rat != _rats.end();) 
        {
            if (rat->isDead())
            {
                _goals += rat->getGoals();
                _rats.erase(rat++);
            } 
            else
            {
                ++rat;
            }
        }
    }

    void haveSex()
    {
        std::size_t sumFitness = 0;

        for (typename std::list<_Rat>::iterator rat = _rats.begin(); rat != _rats.end(); rat++) 
        {
            sumFitness += rat->getFitness();
        }

        std::uniform_int_distribution<std::size_t> breedDistrib(0, sumFitness);

        std::priority_queue<std::size_t, std::vector<std::size_t>, std::greater<std::size_t> > breedRandom;

        for (std::size_t k = 0; k < 2 * c_newRatsPerTurn; k++)
        {
            breedRandom.push(breedDistrib(_urng));
        }

        sumFitness = 0;
        std::vector<_Rat> goodMates;
 
        for (typename std::list<_Rat>::iterator rat = _rats.begin(); !breedRandom.empty() ; rat++) 
        {
            while (!breedRandom.empty() &&
                   sumFitness <= breedRandom.top() &&
                   breedRandom.top() <= sumFitness + rat->getFitness())
            {
                goodMates.push_back(*rat);
                breedRandom.pop();
            }
            sumFitness += rat->getFitness();
        }

        std::shuffle(goodMates.begin(), goodMates.end(), _urng); //work but valgring detects errors

        for (std::size_t i = 0; i < c_newRatsPerTurn; i++)
        {
            _rats.push_back(_Rat(goodMates[2 * i + 0],
                                 goodMates[2 * i + 1], _urng));
        }   
    }

    std::size_t getGoals()
    {
        for (typename std::list<_Rat>::iterator rat = _rats.begin(); rat != _rats.end(); rat++) 
        {
            _goals += rat->getGoals();
        }

        return _goals;
    }

    Map<URNG> _map;
    std::list<_Rat> _rats;
    std::size_t _goals;
    URNG _urng;
};

#endif
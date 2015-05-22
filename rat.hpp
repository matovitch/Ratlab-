#ifndef __RATS_H__
#define __RATS_H__

#include <random>
#include <cstddef>
#include <cassert>
#include <iostream>

#include "dna.hpp"
#include "move.hpp"
#include "constants.hpp"

template <template <class> class BRAIN, class URNG>
class Simulation;

template <class URNG>
class Map;

template <template <class> class BRAIN, class URNG>
class Rat
{
    friend class Simulation<BRAIN, URNG>;
    friend class Map<URNG>;

public:

    int look(int i, int j) const
    {
        return _position.getColor(i, j);
    }

    int getDna(std::size_t i) const
    {
        return _dna._data[i];
    }

private:

    Rat(const Rat& lhs, const Rat& rhs, URNG& urng) :
        _goals(0), _age(0), _dna(lhs._dna, rhs._dna, urng)
    {
        this->build(urng);
    }

    Rat(URNG& urng) :
        _goals(0), _age(0), _dna(urng)
    {
        this->build(urng);
    }

    void build(URNG& urng)
    {
        _position.getRandom(urng);
    }

    void move(URNG& urng)
    {
        _map->template explore<BRAIN>(_position, _brain(*this, urng));

        if (isAlive())
        {
            _age++;
            _goals += (getX() >= c_xGoal);
            _fitness = 1 + (c_xGoal + 1) * _goals + getX();
        }
    }

    bool isAlive() const { return _position.isValid() && _age < c_ratLifeSpan; }

    bool isDead() const { return !isAlive(); }

    std::size_t getFitness() const { return _fitness; }

    std::size_t getGoals() const { return _goals; }

    std::size_t getX() const { return _position.getX(); }

    std::size_t getY() const { return _position.getY(); }

    struct Position
    {
        bool isValid()   const { return _rPtr != NULL; }
        
        bool isInvalid() const { return _rPtr == NULL; }
        
        int getColor(int i, int j) const { return _vPtr[j * _map->_mapWidth + i]; }

        std::size_t getX() const { return (_rPtr - _map->_realMap) % _map->_mapWidth; }

        std::size_t getY() const { return (_rPtr - _map->_realMap) / _map->_mapWidth; }

        void getRandom(URNG& urng)
        {
            std::uniform_int_distribution<std::size_t> startP(0, c_height - 1);

            _rPtr = _map->_realMap + startP(urng) * _map->_mapWidth;
            _vPtr = _map->_virtMap + (_rPtr - _map->_realMap);
        }
        
        std::size_t* _rPtr;
        int* _vPtr;
    };

    Position _position;
    BRAIN<URNG> _brain;
    std::size_t _goals;
    std::size_t _fitness;
    std::size_t _age;
    Dna<URNG> _dna;

    static Map<URNG>* _map;
};

template <template <class> class BRAIN, class URNG>
Map<URNG>* Rat<BRAIN, URNG>::_map;

#endif

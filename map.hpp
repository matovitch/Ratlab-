#ifndef __MAP_HPP__
#define __MAP_HPP__

#include <random>
#include <cstddef>
#include <iostream>
#include <algorithm>

#include "rat.hpp"
#include "constants.hpp"

namespace Ground
{
enum : std::size_t
{
    NONE          = 0,
    TRAP          = 1,
    WALL          = 2,
    EMPTY         = 3,
    TELEPORT_1    = 4,
    TELEPORT_2    = 5,
    TELEPORT_3    = 6,
    TELEPORT_4    = 7
};
}

template <class URNG>
struct Map
{

    Map() :
        _mapWidth(c_width + 2 * c_teleportRange),
        _mapHeight(c_height + 2 * c_teleportRange),
        _viewWidth(c_width + 1),
        _viewHeight(c_height)
    {
        _virtMap = new         int[_mapWidth  *  _mapHeight];
        _realMap = new std::size_t[_mapWidth  *  _mapHeight];
        _viewMap = new        char[_viewWidth * _viewHeight];

        std::fill(_virtMap, _virtMap + _mapWidth * _mapHeight, -1);
        std::fill(_realMap, _realMap + _mapWidth * _mapHeight,  0);
        
        _virtMap += c_teleportRange * (_mapWidth + 1);
        _realMap += c_teleportRange * (_mapWidth + 1);
    }

    void build(URNG& urng)
    {
        std::uniform_int_distribution<std::size_t> colorP(0, c_nColors - 1);

        for (std::size_t i = 0; i < c_height; i++)
        {
            for (std::size_t j = 0; j < c_width; j++)
            {
                _virtMap[i * _mapWidth + j] = colorP(urng);
            }
        }

        _offsetTrap_1.getRandom(c_trapRange, urng);
        _offsetTrap_2.getRandom(c_trapRange, urng);

        std::vector<std::size_t> shuffle(c_nColors);
        
        std::iota(shuffle.begin(), shuffle.end(), 0);
        std::shuffle(shuffle.begin(), shuffle.end(), urng);

        for (std::size_t i = 0; i < c_height; i++)
        {
            for (std::size_t j = 0; j < c_width; j++)
            {
                const std::size_t cell = shuffle[_virtMap[i * _mapWidth + j]];

                const std::size_t trap_1 = (_virtMap[(i - _offsetTrap_1.y) * _mapWidth +
                                                     (j - _offsetTrap_1.x)] == -1) ? 0 :
                                             shuffle[_virtMap[(i - _offsetTrap_1.y) * _mapWidth +
                                                              (j - _offsetTrap_1.x)]];
                const std::size_t trap_2 = (_virtMap[(i - _offsetTrap_2.y) * _mapWidth +
                                                     (j - _offsetTrap_2.x)] == -1) ? 0 :
                                             shuffle[_virtMap[(i - _offsetTrap_2.y) * _mapWidth +
                                                              (j - _offsetTrap_2.x)]];

                if (cell == 0)
                {
                    _realMap[i * _mapWidth + j] = Ground::TELEPORT_1;
                } else
                if (cell == 1)
                {
                    _realMap[i * _mapWidth + j] = Ground::TELEPORT_2;
                } else
                if (cell == 2)
                {
                    _realMap[i * _mapWidth + j] = Ground::TELEPORT_3;
                } else
                if (cell == 3)
                {
                    _realMap[i * _mapWidth + j] = Ground::TELEPORT_4;
                } else
                if (cell < 5)
                {
                    _realMap[i * _mapWidth + j] = Ground::WALL;
                } else
                if (trap_1 == 6 || trap_2 == 7)
                {
                    _realMap[i * _mapWidth + j] = Ground::TRAP;
                } else
                {
                    _realMap[i * _mapWidth + j] = Ground::EMPTY;
                }
            }
        }

        _offsetTeleport_1.getRandom(c_trapRange, urng);
        _offsetTeleport_2.getRandom(c_trapRange, urng);

        _offsetTeleport_3.x = -_offsetTeleport_1.x;
        _offsetTeleport_3.y = -_offsetTeleport_1.y;
        _offsetTeleport_4.x = -_offsetTeleport_2.x;
        _offsetTeleport_4.y = -_offsetTeleport_2.y;
    }

    template <template <class> class BRAIN>
    const Map& build_view(const std::list<Rat<BRAIN, URNG>>& rats)
    {
        for (std::size_t i = 0; i < c_height; i++)
        {
            for (std::size_t j = 0; j < c_width; j++)
            {
                std::size_t cell = _realMap[i * _mapWidth + j];

                if (cell == Ground::EMPTY)
                {
                    _viewMap[i * _viewWidth + j] = ' ';
                } else
                if (cell == Ground::WALL)
                {
                    _viewMap[i * _viewWidth + j] = '#';
                } else
                if (cell == Ground::TRAP)
                {
                    _viewMap[i * _viewWidth + j] = 'X';
                } else
                if (cell == Ground::TELEPORT_1 ||
                    cell == Ground::TELEPORT_2 ||
                    cell == Ground::TELEPORT_3 ||
                    cell == Ground::TELEPORT_4)
                {
                    _viewMap[i * _viewWidth + j] = 'O';
                } 
                else
                {
                    _viewMap[i * _viewWidth + j] = '$';
                }
            }
            _viewMap[(i + 1) * _viewWidth - 1] = '\n';
        }

        for (typename std::list<Rat<BRAIN, URNG>>::const_iterator rat = rats.cbegin();
             rat != rats.end();
             rat++)
        {
            _viewMap[rat->getY() * _viewWidth + rat->getX()] = '@';
        }

        return *this;
    }

    template <template <class> class BRAIN>
    void explore(typename Rat<BRAIN, URNG>::Position& position, const Move& move)
    {
        std::size_t* nptr = position._rPtr + move.y * _mapWidth + move.x;

        if (*nptr == Ground::WALL)
        {
            nptr = position._rPtr;
        } else
        if (*nptr == Ground::TELEPORT_1)
        {
            nptr += _offsetTeleport_1.y * _mapWidth + _offsetTeleport_1.x;
        } else
        if (*nptr == Ground::TELEPORT_2)
        {
            nptr += _offsetTeleport_2.y * _mapWidth + _offsetTeleport_2.x;
        } else
        if (*nptr == Ground::TELEPORT_3)
        {
            nptr += _offsetTeleport_3.y * _mapWidth + _offsetTeleport_3.x;
        } else
        if (*nptr == Ground::TELEPORT_4)
        {
            nptr += _offsetTeleport_4.y * _mapWidth + _offsetTeleport_4.x;
        }

        if (*nptr == Ground::NONE || 
            *nptr == Ground::TRAP ||
            *nptr == Ground::WALL)
        {
            position._rPtr = NULL;
            position._vPtr = NULL;
        }
        else
        {
            position._vPtr += position._rPtr - nptr;
            position._rPtr = nptr;
        }
    }

    ~Map()
    {
        delete[] (_virtMap - c_teleportRange * (_mapWidth + 1));
        delete[] (_realMap - c_teleportRange * (_mapWidth + 1));
        delete[] _viewMap;
    }

    std::size_t* _realMap;
    char*        _viewMap;
    int*         _virtMap;
    std::size_t _mapWidth;
    std::size_t _mapHeight;
    std::size_t _viewWidth;
    std::size_t _viewHeight;

    Move _offsetTrap_1;
    Move _offsetTrap_2;
    Move _offsetTeleport_1;
    Move _offsetTeleport_2;
    Move _offsetTeleport_3;
    Move _offsetTeleport_4;
};

template <class URNG>
std::ostream& operator<<(std::ostream& os, const Map<URNG>& map)
{
  os << map._viewMap << std::endl;
  return os;
}

#endif
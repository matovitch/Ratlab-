#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

static const int c_trapRange = 1;
static const int c_teleportRange = 4;
static const std::size_t c_nMaps = 50;
static const std::size_t c_xGoal = 49;
static const std::size_t c_width = 53;
static const std::size_t c_height = 15;
static const std::size_t c_nColors = 16;
static const std::size_t c_initNRats = 15;
static const std::size_t c_nTurns = 10000;
static const std::size_t c_dnaLength = 100;
static const std::size_t c_ratLifeSpan = 100;
static const std::size_t c_newRatsPerTurn = 10;
static const std::size_t c_mapping[16] = {0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 3, 4, 5, 6, 7};

static const double c_mutatP = 0.01;
static const double c_crossP = 0.05;

#endif
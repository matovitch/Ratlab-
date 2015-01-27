#include "simulation.hpp"
#include "brain.hpp"
#include "move.hpp"
#include "rat.hpp"

template <class URNG>
struct Dumb : Brain<Dumb, URNG>
{
    typedef Rat<::Dumb, URNG> Mouse;

    Move operator()(const Mouse& mouse, URNG& urng) const
    {
        Move move;
        move.x = 1;
        move.y = 0;
        //mouse.look(1, 2);
        //mouse.getDna(35);
        return move;
    }
};

int main()
{
    Simulation<Dumb, std::mt19937> simulation;
    simulation.run();

    return 0;
}
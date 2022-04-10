#ifndef PLAYER_H
#define PLAYER_H

#include "game_meta.hpp"
#include "cmath"


class Player{
    public:
        PlayerChoice choose(size_t heads [[maybe_unused]], size_t tails [[maybe_unused]], size_t coins_left [[maybe_unused]] = 100) const{
            return PlayerChoice::Unsure;
        }
};

#endif // PLAYER_H

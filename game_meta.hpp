#ifndef GAME_META_H
#define GAME_META_H

enum class CoinFlip: bool{
    Heads, Tails
};

enum class Fairness: bool{
    Fair, Biased
};

enum class PlayerChoice: unsigned char{
    Unsure = 0, Fair, Biased
};

#endif // GAME_META_H

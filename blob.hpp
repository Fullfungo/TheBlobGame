#ifndef BLOB_H
#define BLOB_H

#include <random>
#include "game_meta.hpp"


size_t get_random_value(){
    static std::random_device random_source;
    static std::mt19937_64 random_engine(random_source());
    static std::uniform_int_distribution<size_t> rng(0, 3);
    return rng(random_engine);
}

// A random Blob that is either fair or biased, chosen with 50% probability each
class Blob{
    private:
        Fairness blob_fairness = Fairness::Fair;

    public:
        Blob(): blob_fairness(get_random_value() % 2 == 0 ? Fairness::Fair : Fairness::Biased) {};

        Fairness get_fairness() const{
            return blob_fairness;
        }

        CoinFlip flip() const{
            if (blob_fairness == Fairness::Fair)
                return flip_fair();
            else
                return flip_biased();
        }

        std::pair<size_t, size_t> flips(size_t const times) const{
            size_t heads = 0;
            size_t tails = 0;
            for (size_t i = 0; i < times; ++i)
                if (flip() == CoinFlip::Heads)
                    ++heads;
                else
                    ++tails;
            return {heads, tails};
        }

    private:
        CoinFlip flip_fair() const{ // 50% heads, 50% tails
            if (get_random_value() % 2 == 0)
                return CoinFlip::Heads;
            else
                return CoinFlip::Tails;
        }

        CoinFlip flip_biased() const { // 75% heads, 25% tails
            if (get_random_value() % 4 != 0)
                return CoinFlip::Heads;
            else
                return CoinFlip::Tails;
        }
};

#endif // BLOB_H

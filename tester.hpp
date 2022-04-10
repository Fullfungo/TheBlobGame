#ifndef TESTER_H
#define TESTER_H

#include "blob.hpp"
#include "player.hpp"


bool is_verdict_final(PlayerChoice const verdict){
    if (verdict != PlayerChoice::Unsure)
        return true;
    else 
        return false;
}

bool is_verdict_correct(PlayerChoice const verdict, Fairness const real_fairness){
    if ((real_fairness == Fairness::Fair && verdict == PlayerChoice::Fair) ||
        (real_fairness == Fairness::Biased && verdict == PlayerChoice::Biased))
        return true;
    else
        return false;
}

class Tester{
    public:
        // returns player's final score
        size_t run_test(Player const &player){
            long long coins = 100;
            size_t score = 0;
            while (coins > 0){
                Blob blob;
                size_t heads = 0;
                size_t tails = 0;
                PlayerChoice verdict = player.choose(heads, tails, coins);
                while (!is_verdict_final(verdict) && coins > 0){
                    CoinFlip flip = blob.flip();
                    if (flip == CoinFlip::Heads)
                        ++heads;
                    else
                        ++tails;
                    --coins;
                    
                    verdict = player.choose(heads, tails, coins);
                }

                if (!is_verdict_final(verdict)) // player failed to chose and lost all coins
                    return score;
                
                if (is_verdict_correct(verdict, blob.get_fairness())){
                    ++score;
                    coins += 15;
                }else{
                    coins -= 30;
                }
            }
            return score;
        }

        long double run_tests(Player const &player, size_t const rounds = 1'000){
            size_t total_score = 0;
            for (size_t i = 0; i < rounds; ++i)
                total_score += run_test(player);
            long double average_score = static_cast<long double>(total_score) / rounds;
            return average_score;
        }
};

#endif // TESTER_H

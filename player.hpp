#ifndef PLAYER_H
#define PLAYER_H

#include <cmath>
#include <array>
#include <iostream>
#include <iomanip>
#include "game_meta.hpp"


std::ostream &operator<<(std::ostream &os, PlayerChoice const choice){
    switch (choice){
        case PlayerChoice::Fair:
            os << "Fair ";
            break;
        case PlayerChoice::Biased:
            os << "Bias ";
            break;
        case PlayerChoice::Unsure:
            os << "Skip ";
    }
    return os;
}

constexpr size_t grid_size = 2'000;

long double probability_of_fair_blob(size_t const heads, size_t const tails){
    return 1 / (1 + powl(1.5l, heads) / powl(2.0l, tails));
}

auto create_optimal_strategy(){
    //
    std::cout << std::fixed << std::setprecision(6);
    //
    std::array<std::array<long double, grid_size>, grid_size> expected_coin_gain;
    std::array<std::array<PlayerChoice, grid_size>, grid_size> optimal_choice;

    std::array<long double, grid_size> empty_coin_gain_row;
    empty_coin_gain_row.fill(0.0l);
    expected_coin_gain.fill(empty_coin_gain_row);

    std::array<PlayerChoice, grid_size> empty_player_choice;
    empty_player_choice.fill(PlayerChoice::Unsure);
    optimal_choice.fill(empty_player_choice);

    // optimal_choice : use `probability_of_fair_blob >= 0.5` on the far edges
    // expected_coin_gain : E(G | H=h, T=t, C= `optimal_choice` )
    for (size_t j = 0; j < grid_size; ++j){
        auto chance_of_fair = probability_of_fair_blob(grid_size - 1, j);
        if (chance_of_fair >= 0.5){
            optimal_choice[grid_size - 1][j] = PlayerChoice::Fair;
            expected_coin_gain[grid_size - 1][j] = -(static_cast<long double>(grid_size) - 1 + j) + 45 * chance_of_fair - 30;
        }else{
            optimal_choice[grid_size - 1][j] = PlayerChoice::Biased;
            expected_coin_gain[grid_size - 1][j] = -(static_cast<long double>(grid_size) - 1 + j) - 45 * chance_of_fair + 15;
        }
    }
    
    for (size_t i = 0; i < grid_size; ++i){
        auto chance_of_fair = probability_of_fair_blob(i, grid_size - 1);
        if (chance_of_fair >= 0.5){
            optimal_choice[i][grid_size - 1] = PlayerChoice::Fair;
            expected_coin_gain[i][grid_size - 1] = -(i + static_cast<long double>(grid_size) - 1) + 45 * chance_of_fair - 30;
        }else{
            optimal_choice[i][grid_size - 1] = PlayerChoice::Biased;
            expected_coin_gain[i][grid_size - 1] = -(i + static_cast<long double>(grid_size) - 1) - 45 * chance_of_fair + 15;
        }
    }

    // propagate back towards the origin
        // choose maximal gain between
        // E(G | H=h, T=t, C=F) = -(h+t)+15P(F | H=h, T=t)-30P(B | H=h, T=t)
        // E(G | H=h, T=t, C=B) = -(h+t)+15P(B | H=h, T=t)-30P(F | H=h, T=t)
        // E(G | H=h, T=t, C=S) = P(heads | H=h, T=t)E(G | H=h+1, T=t, C')+P(tails | H=h, T=t)E(G | H=h, T=t+1, C'')
        // set the choice acording to the winner
    for (long long i = grid_size - 1; i-->0; )
        for (long long j = grid_size - 1; j-->0; ){
            auto chance_of_fair = probability_of_fair_blob(i, j);
            auto chance_of_heads = 0.75l - 0.25l * chance_of_fair;

            auto gain_if_fair = -(i + j) + 45 * chance_of_fair - 30;
            auto gain_if_biased = -(i + j) - 45 * chance_of_fair + 15;
            auto gain_if_skip = chance_of_heads * (expected_coin_gain[i + 1][j] - expected_coin_gain[i][j + 1]) + expected_coin_gain[i][j + 1];

            if (gain_if_fair >= gain_if_biased && gain_if_fair >= gain_if_skip){
                optimal_choice[i][j] = PlayerChoice::Fair;
                expected_coin_gain[i][j] = gain_if_fair;
            }else if (gain_if_biased >= gain_if_skip){
                optimal_choice[i][j] = PlayerChoice::Biased;
                expected_coin_gain[i][j] = gain_if_biased;
            }else{
                optimal_choice[i][j] = PlayerChoice::Unsure;
                expected_coin_gain[i][j] = gain_if_skip;
            }
        }

    std::cout << "First 25x25 decision cells:\n";
    for (size_t j = 0; j < 26; ++j){
        for (size_t i = 0; i < 26; ++i)
            std::cout << optimal_choice[i][j] << ' ';
        std::cout << '\n';
    }
    
    return optimal_choice;
}

class Player{
    public:
        PlayerChoice choose(size_t heads [[maybe_unused]], size_t tails [[maybe_unused]], size_t coins_left [[maybe_unused]] = 100) const{
            static auto const optimal_choice = create_optimal_strategy();
            if (coins_left == 0){
                if (probability_of_fair_blob(heads, tails) >= 0.5l)
                    return PlayerChoice::Fair;
                else
                    return PlayerChoice::Biased;
            }else
                return optimal_choice[heads][tails];
        }
};

#endif // PLAYER_H

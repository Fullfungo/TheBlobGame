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
            os << "\033[32m";
            os << "Fair";
            os << "\033[0m";
            break;
        case PlayerChoice::Biased:
            os << "\033[31m";
            os << "Bias";
            os << "\033[0m";
            break;
        case PlayerChoice::Unsure:
            os << "\033[33m";
            os << "Skip";
            os << "\033[0m";
    }
    return os;
}

constexpr size_t heads_limit = 100;
constexpr size_t tails_limit = 80;
constexpr size_t flips_limit = 512; // increment by 4096 to fit whole number of pages (or at least by 256)

long double probability_of_fair_blob(size_t const heads, size_t const tails){
    return 1 / (1 + powl(1.5l, heads) / powl(2.0l, tails));
}

using score_table_t = std::array<std::array<std::array<long double, flips_limit>, tails_limit>, heads_limit>; // 64 MB
using choice_table_t = std::array<std::array<std::array<PlayerChoice, flips_limit>, tails_limit>, heads_limit>; // 4 MB

auto run_single_cell(size_t const heads, size_t const tails, size_t const flips, score_table_t &expected_score_gain, choice_table_t &optimal_choice){
    auto probability_fair = probability_of_fair_blob(heads, tails);
    auto probability_bias = 1 - probability_fair;
    auto probability_of_next_heads = 0.75l - 0.25l * probability_fair;
    auto probability_of_next_tails = 1 - probability_of_next_heads;

    long double fair_score_gain = 0;
    long double bias_score_gain = 0;
    long double skip_score_gain = 0;
    
    // calculate the score gain of all posible future states
    if (flips == 0)
        skip_score_gain = 0;
    else
        skip_score_gain = probability_of_next_heads * expected_score_gain[std::min(heads + 1, heads_limit - 1)][tails][flips - 1] + probability_of_next_tails * expected_score_gain[heads][std::min(tails + 1, tails_limit - 1)][flips - 1];

    auto expected_score_gain_on_success = expected_score_gain[0][0][std::min(flips + 15, flips_limit - 1)];
    auto expected_score_gain_on_failure = expected_score_gain[0][0][flips - 30];
    
    if (flips <= 30){
        fair_score_gain = probability_fair * (1 + expected_score_gain_on_success);
        bias_score_gain = probability_bias * (1 + expected_score_gain_on_success);
    }else{
        fair_score_gain = probability_fair * (1 + expected_score_gain_on_success) + probability_bias * expected_score_gain_on_failure;
        bias_score_gain = probability_fair * expected_score_gain_on_failure + probability_bias * (1 + expected_score_gain_on_success);
    }

    // choose the best move (maximise score gain)
    if (fair_score_gain >= bias_score_gain && fair_score_gain >= skip_score_gain){
        expected_score_gain[heads][tails][flips] = fair_score_gain;
        optimal_choice[heads][tails][flips] = PlayerChoice::Fair;
    }else if (bias_score_gain >= skip_score_gain){
        expected_score_gain[heads][tails][flips] = bias_score_gain;
        optimal_choice[heads][tails][flips] = PlayerChoice::Biased;
    }else{
        expected_score_gain[heads][tails][flips] = skip_score_gain;
        optimal_choice[heads][tails][flips] = PlayerChoice::Unsure;
    }
}

auto create_optimal_strategy(){
    // Dimentions: table[n_heads][n_tails][n_flips]
    score_table_t expected_score_gain;
    choice_table_t optimal_choice;

    // Populate the tables with dummy values
    expected_score_gain[0][0].fill(0.0l);
    expected_score_gain[0].fill(expected_score_gain[0][0]);
    expected_score_gain.fill(expected_score_gain[0]);

    optimal_choice[0][0].fill(PlayerChoice::Unsure);
    optimal_choice[0].fill(optimal_choice[0][0]);
    optimal_choice.fill(optimal_choice[0]);

    // run in all directions a lot of times
    // update the tables
    for (size_t i = 0; i < 4'000; ++i){
        std::cout << i << '\n';
        for (size_t heads = 0; heads < heads_limit; ++heads){
            for (size_t tails = 0; tails < tails_limit; ++tails){
                for (size_t flips = 0; flips < flips_limit; ++flips){
                    run_single_cell(heads, tails, flips, expected_score_gain, optimal_choice);
                }
                for (size_t flips = flips_limit; flips --> 0;){
                    run_single_cell(heads, tails, flips, expected_score_gain, optimal_choice);
                }
            }
            for (size_t tails = tails_limit; tails --> 0;){
                for (size_t flips = 0; flips < flips_limit; ++flips){
                    run_single_cell(heads, tails, flips, expected_score_gain, optimal_choice);
                }
                for (size_t flips = flips_limit; flips --> 0;){
                    run_single_cell(heads, tails, flips, expected_score_gain, optimal_choice);
                }
            }
        }
        for (size_t heads = heads_limit; heads --> 0;){
            for (size_t tails = 0; tails < tails_limit; ++tails){
                for (size_t flips = 0; flips < flips_limit; ++flips){
                    run_single_cell(heads, tails, flips, expected_score_gain, optimal_choice);
                }
                for (size_t flips = flips_limit; flips --> 0;){
                    run_single_cell(heads, tails, flips, expected_score_gain, optimal_choice);
                }
            }
            for (size_t tails = tails_limit; tails --> 0;){
                for (size_t flips = 0; flips < flips_limit; ++flips){
                    run_single_cell(heads, tails, flips, expected_score_gain, optimal_choice);
                }
                for (size_t flips = flips_limit; flips --> 0;){
                    run_single_cell(heads, tails, flips, expected_score_gain, optimal_choice);
                }
            }
        }

        // show the 100-coin table of best moves
        for (size_t tails = 0; tails < 10; ++tails){
            for (size_t heads = 0; heads < 10; ++heads)
                std::cout << optimal_choice[heads][tails][100] << ' ';
            std::cout << '\n';
        }

        std::cout << "At (0, 0, 0): " << expected_score_gain[0][0][0] << ' ' << optimal_choice[0][0][0] << '\n';
        std::cout << "At (0, 0, 100): " << expected_score_gain[0][0][100] << ' ' << optimal_choice[0][0][100] << '\n';
    }

    return optimal_choice;
}


class Player{
    public:
        PlayerChoice choose(size_t heads, size_t tails, size_t coins_left) const{
            static const auto optimal_strategy = create_optimal_strategy();
            // don't go out of bounds
            return optimal_strategy[std::min(heads, heads_limit)][std::min(tails, tails_limit)][std::min(coins_left, flips_limit)];
        }
};

#endif // PLAYER_H

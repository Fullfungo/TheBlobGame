#include <iostream>
#include "tester.hpp"


int main(){
    Tester tester;
    Player player;
    auto score = tester.run_tests(player, 1'000'000);
    std::cout << "score: " << score << '\n';
    // a back-of-the-envelope calculation for coin loss
    std::cout << "average coin gain: " << -100/score << '\n';
}

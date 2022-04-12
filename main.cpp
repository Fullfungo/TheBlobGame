#include <iostream>
#include "tester.hpp"


int main(){
    std::cout << std::fixed << std::setprecision(6);
    Tester tester;
    Player player;
    auto score = tester.run_tests(player, 100'000);
    std::cout << "score: " << score << '\n';
}

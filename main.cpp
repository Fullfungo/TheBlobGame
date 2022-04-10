#include <iostream>
#include "tester.hpp"


int main(){
    Tester tester;
    Player player;
    auto score = tester.run_tests(player, 100'000);
    std::cout << "score: " << score << '\n';
}

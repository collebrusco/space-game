//
//  main.cpp
//  risklike
//
//  Created by Frank Collebrusco on 6/2/23.
//
#include <iostream>

#include "RiskLike.h"

int main(int argc, const char * argv[]) {
    RiskLike game;
    if (game.create())
        game.start();
    return 0;
}

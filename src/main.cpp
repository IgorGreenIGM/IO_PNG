#include <iostream>
#include <string>
#include <ctime>
#include "../include/PNG/Utilities.h"
#include "../include/PNG/PNG.h"

int main()
{
    auto t1 = time(nullptr);
    PNG p("./pp.png");
    
    auto t2 = time(nullptr);

    std::cout << t2 - t1 << "\n";

    p.save("./gh.png");

    return 0;
}
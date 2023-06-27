#include <iostream>
#include <chrono>
#include <thread>

using std::cout;
using std::endl;

void funcSleep()
{
    std::this_thread::sleep_for(std::chrono::seconds (1));
}

int main() {

    auto start = std::chrono::system_clock::now();
    funcSleep();
    auto end = std::chrono::system_clock::now();

    std::chrono::duration<double, std::milli> float_ms;
    
    float_ms += end - start;
    float_ms += end - start;

    std::cout << "funcSleep() elapsed time is " << float_ms.count() << " milliseconds" << std::endl;


    return EXIT_SUCCESS;
}
// ProcessModule.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "Processes.h"
#include <chrono>
#include <thread>

using namespace std::this_thread; // sleep_for, sleep_until
using namespace std::chrono; // nanoseconds, system_clock, seconds

int main()
{
    std::cout << "Hello Process!\n";

    Child_Process p("D:/Programing/Untracked/Processes/exampleprogram/x64/Debug/exampleprogram.exe", "helo asd1 asd2 asd3");

    while (!p.isProcess_Ended()) {
        std::cout << p.get_output() << "\n";
        sleep_for(milliseconds(100));
        p.kill_process();
    }
    std::cout << "Ended!\n";



}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file

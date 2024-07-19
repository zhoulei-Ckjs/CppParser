#include <iostream>

#include "example.h"

/**
 * @brief This is a simple function
 */
void foo()
{
    std::cout << "Hello, World!" << std::endl;
}

/**
 * @brief This function adds two numbers
 * @param a
 * @param b
 * @return
 */
int add(int a, int b)
{
    return a + b;
}

int main()
{
    foo();
    std::cout << "3 + 4 = " << add(3, 4) << std::endl;
    return 0;
}


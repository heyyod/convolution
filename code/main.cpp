#include <iostream>
#include <random>

void CreateRandomSignal(int *signal, int size)
{
    std::random_device rd;
    std::uniform_int_distribution<int> distribution(-100,100);

    for (int i = 0; i < size; i++)
    {
        signal[i] = distribution(rd);
    }
}

void main()
{
    unsigned int N = 0;

    while (N <= 10)
    {
        std::cout << "Enter the size of the array (N>10): ";
        std::cin >> N;
        std::cout << std::endl;
    }

    int *signal = new int[N];
    CreateRandomSignal(signal, N);
}

#include <iostream>
#include <random>
#include <algorithm>
#include <assert.h>

void CreateRandomSignal(float *signal, int size)
{
    std::random_device rd;
    std::uniform_real_distribution<float> distribution(-100.0f, 100.0f);

    for (int i = 0; i < size; i++)
    {
        signal[i] = distribution(rd);
    }
}

// This function calculates the convolution of signalA and signalB.
float *MyConvole(float *signalA, float *signalB, int sizeA, int sizeB, int &conv_size)
{
    conv_size = sizeA + sizeB - 1;
    float *result = new float[conv_size];

    // We know that:  f*g = g*f
    // So we will always convolute the smaller signal onto the bigger
    // in order to make some calculations easier.
    float *static_signal = signalA;
    float *moving_signal = signalB;
    int max_size = sizeA;
    int min_size = sizeB;
    if (sizeA < sizeB)
    {
        std::swap(static_signal, moving_signal);
        std::swap(max_size, min_size);
    }

    // First we loop for every sample of the convolution
    for (int n = 0; n < conv_size; n++)
    {
        result[n] = 0;

        // In the inner loop we calculate the sum of the products
        // for every point of the resulting convolution.
        // The start of the loop is detetermined by the shortest of the two lengths.
        // Doing this minimizes the number of reps we need to enter the loop.
        int kStart = n < min_size ? 0 : n - min_size;
        for (int k = kStart;
             k < max_size && k <= n;
             k++)
        {
            result[n] += static_signal[k] * moving_signal[n - k];
        }
    }
    return result;
}

void main()
{
    int sel = 0;
    std::cout << "(1) Calculate convolution for a random signal.\n";
    std::cout << "(2) Calculate convolution for two audio files.\n";

    while (!(sel == 1 || sel == 2))
    {
        std::cout << "Select 1 or 2: ";
        std::cin >> sel;
    }

    if (sel == 1)
    {
        // A.1
        unsigned int N = 0;
        while (N <= 10 && N < UINT_MAX)
        {
            std::cout << "Enter the size of the random signal (N>10): ";
            std::cin >> N;
            std::cout << std::endl;
        }
        float *a = new float[N];
        CreateRandomSignal(a, N);

        // A.3
        float b[5] = {0.2f, 0.2, 0.2, 0.2, 0.2};

        // A.3
        int conv_size = 0;
        float *convolution = MyConvole(a, b, N, 5, conv_size);

        // Uncomment this to print result
        // for(int i = 0; i < conv_size; i++)
        //    std::cout << i << ": " << convolution[i] << std::endl;

        delete[] a;
        delete[] convolution;
    }
    else if (sel == 2)
    {

    }

    char exit;
    std::cin >> exit;
}

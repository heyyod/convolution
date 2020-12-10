#include <iostream>
#include <random>
#include <algorithm>
#include <limits>
#include "AudioFile.h"

float *CreateRandomSignal(int size)
{
    std::random_device rd;
    std::uniform_real_distribution<float> distribution(std::numeric_limits<float>::min(), std::numeric_limits<float>::max());
    float *result = new float[size];

    for (int i = 0; i < size; i++)
    {
        result[i] = distribution(rd);
    }
    return result;
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
        // for every sample of the convolution.
        // The start of the loop is detetermined by the shortest of the two lengths.
        // Doing this minimizes the number of reps.
        int kStart = n < min_size ? 0 : n - min_size;
        for (int k = kStart; k < max_size && k <= n; k++)
        {
            result[n] += static_signal[k] * moving_signal[n - k];
        }
    }
    return result;
}

// This function is used to convolute two signals AND save the result into a vector of samples
// to save as a new wav file.
float *MyConvole(float *signalA, float *signalB, int sizeA, int sizeB, int &conv_size, AudioFile<float> *conv_file)
{
    conv_size = sizeA + sizeB - 1;
    float *result = new float[conv_size];

    // We know that:  f*g = g*f
    // So we will always convolute the smaller signal onto the bigger signal
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
        // for every sample of the convolution.
        // The start of the loop is detetermined by the shortest of the two lengths.
        // Doing this minimizes the number of reps.
        int kStart = n < min_size ? 0 : n - min_size;
        for (int k = kStart; (k < max_size) && (k <= n); k++)
        {
            result[n] += static_signal[k] * moving_signal[n - k];
        }
        conv_file->samples[0].emplace_back(result[n]);
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
        // TASK A.1
        unsigned int N = 0;
        while (N <= 10 && N < UINT_MAX)
        {
            std::cout << "Enter the size of the random signal (N>10): ";
            std::cin >> N;
            std::cout << std::endl;
        }
        float *a = CreateRandomSignal(N);

        // TASK A.2
        float b[5] = {0.2f, 0.2, 0.2, 0.2, 0.2};

        // TASK A.3
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
        AudioFile<float> *pinkFile = new AudioFile<float>;
        AudioFile<float> *sampleFile = new AudioFile<float>;

        pinkFile->load("pink_noise.wav");
        sampleFile->load("sample_audio.wav");

        // Uncomment this to get info for the audio files
        //std::cout << "\n\nPink Noise Info: \n";
        //pinkFile->printSummary();
        //std::cout << "\nSample Audio Info: \n";
        //sampleFile->printSummary();

        // TASK B.A
        // The audio sample is saved into a vectror of channels.
        // Each channel is itself a vector that contains the samples.
        // We will only keep one channel from the two audio files and convolute them.
        float *pink = &pinkFile->samples[0][0];
        float *sample = &sampleFile->samples[0][0];
        int pinkSize = pinkFile->getNumSamplesPerChannel();
        int sampleSize = sampleFile->getNumSamplesPerChannel();

        std::cout << "Pink Noise - Sample Audio Convolution... (~ 2 minute)\n";
        AudioFile<float> *pinkNoise_sampleAudio = new AudioFile<float>;
        int conv_size = 0;
        MyConvole(sample, pink, sampleSize, pinkSize, conv_size, pinkNoise_sampleAudio);

        std::cout << "\nPink Noise - Sample Audio Convolution Audio Info: \n";
        pinkNoise_sampleAudio->printSummary();
        pinkNoise_sampleAudio->save("pinkNoise_sampleAudio.wav", AudioFileFormat::Wave);

        // TASK B.B
        float *white = CreateRandomSignal(pinkSize);
        std::cout << "\nWhite Noise - Sample Audio Convolution... (~ 2 minute)\n";
        AudioFile<float> *whiteNoise_sampleAudio = new AudioFile<float>;
        conv_size = 0;
        MyConvole(sample, white, sampleSize, pinkSize, conv_size, whiteNoise_sampleAudio);

        std::cout << "\bWhite Noise - Sample Audio Convolution Audio Info: \n";
        whiteNoise_sampleAudio->printSummary();
        whiteNoise_sampleAudio->save("whiteNoise_sampleAudio.wav", AudioFileFormat::Wave);

        delete pinkFile;
        delete sampleFile;
        delete pinkNoise_sampleAudio;
        delete whiteNoise_sampleAudio;
    }

    char exit;
    std::cin >> exit;
}

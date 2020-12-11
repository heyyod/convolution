#include <random>
#include <algorithm>
#include "AudioFile.h"

float *CreateRandomSignal(int length)
{
    std::random_device rd;
    std::uniform_real_distribution<float> distribution(-(float)0xFFFFFFFF, (float)0xFFFFFFFF);
    float *result = new float[length];

    for (int i = 0; i < length; i++)
    {
        result[i] = distribution(rd);
    }
    return result;
}

// This function calculates the convolution of signalA and signalB and returns the
// result into an array pointer that has been dynamically allocated.
float *MyConvole(float *signalA, float *signalB, int lengthA, int lengthB, int &conv_length)
{
    conv_length = lengthA + lengthB - 1;
    float *result = new float[conv_length];

    // We know that: f * g = g * f (Commutativity Property).
    // So we will always convolute the shorter signal onto the longer,
    // meaning that we keep the longer signal at the same place while we "roll"
    // the shorter one on it.
    // This will make it easier to find the starting and ending point of the
    // innner loop.
    float *static_signal = signalA;
    float *moving_signal = signalB;
    int max_length = lengthA;
    int min_length = lengthB;
    if (lengthA < lengthB)
    {
        std::swap(static_signal, moving_signal);
        std::swap(max_length, min_length);
    }

    // First we loop for every sample of the convolution
    for (int n = 0; n < conv_length; n++)
    {
        result[n] = 0;

        // In the inner loop we calculate the sum of the products for every point of the convolution.
        // The start of the loop is detetermined by the shortest of the two lengths.
        // Doing this minimizes the number of reps.
        int kStart = n < min_length ? 0 : n - min_length;
        for (int k = kStart; k < max_length && k <= n; k++)
        {
            result[n] += static_signal[k] * moving_signal[n - k];
        }
    }
    return result;
}

// This function is used to convolute two signals AND save the result into and AudioFile that is passed as
// a parametre by the caller of the function.
float *MyConvole(float *signalA, float *signalB, int lengthA, int lengthB, int &conv_length, AudioFile<float> *conv_file)
{
    conv_length = lengthA + lengthB - 1;
    float *result = new float[conv_length];
    conv_file->samples[0].reserve(conv_length);

    // We know that:  f*g = g*f
    // So we will always convolute the smaller signal onto the bigger signal
    // in order to make some calculations easier.
    float *static_signal = signalA;
    float *moving_signal = signalB;
    int max_length = lengthA;
    int min_length = lengthB;
    if (lengthA < lengthB)
    {
        std::swap(static_signal, moving_signal);
        std::swap(max_length, min_length);
    }

    // First we loop for every sample of the convolution
    for (int n = 0; n < conv_length; n++)
    {
        result[n] = 0;

        // In the inner loop we calculate the sum of the products
        // for every sample of the convolution.
        // The start of the loop is detetermined by the shortest of the two lengths.
        // Doing this minimizes the number of reps.
        int kStart = n < min_length ? 0 : n - min_length;
        for (int k = kStart; (k < max_length) && (k <= n); k++)
        {
            result[n] += static_signal[k] * moving_signal[n - k];
        }
        conv_file->samples[0].emplace_back(result[n]); // insert the result into channel 0 of the audio file.
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
            std::cout << "Enter the length of the random signal (N>10): ";
            std::cin >> N;
            std::cout << std::endl;
        }
        float *a = CreateRandomSignal(N);

        // TASK A.2
        float b[5] = {0.2f, 0.2, 0.2, 0.2, 0.2};

        // TASK A.3
        int conv_length = 0;
        float *convolution = MyConvole(a, b, N, 5, conv_length);

        // Uncomment this to print result
        // for(int i = 0; i < conv_length; i++)
        //    std::cout << i << ": " << convolution[i] << std::endl;

        delete[] a;
        delete[] convolution;
    }
    else if (sel == 2)
    {
        AudioFile<float> *sampleFile = new AudioFile<float>;
        AudioFile<float> *pinkFile = new AudioFile<float>;

        sampleFile->load("sample_audio.wav");
        pinkFile->load("pink_noise.wav");

        // Print audio info for pink noise and sample audio files
        std::cout << "\n\nPink Noise Info: \n";
        pinkFile->printSummary();
        std::cout << "\nSample Audio Info: \n";
        sampleFile->printSummary();

        // TASK B.A
        // The audio sample is saved into a vectror of channels.
        // Each channel is itself a vector that contains the samples.
        // We will only keep channel 0 from the two audio files and convolute them.
        float *sample = &sampleFile->samples[0][0];
        float *pink = &pinkFile->samples[0][0];
        int samplelength = sampleFile->getNumSamplesPerChannel();
        int pinklength = pinkFile->getNumSamplesPerChannel();

        // Calculate pink noise - sample audio covolution
        std::cout << "\nPink Noise - Sample Audio Convolution... (~ 2 minute)\n";
        AudioFile<float> *pinkNoise_sampleAudio = new AudioFile<float>;
        int pinkNoise_sampleAudio_length = 0;
        MyConvole(sample, pink, samplelength, pinklength, pinkNoise_sampleAudio_length, pinkNoise_sampleAudio);

        // Print audio info for the pink noise - sample audio convolution
        std::cout << "\nAudio Info: \n";
        pinkNoise_sampleAudio->printSummary();
        pinkNoise_sampleAudio->save("pinkNoise_sampleAudio.wav", AudioFileFormat::Wave);

        // TASK B.B
        // Create a white noise signal
        float *white = CreateRandomSignal(pinklength);

        // Calculate white noise - sample audio covolution
        std::cout << "\nWhite Noise - Sample Audio Convolution... (~ 2 minute)\n";
        AudioFile<float> *whiteNoise_sampleAudio = new AudioFile<float>;
        int whiteNoise_sampleAudio_length = 0;
        MyConvole(sample, white, samplelength, pinklength, whiteNoise_sampleAudio_length, whiteNoise_sampleAudio);

        // Print audio info for the white noise - sample audio convolution
        std::cout << "\nAudio Info: \n";
        whiteNoise_sampleAudio->printSummary();
        whiteNoise_sampleAudio->save("whiteNoise_sampleAudio.wav", AudioFileFormat::Wave);

        delete pinkFile;
        delete sampleFile;
        delete pinkNoise_sampleAudio;
        delete whiteNoise_sampleAudio;
    }

    // Prevent console from closing unless we give an input.
    char exit;
    std::cin >> exit;
}

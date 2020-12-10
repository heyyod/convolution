[pink, Fs_pink] = audioread('pink_noise.wav');
sound(pink, Fs_pink);

[sample, Fs_sample] = audioread('sample_audio.wav');
sound(sample, Fs_sample);

result = conv(pink(:,1), sample);
sound(result, Fs_pink)


# WAV-PCM
_(Project still in development)_

## Project Description
WAV-PCM is a repository containing the development of a pitch analysis tool for WAV audio files. The program extracts the **_fundamental frequencies and MIDI notes_** from the analyzed audio.
This project represents one of the first steps towards developing a  **music/audio engine**, where audio files can be analized and processed for use in certain apps or games.

The long term goal is to develop a **real time pitch detection system** that could be used inside projects such as _karaoke games_ or _voice related applications_, adn eventually, using it inside a **_made in house Graphics/Game Engine_**.

>[!IMPORTANT]
>For the time being, only PCM WAV files with 16-bit samples and mono audio are supported .


## MAIN PROGRAM
The program is based on three main structures used to represent the relevant information containd in a WAV audio file.

The first structure contains a fixed part of the WAV Header, while the `WAVChunk` structure is used to handle the dynamic/variable size chunks found inside the file _(such as fmt or data)_.
`struct WAVChunk{
    char id[4];
    uint32_t size;
};`


This allows the program to locate the required chunks and dynamiclly instead of assuming that the `data chunk` is always located at a fixed position.

The WAV file is read as a `.bin` file inside the `bool LoadWav()` function. The audio samples contained in the `data chunk` are extracted, converted from 16-bit integer samples to normalized floating point values, and stored inside the `sound.samples` vector, located inside a `SoundData` struct. Later, this data is processed by the pitch detection algorithm.

## THE AUTOCORRELATION ALGORITHM
The `void AutoCorrelation()` is currently the main part of the pitch detection system.

The algorithm divides the audio signal into fixed-size windows. For each window, different **LAG** values are tested in order to estimate the period od the fundamental frequency,

The best **LAG** is then converted into a frequency:

$$f = \frac{sampleRate}{lag}$$

Finally, the detected frequency us converted into a **MIDI note** number.

For each analyzed window, the **best LAG, estimated frequency and MIDI note** are stored inside a `.txt` file for further analysis.

The current algorithm searches for LAG values corresponding approximately to frequencies between **80HZ and 500HZ**.

### SQUARED DIFFERENCE FUNCTION (SDF)
The current implementation uses **SDF**.

**$$\sum (x[j] - x[j + \text{lag}])^2$$**</br>

The function compares the original signal with the delayed version of itself.

When delay corresponds to the period of the fundamental frequency, the two signals become similar, producing a low value of the SDF.

Therefore, the objetive is to find the **global minimum** within the allowed LAG range.

### STANDARD AUTOCORRERLATION (ACF)
Another common aproach is the standard AutoCorrelation function:

**$$\sum (x[j] \cdot x[j + \text{lag}])$$**</br>

Instead of mesutring the difference between the original and delayed signals, autocorrelation measures their similarity.

For periodic signals, the correct period tends to produce a high correlation value, s othe objetive in this case is to find the **maximum**.

### YIN Algorithm (global standard)
**YIN** is a widely used pitch detection algorithm that builds upon the same general idea of the SDF approach.

The next stages of this project will include studying and implementing YIN in order to compare results with the current **SDF Implementation**.

## NEXT STEPS
The next major step is implementing **real-time frequency and MIDI note detection**.

This will allow the pitch detection system to process microphone input in real time, making it suitable for applications such as the planned _karaoke game_, initially developed using **Unity** Engine.

In the longer term. the knowledge and code developed in this project will contribute ti the development of a custom **audio engine**, which could and will be integrated into the proprietary game/graphics engine being developed as part of the [Learn OpenGL](https://github.com/FulgencioPedre/LearnOpenGL) project.

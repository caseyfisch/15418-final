# 15418-final
Parallel Audio Recognition by umalik and cjfische

Check out our [project site](https://caseyfisch.github.io/15418-final-project/) if you want.

## About
Here are the essentials for our final project, Kazam! Parallel Audio Recognition.  We developed our own audio fingerprinting algorithm to determine the similarity between two audio files.  This repository contains the following files:

* `generate_histogram.c` : This is our sequential version for generating audio fingerprints.
* `sequential_compare_query.cpp` : This is our first sequential version of finding a match between a query song and the database.
* `parallel_compare_query.cpp` : This is our first parallel version of finding a match.  
* `sequential_service_queries.cpp` : This is our second (and better) sequential version of finding a match.
* `parallel_service_queries.cpp` : This is our parallel version of finding a match.  It's fueled by OpenMP threads.  Nice.
* `./database_songs/` : This is our database of songs!  A whopping 100 manually produced fingerprints.
* `dbpaths*.txt` : These are databases of different sizes.  These files can be fed into any of the `*_compare_query.cpp` or `*_service_queries.cpp` files, as both the database or the batch of queries, or you can make your own.

## Build
Sorry, we don't really know how to do Makefiles, so here are the commands that you can use to compile our files!

### generate_histogram.c
Note that this file requires libsndfile and FFTW3.  We installed these libraries on our mac with the following commands:

To install libsndfile:
```
brew install libsndfile
```
[libsndfile](http://www.mega-nerd.com/libsndfile/) is a library for reading the raw audio in WAV files.  Since generate_histogram.c reads a WAV file and constructs its audio fingerprint, this library is an essential part of our algorithm.

To install FFT3:
```
brew install fftw
```
[FFTW](http://www.fftw.org/) is a long-standing C library for performing all kinds of Fourier transforms.  All kinds.  Since it's useful for us to have the frequency domain of an audio signal, instead of its time domain signal, we chose to use this library instead of implementing our own fast Fourier transform library.

Okay, to actually compile:
If you've installed the above libraries on your computer, you can run the following command to compile the file:
```
gcc generate_histogram.c -lsndfile -lfftw3 -lm -o sequential_generate
```

If you're on GHC, the following command works, too (Sorry, it's a long one :-( ):
```
gcc -std=c99 generate_histogram.c -I/afs/cs.cmu.edu/academic/class/15418-s16/public/libsndfile-1.0.26/include -Iafs/cs.cmu.edu/academic/class/15418-s16/public/fftw-3.3.4/include -L/afs/cs.cmu.edu/academic/class/15418-s16/public/libsndfile-1.0.26/lib -L/afs/cs.cmu.edu/academic/class/15418-s16/public/fftw-3.3.4/lib -lsndfile -lfftw3 -lm -o sequential_generate
```

To run the executable, execute this command:
```
./sequential_generate path/to/wav/file song_name_and_artist path/to/where/you/want/to/store/the/output.out
```
Make sure you fill in the command arguments with the proper paths and you should get an output file filled with a ton of floating point numbers!  We suggest saving the output in `database_songs/`.  Up to you, though.

For example:
```
./sequential_generate IntoYou.wav IntoYou-ArianaGrande IntoYou.out 
```

### sequential_compare_query.cpp

This file doesn't require anything special.  Here's the command to compile:
```
g++ -m64 -std=c++11 sequential_compare_query.cpp -o sequential_compare
```

And to run:
```
./sequential_compare path/to/textfile/of/database/paths path/to/audiofingerprint/of/query
```

For example:
```
./sequential_compare dbpaths100.txt ./database_songs/taro.out
```

You should see output like this:
```
===== RESULTS =====

Computed in : 2805.455 ms
Best match: taro at 99.4695% similar
```

### parallel_compare_query.cpp

### sequential_service_queries.cpp

### parallel_service_queries.cpp

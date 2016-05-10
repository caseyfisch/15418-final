# 15418-final
Parallel Audio Recognition by umalik and cjfische

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

If you search for a song that's not in the database, the program outputs the "most similiar" song, according to our metric of similarity.
```
./sequential_compare dbpaths90.txt ./database_songs/sunny-day.out
===== RESULTS =====

Computed in : 2515.913 ms
Best match: Dark-Necessities,RHCP at 68.8816% similar
```

### parallel_compare_query.cpp

This program uses OpenMP, so we have to compile with the library flag for OpenMP.  No sweat.
```
g++ -m64 -std=c++11 parallel_compare_query.cpp -fopenmp -o parallel_compare
```

Run it exactly as you would the sequential version:
```
./parallel_compare dbpaths100.txt ./database_songs/taro.out
===== RESULTS =====

Computed in : 2837.171 ms
Best match: taro at 99.4846% similar
```

As you might notice, this version takes as long as (if not longer than) the sequential version, which is why we have `sequential_service_queries.cpp` and `parallel_service_queries.cpp`.  Let's check out those!

### sequential_service_queries.cpp

So this match implementation actually loads our entire database up front, then performs any batch of queries you want to lookup, which is much more efficient that using file I/O to stream the comparison line by line for each file.  

To compile:
```
g++ -m64 -std=c++11 sequential_service_queries.cpp -o sequential_service
```

The command line arguments are a little different for this implementation.  
```
./sequential_service path/to/database path/to/batch/of/queries
```

Here's an example.  We can actually feed the `dbpath` files into the executable because they're formatted the right way and contain the same data as a batch of queries would.
```
./sequential_service dbpaths100.txt dbpaths10.txt 
Loading database...
Done reading database!
Getting query fingerprint 0...
Best Match: breezeblocks, alt-j at 99.98% similarity
Computed in : 68.876 ms
 ---- 
Getting query fingerprint 1...
Best Match: everyotherfreckle, alt-j at 99.0156% similarity
Computed in : 66.649 ms
 ---- 
Getting query fingerprint 2...
Best Match: fitzpleasure, alt-j at 99.0153% similarity
Computed in : 66.471 ms
 ---- 
Getting query fingerprint 3...
Best Match: hungerofthepine, alt-j at 98.2377% similarity
Computed in : 67.844 ms
 ---- 
Getting query fingerprint 4...
Best Match: interlude, alt-j at 99.9357% similarity
Computed in : 22.693 ms
 ---- 
Getting query fingerprint 5...
Best Match: lefthandfree, alt-j at 99.8093% similarity
Computed in : 53.955 ms
 ---- 
Getting query fingerprint 6...
Best Match: matilda, alt-j at 99.9802% similarity
Computed in : 66.755 ms
 ---- 
Getting query fingerprint 7...
Best Match: taro at 99.4846% similarity
Computed in : 70.558 ms
 ---- 
Getting query fingerprint 8...
Best Match: somethinggood, alt-j at 98.0752% similarity
Computed in : 61.991 ms
 ---- 
Getting query fingerprint 9...
Best Match: warmfoothills, alt-j at 99.8765% similarity
Computed in : 61.526 ms
 ---- 
TOTAL TIME ELAPSED for 10 QUERIES: 0.607317 s
```

We get a better idea of how long the queries actually take to process in this implementation compared to the first implementation.

### parallel_service_queries.cpp

The parallel version of this implementation is also powered by OpenMP, so we compile with the same flag as before:
```
g++ -m64 -std=c++11 parallel_service_queries.cpp -fopenmp -o parallel_service
```

Same arguments as the sequential version:
```
./parallel_service dbpaths100.txt dbpaths10.txt 
Loading database...
Done reading database!
Getting query fingerprint 0...
Best Match: breezeblocks, alt-j at 99.98% similarity
Computed in : 22.403 ms
 ---- 
Getting query fingerprint 1...
Best Match: everyotherfreckle, alt-j at 99.0156% similarity
Computed in : 21.012 ms
 ---- 
Getting query fingerprint 2...
Best Match: fitzpleasure, alt-j at 99.0153% similarity
Computed in : 22.333 ms
 ---- 
Getting query fingerprint 3...
Best Match: hungerofthepine, alt-j at 98.2377% similarity
Computed in : 21.593 ms
 ---- 
Getting query fingerprint 4...
Best Match: interlude, alt-j at 99.9357% similarity
Computed in : 7.715 ms
 ---- 
Getting query fingerprint 5...
Best Match: lefthandfree, alt-j at 99.8093% similarity
Computed in : 17.057 ms
 ---- 
Getting query fingerprint 6...
Best Match: matilda, alt-j at 99.9802% similarity
Computed in : 21.674 ms
 ---- 
Getting query fingerprint 7...
Best Match: taro at 99.4846% similarity
Computed in : 25.446 ms
 ---- 
Getting query fingerprint 8...
Best Match: somethinggood, alt-j at 98.0752% similarity
Computed in : 21.353 ms
 ---- 
Getting query fingerprint 9...
Best Match: warmfoothills, alt-j at 99.8765% similarity
Computed in : 23.036 ms
 ---- 
TOTAL TIME ELAPSED for 10 QUERIES: 0.203622 s
```
We can see the speedup with the parallel version! Nice!

## For More Information...

Check out our [project site](https://caseyfisch.github.io/15418-final-project/)!

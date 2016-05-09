#include <stdio.h>
#include <stdlib.h>
#include <sndfile.h>
#include <math.h>
#include <fftw3.h>
#include <complex.h>
#include <time.h>
#include <omp.h>

int bin_ranges[] = {0.0, 100.0, 200.0, 300.0, 400.0, 500.0, 600.0, 700.0, 800.0, 900.0, 1000.0};
int num_bins = 10;


// determine what bin the frequency falls into in the histogram
int get_bin_index(float frequency) {
  int bin = 0;
  if (frequency > bin_ranges[9]) {
    return -1;
  }
  while (frequency > bin_ranges[bin]) {
    bin++;
  }

  return bin;
}


// construct histrogram from fft data
// frequencies are mapped into bins, and their magnitudes are accumulated for the
// bin's value
float* construct_histogram(float *freq, float* mags, int len) {
  float* histogram = (float*)malloc(sizeof(float) * num_bins);
  for (int i = 0; i < num_bins; i++) {
    histogram[i] = 0;
  }

  for (int i = 0; i < len; i++) {
    int bin_index = get_bin_index(freq[i]);
    if (bin_index == -1) continue;
    histogram[bin_index] += mags[i];
  }

  return histogram;
}


// normalize histogram values to create a probability distribution basically
void normalize_histogram(float *histogram) {
  float histogram_sum = 0;
  for (int i = 0; i < num_bins; i++) {
    histogram_sum += histogram[i];
  }

  for (int i = 0; i < num_bins; i++) {
    if (histogram_sum == 0) {
      histogram[i] = (float)(1/num_bins);
    } else {
      histogram[i] = histogram[i] / histogram_sum;
    }
  }

}

int main(int argc, char* argv[]) {

  if (argc < 4) {
    printf("not enough args");
    exit(-1);
  }

  clock_t start;
  
  start = clock();
  double duration;

  char* song_path_arg = argv[1];
  char* song_name_arg = argv[2];
  char* output_file_name_arg = argv[3];

  SNDFILE *sf;
  SF_INFO info;
  int num_channels;
  int num, num_items;
  float *buf;
  int f, sr, c;
  int i, j;
  FILE *out;

  /* open wav file */
  info.format = 0;
  sf = sf_open(song_path_arg, SFM_READ, &info);

  if (sf == NULL) {
    printf("failed to open file\n");
    exit(-1);
  }

  /* print info about file */
  f = info.frames;
  sr = info.samplerate;
  c = info.channels;

  printf("frames = %d\n", f);
  printf("sample rate = %d\n", sr);
  printf("channels = %d\n", c);

  num_items = f * c;
  printf("num items = %d\n", num_items);

  /* allocate space to read data and read it */
//  buf = (float*)malloc(sizeof(float) * num_items);
//  num = sf_read_float(sf, buf, num_items);

  int N = pow(2, 11);
  double samples[N];
  double stereo_samples[2 * N];
  int num_samples = N;
  out = fopen(output_file_name_arg,"w");
  fprintf(out, "%s\n", song_name_arg);
  
  for (i = 0; i < num_items; i += N) {
//    printf("%d \t", i);
    if (c == 2) {  // if there are two channels, we're just going to average them
      num = sf_read_double(sf, stereo_samples, num_samples * 2);
      for (j = 0; j < num - 1; j++) {
        samples[j / 2] = (stereo_samples[j] + stereo_samples[j + 1]) / 2; 
      }

      num = num / 2;      

    } else {
      num = sf_read_double(sf, samples, num_samples);
    }

    if (num == num_samples) {

    fftw_complex fft_output[N];
    fftw_plan p;
    p = fftw_plan_dft_r2c_1d(N, samples, fft_output, FFTW_ESTIMATE);

    fftw_execute(p);
    fftw_destroy_plan(p);

    double bin_size = sr / N; 

    float *frequencies = (float*)malloc(sizeof(float) * N/2);
    float *magnitudes  = (float*)malloc(sizeof(float) * N/2);

    for (int k = 0; k < N/2; k++) {
      frequencies[k] = k * bin_size;
      magnitudes[k]  = sqrt(fft_output[k][0] * fft_output[k][0]
                         + fft_output[k][1] * fft_output[k][1]);

//      fprintf(out, "%f %f\n", i*bin_size, sqrt(fft_output[i][0] * fft_output[i][0] 
//                         + fft_output[i][1] * fft_output[i][1]));
    }

    float* h = construct_histogram(frequencies, magnitudes, N/2);
    normalize_histogram(h);

    for (int m = 0; m < num_bins; m++) {
      fprintf(out, "%f ", h[m]);           
    }
    fprintf(out, "\n");
    
    free(frequencies);
    free(magnitudes);
//    free(h);
    }
  }

  fclose(out);

  sf_close(sf);


  duration = (clock() - start) / (double) CLOCKS_PER_SEC;
  //printf("Read %d items\n", num);

  printf("Computed in %f seconds \n", duration);

  return 0;

}

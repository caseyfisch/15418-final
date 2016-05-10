#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <omp.h>
#include "CycleTimer.h"

int main(int argc, char* argv[]) {
  // // std::cout << "argvc = " << argc << std::endl;
  // for (int i = 0; i < argc; i++) {
  //   std::cout << "argv[" << i << "] = " << argv[i] << std::endl;
  // }

  if (argc < 3) {
    printf("Not enough arguments");
    exit(-1);
  }
  
  std::cout << omp_get_max_threads() << std::endl;	

  double startTime = CycleTimer::currentSeconds();

  // Get text file that has the paths to all our directories
  std::string db_paths_text_path = argv[1];
  std::string query_song_path = argv[2];

  std::string line;
  std::ifstream pathfile(db_paths_text_path);
  
  // First line of text file must be number of songs in file
  int num_db_songs = 0;
  getline(pathfile, line);
  num_db_songs = atoi(line.c_str());

  // std::cout <<" num_db_songs " << num_db_songs << std::endl;

  // Create array to store file names to iterate through later
  char **db_paths = (char**)malloc(sizeof(char*) * num_db_songs);
 
  int i = 0;  
  while (getline(pathfile, line) && i < num_db_songs) {
    char *line_arr = new char[100];
    strcpy(line_arr, line.c_str());

    db_paths[i] = line_arr;
    i++;
  }

  // read in each file
  // compare each line to query file
  // calculate similarity

  // store query song's similarity to each db song
  float *similarities = (float*)malloc(sizeof(float) * num_db_songs);
  char **songnames = (char**)malloc(sizeof(char *) * num_db_songs);  
  int *timestepcounts = (int*)malloc(sizeof(int) * num_db_songs);
  bool notenoughdata = false;

  // int num_threads = omp_get_max_threads();
  // std::cout <<"threads:  " << num_threads << std::endl;



  for (int i = 0; i < num_db_songs; i++) {
    char* current_db_path = db_paths[i];

    // read each line 
    std::ifstream dbsong(current_db_path);
    std::ifstream querysong(query_song_path);

    std::string db_line;
    std::string q_line;

    // first line in DB song is the song title
    char *db_line_arr = new char[100];
    char *title = new char[100];
    getline(dbsong, db_line);
    strcpy(title, db_line.c_str());

    songnames[i] = title;
    // std::cout <<" " << songnames[i] << std::endl;

    // std::cout << "Comparing with " << db_line << "..." << std::endl;

    // TODO(fix this for query songs -- first line is still song title)
    char *q_line_arr = new char[100];
    getline(querysong, q_line);

    // strcpy(db_line_arr, db_line.c_str());
    // songnames[i] = db_line;
    //std::cout <<" initialized stuff 2" << std::endl;

    int num_time_steps = 0;


    while (getline(dbsong, db_line) && getline(querysong, q_line)) {
     //std::cout << "within while" << std::endl;
      // Take the first line of each file and stuff into these arrays
      strcpy(db_line_arr, db_line.c_str());
      strcpy(q_line_arr, q_line.c_str());

      // Prepare to tokenize some stuff!
      char *db_pch, *q_pch;

      // Allocate memory for histogram bin data
      float* db_histogram = (float*)malloc(sizeof(float)*10);
      float* q_histogram = (float*)malloc(sizeof(float)*10);

      for (int b = 0; b < 10; b++) {
        db_histogram[b] = 0;
        q_histogram[b] = 0;
      }

      int j = 0;
      db_pch = strtok(db_line_arr, " ");
      // Populate histogram bin data for db song and query song
      while (db_pch != NULL && j < 10) {
        db_histogram[j] = std::stof(db_pch);
        db_pch = strtok(NULL, " ");
        j++;
      }

      j = 0;
      q_pch  = strtok(q_line_arr, " ");
      while (q_pch != NULL && j < 10) {
        q_histogram[j] = std::stof(q_pch);
        q_pch = strtok(NULL, " ");
        j++;
      }

      // calculate similarity for current time step's histogram bins
      for (int k = 0; k < 10; k++) {
        similarities[i] += std::min(db_histogram[k], q_histogram[k]);
      }

      num_time_steps++;
      free(db_histogram);
      free(q_histogram);

    } // while
    timestepcounts[i] = num_time_steps;

    if (num_time_steps < 108) { // fewer than 5 seconds to compare
      notenoughdata = true;
      // break;
    } 


  } // for

  double endTime = CycleTimer::currentSeconds();
  double overallDur = endTime - startTime;

  // std::cout << std::endl;
  // std::cout << "===== RESULTS =====" << std::endl;

  std::cout << std::endl;
  printf("Computed in : %.3f ms\n", 1000.f * overallDur);

  if (notenoughdata) {
    std::cout << "Sorry, we need a longer file to find a match" << std::endl;

  } else {
    float max_sim = 0;
    int max_index = 0;
    for (int i = 0; i < num_db_songs; i++) {
      //std::cout <<" " << songnames[i]<< std::endl;
      if (similarities[i] > max_sim) {
         max_sim = similarities[i];
         max_index = i;
      }
    }

    float percent = max_sim / timestepcounts[max_index];
//    std::cout << max_sim << ", " << timestepcounts[max_index] << std::endl;

    std::cout << "Best match: " << songnames[max_index] << " at " << percent * 100 << "% similar" << std::endl;
  }
  std::cout << std::endl;

  free(db_paths);
  free(similarities);
  free(songnames);
  free(timestepcounts);
  
  return 0;
}

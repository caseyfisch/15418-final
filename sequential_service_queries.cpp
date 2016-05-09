#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include "CycleTimer.h"

typedef struct {
  float** fingerprint;
  int query_num_lines;
} query_info;

void free_query_struct(query_info *q) {
  int num_lines = q->query_num_lines;
  for (int i = 0; i < num_lines; i++) {
    free(q->fingerprint[i]);
  }
  free(q->fingerprint);
  free(q);
}

void free_database(float*** db, int num_db_songs, int *N) {
  for (int i = 0; i < num_db_songs; i++) {
    for (int row = 0; row < N[i]; row++) {
      free(db[i][row]);
    }
    free(db[i]);
  }
  free(db);
}

double match_query(float ***database, float **query, int query_length, int *N, int num_db_songs, char** songnames) {
  double startTime = CycleTimer::currentSeconds();

  // Keep track of how similar query is to each song in DB
  float *similarities = (float*)malloc(sizeof(float) * num_db_songs);

  for (int i = 0; i < num_db_songs; i++) {
    similarities[i] = 0;
  }

  bool stop_comparing = false;

  for (int i = 0; i < num_db_songs; i++) {

    for (int row = 0; row < N[i]; row++) {
      if (row > query_length - 1) {
        stop_comparing = true;
        break;

      } else {
        for (int col = 0; col < 10; col++) {
          similarities[i] += std::min(database[i][row][col], query[row][col]);
        }
      }
    }
  }

  float max_sim = 0;
  int max_index = 0;
  for (int i = 0; i < num_db_songs; i++) {
    if (similarities[i] > max_sim) {
      max_sim = similarities[i];
      max_index = i;
    }
  }
  
  float percent = max_sim / N[max_index];

  double endTime = CycleTimer::currentSeconds();
  double overallDur = endTime - startTime;

  std::cout << "Best Match: " << songnames[max_index] << " at " << percent * 100 << "% similarity" << std::endl;
  printf("Computed in : %.3f ms\n", 1000.f * overallDur);
  std::cout << " ---- " << std::endl;

  free(similarities);
  return overallDur;
}

query_info * get_query_fingerprint(std::string query_path) {
  // Read our query path file into histogram
  std::ifstream qsong(query_path);
  std::string q_line;
  
  int num_lines_in_file = 0;
  while (getline(qsong, q_line)) {
    num_lines_in_file++;
  }

  float** query_fingerprint = (float**)malloc(sizeof(float*) * num_lines_in_file - 1);

  // First line in file is song title
  std::ifstream newqsong(query_path);
  std::string new_q_line;
  getline(newqsong, new_q_line);
  char* new_q_line_arr = new char[100];

  int num_time_steps = 0;
  while (getline(newqsong, new_q_line) && num_time_steps < num_lines_in_file - 1) {
    strcpy(new_q_line_arr, new_q_line.c_str());
    
    char *q_pch;
    float *q_histogram = (float*)malloc(sizeof(float) * 10);

    for (int k = 0; k < 10; k++) {
      q_histogram[k] = 0;
    }

    int m = 0;
    q_pch = strtok(new_q_line_arr, " ");

    while (q_pch != NULL && m < 10) {
      q_histogram[m] = std::stof(q_pch);
      q_pch = strtok(NULL, " ");
      m++;
    }

    query_fingerprint[num_time_steps] = q_histogram;
    num_time_steps++;
  }

  int query_time_steps = num_time_steps;

  query_info *q = (query_info*)malloc(sizeof(query_info));

  q->fingerprint = query_fingerprint;
  q->query_num_lines = query_time_steps;
  
  return q;
}

int main (int argc, char* argv[]) {
  if (argc < 3) {
    printf("Not enough arguments\n");
    exit(-1);
  }

  printf("Loading database...\n");
  // Get path to list of database song paths
  std::string db_paths_text_path = argv[1];

  // Create array to store files names to iterate through later
  std::string line;
  std::ifstream pathfile(db_paths_text_path);

  // First line of text file must be num songs in file
  int num_db_songs = 0;
  getline(pathfile, line);
  num_db_songs = atoi(line.c_str());

  char **db_paths = (char**)malloc(sizeof(char*) * num_db_songs);

  int i = 0;
  while (getline(pathfile, line) && i < num_db_songs) {
    char *line_arr = new char[100];
    strcpy(line_arr, line.c_str());

    db_paths[i] = line_arr;
    i++;
  }

  float ***database = (float***)malloc(sizeof(float**) * num_db_songs);

  char **songnames = (char**)malloc(sizeof(char*) * num_db_songs);
  int *numtimesteps = (int*)malloc(sizeof(int) * num_db_songs);

  // Iterate over files to populate audio fingerprint array
  for (int j = 0; j < num_db_songs; j++) {
    // Read through file once to count number of lines

    char* current_db_path = db_paths[j];
    std::ifstream dbsong(current_db_path);
    std::string db_line;
    
    int num_lines_in_file = 0;
    while (getline(dbsong, db_line)) {
      num_lines_in_file++;
    }

//    std::cout << "Num lines in file " << j << ": " << num_lines_in_file << std::endl;

    // Now that we know how many lines are the file, we can allocate array!!!

    float** fingerprint = (float**)malloc(sizeof(float*) * num_lines_in_file - 1);
    
    std::ifstream newdbsong(current_db_path);
    std::string new_db_line;
    char* new_db_line_arr = new char[100];

    // First line in file is song title
    char* title = new char[100];
    getline(newdbsong, new_db_line);
    strcpy(title, new_db_line.c_str());
    
    songnames[j] = title;

    int num_time_steps = 0;

    // Read the file for real this time
    while (getline(newdbsong, new_db_line) && num_time_steps < num_lines_in_file - 1) {
      strcpy(new_db_line_arr, new_db_line.c_str());

      char *db_pch;
      float *db_histogram = (float*)malloc(sizeof(float) * 10); // hardcoded

      for (int k = 0; k < 10; k++) {
        db_histogram[k] = 0;
      }

      int m = 0;
      db_pch = strtok(new_db_line_arr, " ");
      // Populate histogram bin data for db song
      while (db_pch != NULL && m < 10) {
        db_histogram[m] = std::stof(db_pch);
        db_pch = strtok(NULL, " ");
        m++;
      }

      fingerprint[num_time_steps] = db_histogram;
      num_time_steps++;
    }

    numtimesteps[j] = num_time_steps;
    database[j] = fingerprint;
  }
  
  printf("Done reading database!\n");

/*
  for (int q = 0; q < num_db_songs; q++) {
    for (int row = 0; row < numtimesteps[q]; row++) {
      for (int col = 0; col < 10; col++) {
        printf("%f\t", database[q][row][col]);
      }
      printf("\n");
    }
    printf("----------\n");
  }
  printf("done!\n");
*/


  std::string query_path_text = argv[2];
  // Create array to store query path names
  std::string q_line;
  std::ifstream queryfilesfile(query_path_text);

  // First line of text file must be num queries in file
  int num_queries = 0;
  getline(queryfilesfile, q_line);
  num_queries = atoi(q_line.c_str());

  char **queries = (char**)malloc(sizeof(char*) * num_queries);
  int qsongs = 0;
  while (getline(queryfilesfile, q_line) && qsongs < num_queries) {
    char *q_line_arr = new char[100];
    strcpy(q_line_arr, q_line.c_str());
    queries[qsongs] = q_line_arr;
    qsongs++;
  }

  double total_time = 0;
  double single_query_time = 0;

  for (int k = 0; k < num_queries; k++) {
    printf("Getting query fingerprint %d...\n", k);
    std::string query_path = queries[k];

    query_info *my_query = get_query_fingerprint(query_path);

    if (my_query->query_num_lines < 100) {
      printf("Sample not long enough to determine match\n");
    } else {
      // match
      single_query_time = match_query(database, my_query->fingerprint, my_query->query_num_lines, 
                             numtimesteps, num_db_songs, songnames);
      total_time += single_query_time;
    }
  
    free_query_struct(my_query);
  }

  std::cout << "TOTAL TIME ELAPSED for " << num_queries << " QUERIES: " << total_time << std::endl;
  free_database(database, num_db_songs, numtimesteps);
  free(numtimesteps);
  free(songnames);
  free(db_paths);
  free(queries);
  return 1;
}

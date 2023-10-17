#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/stat.h>

#define MAX_FILES 1000000

// Structure to hold the data for each file fragment
typedef struct {
  char * data;
  int size;
}
FileFragment;

// Global array to hold the file fragments
FileFragment fragments[MAX_FILES];

// Mutex to protect the shared data structure
pthread_mutex_t mutex;

// Thread function to traverse a directory and concatenate the files
void * concatenate_files(void * path) {
  DIR * dir = opendir((char * ) path);

  if (dir == NULL) {
    fprintf(stderr, "Error opening directory: %s\n", (char * ) path);
    return NULL;
  }

  struct dirent * entry;
  while ((entry = readdir(dir)) != NULL) {
    if (strcmp(entry -> d_name, ".") == 0 || strcmp(entry -> d_name, "..") == 0) {
      continue;
    }

    char * subpath = malloc(strlen((char * ) path) + strlen(entry -> d_name) + 2);
    sprintf(subpath, "%s/%s", (char * ) path, entry -> d_name);

    struct stat sb;
    if (stat(subpath, & sb) == -1) {
      fprintf(stderr, "Error statting file: %s\n", subpath);
      continue;
    }

    // Only process regular files with the .bin extension
    if (S_ISREG(sb.st_mode)) {
      char * ext = strrchr(entry -> d_name, '.');
      if (ext != NULL && strcmp(ext, ".bin") == 0) {
        // Found a .bin file, extract the number from the
        int num = atoi(entry -> d_name);

        // Open the file and read the data
        FILE * fp = fopen(subpath, "rb");
        if (fp == NULL) {
          fprintf(stderr, "Error opening file: %s\n", subpath);
          continue;
        }

        fseek(fp, 0, SEEK_END);
        int size = ftell(fp);
        fseek(fp, 0, SEEK_SET);

        char * data = malloc(size);
        fread(data, 1, size, fp);
        fclose(fp);

        // Store the data in the global array
        pthread_mutex_lock( & mutex);
        fragments[num].data = data;
        fragments[num].size = size;
        pthread_mutex_unlock( & mutex);
      }
    }
    free(subpath);
  }
  closedir(dir);
  return NULL;
}

int main(int argc, char * argv[]) {
  // Check for the correct number of arguments
  if (argc != 3) {
    fprintf(stderr, "Usage: defrag <directory> <output file>\n");
    return 1;
  }

  // Initialize the mutex
  if (pthread_mutex_init( & mutex, NULL) != 0) {
    fprintf(stderr, "Error initializing mutex\n");
    return 1;
  }

  // Start up a thread to process the top level directory
  pthread_t thread;
  if (pthread_create( & thread, NULL, concatenate_files, argv[1]) != 0) {
    fprintf(stderr, "Error creating thread\n");
    return 1;
  }
  pthread_join(thread, NULL);

  // Concatenate the file fragments and write to the output file
  FILE * fp = fopen(argv[2], "wb");
  if (fp == NULL) {
    fprintf(stderr, "Error opening output file\n");
    return 1;
  }

  for (int i = 0; i < MAX_FILES; i++) {
    if (fragments[i].data != NULL) {
      fwrite(fragments[i].data, 1, fragments[i].size, fp);
      free(fragments[i].data);
    }
  }

  fclose(fp);

  // Clean up resources
  pthread_mutex_destroy( & mutex);

  return 0;
}

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>
#include <dpu.h>
#include <dpu_log.h>
#include "trees.h"

#define BUFFER_SIZE 1000

char* call_home(char * filename)
{
    FILE * fPtr;

    char *out;

    char buffer[BUFFER_SIZE];
    int totalRead = 0;

    fPtr = fopen(filename, "r");

    if (fPtr == NULL)
    {
        printf("Unable to open file %s\n", filename);
        exit(EXIT_FAILURE);
    }

    out = fgets(buffer, BUFFER_SIZE, fPtr);

    fclose(fPtr);

    return out;
}

// #ifndef DPU_BINARY
// #define DPU_BINARY "./src/dpu_program/helloworld"
// #endif

int dpu_test(char* filename) {
  struct dpu_set_t set, dpu;

  const char *DPU_BINARY = filename;

  DPU_ASSERT(dpu_alloc(1, NULL, &set));
  DPU_ASSERT(dpu_load(set, DPU_BINARY, NULL));
  DPU_ASSERT(dpu_launch(set, DPU_SYNCHRONOUS));

  DPU_FOREACH(set, dpu) {
    DPU_ASSERT(dpu_log_read(dpu, stdout));
  }

  DPU_ASSERT(dpu_free(set));

  return 0;
}

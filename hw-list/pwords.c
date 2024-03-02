/*
 * Word count application with one thread per input file.
 *
 * You may modify this file in any way you like, and are expected to modify it.
 * Your solution must read each input file from a separate thread. We encourage
 * you to make as few changes as necessary.
 */

/*
 * Copyright © 2021 University of California, Berkeley
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <pthread.h>

#include "word_count.h"
#include "word_helpers.h"

typedef struct pargs {
  char *filename;
  word_count_list_t *word_counts;
}pargs;

char *new_str(char *str) {
  char *new_string = (char *) malloc(strlen(str) + 1);
  if (new_string == NULL) {
    return NULL;
  }
  return strcpy(new_string, str);
};

void* countword(void* arg){ 
  pargs *thread_arg = (pargs*)arg;
  FILE* infile = fopen(thread_arg->filename, "r");
    if (infile == NULL) {
    fprintf(stderr, "File does not exist.\n");
    exit(1);
  };
  count_words(thread_arg->word_counts, infile);

  fclose(infile);
  pthread_exit(NULL);
};
/*
 * main - handle command line, spawning one thread per file.
 */
int main(int argc, char* argv[]) {
  /* Create the empty data structure. */
  word_count_list_t word_counts;
  init_words(&word_counts);
  if (argc <= 1) {
    /* Process stdin in a single thread. */
    count_words(&word_counts, stdin);
  } else {
    /* 几个文件几个thread，每个thread打开自己的文件，但是对于word_counts改变是并发的
        传参要传入文件名，传入公共变量
    */
    int rc;
    pthread_t threads[argc-1];
    pargs arg[argc-1];

    for(int t = 0; t < argc-1; t++) {
      
      arg[t].word_counts = &word_counts;
      arg[t].filename = new_str(argv[t+1]);
     
      rc = pthread_create(&threads[t], NULL, countword, (void*)&arg[t]);
      
      if(rc) {
      printf("ERROR; return code from pthread_create() is %d\n", rc);
      exit(-1);
      }
    }
    for(int t = 0; t < argc-1; t ++) {
      pthread_join(threads[t], NULL);
    }
  }
  /* Output final result of all threads' work. */
  wordcount_sort(&word_counts, less_count);
  fprint_words(&word_counts, stdout);
  return 0;
}

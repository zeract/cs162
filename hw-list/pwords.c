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
typedef struct{
  word_count_list_t* word_counts;
  FILE* input_file;
}ThreadArgs;

void * pcount_words(void* args) {
  ThreadArgs* thread_args = (ThreadArgs*)args;
  word_count_list_t* wclist = thread_args->word_counts;
  FILE* infile = thread_args->input_file;
  // errors
  if( wclist == NULL || infile == NULL ){
    return 1;
  }
  int num_words = 0;
  int in_word = 0;
  int word_count = 0;
  int c;
  char *word = NULL;
  while((c = fgetc(infile)) != EOF){
    if (isalpha(c)) {
      if (!in_word) {
        num_words++;
        word = (char *)malloc(64 * sizeof(char));
        in_word = 1;
      }
      word[word_count++] = tolower(c);
    } 
    else 
    {
      if( word != NULL){
        word[word_count] = '\0';
        add_word(wclist,word);
        free(word);
        word = NULL;
      }
      word_count = 0;
      in_word = 0;
    }
  }
  return 0;
}

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
    /* TODO */
    int nthreads = argc-1;
    pthread_t threads[nthreads];
    for (long t = 0; t < nthreads; t++) {
      FILE* infile = NULL;
      infile = fopen(argv[t+1],"r");
      if (infile == NULL){
        fprintf(stderr, "File does not exist.\n");
        return 1;
      }
      ThreadArgs thread_args = {&word_counts,infile};
      
      int rc = pthread_create(&threads[t], NULL, (void *)pcount_words, &thread_args);
      if (rc) {
        printf("ERROR; return code from pthread_create() is %d\n", rc);
        exit(-1);
      }
    }
    for (long t = 0; t < nthreads; t++) {
      pthread_join(threads[t], NULL);
    }
    
  }
  
  /* Output final result of all threads' work. */
  wordcount_sort(&word_counts, less_count);
  fprint_words(&word_counts, stdout);
  if(argc > 1){
    /* Last thing that main() should do */
    pthread_exit(NULL);
  }
  return 0;
}

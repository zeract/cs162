/*
 * Implementation of the word_count interface using Pintos lists and pthreads.
 *
 * You may modify this file, and are expected to modify it.
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
#define PINTOS_LIST
#define PTHREADS
#ifndef PINTOS_LIST
#error "PINTOS_LIST must be #define'd when compiling word_count_lp.c"
#endif

#ifndef PTHREADS
#error "PTHREADS must be #define'd when compiling word_count_lp.c"
#endif

#include "word_count.h"

void init_words(word_count_list_t* wclist) { /* TODO */
  list_init(&wclist->lst);
  pthread_mutex_init(&(wclist->lock),NULL);
}

size_t len_words(word_count_list_t* wclist) {
  /* TODO */
  pthread_mutex_lock(&wclist->lock);
  struct list_elem *e;
  int count = 0;
  for(e = list_begin (&wclist->lst); e != list_end (&wclist->lst);e = list_next (e)){
    count += 1;
  }
  pthread_mutex_unlock(&wclist->lock);
  return count;
}

word_count_t* find_word(word_count_list_t* wclist, char* word) {
  /* TODO */
  pthread_mutex_lock(&wclist->lock);
  struct list_elem *e;
  for(e = list_begin (&wclist->lst); e != list_end (&wclist->lst);e = list_next (e)){
    word_count_t *w = list_entry(e,word_count_t,elem);
    if(strcmp(w->word,word)==0){
      pthread_mutex_unlock(&wclist->lock);
      return w;
    }
  }
  pthread_mutex_unlock(&wclist->lock);
  return NULL;
}

word_count_t* add_word(word_count_list_t* wclist, char* word) {
  /* TODO */
  pthread_mutex_lock(&wclist->lock);
  struct list_elem *e;
  for(e = list_begin (&wclist->lst); e != list_end (&wclist->lst);e = list_next (e)){
    word_count_t *w = list_entry(e,word_count_t,elem);
    if(strcmp(w->word,word)==0){
      w->count++;
      pthread_mutex_unlock(&wclist->lock);
      return w;
    }
  }
  word_count_t *node = (word_count_t*)malloc(sizeof(word_count_t));
  node->word = (char *) malloc(strlen(word) + 1);
  if(node->word == NULL){
    pthread_mutex_unlock(&wclist->lock);
    return NULL;
  }
  strcpy(node->word,word);
  node->count = 1;
  list_push_back(&wclist->lst,&node->elem);
  pthread_mutex_unlock(&wclist->lock);
  return node;
}

void fprint_words(word_count_list_t* wclist, FILE* outfile) { /* TODO */
  if(wclist==NULL || outfile==NULL){
    fprintf(stderr, "Fprint_words() error.\n");
    return;
  }
  pthread_mutex_lock(&wclist->lock);
  struct list_elem *e;
  for(e = list_begin (&wclist->lst); e != list_end (&wclist->lst);e = list_next (e)){
    word_count_t *wc = list_entry(e,word_count_t,elem);
    fprintf(outfile, "%i\t%s\n", wc->count, wc->word);
  }
  pthread_mutex_unlock(&wclist->lock);
}

static bool less_list(const struct list_elem* ewc1, const struct list_elem* ewc2, void* aux) {
  /* TODO */
  word_count_t *wc1 = list_entry(ewc1,word_count_t,elem);
  word_count_t *wc2 = list_entry(ewc2,word_count_t,elem);
  if(wc1->count!=wc2->count){
    return wc1->count < wc2->count;
  }

  return strcmp(wc1->word,wc2->word) < 0;
}

void wordcount_sort(word_count_list_t* wclist,
                    bool less(const word_count_t*, const word_count_t*)) {
  /* TODO */
  pthread_mutex_lock(&wclist->lock);
  list_sort(&wclist->lst, less_list, less);
  pthread_mutex_unlock(&wclist->lock);
}

#include <stdio.h>
#include <stdlib.h>
#define INITIAL_HEAP 100
#define HEAP_STEP 10

size_t heap_size;
char* heap;     /* heap */
char* p;        /* current heap pointer */
char* progmem;  /* instruction buffer */
char* ip;       /* instruction pointer */

/*
 * Resizes the the heap
 */
void resize_heap(size_t old_size, size_t new_size) {
  heap = realloc(heap, new_size);
  /* clear out new heap space */
  while(old_size < new_size)
    heap[old_size++] = 0;
}

/*
 * Advances the instruction pointer after the matching parenthesis
 */
void seek_match() {
  int depth = 1;
  while(depth > 0) {
    ip++;
    if(*ip == '[') depth++;
    if(*ip == ']') depth--;
  }
  ip++;
}

/*
 * Interprets the program. The parameter rp specifies the
 * address of the last '[' character. Used for jumping back in loops.
 * Gets called recursively
 */
void interpret(char* rp) {
  char instr;
  while((instr = *(ip++))) {
      switch(instr) {
          case '+':
              (*p)++;
              break;

          case '-':
              (*p)--;
              break;

          case '>':
              if(p-heap+1 >= heap_size) {
                resize_heap(heap_size, heap_size + HEAP_STEP);
                heap_size += HEAP_STEP;
              }
              p++;
              break;

          case '<':
              p--;
              break;

          case '.':
              putchar(*p);
              break;

          case ',':
              *p = getchar();
              break;

          case '[':
              /* special case if we need to skip loop. Seek to corresponding ']' */
              if(*p == 0)
                seek_match();

              /* Normal case, evaluate the loop. Call recursively and put the
               * current address onto the stack for looping back */
              else
                interpret(ip);
              break;

          case ']':
              /* If 0, we loop is finished. give control back to parent */
              if(*p == 0)
                return;
              /* Not 0 yet. jump back to beginning of loop */
              else
                ip = rp;
              break;
      }
  }
}


/*
 * Program that interprets a Brainfuck program given
 * by command line attribute
 */
int main(int argc, char *argv[]) {

    FILE *fp;
    size_t f_size;

    if(argc <= 1) fputs("No file specified", stderr), exit(1);

    /* open file */
    fp = fopen (argv[1] , "rb");
    if(!fp) perror(argv[1]), exit(1);

    /* get file size */
    fseek(fp , 0L , SEEK_END);
    f_size = ftell( fp );
    rewind(fp);

    /* create program memory */
    progmem = (char*)calloc(f_size+1, 1);
    /* set instruction pointer to beginning of instruction memory */
    ip = progmem;

    /* read the file in */
    fread(progmem, 1, f_size, fp);
    fclose(fp);

    /* initialize heap */
    heap_size = INITIAL_HEAP;
    heap = (char*)calloc(heap_size, 1);
    p = heap;

    interpret(0);

    /* release heap */
    free(heap);
    free(progmem);
    exit(0);
}

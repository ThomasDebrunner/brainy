#include <stdio.h>
#include <stdlib.h>
#define INITIAL_HEAP 100
#define HEAP_STEP 10


/*
 * Resizes the given heap
 */
void resize_heap(char *heap, size_t old_size, size_t new_size) {
  realloc(heap, new_size);
  /* clear out new heap space */
  while(old_size < new_size)
    heap[old_size++] = 0;
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

    /* read file in */
    char* inst_buff = (char*)calloc(f_size+1, 1);

    fread(inst_buff, 1, f_size, fp);
    fclose(fp);

    /* create heap */
    size_t heap_size = INITIAL_HEAP;
    int pos = 0;
    char* heap = (char*)calloc(heap_size, 1);

    /* set instruction pointer to beginning */
    char* instr_p = inst_buff;

    /* interpret the file */
    char instr;
    while((instr = *(instr_p++))) {
        switch(instr) {
            case '+':
                heap[pos]++;
                break;

            case '-':
                heap[pos]--;
                break;

            case '>':
                pos++;
                if(pos >= heap_size) {
                  resize_heap(heap, heap_size, heap_size + HEAP_STEP);
                  heap_size += HEAP_STEP;
                }
                break;

            case '<':
                pos--;
                if(pos < 0) {
                    fputs("Negative memory accessed", stderr);
                    exit(1);
                }
                break;

            case '.':
                putchar(heap[pos]);
                break;

            case ',':
                heap[pos] = getchar();
                break;

            case '[':
                if(heap[pos] == 0)
                    while(*(instr_p++) != ']');
                break;

            case ']':
                if(heap[pos] != 0) {
                    while(*(--instr_p) != '[');
                }
                break;

            default:
                break;
        }
    }

    /* release heap */
    free(heap);
    free(inst_buff);
    exit(0);
}

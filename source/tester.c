// tester.c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <libgen.h>
#include <errno.h>
#include <string.h>
#include <getopt.h>
#include <sys/types.h>
#include <math.h>
#include <pthread.h>
#include <limits.h>
#include <png.h>

#include "disassembler.h"
#include "../../6502/API/emulation/CPU/6502.h"

#define max(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a > _b ? _a : _b; })

#define min(a,b) \
   ({ __typeof__ (a) _a = (a); \
       __typeof__ (b) _b = (b); \
     _a < _b ? _a : _b; })

#define OPTSTR                   "i:o:l:e:t:n:"
#define USAGE_FMT                "%s [-h] [-i inputfile] [-l loadaddr]  [-e execaddr] [-o outputfile] [-n threads]\n"
#define DEFAULT_PROGNAME         "tester"
#define ERR_FOPEN_INPUT          "input file could not be opened"
#define ERR_FOPEN_OUTPUT         "output file could not be created"
#define ERR_PROCESS              "process failed"
#define ERR_INVALID_LOAD_ADDRESS "invalid load address, must be in range 0 to 65535"
#define ERR_INVALID_EXEC_ADDRESS "invalid execution address, must be in range 0 to 65535"
#define ERR_TOO_MANY_THREADS     "too many threads. Maximum is 100."

#define MAX_THREADS 100

typedef struct {
  char*         title;
  uint32_t      flags;
  uint32_t      load_address;
  uint32_t      exec_address;
  uint32_t      num_threads;
  FILE *        input;
  char *        output;
} options_t;

typedef struct {
    uint64_t min_cycles;
    uint64_t max_cycles;
    uint64_t total_cycles;
    uint64_t count;
} result_t;

typedef struct {
    uint64_t    thread_index;
    M6502       machine;
    options_t * options;
    uint64_t    start;
    uint64_t    end;

    result_t *  results;
    uint64_t    total_errors;
} thread_context_t;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

void usage(char *progname, int opt);
int  process(options_t *options);
void perror(const char *message);
void *thread_main(void *threadId);

#define MAX_RESULTS 65536
static int result[65536UL];

#include "../build/parameters.c"

// **************************************************************************************
int main(int argc, char *argv[]) {
    int opt;
    options_t options = { "default", 0, 0x0200, -1, 19, stdin, "" };

    opterr = 0;

    while ((opt = getopt(argc, argv, OPTSTR)) != EOF) {
        switch(opt) {
            case 'i':
                if (!(options.input = fopen(optarg, "rb")) ){
                    perror(ERR_FOPEN_INPUT);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'o':
                options.output = calloc(strlen(optarg) + 1, 1);
                strcpy(options.output, optarg);
                break;

            case 'l':
                options.load_address = (uint32_t )strtoul(optarg, NULL, 0);
                if (options.load_address > 65535) {
                    perror(ERR_INVALID_LOAD_ADDRESS);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'e':
                options.exec_address = (uint32_t )strtoul(optarg, NULL, 0);
                if (options.exec_address > 65535) {
                    perror(ERR_INVALID_EXEC_ADDRESS);
                    exit(EXIT_FAILURE);
                }
                break;

            case 't':
                options.title = calloc(strlen(optarg) + 1, 1);
                strcpy(options.title, optarg);
                break;

            case 'n':
                options.num_threads = (uint32_t )strtoul(optarg, NULL, 0);
                if (options.num_threads > MAX_THREADS) {
                    perror(ERR_TOO_MANY_THREADS);
                    exit(EXIT_FAILURE);
                }
                break;

            case 'h':
            default:
                usage(basename(argv[0]), opt);
                break;
        }
    }

    // Set exec address if not set explicitly
    if (options.exec_address == -1)
    {
        options.exec_address = options.load_address;
    }

    int exit_code = process(&options);
    if (exit_code != EXIT_SUCCESS) {
        perror(ERR_PROCESS);
        exit(exit_code);
    }

    return EXIT_SUCCESS;
}

// **************************************************************************************
void usage(char *progname, int opt) {
    fprintf(stderr, USAGE_FMT, progname?progname:DEFAULT_PROGNAME);
    exit(EXIT_FAILURE);
}

// **************************************************************************************
void perror(const char *message) {
    printf("ERROR: %s\n", message);
}

// **************************************************************************************
zuint8 myread(void *context, zuint16 address) {
    return ((zuint8 *)context)[address];
}

// **************************************************************************************
void mywrite(void *context, zuint16 address, zuint8 value) {
    ((zuint8 *)context)[address] = value;
}

// **************************************************************************************
// from https://stackoverflow.com/a/4144956
uint64_t muldiv(uint64_t a, uint64_t b, uint64_t c)
{
    uint64_t q = 0;              // the quotient
    uint64_t r = 0;              // the remainder
    uint64_t qn = b / c;
    uint64_t rn = b % c;

    while(a)
    {
        if (a & 1)
        {
            q += qn;
            r += rn;
            if (r >= c)
            {
                q++;
                r -= c;
            }
        }
        a  >>= 1;
        qn <<= 1;
        rn <<= 1;
        if (rn >= c)
        {
            qn++;
            rn -= c;
        }
    }
    return q;
}

pthread_mutexattr_t     attr;
pthread_mutex_t         mut;

// **************************************************************************************
int process(options_t *options) {
    thread_context_t    thread_context[MAX_THREADS];
    pthread_t           threads[MAX_THREADS];

    int rc;
    uint64_t number_per_thread = (INPUT_END - INPUT_START) / options->num_threads;
    uint64_t remainder = (INPUT_END - INPUT_START) - number_per_thread * options->num_threads;
    size_t bytes_read;

    // create and initialise results buffer
    result_t *results = calloc(MAX_RESULTS * sizeof(result_t), 1);
    for(uint64_t i = 0; i < MAX_RESULTS; i++) {
        results[i].max_cycles   = 0;
        results[i].min_cycles   = ULONG_MAX;
        results[i].total_cycles = 0;
        results[i].count        = 0;
    }

    if (pthread_mutexattr_init(&attr) == -1) {
        printf("Error:mutexattr_init error");
        exit(-2);
    }

    if (pthread_mutex_init(&mut, &attr) == -1) {
        printf("Error:mutex_init error");
        exit(-3);
    }

    // create and start all threads
    uint64_t start = INPUT_START;
    for(uint64_t i = 0; i < options->num_threads; i++ ) {
        zuint8* memory = calloc(65536, 1);
        if (i == 0) {
            // first time around the loop the binary file into memory at the load address
            if (fseek(options->input, 0, SEEK_SET) != 0) {
                printf("Error: unable to seek input file\n");
                exit(-1);
            }
            bytes_read = fread(memory + options->load_address, 1, 65536-options->load_address, options->input);
            printf("read binary file of %zu bytes\n", bytes_read);
        } else {
            // Once loaded, we copy the memory from the file data already loaded
            memcpy(memory + options->load_address, thread_context[0].machine.context + options->load_address, bytes_read);
        }

        // Clear context
        memset(&thread_context[i], 0, sizeof(thread_context[i]));

        // Set up the thread context
        thread_context[i].machine.context = memory;
        thread_context[i].machine.read = &myread;
        thread_context[i].machine.write = &mywrite;

        thread_context[i].options = options;
        thread_context[i].start = start;
        thread_context[i].end = start + number_per_thread;
        if (i < remainder) {
            thread_context[i].end++;
        }
        start = thread_context[i].end;
        thread_context[i].thread_index = i;

        thread_context[i].results = results;
        thread_context[i].total_errors = 0;

        // create and start a new thread
        rc = pthread_create(&threads[i], NULL, thread_main, (void *) &thread_context[i]);
        if (rc) {
            printf("Error:unable to create thread, %d\n", rc);
            exit(-1);
        }
    }

    // Wait for threads to finish
    for(uint64_t i = 0; i < options->num_threads; i++ ) {
        if(pthread_join(threads[i], NULL)!=0) {
            printf("Thread finish failed\n");
        }
    }

    // Output results
    uint64_t total_cycles = 0;
    uint64_t total_count = 0;
    uint64_t min_cycles = ULONG_MAX;
    uint64_t max_cycles = 0;

    for(uint64_t i = 0; i < MAX_RESULTS; i++) {
        double average_cycles = results[i].count ? muldiv(results[i].total_cycles, 100UL, results[i].count) : 0.0;
        average_cycles /= 100.0;
        //fprintf(options->output, "%llu,%llu,%llu,%lf\n", i, results[i].min_cycles, results[i].max_cycles, average_cycles);
        total_cycles += results[i].total_cycles;
        total_count  += results[i].count;

        if (results[i].min_cycles < min_cycles) {
            min_cycles = results[i].min_cycles;
        }
        if (results[i].max_cycles > max_cycles) {
            max_cycles = results[i].max_cycles;
        }
    }
    printf("total cycles   = %llu\n", total_cycles);
    printf("total count    = %llu\n", total_count);
    printf("average cycles = %.2lf\n", (double) total_cycles / total_count);

    FILE * file_output = fopen(options->output, "w");
    if (!file_output) {
        perror(ERR_FOPEN_OUTPUT);
        exit(EXIT_FAILURE);
    }

    fprintf(file_output, "{\n");
    fprintf(file_output, "    \"results\": {\n");
    fprintf(file_output, "        \"title\": \"%s\",\n", options->title);
    fprintf(file_output, "        \"bytes\": \"%zu\",\n", bytes_read);
    fprintf(file_output, "        \"AverageCycles\": \"%.2lf\",\n", (double) total_cycles / total_count);
    fprintf(file_output, "        \"MinCycles\": \"%llu\",\n", min_cycles);
    fprintf(file_output, "        \"MaxCycles\": \"%llu\"\n", max_cycles);
    fprintf(file_output, "    }\n");
    fprintf(file_output, "}\n");
    fclose(file_output);
    file_output = NULL;

    // Clean up
    for(uint64_t i = 0; i < options->num_threads; i++ ) {
        free(thread_context[i].machine.context);
        thread_context[i].machine.context = NULL;
    }
    free(results);
    results = NULL;

    test_cleanup();
    return(0);
}

// **************************************************************************************
void record_result(thread_context_t* threadContext, uint64_t input, uint64_t cycles, uint64_t errors) {

    uint64_t bucket = (input * 65536) / (INPUT_END - INPUT_START);

    threadContext->results[bucket].max_cycles = max(cycles, threadContext->results[bucket].max_cycles);
    threadContext->results[bucket].min_cycles = min(cycles, threadContext->results[bucket].min_cycles);
    threadContext->results[bucket].total_cycles += cycles;
    threadContext->results[bucket].count++;
    threadContext->total_errors += errors;
}

// **************************************************************************************
void write_image(char *filename)
{
    // output png
    int width  = 256;
    int height = 256;
    int y;
    png_bytep *row_pointers = NULL;

    FILE *fp = fopen(filename, "wb");
    if(!fp) abort();

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png) abort();

    png_infop info = png_create_info_struct(png);
    if (!info) abort();

    if (setjmp(png_jmpbuf(png))) abort();

    png_init_io(png, fp);

    // Output is 8bit depth, RGBA format.
    png_set_IHDR(
        png,
        info,
        width, height,
        8,
        PNG_COLOR_TYPE_RGBA,
        PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT,
        PNG_FILTER_TYPE_DEFAULT);

    png_write_info(png, info);
    // To remove the alpha channel for PNG_COLOR_TYPE_RGB format,
    // Use png_set_filler().
    //png_set_filler(png, 0, PNG_FILLER_AFTER);

    // allocate rows of pixels
    row_pointers = (png_bytep*)malloc(sizeof(png_bytep) * height);
    for(int y = 0; y < height; y++) {
        row_pointers[y] = (png_byte*)malloc(png_get_rowbytes(png,info));
    }

    if (!row_pointers) abort();

    for(int y = 0; y < height; y++) {
        png_bytep row = row_pointers[height-1-y];
        for(int x = 0; x < width; x++) {
            png_bytep px = &(row[x * 4]);

            uint64_t input = 256*y + x;

            int actual_result = result[input];
            int expected = (x*y)/256;

            if (actual_result == expected) {
                // Green
                px[0] = 0;
                px[1] = 255;
                px[2] = 0;
                px[3] = 255;
            } else if (actual_result > expected) {
                // Blue
                px[0] = 0;
                px[1] = 0;
                px[2] = 255;
                px[3] = 255;
            } else {
                // Red
                px[0] = 255;
                px[1] = 0;
                px[2] = 0;
                px[3] = 255;
            }
            // Do something awesome for each pixel here...
            //printf("%4d, %4d = RGBA(%3d, %3d, %3d, %3d)\n", x, y, px[0], px[1], px[2], px[3]);
        }
    }

    png_write_image(png, row_pointers);
    png_write_end(png, NULL);

    for(int y = 0; y < height; y++) {
        free(row_pointers[y]);
    }
    free(row_pointers);

    fclose(fp);

    png_destroy_write_struct(&png, &info);
}

// **************************************************************************************
void *thread_main(void *context) {
    thread_context_t* threadContext = context;

    zuint8* memory = threadContext->machine.context;
    uint64_t errors = 0;
    uint64_t expected;
    char registers[25600];
    char to_append[256];
    char flags[9];

    pthread_mutex_lock(&mut);
    printf("thread %llu: starting range %llu to %llu\n", threadContext->thread_index, threadContext->start, threadContext->end);
    pthread_mutex_unlock(&mut);

    for(uint64_t i = threadContext->start; i < threadContext->end; i++) {
        // Set stack pointer and pc
        threadContext->machine.state.s = 0;
        threadContext->machine.state.pc = threadContext->options->exec_address;

        // Set up input values
        test_pre(threadContext, i);

        // Run test instruction by instruction, counting the cycles taken
        int total_cycles = 0;
        do {
            int cycles_executed = m6502_run(&threadContext->machine, 1);
            total_cycles += cycles_executed;
        }
        while(threadContext->machine.state.s != 2);   // loop until stack is exhausted

        // Get result
        uint64_t actual_result = test_post(threadContext);

        // Is the result correct?
        if (!is_correct(threadContext, i, actual_result, &expected)) {
            if (errors < 100) {
                pthread_mutex_lock(&mut);
                printf("thread %llu: for input %llu (0x%llx), expected %llu but got %llu\n", threadContext->thread_index, i, i, (uint64_t) expected, (uint64_t) actual_result);

                // Run again for debugging purposes, through a debugger:
                {
                    // Set stack pointer and pc
                    threadContext->machine.state.s = 0;
                    threadContext->machine.state.pc = threadContext->options->exec_address;

                    // Set up input values
                    test_pre(threadContext, i);

                    // Run test instruction by instruction, counting the cycles taken
                    int memory_locations_to_output = 16;
                    int total_cycles = 0;
                    printf("                                                                            ");
                    for(i = 0; i < memory_locations_to_output; i++) {
                        printf(" %3llx", i);
                    }
                    printf("\n");
                    do {
                        disassembler.pc = (char *) memory + threadContext->machine.state.pc;

                        // get string containing instruction we are about to execute
                        disassemble_next();

                        // format the string
                        int len = strlen(disassembler.output);
                        while(len < 20) {
                            disassembler.output[len] = ' ';
                            len++;
                        }
                        disassembler.output[len] = 0;

                        // remember current pc
                        int pc = threadContext->machine.state.pc;

                        // execute one instruction
                        int cycles_executed = m6502_run(&threadContext->machine, 1);

                        // format results (registers, flags, memory)
                        int j = 128;
                        for(int i = 0; i < 8; i++) {
                            if (threadContext->machine.state.p & j) {
                                flags[i] = "NV_BDIZC"[i];
                            }
                            else {
                                flags[i] = "nv_bdizc"[i];
                            }
                            j = j/2;
                        }
                        sprintf(registers, "A=$%02x X=$%02x Y=$%02x SP=$01%02x P=%s mem[0...]=",
                            threadContext->machine.state.a,
                            threadContext->machine.state.x,
                            threadContext->machine.state.y,
                            threadContext->machine.state.s,
                            flags);

                        for(i = 0; i < memory_locations_to_output; i++) {
                            sprintf(to_append, " $%02x", memory[i]);
                            strcat(registers, to_append);
                        }

                        // show pc, instruction and results
                        printf("%04x:  %s %s\n", pc, disassembler.output, registers);

                        // add cycles to total
                        total_cycles += cycles_executed;
                    }
                    while(threadContext->machine.state.s != 2);   // loop until stack exhausted
                    pthread_mutex_unlock(&mut);
                    return NULL;
                }

            }
            else if (errors == 100) {
                printf("Too many errors.\n");
            }
            errors++;
        }

        // Record the cycle count
        record_result(threadContext, i, total_cycles, errors);
    }

    return NULL;
}

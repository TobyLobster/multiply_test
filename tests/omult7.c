// omult7.c

// specify range of input values
static const uint64_t INPUT_START = 0UL;
static const uint64_t INPUT_END   = 65536UL;

static int result[65536UL];
__thread int test_input = 0;

// **************************************************************************************
void test_pre(thread_context_t* threadContext, uint64_t input) {
    zuint8* memory = threadContext->machine.context;

    threadContext->machine.state.a = input & 255UL;
    memory[2] = (input / 256UL) & 255UL;

    test_input = input;
}

// **************************************************************************************
uint64_t test_post(thread_context_t* threadContext) {
    zuint8* memory = threadContext->machine.context;

    uint64_t high = threadContext->machine.state.a;

    result[test_input] = high;
    return high;
}

// **************************************************************************************
int is_correct(thread_context_t* threadContext, uint64_t input, uint64_t actual_result, uint64_t* expected) {
    uint64_t a = input & 255UL;
    uint64_t b = (input / 256UL) & 255UL;

    uint64_t e = (a*b) / 256;
    *expected = e;

    // log is 'close enough' to expected result    
    return (max(actual_result,e) - min(actual_result,e)) <= 6;
}

// **************************************************************************************
void test_cleanup()
{
/*
    FILE* out_file = fopen("in_outs.csv", "w");
    if (out_file) {
        fprintf(out_file, "a,b,a*b\n");
        for(uint64_t input = INPUT_START; input < INPUT_END; input++)
        {
            uint64_t a = input & 255UL;
            uint64_t b = (input / 256UL) & 255UL;

            uint64_t e = (a*b) / 256;
            
            fprintf(out_file, "%llu,%llu,%d,%d\n",
                    a,
                    b,
                    result[input],
                    (e-result[input])>1);
            if ((e-result[input])>5) {
                printf("%d\n", (e-result[input]));
            }
        }

        fclose(out_file);
        out_file = NULL;
    }
*/
}

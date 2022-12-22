// mult12.c

// specify range of input values
static const uint64_t INPUT_START = 0UL;
static const uint64_t INPUT_END   = 65536UL;

static int result[65536UL];
__thread int test_input = 0;

// **************************************************************************************
void test_pre(thread_context_t* threadContext, uint64_t input) {
    zuint8* memory = threadContext->machine.context;

    memory[2] = input & 255UL;
    threadContext->machine.state.x = (input / 256UL) & 255UL;

    test_input = input;
}

// **************************************************************************************
uint64_t test_post(thread_context_t* threadContext) {
    zuint8* memory = threadContext->machine.context;

    uint64_t high = threadContext->machine.state.a;
    uint64_t low = memory[2];

    result[test_input] = 256*high + low;
    return 256*high + low;
}

// **************************************************************************************
int is_correct(thread_context_t* threadContext, uint64_t input, uint64_t actual_result, uint64_t* expected) {
    uint64_t a = input & 255UL;
    uint64_t b = (input / 256UL) & 255UL;

    uint64_t e = a*b;
    *expected = e;

    return actual_result == e;
}

// **************************************************************************************
void test_cleanup()
{
    // write results to file
    /*
    FILE* out_file = fopen("in_outs.csv", "w");
    if (out_file) {
        fprintf(out_file, "a,b,a*b\n");
        for(uint64_t input = INPUT_START; input < INPUT_END; input++)
        {
            fprintf(out_file, "%llu,%llu,%d\n",
                    input & 255,
                    (input / 256) & 255,
                    result[input]
                    );
        }

        fclose(out_file);
        out_file = NULL;
    }
    */
}

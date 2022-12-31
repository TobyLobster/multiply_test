import math
import png

# Suppose we want a 256 entry table of log values, each entry being one byte
# Let 'log2()' be the the logarithm function in base 2.
#
# log2(0) is not defined, so we start the table from log(1) = 0, up to log2(255) = 7.9943534369
# We want to scale the results to fit in one byte, so let f = 255/log2(255)
# then:
#
#     for i in range(1,256):
#         log_table[i] = int(f*log2(i)+0.5)         # '+0.5' to round the number
#
# Since we add the two logarithms before taking the power, then range of the exp table
# needs to be 0-510. For the exponential operation, we want:
#
#     for i in range(0,510):
#         exp_table[i] = int(pow2(i/f-8)+0.5)

log_table = [0]
pow_table = []
error_hist = [];

def make_tables():
    global log_table
    global pow_table

    log_table = [0]
    pow_table = []

    f = 255/math.log(255, 2)

    for i in range(1, 256):
        log_table.append(int(f*math.log(i, 2)+0.5))

    for i in range(0, 511):
        y = pow(2, i/f-8) #+0.5
        pow_table.append(int(y))

def output_tables(log_table_name, pow_table_name):

    print(log_table_name)
    for i in range(0, 256):
        if ((i & 15) == 0):
            if i != 0:
                print()
            print("    !byte ", end='')
        else:
            print(",", end='')
        print("${0:02x}".format(log_table[i]), end='')
    print()
    print()

    print(pow_table_name)
    for i in range(0, 511):
        if ((i & 15) == 0):
            if i != 0:
                print()
            print("    !byte ", end='')
        else:
            print(",", end='')
        print("${0:02x}".format(pow_table[i]), end='')
    print()
    print()


def make_asm_file(from_filepath, to_filepath):
    with open(from_filepath, mode='rb') as file:
        fileContent = file.read()

    with open(to_filepath, 'w') as f:
        index = 0
        f.write('; ' + to_filepath + '\n')
        f.write(';\n')
        f.write('\n')
        f.write(to_filepath.split(".")[0].split("_")[-1] + '\n')
        for b in fileContent:
            if (index & 15) == 0:
                if index != 0:
                    f.write('\n')
                f.write('    !byte ')
            else:
                f.write(',')
            f.write("${0:02x}".format(b))
            index += 1

        f.write('\n')


def make_asm(elite_dir):

    # TODO:
    # 1. Create asm files for each of these files, and the (Mark Moxon?) versions of the tables.
    # 2. Create tests for each of these versions
    # 3. Measure the errors
    # 4. Conclude which is best :-)

    files = [
        # SP
        ("6502sp-elite-beebasm/4-reference-binaries/sng45/workspaces/ELTG-logL.bin",             "sng45_logL.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/executive/workspaces/ELTG-logL.bin",         "execu_logL.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/source-disc/workspaces/ELTG-logL.bin",       "sourc_logL.asm"),

        ("6502sp-elite-beebasm/4-reference-binaries/sng45/workspaces/ELTG-log.bin",              "sng45_log.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/executive/workspaces/ELTG-log.bin",          "execu_log.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/source-disc/workspaces/ELTG-log.bin",        "sourc_log.asm"),

        ("6502sp-elite-beebasm/4-reference-binaries/sng45/workspaces/ELTG-antilog.bin",           "sng45_antilog.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/executive/workspaces/ELTG-antilog.bin",       "execu_antilog.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/source-disc/workspaces/ELTG-antilog.bin",     "sourc_antilog.asm"),

        ("6502sp-elite-beebasm/4-reference-binaries/sng45/workspaces/ELTG-antilogODD.bin",        "sng45_antilogODD.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/executive/workspaces/ELTG-antilogODD.bin",    "execu_antilogODD.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/source-disc/workspaces/ELTG-antilogODD.bin",  "sourc_antilogODD.asm"),

        # SP: I/O processor
        ("6502sp-elite-beebasm/4-reference-binaries/sng45/workspaces/ICODE-log.bin",              "sng45_icode_log.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/executive/workspaces/ICODE-log.bin",          "execu_icode_log.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/source-disc/workspaces/ICODE-log.bin",        "sourc_icode_log.asm"),

        ("6502sp-elite-beebasm/4-reference-binaries/sng45/workspaces/ICODE-logL.bin",             "sng45_icode_logL.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/executive/workspaces/ICODE-logL.bin",         "execu_icode_logL.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/source-disc/workspaces/ICODE-logL.bin",       "sourc_icode_logL.asm"),

        ("6502sp-elite-beebasm/4-reference-binaries/sng45/workspaces/ICODE-antilog.bin",          "sng45_icode_antilog.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/executive/workspaces/ICODE-antilog.bin",      "execu_icode_antilog.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/source-disc/workspaces/ICODE-antilog.bin",    "sourc_icode_antilog.asm"),

        ("6502sp-elite-beebasm/4-reference-binaries/sng45/workspaces/ICODE-antilogODD.bin",       "sng45_icode_antilogODD.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/executive/workspaces/ICODE-antilogODD.bin",   "execu_icode_antilogODD.asm"),
        ("6502sp-elite-beebasm/4-reference-binaries/source-disc/workspaces/ICODE-antilogODD.bin", "sourc_icode_antilogODD.asm"),

        # Master:
        ("master-elite-beebasm/4-reference-binaries/sng47/workspaces/ELTA-logL.bin",            "sng47_logL.asm"),
        ("master-elite-beebasm/4-reference-binaries/compact/workspaces/ELTA-logL.bin",          "compa_logL.asm"),

        ("master-elite-beebasm/4-reference-binaries/sng47/workspaces/ELTA-log.bin",             "sng47_log.asm"),
        ("master-elite-beebasm/4-reference-binaries/compact/workspaces/ELTA-log.bin",           "compa_log.asm"),

        ("master-elite-beebasm/4-reference-binaries/sng47/workspaces/ELTA-antilog.bin",         "sng47_antilog.asm"),
        ("master-elite-beebasm/4-reference-binaries/compact/workspaces/ELTA-antilog.bin",       "compa_antilog.asm"),
    ]

    for tup in files:
        full_filepath = elite_dir + tup[0]
        make_asm_file(full_filepath, tup[1])

def mult(a,b):
    index = log_table[a] + log_table[b]
    return pow_table[index]

def make_png(filename):
    width = 256
    height = 256
    img = []
    for y in range(height-1, -1, -1):
        row = ()
        for x in range(width):
            expected = int(x*y/256)
            actual = mult(x,y)
            #print(expected, actual)
            if actual == expected:
                row = row + (0,255 ,0 )
            elif actual == (expected + 1):
                row = row + (200,0,0)
            elif actual == (expected + 2):
                row = row + (150,0,0)
            elif actual == (expected + 3):
                row = row + (100,0,0)
            elif actual == (expected + 4):
                row = row + (50,0,0)
            elif actual == (expected + 5):
                row = row + (25,0,0)
            elif actual == (expected - 1):
                row = row + (0,255 ,0 ) #(0,0,200)
            elif actual == (expected - 2):
                row = row + (0,0,150)
            elif actual == (expected - 3):
                row = row + (0,0,100)
            elif actual == (expected - 4):
                row = row + (0,0,50)
            elif actual == (expected - 5):
                row = row + (0,0,25)
            else:
                row = row + (255,0,0)
        img.append(row)

    with open(filename, 'wb') as f:
        w = png.Writer(width, height, greyscale=False)
        w.write(f, img)

def init_hist():
    global error_hist
    error_hist = [0] * 512
    for i in range(0, 512):
        error_hist[i] = 0

def record_error(error):
    error = int(error)
    error_hist[256 + error] += 1

def calculate_error():
    init_hist()

    width = 256
    height = 256
    img = []
    error_sum = 0
    for y in range(height):
        row = ()
        for x in range(width):
            expected = int(x*y//256)
            actual = mult(x,y)
            error = actual-expected
            record_error(error)
            error_sum += (actual-expected)*(actual-expected)
    error_sum = math.sqrt(error_sum)
    print("Root-mean-square deviation:", error_sum, "(smaller is better)")

def display_error_histogram():
    largest_count = 0
    index = 0
    first_non_zero_entry = -1
    last_non_zero_entry = -1
    for i in error_hist:
        largest_count = max(largest_count, i)
        if (i != 0) and (first_non_zero_entry < 0):
            first_non_zero_entry = index
        if (i != 0):
            last_non_zero_entry = index
        index += 1
    if last_non_zero_entry >= 0:
        for col in range(first_non_zero_entry, last_non_zero_entry+1):
            if error_hist[col] != 0:
                print("Error:", col-256, " count:", error_hist[col])

#make_asm("/Users/tobynelson/Code/6502/elite/")

make_tables()
output_tables("log2tab", "pow2tab")
calculate_error()
display_error_histogram()
make_png('results.png')

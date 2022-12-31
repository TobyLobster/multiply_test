import math
#import png

def output_tables(low_table_name, high_table_name, pow_table_name, pow_table2_name):

    print(low_table_name)
    for i in range(0, 256):
        if ((i & 15) == 0):
            if i != 0:
                print()
            print("    !byte ", end='')
        else:
            print(",", end='')
        print("${0:02x}".format(log_table[i] & 255), end='')
    print()
    print()

    print(high_table_name)
    for i in range(0, 256):
        if ((i & 15) == 0):
            if i != 0:
                print()
            print("    !byte ", end='')
        else:
            print(",", end='')
        print("${0:02x}".format(log_table[i] // 256), end='')
    print()
    print()

    print(pow_table_name)
    for i in range(0, 256):
        if ((i & 15) == 0):
            if i != 0:
                print()
            print("    !byte ", end='')
        else:
            print(",", end='')
        print("${0:02x}".format(pow_table[i]), end='')
    print()
    print()

    if (pow_table2 != None) and (pow_table2_name != None):
        print(pow_table2_name)
        for i in range(0, 256):
            if ((i & 15) == 0):
                if i != 0:
                    print()
                print("    !byte ", end='')
            else:
                print(",", end='')
            print("${0:02x}".format(pow_table2[i]), end='')
        print()
        print()

log_table = [0]
pow_table = []
pow_table2 = None

for i in range(1, 256):
    v = int(0.5 + 32*math.log(i, 2)*256)
    log_table.append(v)

for i in range(0, 256):
    v = int(pow(2, i/32 + 8) / 256)
    pow_table.append(v)

output_tables("logL", "log", "antilog", None)


log_table = [0]
pow_table = []
pow_table2 = []

for i in range(1, 65536):
    v = int(32*math.log(i, 2)*256)
    log_table.append(v)

for i in range(0, 256):
    v = int(pow(2, i/32 + 8) / 256)
    pow_table.append(v)

for i in range(0, 256):
    v = int(pow(2, (i+0.5) / 32 + 8) / 256)
    pow_table2.append(v)

#output_tables("logL", "log", "antilog", "antilogODD")

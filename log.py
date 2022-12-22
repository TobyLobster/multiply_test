import math
import png

log_table = [0]
pow_table = [0]

f = 255/math.log(255, 2)

for i in range(1, 256):
    log_table.append(int(f*math.log(i, 2)+.5))

for i in range(1, 511):
    y = pow(2, i/f-8)+0.5
    pow_table.append(int(y))
    print(pow_table[i])

def mult(a,b):
    index = log_table[a] + log_table[b]
    return pow_table[index]

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
            row = row + (0,255 ,0 ) #(200,0,0)
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

with open('results.png', 'wb') as f:
    w = png.Writer(width, height, greyscale=False)
    w.write(f, img)

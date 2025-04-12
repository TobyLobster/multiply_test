set -e

mkdir -p build

function test() {
    cp tests/$1.c build/parameters.c
    acme -o build/TESTME -r build/report.txt tests/$1.a
    clang -O3 -Wno-unknown-warning-option -I../Z/API -I/usr/local/include/libpng16 -lpng -o build/tester -DCPU_6502_STATIC -DCPU_6502_USE_LOCAL_HEADER source/tester.c source/6502.c source/disassembler.c
    echo $1
    time build/tester -t$1 -l0x200 -e$2 -n19 -r "$3" -ibuild/TESTME -oresults/results_$1.json
}

#test "mult1"  0x200
#test "mult2"  0x200
#test "mult3"  0x200
#test "mult4"  0x200
#test "mult5"  0x500
#test "mult6"  0x400
#test "mult7"  0x200
#test "mult8"  0x200
#test "mult9"  0x200
#test "mult10" 0x200
#test "mult11" 0x200
#test "mult12" 0x200
#test "mult13" 0x600
#test "mult14" 0xA00
#test "mult15" 0xA00
#test "mult16" 0x400
#test "mult17" 0x200
#test "mult18" 0x200
#test "mult19" 0x200
#test "mult20" 0x200
#test "mult21" 0x200
#test "mult22" 0x400
#test "mult23" 0x200
#test "mult24" 0x200
#test "mult25" 0x200
#test "mult26" 0x200
#test "mult27" 0x700
#test "mult28" 0x200
#test "mult29" 0x200
#test "mult30" 0x200
#test "mult31" 0xa00
#test "mult32" 0x400
#test "mult33" 0x5ff
#test "mult34" 0x200
#test "mult35" 0x200
#test "mult36" 0x200
#test "mult37" 0x200
#test "mult38" 0x700
#test "mult39" 0x200
#test "mult40" 0x200
#test "mult41" 0x600
#test "mult42" 0x400
#test "mult43" 0x200
#test "mult44" 0x200
#test "mult45" 0x200
#test "mult46" 0x200
#test "mult47" 0x200
#test "mult48" 0x200
#test "mult49" 0x200
#test "mult50" 0x200
#test "mult51" 0x200
#test "mult52" 0x200
#test "mult53" 0x200
#test "mult54" 0x200
#test "mult55" 0x200
#test "mult56" 0x600
#test "mult57" 0x600
#test "mult58" 0x400
#test "mult59" 0x200
#test "mult60" 0x200
#test "mult61" 0x200
#test "mult62" 0x200
#test "mult63" 0x200
#test "mult64" 0x200
#test "mult65" 0x600
#test "mult66" 0x800
#test "mult67" 0x200
#test "mult68" 0x200
#test "mult69" 0x200
#test "mult70" 0x200
#test "mult72" 0x200
#test "mult73" 0x200
#test "mult74" 0x200
#test "mult75" 0x200
#test "mult76" 0x200
#test "mult77" 0x200
#test "mult78" 0x200
#test "mult79" 0x200
#test "mult80" 0x300
#test "mult81" 0x200
#test "mult82" 0x500
#test "mult83" 0x600
#test "mult84" 0x600
#test "mult85" 0x200
#test "mult86" 0xA00
#test "mult87" 0x400
#test "mult88" 0x200
#test "mult89" 0x200

#test "smult1" 0xa00
#test "smult2" 0x200
#test "smult3" 0xa00
#test "smult4" 0x200
#test "smult5" 0x200
#test "smult6" 0x200
#test "smult7" 0x700
#test "smult8" 0x600
#test "smult9" 0x200
#test "smult10" 0x9ff
#test "smult11" 0xb00
test "smult12" 0xa00

#test "omult1" 0x200
#test "omult2" 0x200
#test "omult3" 0x200
#test "omult4" 0x200
#test "omult5" 0x200
#test "omult6" 0x200
#test "omult7" 0x500
#test "omult8" 0x600
#test "omult9" 0x4ff
#test "omult10" 0x200
#test "omult11" 0x400
#test "omult12" 0x200
#test "omult13" 0x200
#test "omult14" 0x200
#test "omult15" 0x200
#test "omult16" 0x200
#test "omult17" 0x200
#test "omult18" 0x200
#test "omult19" 0x200
#test "omult19" 0x200 10000000        # test ten million random inputs on each thread
#test "omult20" 0x200
#test "omult20" 0x200 10000000
#test "omult21" 0x200
#test "omult21" 0x200 10000000
#test "omult22" 0x200
#test "omult22" 0x200 10000000
#test "omult23" 0x200
#test "omult23" 0x200 10000000
#test "omult24" 0x200
#test "omult24" 0x200 10000000
#test "omult25" 0x200
#test "omult26" 0x200
#test "omult27" 0x200
#test "omult28" 0x200
#test "omult29" 0x200
#test "omult30" 0x200
#test "omult31" 0xa00
#test "omult32" 0x200

clear
mkdir -p bin
echo "static int8_t clockHour = `date +%-H` % 12;" > src/stamp.inc
echo "static int8_t clockMin = `date +%-M`;" >> src/stamp.inc
echo "static int8_t clockSec = `date +%-S`;" >> src/stamp.inc
g++ -std=c++11 -pthread -O3 -L. `sdl2-config --cflags --libs` src/ledclock.cpp -o bin/ledclock -lSDL2

clear
mkdir -p bin
echo "static uint32_t clockHour = `date +%-H`;" > stamp.inc
echo "static uint32_t clockMin = `date +%-M`;" >> stamp.inc
echo "static uint32_t clockSec = `date +%-S`;" >> stamp.inc
g++ -x c++ -std=c++11 -pthread `sdl2-config --cflags --libs` ledclock.ino -o bin/ledclock -lSDL2

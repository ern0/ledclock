clear
mkdir -p bin
echo "static uint8_t clockHour = `date +%-H`;" > stamp.inc
echo "static uint8_t clockMin = `date +%-M`;" >> stamp.inc
echo "static uint8_t clockSec = `date +%-S`;" >> stamp.inc
g++ -x c++ -std=c++11 -pthread -O3 `sdl2-config --cflags --libs` ledclock.ino -o bin/ledclock -lSDL2

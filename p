clear
mkdir -p bin
g++ -x c++ -DHOUR=`date +%-H` -DMIN=`date +%-M` -DSEC=`date +%-S` -std=c++11 -pthread `sdl2-config --cflags --libs` ledclock.ino -o bin/ledclock -lSDL2

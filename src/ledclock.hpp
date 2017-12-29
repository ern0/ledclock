# if ( defined(__unix__) || defined(__APPLE__) )
# define SDL_DISPLAY (0)
# define TIMER2 ( 1000 / 100 )   
# include "../posixino/posixino.cpp"
# endif

# define FRONT_LEFT 1
# define REAR_LEFT 3
# define FRONT_RIGHT 4
# define REAR_RIGHT 2

# define PIN 13
# define LARGE_PIX 24
# define SMALL_PIX 12

# ifdef SDL_DISPLAY
static char large[LARGE_PIX] = { 0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23 };
static char small[SMALL_PIX] = { 24,25,26,27,28,29,30,31,32,33,34,35 };
# else 
static char large[LARGE_PIX] = { 23,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22 };
static char small[SMALL_PIX] = { 24,25,26,27,28,29,30,31,32,33,34,35 };
# endif


#define BRITE_LOW 0x08


void setupTimerInterrupt();
void tick();
inline void buttonHandler();
inline void incDeci();
inline void incSec();
inline void incMin();
inline void decMin();
inline void incHour();
inline void decHour();
inline void calcHour();
inline void redrawClock();
inline void redrawSmall();
inline void redrawLarge();


# ifdef SDL_DISPLAY
void setupEmu();
# endif

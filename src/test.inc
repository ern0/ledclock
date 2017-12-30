# ifdef SDL_DISPLAY
	void dump() {

    printf(
      "%02d:%02d:%02d.%d  "
      "%02d->%02d %03d:%03d  "
      "%02d->%02d %03d:%03d  "
      "\n"
      ,(int)clockHour
      ,(int)clockMin
      ,(int)clockSec
      ,(int)clockDeci
      ,hourFadeOutPos
      ,hourFadeInPos
      ,hourFadeOutValue
      ,hourFadeInValue
      ,minFadeOutPos
      ,minFadeInPos
      ,minFadeOutValue
     	,minFadeInValue
    );

   } // dump()
# endif


# if TEST

	void test(int h,int m,int s,int d) {

		clockHour = h;
		clockMin = m;
		clockSec = s;
		clockDeci = d;

		calcHour();
		calcMin();

		dump();

	} // test()


	void performTest() {

    test(10,59,58,5);
    test(10,59,59,9);
    test(11,0,0,0);

    test(0,30,0,4);
    exit(0);

   }

# endif
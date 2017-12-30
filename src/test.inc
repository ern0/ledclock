# ifdef SDL_DISPLAY
	void dump() {

    printf(
      "%02d:%02d:%02d.%d  "
      "%02d->%02d %03d:%03d  "
      "%02d->%02d %03d:%03d  "
      "\n"
      ,(int)showHour
      ,(int)showMin
      ,(int)showSec
      ,(int)showDeci
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

	void test(int h,int m,int s,int c) {

		showHour = h;
		showMin = m;
		showSec = s;
		showCenti = c;
		showDeci = c / 10;

		calcHour();
		calcMin();

		dump();

	} // test()


	void performTest() {

    test(10,59,58,50);
    test(10,59,59,90);
    test(11,0,0,0);

    test(0,30,0,40);
    exit(0);

   }

# endif
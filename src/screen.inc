  # ifdef SDL_DISPLAY
  void setupEmu() {

    strip.emuSetGridScreenAnchor("ne");
    strip.emuSetGridScreenPercent(30);
    strip.emuSetGridCells(36,36);
    
    int n = 0;
    
    strip.emuSetPixelPos(n++,16,0);
    strip.emuSetPixelPos(n++,20,1);
    strip.emuSetPixelPos(n++,24,2);
    strip.emuSetPixelPos(n++,28,4);
    strip.emuSetPixelPos(n++,30,8);
    strip.emuSetPixelPos(n++,31,12);
    strip.emuSetPixelPos(n++,32,16);
    strip.emuSetPixelPos(n++,31,20);
    strip.emuSetPixelPos(n++,30,24);
    strip.emuSetPixelPos(n++,28,28);
    strip.emuSetPixelPos(n++,24,30);
    strip.emuSetPixelPos(n++,20,31);
    strip.emuSetPixelPos(n++,16,32);
    strip.emuSetPixelPos(n++,12,31);
    strip.emuSetPixelPos(n++,8,30);
    strip.emuSetPixelPos(n++,4,28);
    strip.emuSetPixelPos(n++,2,24);
    strip.emuSetPixelPos(n++,1,20);
    strip.emuSetPixelPos(n++,0,16);
    strip.emuSetPixelPos(n++,1,12);
    strip.emuSetPixelPos(n++,2,8);
    strip.emuSetPixelPos(n++,4,4);
    strip.emuSetPixelPos(n++,8,2);
    strip.emuSetPixelPos(n++,12,1);
    
    strip.emuSetPixelPos(n++,16,6);
    strip.emuSetPixelPos(n++,21,7);
    strip.emuSetPixelPos(n++,25,11);
    strip.emuSetPixelPos(n++,26,16);
    strip.emuSetPixelPos(n++,25,21);
    strip.emuSetPixelPos(n++,21,25);
    strip.emuSetPixelPos(n++,16,26);
    strip.emuSetPixelPos(n++,11,25);
    strip.emuSetPixelPos(n++,7,21);
    strip.emuSetPixelPos(n++,6,16);
    strip.emuSetPixelPos(n++,7,11);
    strip.emuSetPixelPos(n++,11,7);

    for (int n = 0; n < strip.numPixels(); n++ ) {
      strip.emuSetPixelCellSize(n,4,4);
      strip.emuSetPixelPixGap(n,5,5);
    }

  } // setupEmu()
  # endif

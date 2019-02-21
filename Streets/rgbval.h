#ifndef RGBVAL_H
#define RGBVAL_H

class RGBVal {
public:

  RGBVal():_red(0),_green(0),_blue(0){}

  RGBVal(unsigned int r, unsigned int g, unsigned int b):_red(r),_green(g),_blue(b){}

  unsigned int _red;
  unsigned int _green;
  unsigned int _blue;

};

#endif // RGBVAL_H

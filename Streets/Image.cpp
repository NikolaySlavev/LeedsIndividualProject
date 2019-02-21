#include <Windows.h>
#include "Image.h"
#include <vector>
#include <iostream>
#include <cstdlib>

using namespace std;

Image::Image(const std::string& file_name) {
  p_qimage = new QImage(QString(file_name.c_str()));
  _width  = p_qimage->width();
  _height = p_qimage->height();
  _image = new GLubyte[_width*_height*3];

  unsigned int nm = _width*_height;
  for (unsigned int i = 0; i < nm; i++){
    std::div_t part = std::div((int)i, (int)_width);
    QRgb colval = p_qimage->pixel(_width-part.rem-1, part.quot);
    _image[3*nm-3*i-3] = qRed(colval);
    _image[3*nm-3*i-2] = qGreen(colval);
    _image[3*nm-3*i-1] = qBlue(colval);
  }
}

GLubyte* Image::imageField() { return _image; }

Image::~Image() {
  delete _image;
  delete p_qimage;
}

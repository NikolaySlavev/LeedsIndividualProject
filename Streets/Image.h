#ifndef IMAGE_H
#define IMAGE_H

#include<string>
#include <QImage>
#include <GL/glu.h>


class Image {
 public:
  Image(const std::string& fn);
  ~Image();
  GLubyte* imageField();
  unsigned int Width()  { return _width;}
  unsigned int Height() {return _height;}

  private:
  Image(const Image&);
  unsigned int _width;
  unsigned int _height;
  QImage* p_qimage;
  GLubyte* _image;
};

#endif // IMAGE_H

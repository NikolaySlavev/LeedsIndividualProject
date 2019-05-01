# University of Leeds - Individual Project 2018/2019

# Overview
The project is about Procedural Modelling of Virtual Cities. It will encompass modelling of streets (straight and curved), junctions (with rounded corners), building areas (the regions bounded by the streets), and buildings.

The project aims to produce a procedural approach of modelling cities primarily focuses on street generation and secondarily on building architecture. To increase the user experience, diverse input and output options are be provided as well as a real-time editing functionality. Users are able to provide real geographic data that will be used to automatically model a city. The resulting models have the option to be either visualised in OpenGL or exported as an OBJ file.

# Software Manual
The software was developed and tested on a Windows machine only but with little library adjustments should run on Linux as well. 

The source code is comprised of two parts, Parser and Main, which are run separately. As a result, the parsing will be applied ones per extracted map region and will not run every time the Main software is visualised.

  # Parser (Installation)
  The only installation required is the library ReadOSM explained here: http://www.gaia-gis.it/gaia-sins/readosm-1.1.0-doxy-doc/index.html 
  Note: The steps are for installing it through Cygwin but anything else can be used as well. 
  The steps are as follows: 
  1. Install Cygwin: https://www.cygwin.com/install.html 
  2. Install Expat: https://libexpat.github.io/ (prerequisite for ReadOSM)
  3. Download the appropriate ReadOSM 
  4. From Cygwin run:  ./configure
                        make
                        make install 
  Note: The installation is quite tricky because there are many prerequisite packages. If it fails for some reason, saying that it cannot     find something, there is a high chance that it is a package that can be installed from Cygwin. 
  After the installation is complete, compile the Parser code as follows: g++ -I/usr/local/include parseinput.cpp parseinput.h –o
  my_program –L/usr/local/lib – lreadosm –lexpat –lz

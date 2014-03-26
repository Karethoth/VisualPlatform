#pragma once
#ifndef IMAGEIO_HH
#define IMAGEIO_HH

#include <iostream>
#include <memory>

#include "image.hh"


void WriteJpegFile( const std::string &filename, const Image &image, int quality );
std::shared_ptr<Image> ReadJpegFile( const std::string &filename );


#endif


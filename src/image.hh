#pragma once
#ifndef IMAGE_HH
#define IMAGE_HH

#include <iostream>
#include <jpeglib.h>
#include <vector>


enum ImageColorSpace
{
	COLOR_SPACE_UNKNOWN,
	COLOR_SPACE_RBG,
	COLOR_SPACE_HSV,
	COLOR_SPACE_HSL
};


struct ImageInfo
{
	unsigned int    width;
	unsigned int    height;
	unsigned char   bitdepth;
	ImageColorSpace colorspace;
};



struct Image
{
	ImageInfo            info;
	std::vector<JSAMPLE> data;
};


#endif


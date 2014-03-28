#pragma once
#ifndef IMAGE_HH
#define IMAGE_HH

#include <iostream>
#include <jpeglib.h>
#include <vector>
#include <memory>


enum ColorSpace
{
	COLOR_SPACE_UNKNOWN,
	COLOR_SPACE_RGB,
	COLOR_SPACE_HSV,
	COLOR_SPACE_HSL
};


// Predefine structs
struct ByteRGB;
struct RGB;
struct HSV;


// 0..255 RGB presentation of pixel
struct ByteRGB
{
	UINT8 r;
	UINT8 g;
	UINT8 b;

	ByteRGB() : r(0), g(0), b(0) {};
	ByteRGB( UINT8 _r, UINT8 _g, UINT8 _b ) : r(_r), g(_g), b(_b) {};

	ByteRGB( const RGB& );

	RGB ToRGB() const;
};


// 0..1 RGB presentation of pixel
struct RGB
{
	double r;
	double g;
	double b;

	RGB() : r(0), g(0), b(0) {};
	RGB( double _r, double _g, double _b ) : r(_r), g(_g), b(_b) {};

	RGB( const ByteRGB& );

	ByteRGB ToByteRGB() const;
};


// HSV presentation of pixel
struct HSV
{
	double h;
	double s;
	double v;

	HSV() : h(0), s(0), v(0) {};
	HSV( double _h, double _s, double _v ) : h(_h), s(_s), v(_v) {};
};


// Helpers
HSV RGBtoHSV( const RGB& );
RGB HSVtoRGB( const HSV& );



struct ImageInfo
{
	unsigned int  width;
	unsigned int  height;
	unsigned char bitdepth;
	ColorSpace    colorspace;
};



struct Image
{
	ImageInfo            info;
	std::vector<ByteRGB> pixels;
	std::vector<JSAMPLE> rawData;

	void UpdatePixels();
	void UpdateRawData();

	std::shared_ptr<Image> ScaleTo( unsigned int width, unsigned int height );
};


#endif


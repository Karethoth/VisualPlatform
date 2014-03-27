#include "image.hh"


// Main conversions between ByteRGBs and RGBs
ByteRGB::ByteRGB( const RGB &a )
{
	*this = a.ToByteRGB();
}

RGB::RGB( const ByteRGB &a )
{
	*this = a.ToRGB();
}



RGB ByteRGB::ToRGB() const
{
	return RGB( r/255.f, g/255.f, b/255.f );
};




ByteRGB RGB::ToByteRGB() const
{
	return ByteRGB( (char)(r*255.f),
	                (char)(g*255.f),
	                (char)(b*255.f) );
};



// Conversions between RGB and HSV color spaces
HSV RGBtoHSV( const RGB &rgb )
{
    HSV   out;
    float min, max, delta;

    min = rgb.r < rgb.g ? rgb.r : rgb.g;
    min = min  < rgb.b ? min  : rgb.b;

    max = rgb.r > rgb.g ? rgb.r : rgb.g;
    max = max  > rgb.b ? max  : rgb.b;

    out.v = max;
    delta = max - min;

    if( max > 0.f )
	{
        out.s = (delta / max);
    } else
	{
        out.s = 0.f;
        out.h = 0.f;
        return out;
    }
    if( rgb.r >= max )
        out.h = ( rgb.g - rgb.b ) / delta;
    else
    if( rgb.g >= max )
        out.h = 2.f + ( rgb.b - rgb.r ) / delta;
    else
        out.h = 4.f + ( rgb.r - rgb.g ) / delta;

    out.h *= 60.f;

    if( out.h < 0.0 )
        out.h += 360.0;

    return out;
}


RGB HSVtoRGB( const HSV &hsv )
{
    float      hh, p, q, t, ff;
    long        i;
    RGB         out;

    if( hsv.s <= 0.0 )
	{
        out.r = hsv.v;
        out.g = hsv.v;
        out.b = hsv.v;
        return out;
    }

    hh = hsv.h;

    if( hh >= 360.0 )
		hh = 0.0;
	else
	    hh /= 60.0;
    i = (long)hh;
    ff = hh - i;
    p = hsv.v * (1.f -  hsv.s);
    q = hsv.v * (1.f - (hsv.s * ff));
    t = hsv.v * (1.f - (hsv.s * (1.f - ff)));

    switch( i )
	{
    case 0:
        out.r = hsv.v;
        out.g = t;
        out.b = p;
        break;
    case 1:
        out.r = q;
        out.g = hsv.v;
        out.b = p;
        break;
    case 2:
        out.r = p;
        out.g = hsv.v;
        out.b = t;
        break;

    case 3:
        out.r = p;
        out.g = q;
        out.b = hsv.v;
        break;
    case 4:
        out.r = t;
        out.g = p;
        out.b = hsv.v;
        break;
    case 5:
    default:
        out.r = hsv.v;
        out.g = p;
        out.b = q;
        break;
    }
    return out;
}


void Image::UpdatePixels()
{
	unsigned char *p = &pixels[0].r;

	for( auto it = rawData.begin(); it != rawData.end(); it++ )
	{
		*(p++) = (*it);
	}
}


void Image::UpdateRawData()
{
	unsigned long index = 0;

	for( auto it = pixels.begin(); it != pixels.end(); it++ )
	{
		rawData[index++] = (*it).r;
		rawData[index++] = (*it).g;
		rawData[index++] = (*it).b;
	}
}


std::shared_ptr<Image> Image::ScaleTo( unsigned int width, unsigned int height )
{
	auto img = std::make_shared<Image>();
	img->info.width      = width;
	img->info.height     = height;
	img->info.bitdepth   = 24;
	img->info.colorspace = COLOR_SPACE_RGB;

	img->rawData = std::vector<JSAMPLE>( width * height * 3 );
	img->pixels  = std::vector<ByteRGB>( width * height );

	double ratioX = (double)info.width  / (double)width;
	double ratioY = (double)info.height / (double)height;


	for( unsigned int y = 0; y < height; ++y )
	{
		float posY = (float)(y*ratioY);
		for( unsigned int x = 0; x < width; ++x )
		{
			float posX = (float)(x*ratioX);
			img->pixels[y*width+x] = pixels[ (unsigned int)posY*info.width + (unsigned int)posX ];
		}
	}

	img->UpdateRawData();

	return img;
}


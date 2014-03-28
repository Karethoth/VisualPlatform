#pragma once
#ifndef MAIN_HH
#define MAIN_HH

#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <random>

#include <jpeglib.h>

#include "imageIO.hh"
#include "kMeans.hh"

#include "win/escapi/escapi.h"


struct Vector2D
{
	long long x;
	long long y;


	Vector2D()
	{
		x = 0;
		y = 0;
	}

	Vector2D& operator+=( const Vector2D &other )
	{
		this->x += other.x;
		this->y += other.y;
		return *this;
	}

	Vector2D& operator/=( const float &divider )
	{
		this->x /= divider;
		this->y /= divider;
		return *this;
	}
};

struct HSVElement : KMeans::Element
{
	HSV      hsv;
	Vector2D position;
	std::vector<double> hueAngles;

	HSVElement()
	{
		hsv = HSV( 0, 0, 0 );
		position.x = 0;
		position.y = 0;
	}

	float Distance( const Element *other )
	{
		const HSVElement *p = (const HSVElement*)other;
		HSVElement distance;
		distance.position.x = p->position.x - position.x;
		distance.position.y = p->position.y - position.y;
		distance.hsv.h = p->hsv.h - hsv.h;
		distance.hsv.s = p->hsv.s - hsv.s;
		distance.hsv.v = p->hsv.v - hsv.v;

		if( distance.position.x < 0 )
			distance.position.x *= -1.0;
		if( distance.position.y < 0 )
			distance.position.y *= -1.0;
		if( distance.hsv.h < 0 )
			distance.hsv.h *= -1.0;
		if( distance.hsv.s < 0 )
			distance.hsv.s *= -1.0;
		if( distance.hsv.v < 0 )
			distance.hsv.v *= -1.0;

		return distance.position.x/50.0 +
		       distance.position.y/50.0 +
			   distance.hsv.h*2 +
		       distance.hsv.s*2 +
		       distance.hsv.v*2;
	}

	void Add( const KMeans::Element *other )
	{
		const HSVElement *p = (const HSVElement*)other;
		position += p->position;
		hsv.s    += p->hsv.s;
		hsv.v    += p->hsv.v;
		hueAngles.push_back( p->hsv.h );
	}

	void Divide( const float divider )
	{
		position /= divider;
		hsv.s    /= divider;
		hsv.v    /= divider;

		double x = 0, y = 0;
		for( auto angle  = hueAngles.begin();
		          angle != hueAngles.end();
		          angle++ )
		{
			x += cosf( (*angle) * 3.14f / 180.f );
			y += sinf( (*angle) * 3.14f / 180.f );
		}

		hsv.h = atan2f( y, x )  * 180.f / 3.14f;
		if( hsv.h < 0 )
		{
			hsv.h += 360.0;
		}
		else if( hsv.h > 360.0 )
		{
			hsv.h -= 360.0;
		}
		hueAngles.clear();
	}
};

#endif


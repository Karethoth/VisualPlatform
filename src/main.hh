#pragma once
#ifndef MAIN_HH
#define MAIN_HH

#include <iostream>
#include <fstream>
#include <vector>
#include <random>

#include <jpeglib.h>

#include "imageIO.hh"
#include "kMeans.hh"


struct Vector2D
{
	long x;
	long y;

	Vector2D operator+=( const Vector2D &other )
	{
		Vector2D ret = *this;
		ret.x += other.x;
		ret.y += other.y;
		return ret;
	}

	Vector2D operator/=( const float &divider )
	{
		Vector2D ret = *this;
		ret.x = (long)(ret.x / divider);
		ret.y = (long)(ret.y / divider);
		return ret;
	}

	bool operator>=( const Vector2D &other )
	{
		return((other.x*2.f + other.y*2.f) >= (x*2.f + y*2.f));
	}
};

struct HSVElement : KMeans::Element<Vector2D>
{
	HSV hsv;

	Vector2D Distance( const Element<Vector2D> &other )
	{
		Vector2D distance;
		distance.x = other.position.x - position.x;
		distance.y = other.position.y - position.y;
		return distance;
	}
};

#endif


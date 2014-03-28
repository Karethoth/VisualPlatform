#include "main.hh"

using namespace std;


vector<string> ValidateFilepaths( const vector<string>& filepaths )
{
	vector<string> inputFilepaths;

	for( auto e=filepaths.begin(); e != filepaths.end(); e++ )
	{
		ifstream inputHandle( (*e).c_str() );

		if( !inputHandle.is_open() )
		{
			std::cout << "Couldn't open file '" << (*e).c_str() << "'" << endl;
			continue;
		}

		inputFilepaths.push_back( (*e) );
		inputHandle.close();
	}

	return inputFilepaths;
}


std::shared_ptr<Image> GetWebcamImage( int device, SimpleCapParams &capture )
{
	doCapture( device );

	auto img = std::make_shared<Image>();
	img->info.width  = 320;
	img->info.height = 240;
	img->info.bitdepth   = 24;
	img->info.colorspace = COLOR_SPACE_RGB;
	img->pixels.resize(  img->info.width * img->info.height );
	img->rawData.resize( img->info.width * img->info.height * 3 );

	while( isCaptureDone( device ) == 0 )
	{
	}

	for( auto index = 0; index < 320 * 240; ++index )
	{
		unsigned int rgba = (unsigned int)capture.mTargetBuf[index];
		ByteRGB rgb;
		rgb.r = (rgba>>24);
		rgb.g = (rgba>>16);
		rgb.b = (rgba>>8);
		img->pixels[index] = rgb;
	}
	return img;
}


int main( int argc, char **argv )
{
	// Setup random number generator
	mt19937 rng;
	rng.seed( random_device()() );
	uniform_int_distribution<uint32_t> random;
	srand( random( rng ) );


	int devices = setupESCAPI();
	if( !devices )
	{
		std::cerr << "Couldn't open any webcam devices!" << endl;
		return 1;
	}

	// Initialize captures for webcam(s)
	SimpleCapParams *captures = new SimpleCapParams[devices];
	for( int dev=0; dev < devices; ++dev )
	{
		captures[dev].mWidth  = 320;
		captures[dev].mHeight = 240;
		captures[dev].mTargetBuf = new int[320 * 240];
		if( initCapture( dev, &captures[dev] ) == 0 )
		{
			std::cout << "Capture failed - device may already be in use." << endl;
		}
	}


	// Get 10 images from each webcam and handle them
	for( int counter=0; counter < 10; ++counter )
	for( int dev=0; dev < devices; ++dev )
	{
		try
		{
			// Fetch image from the webcam
			auto img = GetWebcamImage( dev, captures[dev] );
			if( img == nullptr )
			{
				std::cerr << "Couldn't fetch the image!" << endl;
				continue;
			}
			img->UpdateRawData();

			auto resized = img;

			// Construct the elements for the k-means algorithm
			std::vector<HSVElement*> elements;

			unsigned int posX=0;
			unsigned int posY=0;
			for( auto it  = resized->pixels.begin();
					  it != resized->pixels.end();
					  it++ )
			{
				HSV hsv = RGBtoHSV( *it );
				HSVElement *element = new HSVElement();
				element->clusterId  = -1;
				element->hsv = hsv;
				element->position.x = posX;
				element->position.y = posY;
				elements.push_back( element );

				if( ++posX >= resized->info.width )
				{
					posX = 0;
					posY++;
				}
			}

			std::cout << "Generating elements for K-Means.." << endl;

			KMeans::KMeans<HSVElement> kmeans;
			kmeans.Init( elements, 5 ); // Five clusters

			for( auto cluster  = kmeans.clusters.begin();
			          cluster != kmeans.clusters.end();
			          cluster++ )
			{
				HSVElement tmpElement = *(kmeans.data[rand() % kmeans.data.size()]);
				tmpElement.clusterId  = (*cluster)->id;
				(*cluster)->centroid  = tmpElement;
			}

			std::cout << "Done, now finding the clusters..." << endl;

			long changes = 1;
			do
			{
				std::cout << "changes: " << changes << endl;
				for( auto cluster  = kmeans.clusters.begin();
				          cluster != kmeans.clusters.end();
				          cluster++ )
				{
					HSVElement centroid = (*cluster)->centroid;
				}
			} while( (changes = kmeans.Update()) );
			std::cout << "Convergence found." << endl;



			std::cout << endl << "Generating new image." << endl;
			Image clusterImg;
			clusterImg.info.width  = resized->info.width;
			clusterImg.info.height = resized->info.height;
			clusterImg.info.bitdepth   = 24;
			clusterImg.info.colorspace = COLOR_SPACE_RGB;
			clusterImg.pixels = resized->pixels;
			clusterImg.rawData.resize( resized->info.width * resized->info.height * 3 );
 
			for( auto cluster  = kmeans.clusters.begin();
			          cluster != kmeans.clusters.end();
			          cluster++ )
			{
				HSVElement element = (*cluster)->centroid;
				ByteRGB    color   = HSVtoRGB( element.hsv );

				for( auto index  = (*cluster)->indices.begin();
				          index != (*cluster)->indices.end();
				          index++ )
				{
					Vector2D position = kmeans.data[ *index ]->position;
					clusterImg.pixels[ position.y * clusterImg.info.width +
										position.x ] = color;
				}
			}
			clusterImg.UpdateRawData();

			ostringstream oss;
			oss << "outp/" << dev << "_" << counter << ".jpg";
			string path = oss.str();

			oss = ostringstream();
			oss << "outp/" << dev << "_" << counter << "o.jpg";
			string path2 = oss.str();

			cout << endl << "Writing it out as '" << path.c_str() << "'.." << endl;
			WriteJpegFile( path, clusterImg, 100 );
			WriteJpegFile( path2, *img, 100 );
			cout << "Success." << endl;
			resized.reset();
			img.reset();
		}
		catch( exception &e )
		{
			cout << "Failed! Reason was:" << endl;
			cout << " \"" << e.what() << "\"" << endl;
		}
	}

	for( int dev=0; dev < devices; ++dev )
	{
		deinitCapture( dev );
		delete[] captures[dev].mTargetBuf;
	}

	cout << "Done!" << endl;

	return 0;
}


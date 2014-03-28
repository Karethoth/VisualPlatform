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



int main( int argc, char **argv )
{
	if( argc <= 1 )
	{
		cout << "Give me the filepath to the image or many images." << endl;
		return 1;
	}

	// Setup random number generator
	mt19937 rng;
	rng.seed( random_device()() );
	uniform_int_distribution<uint32_t> random;
	srand( random( rng ) );

	// Generate vector of arguments
	vector<string> filepaths;
	for( int i = argc-1; i > 0; --i )
	{
		char *filepath = argv[i];
		filepaths.push_back( string( filepath ) );
	}
	
	// Validate the filepaths
	filepaths = ValidateFilepaths( filepaths );


	// Make sure we got at least one valid path to work with
	if( filepaths.size() <= 0 )
	{
		cout << "No inputs could be opened." << endl;
		return 1;
	}

	// Let user know what files we were able to open
	cout << endl << "Were able to open following file(s):" << endl;
	for( auto e = filepaths.begin(); e != filepaths.end(); e++ )
	{
		cout << " - " << (*e).c_str() << endl;
	}

	cout << endl << "Generating copies.." << endl << endl;

	for( auto e = filepaths.begin(); e != filepaths.end(); e++ )
	{
		shared_ptr<Image> img;

		try
		{
			cout << endl << "Reading image '" << (*e).c_str() << "'.." << endl;

			img = ReadJpegFile( *e );

			cout << "Success, image info:" << endl;
			cout << " - Size:  '" << img->info.width << "x" << img->info.height << "':" << endl;

			cout << endl << "Scaling the image.." << endl;
			auto resized = img->ScaleTo( (unsigned int)(img->info.width  * 0.2f),
			                             (unsigned int)(img->info.height * 0.2f) );
			cout << "Success." << endl;


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


			std::string path = *e;
			path[0] = 'o';
			path[1] = 'u';
			path[2] = 't';
			path[3] = 'p';
			cout << endl << "Writing it out as '" << path.c_str() << "'.." << endl;
			WriteJpegFile( path, clusterImg, 100 );
			cout << "Success." << endl;
			resized.reset();
			img.reset();
		}
		catch( exception &e )
		{
			cout << "Failed! Reason was:" << endl;
			cout << " \"" << e.what() << "\"" << endl;
			continue;
		}
	}

	cout << "Done!" << endl;

	return 0;
}


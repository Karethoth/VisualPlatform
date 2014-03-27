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
			auto resized = img->ScaleTo( (unsigned int)(img->info.width  * 0.1f),
			                             (unsigned int)(img->info.height * 0.1f) );
			cout << "Success." << endl;



			// Construct the elements for the k-means algorithm
			std::vector<KMeans::Element<Vector2D>*> elements;

			unsigned int posX=0;
			unsigned int posY=0;
			for( auto it  = resized->pixels.begin();
			          it != resized->pixels.end();
			          it++ )
			{
				HSV hsv = RGBtoHSV( *it );
				HSVElement *element = new HSVElement();
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

			KMeans::KMeans<Vector2D> kmeans;
			kmeans.Init( elements, 2 );

			for( auto cluster  = kmeans.clusters.begin();
			          cluster != kmeans.clusters.end();
			          cluster++ )
			{
				auto tmpElement = new HSVElement();
				tmpElement->position.x =  rand() % resized->info.width;
				tmpElement->position.y =  rand() % resized->info.height;
				tmpElement->hsv.h      = (rand() % 360) * 1.f;
				tmpElement->hsv.s      = (rand() % 100) * 0.01f;
				tmpElement->hsv.v      = (rand() % 100) * 0.01f;
				(*cluster)->centroid = tmpElement;
			}

			long changes = 1;
			while( (changes = kmeans.Update()) )
			{
				std::cout << "changes: " << changes << endl;
			}

			cout << endl << "Writing it out as 'test.jpg'.." << endl;
			WriteJpegFile( "test.jpg", *resized, 100 );
			cout << "Success." << endl;
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


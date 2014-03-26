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

			for( auto it=img->pixels.begin(); it != img->pixels.end(); it++ )
			{
				HSV tmp = RGBtoHSV( *it );

				tmp.h += 180.f;
				if( tmp.h > 360.f )
					tmp.h -= 360.f;

				(*it) = HSVtoRGB( tmp ).ToByteRGB();
			}

			img->UpdateRawData();

			cout << endl << "Writing it out as 'test.jpg'.." << endl;
			WriteJpegFile( "test.jpg", *img, 50 );
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


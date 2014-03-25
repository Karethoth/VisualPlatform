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

	}

	// Let user know what files we were able to open
	cout << endl << "Were able to open following file(s):" << endl;
	for( auto e = filepaths.begin(); e != filepaths.end(); e++ )
	{
		cout << " - " << (*e).c_str() << endl;
	}

	return 0;
}


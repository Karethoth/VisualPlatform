#include "imageIO.hh"
#include <iostream>
#include <fstream>
#include <vector>
#include <exception>



// Helpers for writing to memory
std::vector<JOCTET> outBuffer;
unsigned long outBufferSize;


void InitDestination( j_compress_ptr cinfo )
{
    outBuffer.resize( outBufferSize );
    cinfo->dest->next_output_byte = &outBuffer[0];
    cinfo->dest->free_in_buffer = outBuffer.size();
}

boolean EmptyOutputBuffer( j_compress_ptr cinfo )
{
    size_t oldsize = outBuffer.size();
    outBuffer.resize(oldsize + outBufferSize );
    cinfo->dest->next_output_byte = &outBuffer[oldsize];
    cinfo->dest->free_in_buffer = outBuffer.size() - oldsize;
    return true;
}

void TermDestination( j_compress_ptr cinfo )
{
    outBuffer.resize( outBuffer.size() - cinfo->dest->free_in_buffer );
}



// Write
void WriteJpegFile( const std::string &filename, const Image &image, int quality=100 )
{
	struct jpeg_compress_struct cinfo;
	struct jpeg_error_mgr       jerr;

	int rowStride;

	std::ofstream fout( filename, std::ios::out | std::ios::binary);
	if( !fout.is_open() )
	{
		std::string errorMsg = "Couldn't open the file '";
		errorMsg += filename;
		errorMsg += "'!";
		throw std::exception( errorMsg.c_str() );
	}

	// Well sized buffer, just in case
	outBufferSize = sizeof( JSAMPLE ) * image.info.width * 5;
	outBuffer = std::vector<JOCTET>( outBufferSize );

	cinfo.err = jpeg_std_error( &jerr );
	jpeg_create_compress( &cinfo );


	jpeg_mem_dest( &cinfo, (unsigned char**)(&outBuffer[0]), &outBufferSize );
	cinfo.dest->init_destination    = InitDestination;
	cinfo.dest->empty_output_buffer = EmptyOutputBuffer;
	cinfo.dest->term_destination    = TermDestination;

	/* Set parameters for compression */
	cinfo.image_width      = image.info.width;
	cinfo.image_height     = image.info.height;
	cinfo.input_components = 3;
	cinfo.in_color_space   = JCS_RGB;

	jpeg_set_defaults( &cinfo );
	jpeg_set_quality( &cinfo, quality, TRUE );

	/* Start compressor */
	jpeg_start_compress( &cinfo, TRUE );

	// Loop
	rowStride = image.info.width * 3;	/* JSAMPLEs per row in image_buffer */

	JSAMPROW row = (JSAMPROW)malloc( sizeof( JSAMPLE ) * rowStride );

	while( cinfo.next_scanline < cinfo.image_height )
	{
		memcpy( row,
		        &image.rawData[cinfo.next_scanline * rowStride],
				sizeof( JSAMPLE ) * rowStride );

		jpeg_write_scanlines( &cinfo, &row, 1 );
	}

	
	fout.write( (const char*)outBuffer.data(), outBuffer.size() );
	fout.close();

	// Finish
	jpeg_finish_compress( &cinfo );
	jpeg_destroy_compress( &cinfo );
}



// Helpers for reading (error handling)
struct JpegErrorHandler
{
  struct jpeg_error_mgr pub;
  jmp_buf               setjmpBuffer;
};


void CustomErrorHandler( j_common_ptr cinfo )
{
  JpegErrorHandler *myerr = (JpegErrorHandler*)cinfo->err;

  (*cinfo->err->output_message)( cinfo );
  longjmp( myerr->setjmpBuffer, 1 );
}



// Read
std::shared_ptr<Image> ReadJpegFile( const std::string &filename )
{
	struct jpeg_decompress_struct cinfo;
	
	JSAMPARRAY buffer;
	int        rowStride;
	struct JpegErrorHandler jerr;

	std::vector<unsigned char> dataIn;

	std::ifstream file;
	std::streamsize size;

	std::shared_ptr<Image> img = std::make_shared<Image>();


	/* We set up the normal JPEG error routines, then override error_exit. */
	cinfo.err = jpeg_std_error( &jerr.pub );
	jerr.pub.error_exit = CustomErrorHandler;


	/* Establish the setjmp return context for CustomErrorHandler to use. */
	if( setjmp( jerr.setjmpBuffer ) )
	{
		/* If we get here, the JPEG code has signaled an error.
			* We need to clean up the JPEG object, close the input file, and return.
			*/
		jpeg_destroy_decompress( &cinfo );

		std::string errorMsg = "Couldn't read the JPEG file '";
		errorMsg += filename;
		errorMsg += "'! Maybe it's corrupted?";
		throw std::exception( errorMsg.c_str() );
	}


	// Read the data to memory
	file = std::ifstream( filename, std::ios::binary);
	file.seekg( 0, std::ios::end );
	size = file.tellg();
	file.seekg( 0, std::ios::beg );

	dataIn = std::vector<unsigned char>( static_cast<unsigned int>( size ) );
	if( !file.read( (char*)( dataIn.data() ), size ) )
	{
		std::string errorMsg = "Couldn't open the file '";
		errorMsg += filename;
		errorMsg += "'!";
		throw std::exception( errorMsg.c_str() );
	}


	// Start the decompression operation
	jpeg_create_decompress( &cinfo );
	jpeg_mem_src( &cinfo, dataIn.data(), dataIn.size() );

	// Read the header
	jpeg_read_header( &cinfo, TRUE );
	img->info.width      = cinfo.image_width;
	img->info.height     = cinfo.image_height;
	img->info.bitdepth   = 24;
	img->info.colorspace = COLOR_SPACE_RGB;

	jpeg_start_decompress( &cinfo );


	rowStride = cinfo.output_width * cinfo.output_components;

	/* Make a one-row-high sample array that will go away when done with image */
	buffer = (*cinfo.mem->alloc_sarray)( (j_common_ptr) &cinfo, JPOOL_IMAGE, rowStride, 1 );


	while( cinfo.output_scanline < cinfo.output_height )
	{
		jpeg_read_scanlines( &cinfo, buffer, 1 );
		
		for( int i=0; i < rowStride; ++i )
		{
			img->rawData.push_back( buffer[0][i] );
		}
	}

	img->pixels = std::vector<ByteRGB>( img->info.width * img->info.height );
	img->UpdatePixels();

	/* Finish*/
	jpeg_finish_decompress( &cinfo );
	jpeg_destroy_decompress( &cinfo );
	return img;
}

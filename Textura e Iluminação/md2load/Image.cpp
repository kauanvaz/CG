/* -*- c++ -*- */
/////////////////////////////////////////////////////////////////////////////
//
// Image.cpp -- Copyright (c) 2006-2007 David Henry
// last modification: may. 7, 2007
//
// This code is licenced under the MIT license.
//
// This software is provided "as is" without express or implied
// warranties. You may freely copy and compile this source into
// applications you distribute provided that the copyright text
// below is included in the resulting source code.
//
// Implementation of image loader classes for TGA and PCX image formats.
//
/////////////////////////////////////////////////////////////////////////////

#include <GL/glew.h>
#include "Image.h"


/////////////////////////////////////////////////////////////////////////////
//
// class ImageBuffer implementation.
//
/////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
// ImageBuffer::ImageBuffer
//
// Constructor.  Load a file into memory buffer.
// --------------------------------------------------------------------------

ImageBuffer::ImageBuffer (const string &filename)
  : _filename (filename), _data (NULL), _length (0)
{
  // Open file
  std::ifstream ifs (filename.c_str(), std::ios::in | std::ios::binary);

  if (ifs.fail ())
    throw ImageException ("Couldn't open file", filename);

  // Get file length
  ifs.seekg (0, std::ios::end);
  _length = ifs.tellg ();
  ifs.seekg (0, std::ios::beg);

  try
    {
      // Allocate memory for holding file data
      _data = new GLubyte[_length];

      // Read whole file data
      ifs.read (reinterpret_cast<char *> (_data), _length);
      ifs.close ();
    }
  catch (...)
    {
      delete [] _data;
      throw;
    }
}


// --------------------------------------------------------------------------
// ImageBuffer::~ImageBuffer
//
// Destructor.  Free memory buffer.
// --------------------------------------------------------------------------

ImageBuffer::~ImageBuffer ()
{
  delete [] _data;
  _data = NULL;
}


/////////////////////////////////////////////////////////////////////////////
//
// class Image implementation.
//
/////////////////////////////////////////////////////////////////////////////

// --------------------------------------------------------------------------
// Image::Image
//
// Constructors.  Create an image from a pixel buffer.  It allows to
// create an image from an other source of data than an ImageBuffer.
// This is also the copy constructor to use if you want pixel data
// to be copied, since the Image(const Image&) constructor is protected.
// --------------------------------------------------------------------------

Image::Image (const string &name, GLsizei w, GLsizei h, GLint numMipMaps,
	      GLenum format, GLint components, const GLubyte *pixels,
	      bool stdCoordSystem)
  : _width (w), _height (h), _numMipmaps (numMipMaps),
    _format (format), _components (components), _name (name),
    _standardCoordSystem (stdCoordSystem)
{
  // NOTE: pixels should be a valid pointer. w, h and components
  // have to be non-zero positive values.

  long size = _width * _height * _components;

  if (size <= 0)
    throw std::invalid_argument
      ("Image::Image: Invalid width, height or component value");

  if (!pixels)
    throw std::invalid_argument
      ("Image::Image: Invalid pixel data source");

  // allocate memory for pixel data
  _pixels = new GLubyte[size];

  // Copy pixel data from buffer
  memcpy (_pixels, pixels, size);
}


// --------------------------------------------------------------------------
// Image::~Image
//
// Destructor.  Delete all memory allocated for this object, i.e. pixel
// data.
// --------------------------------------------------------------------------

Image::~Image ()
{
  delete [] _pixels;
  _pixels = NULL;
}


// --------------------------------------------------------------------------
// Image::isPowerOfTwo
//
// Check if the image dimensions are powers of two.
// --------------------------------------------------------------------------

bool
Image::isPowerOfTwo () const
{
  GLsizei m;
  for (m = 1; m < _width; m *= 2)
    ;

  if (m != _width)
    return false;

  for (m = 1; m < _height; m *= 2)
    ;

  if (m != _height)
    return false;

  return true;
}


/////////////////////////////////////////////////////////////////////////////
//
// class ImageTGA implementation.
//
/////////////////////////////////////////////////////////////////////////////

// Pixel's component table access
int ImageTGA::rgbaTable[4] = { 2, 1, 0, 3 };
int ImageTGA::bgraTable[4] = { 0, 1, 2, 3 };


// --------------------------------------------------------------------------
// ImageTGA::ImageTGA
//
// Constructor.  Read a TGA image from memory.
// --------------------------------------------------------------------------

ImageTGA::ImageTGA (const ImageBuffer &ibuff)
  : _header (NULL)
{
  const GLubyte *colormap = NULL;
  const GLubyte *data_ptr;

  try
    {
      _name = ibuff.filename ();
      data_ptr = ibuff.data ();
      _standardCoordSystem = true;

      // Read TGA header
      _header = reinterpret_cast<const TGA_Header *>(data_ptr);
      data_ptr += sizeof (TGA_Header) + _header->id_lenght;

      // Get image information
      getTextureInfo ();

      // Memory allocation for pixel data
      _pixels = new GLubyte[_width * _height * _components];

      // Read color map, if present
      if (_header->colormap_type)
	{
	  // NOTE: color map is stored in BGR
	  colormap = data_ptr;
	  data_ptr += _header->cm_length * (_header->cm_size >> 3);
	}

      // Decode image data
      switch (_header->image_type)
	{
	case 0:
	  // No data
	  break;

	case 1:
	  // Uncompressed 8 bits color index
	  readTGA8bits (data_ptr, colormap);
	  break;

	case 2:
	  // Uncompressed 16-24-32 bits
	  switch (_header->pixel_depth)
	    {
	    case 16:
	      readTGA16bits (data_ptr);
	      break;

	    case 24:
	      readTGA24bits (data_ptr);
	      break;

	    case 32:
	      readTGA32bits (data_ptr);
	      break;
	    }

	  break;

	case 3:
	  // Uncompressed 8 or 16 bits grayscale
	  if (_header->pixel_depth == 8)
	    readTGAgray8bits (data_ptr);
	  else // 16 bits
	    readTGAgray16bits (data_ptr);

	  break;

	case 9:
	  // RLE compressed 8 bits color index
	  readTGA8bitsRLE (data_ptr, colormap);
	  break;

	case 10:
	  // RLE compressed 16-24-32 bits
	  switch (_header->pixel_depth)
	    {
	    case 16:
	      readTGA16bitsRLE (data_ptr);
	      break;

	    case 24:
	      readTGA24bitsRLE (data_ptr);
	      break;

	    case 32:
	      readTGA32bitsRLE (data_ptr);
	      break;
	    }

	  break;

	case 11:
	  // RLE compressed 8 or 16 bits grayscale
	  if (_header->pixel_depth == 8)
	    readTGAgray8bitsRLE (data_ptr);
	  else // 16 bits
	    readTGAgray16bitsRLE (data_ptr);

	  break;

	default:
	  // Image type is not correct, free memory and quit
	  throw ImageException ("Unknown TGA image type", _name);
	}
    }
  catch (...)
    {
      delete [] _pixels;
      throw;
    }
}


// --------------------------------------------------------------------------
// ImageTGA::getTextureInfo
//
// Extract OpenGL texture informations from TGA header.
// --------------------------------------------------------------------------

void
ImageTGA::getTextureInfo ()
{
  _width = _header->width;
  _height = _header->height;

  switch (_header->image_type)
    {
    case 3:  // grayscale 8 bits
    case 11: // grayscale 8 bits (RLE)
      {
	if (_header->pixel_depth == 8)
	  {
	    _format = GL_LUMINANCE;
	    _components = 1;
	  }
	else // 16 bits
	  {
	    _format = GL_LUMINANCE_ALPHA;
	    _components = 2;
	  }

	break;
      }

    case 1:    // 8 bits color index
    case 2:    // BGR 16-24-32 bits
    case 9:    // 8 bits color index (RLE)
    case 10:   // BGR 16-24-32 bits (RLE)
      {
	// 8 bits and 16 bits images will be converted to 24 bits
	if (_header->pixel_depth <= 24)
	  {
	    _format = GLEW_EXT_bgra ? GL_BGR : GL_RGB;
	    _components = 3;
	  }
	else // 32 bits
	  {
	    _format = GLEW_EXT_bgra ? GL_BGRA : GL_RGBA;
	    _components = 4;
	  }

	break;
      }
    }
}


// --------------------------------------------------------------------------
// ImageTGA::readTGA8bits
//
// Read 8 bits pixel data from TGA file.
// --------------------------------------------------------------------------

void
ImageTGA::readTGA8bits (const GLubyte *data, const GLubyte *colormap)
{
  const GLubyte *pData = data;
  int *compTable = GLEW_EXT_bgra ? bgraTable : rgbaTable;

  for (int i = 0; i < _width * _height; ++i)
    {
      // Read index color byte
      GLubyte color = *(pData++);

      // Convert to BGR/RGB 24 bits
      _pixels[(i * 3) + compTable[0]] = colormap[(color * 3) + 0];
      _pixels[(i * 3) + compTable[1]] = colormap[(color * 3) + 1];
      _pixels[(i * 3) + compTable[2]] = colormap[(color * 3) + 2];
    }
}


// --------------------------------------------------------------------------
// ImageTGA::readTGA16bits
//
// Read 16 bits pixel data from TGA file.
// --------------------------------------------------------------------------

void
ImageTGA::readTGA16bits (const GLubyte *data)
{
  const GLubyte *pData = data;
  int *compTable = GLEW_EXT_bgra ? bgraTable : rgbaTable;

  for (int i = 0; i < _width * _height; ++i, pData += 2)
    {
      // Read color word
      unsigned short color = pData[0] + (pData[1] << 8);

      // convert to BGR/RGB 24 bits
      _pixels[(i * 3) + compTable[2]] = (((color & 0x7C00) >> 10) << 3);
      _pixels[(i * 3) + compTable[1]] = (((color & 0x03E0) >>  5) << 3);
      _pixels[(i * 3) + compTable[0]] = (((color & 0x001F) >>  0) << 3);
    }
}


// --------------------------------------------------------------------------
// ImageTGA::readTGA24bits
//
// Read 24 bits pixel data from TGA file.
// --------------------------------------------------------------------------

void
ImageTGA::readTGA24bits (const GLubyte *data)
{
  if (GLEW_EXT_bgra)
    {
      memcpy (_pixels, data, _width * _height * 3);
    }
  else
    {
      const GLubyte *pData = data;

      for (int i = 0; i < _width * _height; ++i)
	{
	  // Read RGB 24 bits pixel
	  _pixels[(i * 3) + rgbaTable[0]] = *(pData++);
	  _pixels[(i * 3) + rgbaTable[1]] = *(pData++);
	  _pixels[(i * 3) + rgbaTable[2]] = *(pData++);
	}
    }
}


// --------------------------------------------------------------------------
// ImageTGA::readTGA32bits
//
// Read 32 bits pixel data from TGA file.
// --------------------------------------------------------------------------

void
ImageTGA::readTGA32bits (const GLubyte *data)
{
  if (GLEW_EXT_bgra)
    {
      memcpy (_pixels, data, _width * _height * 4);
    }
  else
    {
      const GLubyte *pData = data;

      for (int i = 0; i < _width * _height; ++i)
	{
	  // Read RGB 32 bits pixel
	  _pixels[(i * 4) + rgbaTable[0]] = *(pData++);
	  _pixels[(i * 4) + rgbaTable[1]] = *(pData++);
	  _pixels[(i * 4) + rgbaTable[2]] = *(pData++);
	  _pixels[(i * 4) + rgbaTable[3]] = *(pData++);
	}
    }
}


// --------------------------------------------------------------------------
// ImageTGA::readTGAgray8bits
//
// Read grey 8 bits pixel data from TGA file.
// --------------------------------------------------------------------------

void
ImageTGA::readTGAgray8bits (const GLubyte *data)
{
  memcpy (_pixels, data, _width * _height);
}


// --------------------------------------------------------------------------
// ImageTGA::readTGAgray16bits
//
// Read grey 16 bits pixel data from TGA file.
// --------------------------------------------------------------------------

void
ImageTGA::readTGAgray16bits (const GLubyte *data)
{
  memcpy (_pixels, data, _width * _height * 2);
}


// --------------------------------------------------------------------------
// ImageTGA::readTGA8bitsRLE
//
// Read 8 bits pixel data from TGA file with RLE compression.
// --------------------------------------------------------------------------

void
ImageTGA::readTGA8bitsRLE (const GLubyte *data, const GLubyte *colormap)
{
  GLubyte *ptr = _pixels;
  const GLubyte *pData = data;
  int *compTable = GLEW_EXT_bgra ? bgraTable : rgbaTable;

  while (ptr < _pixels + (_width * _height) * 3)
    {
      // Read first byte
      GLubyte packet_header = *(pData++);
      int size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
	{
	  // Run-length packet
	  GLubyte color = *(pData++);

	  for (int i = 0; i < size; ++i, ptr += 3)
	    {
	      ptr[0] = colormap[(color * 3) + compTable[0]];
	      ptr[1] = colormap[(color * 3) + compTable[1]];
	      ptr[2] = colormap[(color * 3) + compTable[2]];
	    }
	}
      else
	{
	  // Non run-length packet
	  for (int i = 0; i < size; ++i, ptr += 3)
	    {
	      GLubyte color = *(pData++);

	      ptr[0] = colormap[(color * 3) + compTable[0]];
	      ptr[1] = colormap[(color * 3) + compTable[1]];
	      ptr[2] = colormap[(color * 3) + compTable[2]];
	    }
	}
    }
}


// --------------------------------------------------------------------------
// ImageTGA::readTGA16bitsRLE
//
// Read 16 bits pixel data from TGA file with RLE compression.
// --------------------------------------------------------------------------

void
ImageTGA::readTGA16bitsRLE (const GLubyte *data)
{
  const GLubyte *pData = data;
  GLubyte *ptr = _pixels;
  int *compTable = GLEW_EXT_bgra ? bgraTable : rgbaTable;

  while (ptr < _pixels + (_width * _height) * 3)
    {
      // Read first byte
      GLubyte packet_header = *(pData++);
      int size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
	{
	  // Run-length packet
	  unsigned short color = pData[0] + (pData[1] << 8);
	  pData += 2;

	  for (int i = 0; i < size; ++i, ptr += 3)
	    {
	      ptr[compTable[2]] = (((color & 0x7C00) >> 10) << 3);
	      ptr[compTable[1]] = (((color & 0x03E0) >>  5) << 3);
	      ptr[compTable[0]] = (((color & 0x001F) >>  0) << 3);
	    }
	}
      else
	{
	  // Non run-length packet
	  for (int i = 0; i < size; ++i, ptr += 3)
	    {
	      unsigned short color = pData[0] + (pData[1] << 8);
	      pData += 2;

	      ptr[compTable[2]] = (((color & 0x7C00) >> 10) << 3);
	      ptr[compTable[1]] = (((color & 0x03E0) >>  5) << 3);
	      ptr[compTable[0]] = (((color & 0x001F) >>  0) << 3);
	    }
	}
    }
}


// --------------------------------------------------------------------------
// ImageTGA::readTGA24bitsRLE
//
// Read 24 bits pixel data from TGA file with RLE compression.
// --------------------------------------------------------------------------

void
ImageTGA::readTGA24bitsRLE (const GLubyte *data)
{
  const GLubyte *pData = data;
  GLubyte *ptr = _pixels;
  int *compTable = GLEW_EXT_bgra ? bgraTable : rgbaTable;

  while (ptr < _pixels + (_width * _height) * 3)
    {
      // Read first byte
      GLubyte packet_header = *(pData++);
      int size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
	{
	  // Run-length packet
	  for (int i = 0; i < size; ++i, ptr += 3)
	    {
	      ptr[0] = pData[compTable[0]];
	      ptr[1] = pData[compTable[1]];
	      ptr[2] = pData[compTable[2]];
	    }

	  pData += 3;
	}
      else
	{
	  // Non run-length packet
	  for (int i = 0; i < size; ++i, ptr += 3)
	    {
	      ptr[0] = pData[compTable[0]];
	      ptr[1] = pData[compTable[1]];
	      ptr[2] = pData[compTable[2]];
	      pData += 3;
	    }
	}
    }
}


// --------------------------------------------------------------------------
// ImageTGA::readTGA32bitsRLE
//
// Read 32 bits pixel data from TGA file with RLE compression.
// --------------------------------------------------------------------------

void
ImageTGA::readTGA32bitsRLE (const GLubyte *data)
{
  const GLubyte *pData = data;
  GLubyte *ptr = _pixels;
  int *compTable = GLEW_EXT_bgra ? bgraTable : rgbaTable;

  while (ptr < _pixels + (_width * _height) * 4)
    {
      // Read first byte
      GLubyte packet_header = *(pData++);
      int size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
	{
	  // Run-length packet */
	  for (int i = 0; i < size; ++i, ptr += 4)
	    {
	      ptr[0] = pData[compTable[0]];
	      ptr[1] = pData[compTable[1]];
	      ptr[2] = pData[compTable[2]];
	      ptr[3] = pData[compTable[3]];
	    }

	  pData += 4;
	}
      else
	{
	  // Non run-length packet
	  for (int i = 0; i < size; ++i, ptr += 4)
	    {
	      ptr[0] = pData[compTable[0]];
	      ptr[1] = pData[compTable[1]];
	      ptr[2] = pData[compTable[2]];
	      ptr[3] = pData[compTable[3]];
	      pData += 4;
	    }
	}
    }
}


// --------------------------------------------------------------------------
// ImageTGA::readTGAgray8bitsRLE
//
// Read grey 8 bits pixel data from TGA file with RLE compression.
// --------------------------------------------------------------------------

void
ImageTGA::readTGAgray8bitsRLE (const GLubyte *data)
{
  const GLubyte *pData = data;
  GLubyte *ptr = _pixels;

  while (ptr < _pixels + (_width * _height))
    {
      // Read first byte
      GLubyte packet_header = *(pData++);
      int size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
	{
	  // Run-length packet
	  GLubyte color = *(pData++);

	  memset (ptr, color, size);
	  ptr += size;
	}
      else
	{
	  // Non run-length packet
	  memcpy (ptr, pData, size);
	  ptr += size;
	  pData += size;
	}
    }
}


// --------------------------------------------------------------------------
// ImageTGA::readTGAgray16bitsRLE
//
// Read grey 16 bits pixel data from TGA file with RLE compression.
// --------------------------------------------------------------------------

void
ImageTGA::readTGAgray16bitsRLE (const GLubyte *data)
{
  const GLubyte *pData = data;
  GLubyte *ptr = _pixels;

  while (ptr < _pixels + (_width * _height) * 2)
    {
      // Read first byte
      GLubyte packet_header = *(pData++);
      int size = 1 + (packet_header & 0x7f);

      if (packet_header & 0x80)
	{
	  // Run-length packet
	  GLubyte color = *(pData++);
	  GLubyte alpha = *(pData++);

	  for (int i = 0; i < size; ++i, ptr += 2)
	    {
	      ptr[0] = color;
	      ptr[1] = alpha;
	    }
	}
      else
	{
	  // Non run-length packet
	  memcpy (ptr, pData, size * 2);
	  ptr += size * 2;
	  pData += size * 2;
	}
    }
}


/////////////////////////////////////////////////////////////////////////////
//
// class ImagePCX implementation.
//
/////////////////////////////////////////////////////////////////////////////

// Pixel's component table access
int ImagePCX::rgbTable[3] = { 0, 1, 2 };
int ImagePCX::bgrTable[3] = { 2, 1, 0 };


// --------------------------------------------------------------------------
// ImagePCX::ImagePCX
//
// Constructor.  Read a PCX image from memory.
// --------------------------------------------------------------------------

ImagePCX::ImagePCX (const ImageBuffer &ibuff)
  : _header (NULL)
{
  const GLubyte *data_ptr;

  try
    {
      _name = ibuff.filename ();
      _standardCoordSystem = true;

      // Get pointer on file data
      data_ptr = ibuff.data ();

      // Read PCX header
      _header = reinterpret_cast<const PCX_Header *>(data_ptr);
      data_ptr += sizeof (PCX_Header);

      // Check if is valid PCX file
      if (_header->manufacturer != 0x0a)
	throw ImageException ("Bad version number", _name);

      // Initialize image variables
      _width = _header->xmax - _header->xmin + 1;
      _height = _header->ymax - _header->ymin + 1;
      _format = GLEW_EXT_bgra ? GL_BGR : GL_RGB;
      _components = 3;
      _pixels = new GLubyte[_width * _height * _components];

      int bitcount = _header->bitsPerPixel * _header->numColorPlanes;
      int palette_pos = ibuff.length () - 768;

      // Read image data
      switch (bitcount)
	{
	case 1:
	  // 1 bit color index
	  readPCX1bit (data_ptr);
	  break;

	case 4:
	  // 4 bits color index
	  readPCX4bits (data_ptr);
	  break;

	case 8:
	  // 8 bits color index
	  readPCX8bits (data_ptr, ibuff.data () + palette_pos);
	  break;

	case 24:
	  // 24 bits
	  readPCX24bits (data_ptr);
	  break;

	default:
	  // Unsupported
	  throw ImageException ("Unhandled PCX format (bad bitcount)", _name);
	}
    }
  catch (...)
    {
      delete [] _pixels;
      throw;
    }
}


// --------------------------------------------------------------------------
// ImagePCX::readPCX1bit
//
// Read 1 bit PCX image.
// --------------------------------------------------------------------------

void
ImagePCX::readPCX1bit (const GLubyte *data)
{
  int rle_count = 0, rle_value = 0;
  const GLubyte *pData = data;
  GLubyte *ptr = _pixels;
  int *compTable = GLEW_EXT_bgra ? bgrTable : rgbTable;

  for (int y = 0; y < _height; ++y)
    {
      ptr = &_pixels[(_height - (y + 1)) * _width * 3];
      int bytes = _header->bytesPerScanLine;

      // Decode line number y
      while (bytes--)
	{
	  if (rle_count == 0)
	    {
	      if ( (rle_value = *(pData++)) < 0xc0)
		{
		  rle_count = 1;
		}
	      else
		{
		  rle_count = rle_value - 0xc0;
		  rle_value = *(pData++);
		}
	    }

	  rle_count--;

	  // Fill height pixels chunk
	  for (int i = 7; i >= 0; --i, ptr += 3)
	    {
	      int colorIndex = ((rle_value & (1 << i)) > 0);

	      ptr[0] = _header->palette[colorIndex * 3 + compTable[0]];
	      ptr[1] = _header->palette[colorIndex * 3 + compTable[1]];
	      ptr[2] = _header->palette[colorIndex * 3 + compTable[2]];
	    }
	}
    }
}


// --------------------------------------------------------------------------
// ImagePCX::readPCX4bits
//
// Read 4 bits PCX image.
// --------------------------------------------------------------------------

void
ImagePCX::readPCX4bits (const GLubyte *data)
{
  const GLubyte *pData = data;
  GLubyte *colorIndex = NULL;
  GLubyte *line = NULL;
  GLubyte *ptr;
  int rle_count = 0, rle_value = 0;
  int *compTable = GLEW_EXT_bgra ? bgrTable : rgbTable;

  try
    {
      // Memory allocation for temporary buffers
      colorIndex = new GLubyte[_width];
      line = new GLubyte[_header->bytesPerScanLine];
    }
  catch (std::bad_alloc &err)
    {
      delete [] colorIndex;
      delete [] line;
      throw;
    }

  for (int y = 0; y < _height; ++y)
    {
      ptr = &_pixels[(_height - (y + 1)) * _width * 3];
      memset (colorIndex, 0, _width);

      for (int c = 0; c < 4; ++c)
	{
	  GLubyte *pLine = line;
	  int bytes = _header->bytesPerScanLine;

	  // Decode line number y
	  while (bytes--)
	    {
	      if (rle_count == 0)
		{
		  if ( (rle_value = *(pData++)) < 0xc0)
		    {
		      rle_count = 1;
		    }
		  else
		    {
		      rle_count = rle_value - 0xc0;
		      rle_value = *(pData++);
		    }
		}

	      rle_count--;
	      *(pLine++) = rle_value;
	    }

	  // Compute line's color indexes
	  for (int x = 0; x < _width; ++x)
	    {
	      if (line[x / 8] & (128 >> (x % 8)))
		colorIndex[x] += (1 << c);
	    }
	}

      // Decode scanline.  color index => rgb
      for (int x = 0; x < _width; ++x, ptr += 3)
	{
	  ptr[0] = _header->palette[colorIndex[x] * 3 + compTable[0]];
	  ptr[1] = _header->palette[colorIndex[x] * 3 + compTable[1]];
	  ptr[2] = _header->palette[colorIndex[x] * 3 + compTable[2]];
	}
    }

  // Release memory
  delete [] colorIndex;
  delete [] line;
}


// --------------------------------------------------------------------------
// ImagePCX::readPCX8bits
//
// Read 8 bits PCX image.
// --------------------------------------------------------------------------

void
ImagePCX::readPCX8bits (const GLubyte *data, const GLubyte *palette)
{
  const GLubyte *pData = data;
  int rle_count = 0, rle_value = 0;
  GLubyte *ptr;
  int *compTable = GLEW_EXT_bgra ? bgrTable : rgbTable;

  // Palette should be preceded by a value of 0x0c (12)...
  GLubyte magic = palette[-1];
  if (magic != 0x0c)
    {
      // ... but sometimes it is not
      std::cerr << "Warning: PCX palette should start with "
		<< "a value of 0x0c (12)!" << std::endl;
    }

  // Read pixel data
  for (int y = 0; y < _height; ++y)
    {
      ptr = &_pixels[(_height - (y + 1)) * _width * 3];
      int bytes = _header->bytesPerScanLine;

      // Decode line number y
      while (bytes--)
	{
	  if (rle_count == 0)
	    {
	      if( (rle_value = *(pData++)) < 0xc0)
		{
		  rle_count = 1;
		}
	      else
		{
		  rle_count = rle_value - 0xc0;
		  rle_value = *(pData++);
		}
	    }

	  rle_count--;

	  ptr[0] = palette[rle_value * 3 + compTable[0]];
	  ptr[1] = palette[rle_value * 3 + compTable[1]];
	  ptr[2] = palette[rle_value * 3 + compTable[2]];
	  ptr += 3;
	}
    }
}


// --------------------------------------------------------------------------
// ImagePCX::readPCX24bits
//
// Read 24 bits PCX image.
// --------------------------------------------------------------------------

void
ImagePCX::readPCX24bits (const GLubyte *data)
{
  const GLubyte *pData = data;
  GLubyte *ptr;
  int rle_count = 0, rle_value = 0;
  int *compTable = GLEW_EXT_bgra ? bgrTable : rgbTable;

  for (int y = 0; y < _height; ++y)
    {
      // For each color plane
      for (int c = 0; c < 3; ++c)
	{
	  ptr = &_pixels[(_height - (y + 1)) * _width * 3];
	  int bytes = _header->bytesPerScanLine;

	  // Decode line number y
	  while (bytes--)
	    {
	      if (rle_count == 0)
		{
		  if( (rle_value = *(pData++)) < 0xc0)
		    {
		      rle_count = 1;
		    }
		  else
		    {
		      rle_count = rle_value - 0xc0;
		      rle_value = *(pData++);
		    }
		}

	      rle_count--;
	      ptr[compTable[c]] = static_cast<GLubyte>(rle_value);
	      ptr += 3;
	    }
	}
    }
}

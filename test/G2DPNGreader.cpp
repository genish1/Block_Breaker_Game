#include <string>
#include <memory>
#include <fstream>
#include <iostream>
#include <vector>

using namespace std;
#include "G2D.h"
#include "lodepng.h"

using byte = unsigned char;


// create texture from RGBA data (G2D.cpp)
int  CORECreateTextureFromRGBA(unsigned char* data, int width, int height);  
// convert RGB data to RGBA data
void ExtendRGBtoRGBA(vector<byte>& data);	
// Vertical-symetry
void VsymetryRGBAImage(vector<byte>& data, int Width, int Height);
// any pixel with given RGB color value becomes transparent
void SetRGBATransparentPixels(vector<byte>& data, byte RA, byte GA, byte BA);
// call lodepng
void LoadPNG(const std::string& filepath, std::vector<byte> & image, unsigned int& width, unsigned int& height);

int G2D::ExtractTextureFromPNG(const std::string& filepath, Transparency T)
{
	std::vector<byte> image; //the raw pixels
	unsigned int width, height;

	LoadPNG(filepath, image, width, height);

	if ( T != Transparency::None )
	{ 
		int index = 0; // Transparency::BottomLeft
		if (T == Transparency::BottomRight)  index = 4 * (width-1);
		if (T == Transparency::UpperLeft)    index = 4 * width * (height - 1);
		if (T == Transparency::UpperRight)   index = 4 * width * height - 4;
		 
		SetRGBATransparentPixels(image, image[index], image[index+1], image[index+2]);
	}

	int id = CORECreateTextureFromRGBA(image.data(), width, height);
	return id;
}

int G2D::ExtractTextureFromPNG(const std::string& filepath, int RT, int GT, int BT)
{
	std::vector<byte> image; //the raw pixels
	unsigned int width, height;

	LoadPNG(filepath, image, width, height);

	SetRGBATransparentPixels(image, RT,GT,BT);

	int id = CORECreateTextureFromRGBA(image.data(), width, height);
	return id;
}



// 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...
void LoadPNG(const std::string& filepath, std::vector<byte> & image, unsigned int & width, unsigned int & height)
{
	//decode
	unsigned error = lodepng::decode(image, width, height, filepath.c_str());

	//if there's an error, display it
	if (error)
	{
		std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
		cout.flush();
	}

	VsymetryRGBAImage(image, width, height);
	
}



 

 
// convert RGB data to RGBA data
void ExtendRGBtoRGBA(vector<byte> & data)
{
	int nbPixels = data.size();
	data.resize(nbPixels * 4); // RGBA

	// shift RGB color to create RGBA colors 
	// starting from the end of the array

	int pRGBA = nbPixels * 4 - 4;
	int pRGB = nbPixels * 3 - 3;

	for (int i = 0; i < nbPixels; i++)
	{
		data[pRGBA + 3] = 255;
		data[pRGBA + 2] = data[pRGB + 2];
		data[pRGBA + 1] = data[pRGB + 1];
		data[pRGBA + 0] = data[pRGB + 0];
		pRGB -= 3;
		pRGBA -= 4;
	}
}

 
// Vertical-symetry
void VsymetryRGBAImage(vector<byte> & data,int Width, int Height)
{
	for (int y = 0; y < Width / 2; y++)
	{
		int width = Width * 4;
		int pY = y * width;
		int sY = (Height - y - 1) * width;

		for (int x = 0; x < width; x++)
		{
			unsigned char t = data[pY + x];
			data[pY + x] = data[sY + x];
			data[sY + x] = t;
		}
	}
}

// any pixel with given RGB color value becomes transparent
void SetRGBATransparentPixels(vector<byte> & data, byte RA, byte GA, byte BA )
{
  
	int nbPixels = data.size() / 4;
	
	int p = 0;
	for (int i = 0; i < nbPixels; i++)
	{
		if (data[p] == RA && data[p + 1] == GA && data[p + 2] == BA)
			data[p + 3] = 0;
		p += 4;
	}
}
 
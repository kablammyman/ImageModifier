#include "SDL.h"
#undef main
//#include "SDL_ttf.h"
#include "SDL_syswm.h"
#include <SDL_image.h>

#include <stdio.h>
#include <string>

#include "stb_image.h"
#include "stb_image_write.h"

#include "BitmapFont.h"

#include <string>
#include <time.h>

using namespace std;

void parseArgs(int argc, const char * argv[], string &inputFile, string &outputDir,string &text)
{
	for (int i = 0; i < argc; i++)
	{
		string curArg = argv[i];

		if (curArg == "-i" || curArg == "--input")
		{
			if (i < argc - 1)
			{
				inputFile = argv[i + 1];
				i++;

				size_t found = inputFile.find_last_of("/\\");
				if (found == string::npos)
				{
					string mainPath = argv[0];
					inputFile = mainPath.substr(0, mainPath.find_last_of("/\\") + 1) + inputFile;
				}
			}
		}
		else if (curArg == "-o" || curArg == "--output")
		{
			if (i < argc - 1)
			{
				outputDir = argv[i + 1];
				i++;

				size_t found = outputDir.find_last_of("/\\");
				bool inSubdir = (outputDir[0] == '.' && outputDir.find_first_of("/\\") == 1);

				if (found == string::npos || inSubdir)
				{
					if (inSubdir)
					{
						outputDir.erase(0, 2);
					}
					string mainPath = argv[0];
					outputDir = mainPath.substr(0, mainPath.find_last_of("/\\") + 1) + outputDir;
				}
			}
		}
		else if (curArg == "-t" || curArg == "--text")
		{
			if (i < argc - 1)
			{
				text = argv[i + 1];
				i++;
			}
			
		}
	}//end for loop
}
unsigned int GetRandomNum(int min, int max)
{
	unsigned int diff = ((max - min) + 1);
	return ((diff * rand()) / RAND_MAX) + min;
}
/*SDL_Surface* SDL_ScaleSurface(SDL_Surface *Surface, Uint16 Width, Uint16 Height)
{
	for (Sint32 y = 0; y < Surface->h; y++) //Run across all Y pixels.
		for (Sint32 x = 0; x < Surface->w; x++) //Run across all X pixels.
			for (Sint32 o_y = 0; o_y < _stretch_factor_y; ++o_y) //Draw _stretch_factor_y pixels for each Y pixel.
				for (Sint32 o_x = 0; o_x < _stretch_factor_x; ++o_x) //Draw _stretch_factor_x pixels for each X pixel.
					DrawPixel(_ret, static_cast<Sint32>(_stretch_factor_x * x) + o_x,static_cast<Sint32>(_stretch_factor_y * y) + o_y, ReadPixel(Surface, x, y));
}*/
int main(int argc, const char * argv[])
{
	string inputFile, outputDir;
	string outputFileNameOnly;
	string destFile;
	string fileExt;
	string waterMarkText;
	PIXMAP *img2 = new PIXMAP(10, 10);
	PIXMAP *photo;
	bool showImageWindow = false;

	int iBitDepth = 8, iWidth = 256, iHeight = 128;
	parseArgs(argc, argv, inputFile, outputDir, waterMarkText);

	if (inputFile.empty())
	{
		printf("specify a .png or .jpg image to modify. Use the param -i or --input\n");
		printf("you can optionally specify the fully qualified output path or path+filename using -o or --output\n");
		return -1;
	}

	std::size_t found = inputFile.find_last_of(".");
	fileExt = inputFile.substr(found + 1);

	if (outputDir.empty())
	{
		outputDir = inputFile.substr(0, inputFile.find_last_of("/\\") + 1);
	}

	if (outputDir.back() != '\\' && outputDir.back() != '/' && outputDir.find_last_of(".") == string::npos)
	{
#if _WIN32
		outputDir += "\\";
#else
		outputDir += "/";
#endif
	}

	if (outputDir.find_last_of(".") == string::npos)
		outputFileNameOnly = inputFile.substr(inputFile.find_last_of("/\\") + 1);
	else
	{
		outputFileNameOnly = outputDir.substr(outputDir.find_last_of("/\\") + 1);
		outputDir = outputDir.substr(0, outputDir.find_last_of("/\\") + 1);
	}
	outputFileNameOnly = outputFileNameOnly.substr(0, outputFileNameOnly.find_last_of("."));//remove file ext


	destFile = outputDir + "modified_" + outputFileNameOnly + ".png";


	string inputFileExt = inputFile.substr(inputFile.find_last_of(".") + 1);//remove file ext 
	
	int bw;
	int bh;
	int bpp;
	unsigned char* imgData = stbi_load(inputFile.c_str(), &bw, &bh, &bpp, 4);
	if (!imgData)
	{
		printf("couldnt load image: %s\n", inputFile.c_str());
		return -1;
	}
	photo = new PIXMAP(imgData, bw, bh);
	//once the data ahs been copied into our class, get rid of raw data
	stbi_image_free(imgData);

	if (!photo)//sometimes the bmp fails to load, until i figure out why, ill just do this
	{
		photo = nullptr;
		printf("couldnt convert image into PIXMAP: %s\n", inputFile.c_str());
		return -1;
	}

	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window * window;
	SDL_Renderer * renderer;
	SDL_Texture * texture;
	if (showImageWindow)
	{
		window = SDL_CreateWindow("SDL2 Pixel Drawing", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, bw, bh, 0);
		renderer = SDL_CreateRenderer(window, -1, 0);
		texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ABGR8888, SDL_TEXTUREACCESS_STREAMING, bw, bh);
	}


	//add watermark text in random spot
	//add random "noise" or change color of a few random pixels as water mark"
	//change size of image
	//add border?
	srand(time(0));
	if (!waterMarkText.empty())
	{
		StockBitmapFont testFont;
		int xPos = GetRandomNum(0, bw - 1);
		int yPos = GetRandomNum(0, bh - 1);
		testFont.Draw(photo, waterMarkText, xPos, yPos, true);
	}
	

	for (int i = 0; i < 100; i++)
	{
		int pix = GetRandomNum(0,(bw*bh)-1);
		photo->pixels[pix] = RGBA{255,0,255,255};
	}
	unsigned char *temp = (unsigned char *)photo->pixels;
	//4th param = comp, which is 1=Y, 2=YA, 3=RGB, 4=RGBA.
	stbi_write_png(destFile.c_str(), bw, bh, 4, temp, bw * sizeof(Uint32));
	
	if (showImageWindow)
	{
		bool quit = false;
		SDL_Event event;
		bool leftMouseButtonDown = false;

		while (!quit)
		{
			SDL_UpdateTexture(texture, NULL, photo->pixels, bw * sizeof(Uint32));
			SDL_WaitEvent(&event);

			switch (event.type)
			{
			case SDL_KEYDOWN:
				if (event.key.keysym.sym == SDLK_ESCAPE)
					quit = true;
				break;

			case SDL_MOUSEBUTTONUP:
				if (event.button.button == SDL_BUTTON_LEFT)
					leftMouseButtonDown = false;
				break;
			case SDL_MOUSEBUTTONDOWN:
				if (event.button.button == SDL_BUTTON_LEFT)
					leftMouseButtonDown = true;
			case SDL_MOUSEMOTION:
				if (leftMouseButtonDown)
				{
					int mouseX = event.motion.x;
					int mouseY = event.motion.y;
					//rawPixels[mouseY * 640 + mouseX] = 0;
				}
				break;
			case SDL_QUIT:
				quit = true;
				break;
			}

			SDL_RenderClear(renderer);
			SDL_RenderCopy(renderer, texture, NULL, NULL);
			SDL_RenderPresent(renderer);
		}

		SDL_DestroyTexture(texture);
		SDL_DestroyRenderer(renderer);
		SDL_DestroyWindow(window);
	}

	
	SDL_Quit();

	return 0;
}

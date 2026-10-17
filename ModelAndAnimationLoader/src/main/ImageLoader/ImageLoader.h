#pragma once

#include <string>
#include <vector>

#include <assimp/scene.h>

struct RawImage {
	std::string mFilename;
	int mWidth;
	int mHeight;
	int mChannels;
	std::vector<unsigned char> mPixels;
};

class ImageLoader {
public:
	static void loadImage(const aiTexture* texture, RawImage* image);
};
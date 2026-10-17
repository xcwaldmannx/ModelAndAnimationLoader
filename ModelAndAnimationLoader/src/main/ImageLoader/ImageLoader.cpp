#include "ImageLoader.h"

#include <stdexcept>

#ifndef STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#endif

void ImageLoader::loadImage(const aiTexture* texture, RawImage* image)
{
    if (!texture || !image)
    {
        throw std::runtime_error("Invalid texture or image pointer");
    }

    image->mFilename = texture->mFilename.C_Str();

    int width = 0;
    int height = 0;
    int channels = 4;

    image->mPixels.clear();

    if (texture->mHeight == 0)
    {
        if (texture->mWidth > static_cast<unsigned int>(INT_MAX))
        {
            throw std::runtime_error("Embedded texture too large");
        }

        stbi_uc* pixels = stbi_load_from_memory(
            reinterpret_cast<const stbi_uc*>(texture->pcData),
            static_cast<int>(texture->mWidth),
            &width,
            &height,
            &channels,
            STBI_rgb_alpha);

        if (!pixels)
        {
            throw std::runtime_error(std::string("Failed to decode embedded texture: ") + stbi_failure_reason());
        }

        const size_t size =
            static_cast<size_t>(width) * height * 4;

        image->mPixels.assign(pixels, pixels + size);

        stbi_image_free(pixels);
    }
    else
    {
        width = static_cast<int>(texture->mWidth);
        height = static_cast<int>(texture->mHeight);

        const size_t count = static_cast<size_t>(width) * height;

        image->mPixels.resize(count * 4);

        for (size_t i = 0; i < count; ++i)
        {
            const aiTexel& pixel = texture->pcData[i];

            image->mPixels[i * 4 + 0] = pixel.r;
            image->mPixels[i * 4 + 1] = pixel.g;
            image->mPixels[i * 4 + 2] = pixel.b;
            image->mPixels[i * 4 + 3] = pixel.a;
        }
    }

    image->mWidth = width;
    image->mHeight = height;
    image->mChannels = 4;
}


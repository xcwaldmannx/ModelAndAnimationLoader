#include "ImageLoader.h"

#include <stdexcept>

#include <memory>

#define STB_IMAGE_STATIC
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

void ImageLoader::loadImage(
    const aiTexture* texture,
    RawImage* image)
{
    if (!texture || !image) throw std::runtime_error("Invalid texture or image pointer");

    if (!texture->pcData) throw std::runtime_error("Texture contains no pixel data");

    image->mFilename = texture->mFilename.C_Str();

    int width = 0;
    int height = 0;
    int channels = 0;

    std::vector<unsigned char> pixels;

    if (texture->mHeight == 0)
    {
        if (texture->mWidth > static_cast<unsigned int>(std::numeric_limits<int>::max()))
        {
            throw std::runtime_error("Compressed texture exceeds stb_image limits");
        }

        stbi_uc* decoded = stbi_load_from_memory(
            reinterpret_cast<const stbi_uc*>(texture->pcData),
            static_cast<int>(texture->mWidth),
            &width,
            &height,
            &channels,
            STBI_rgb_alpha);

        if (!decoded)
        {
            const char* reason = stbi_failure_reason();

            throw std::runtime_error(std::string("Failed to decode texture: ") + (reason ? reason : "unknown error"));
        }

        std::unique_ptr<stbi_uc, decltype(&stbi_image_free)> decodedOwner(decoded, stbi_image_free);

        if (width <= 0 || height <= 0) throw std::runtime_error("Invalid decoded image dimensions");

        const size_t w = static_cast<size_t>(width);
        const size_t h = static_cast<size_t>(height);

        if (w > std::numeric_limits<size_t>::max() / 4 || h > std::numeric_limits<size_t>::max() / (w * 4))
        {
            throw std::runtime_error("Image size overflow");
        }

        const size_t size = w * h * 4;

        pixels.resize(size);
        std::memcpy(pixels.data(), decoded, size);
    }
    else
    {
        if (texture->mWidth  > static_cast<unsigned int>(std::numeric_limits<int>::max()) ||
            texture->mHeight > static_cast<unsigned int>(std::numeric_limits<int>::max()))
        {
            throw std::runtime_error("Uncompressed texture dimensions too large");
        }

        width = static_cast<int>(texture->mWidth);
        height = static_cast<int>(texture->mHeight);

        if (width <= 0 || height <= 0) throw std::runtime_error("Invalid texture dimensions");

        const size_t w = static_cast<size_t>(width);
        const size_t h = static_cast<size_t>(height);

        if (w > std::numeric_limits<size_t>::max() / 4 ||
            h > std::numeric_limits<size_t>::max() / (w * 4))
        {
            throw std::runtime_error("Image size overflow");
        }

        const size_t count = w * h;

        pixels.resize(count * 4);

        for (size_t i = 0; i < count; ++i)
        {
            const aiTexel& texel = texture->pcData[i];

            pixels[i * 4 + 0] = texel.r;
            pixels[i * 4 + 1] = texel.g;
            pixels[i * 4 + 2] = texel.b;
            pixels[i * 4 + 3] = texel.a;
        }
    }

    image->mPixels = std::move(pixels);
    image->mWidth = width;
    image->mHeight = height;
    image->mChannels = 4;
}

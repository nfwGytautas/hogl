#include "hogl_loader.hpp"

#include <iostream>
#include <fstream>
#include <sstream>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

HOGL_NSPACE_BEGIN

hogl_cs_loader::hogl_cs_loader()
{
    stbi_set_flip_vertically_on_load(true);
}

std::string hogl_cs_loader::read_text_file(const char* file)
{
    std::ifstream iFile;
    std::stringstream fstream;

    iFile.open(file);

    if (!iFile)
    {
        HOGL_LOG_ERROR("Couldn't open the file " << file);
        return "";
    }

    fstream << iFile.rdbuf();
    iFile.close();
    
    return fstream.str();
}

void hogl_cs_loader::load_image(hogl_loader_image* image, const char* file)
{
    int components;
    unsigned char* data = stbi_load(file, &image->width, &image->height, &components, 0);

    if (components == 1)
    {
        image->format = hogl_texture_format::RED;
    }
    else if (components == 3)
    {
        image->format = hogl_texture_format::RGB;
    }
    else if (components == 4)
    {
        image->format = hogl_texture_format::RGBA;
    }
    
    if (data)
    {
        image->data = std::shared_ptr<unsigned char>(data);
    }
    else
    {
        HOGL_LOG_ERROR("Failed to load image " << file);
        stbi_image_free(data);
    }
}

HOGL_NSPACE_END

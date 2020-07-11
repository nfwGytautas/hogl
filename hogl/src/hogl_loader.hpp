#pragma once

#include <string>
#include <memory>

#include "hogl_types.hpp"

HOGL_NSPACE_BEGIN

/**
 * @brief hogl external resource loader
*/
class HOGL_API hogl_cs_loader
{
public:
	hogl_cs_loader();

	/**
	 * @brief Reads file contents and returns it as a std::string
	 * @param file Path to the file
	 * @return std::string with file contents
	*/
	std::string read_text_file(const char* file);

	/**
	 * @brief Load image from file into a struct
	 * @param image hogl_loader_image instance
	 * @param file Path to the file
	*/
	void load_image(hogl_loader_image* image, const char* file);
};

/**
 * @brief This struct describes a image loaded into memory using hogl_cs_loader
*/
struct hogl_loader_image
{
	/**
	 * @brief Data buffer
	*/
	std::shared_ptr<unsigned char> data;

	/**
	 * @brief Width of the image
	*/
	int width = 0;

	/**
	 * @brief Height of the image
	*/
	int height = 0;

	/**
	 * @brief Image format
	*/
	hogl_texture_format format;
};

HOGL_NSPACE_END

#include "hogl_vf.h"

#include <stdio.h>
#include <string.h>

/**
 * @brief VF format is as follows:
 * 4 Bytes for endian check
*/

#define ENDIAN_CHECK_VAL 0x01234567

// Version
// Item count
// Buffer size
#define VF_HEADER_LEN sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint64_t)

typedef struct _hogl_vfi {
	uint32_t* type;
	uint32_t* name_length;
	char* name;
	uint64_t* data_length;
	char* data;
} hogl_vfi;

typedef struct _hogl_vf {
	// Header
	uint32_t version;
	uint64_t item_count;
	uint64_t buffer_size;

	// Data
	hogl_vfi* items;
	char* buffer;
} hogl_vf;

uint32_t get_endian(uint32_t val) {
	volatile uint32_t i = val;
	return (*((uint8_t*)(&i))) == 0x67; // 1 for big endian, 0 for little endian
}

hogl_error hogl_vf_read(hogl_vf** vf, const char* path) {
	FILE* fp = NULL;
	uint32_t echeck = 0;
	size_t rsize = 0;
	char header_buffer[VF_HEADER_LEN];
	char* header_bp = &header_buffer[0];
	
	hogl_log_trace("Reading virtual file %s", path);
	
	fp = fopen(path, "rb");

	if (fp == NULL) {
		hogl_log_error("Failed to open %s", path);
		perror("Cause: ");
		return HOGL_ERROR_BAD_PATH;
	}

	// Endian check
	rsize = fread(&echeck, sizeof(uint32_t), 1, fp);

	if (rsize != 1) {
		hogl_log_error("Failed to read endian information from %s");
		return HOGL_ERROR_BAD_READ;
	}
	echeck = get_endian(echeck);

	*vf = (hogl_vf*)hogl_malloc((sizeof(hogl_vf)));

	if (get_endian(ENDIAN_CHECK_VAL) != echeck) {
		hogl_log_error("Virtual file %s and machine endianness don't match");
		return HOGL_ERROR_ENDIAN_MISMATCH;
	}

	// Header
	rsize = fread(header_buffer, VF_HEADER_LEN, 1, fp);

	if (rsize != 1) {
		hogl_log_error("Failed to read header information from %s");
		hogl_free(*vf);
		return HOGL_ERROR_BAD_READ;
	}

	(*vf)->version = *(uint32_t*)header_bp;
	(*vf)->item_count = *(uint64_t*)(header_bp + sizeof(uint32_t));
	(*vf)->buffer_size = *(uint64_t*)(header_bp + sizeof(uint32_t) + sizeof(uint64_t));

	// Allocate buffers
	(*vf)->items = hogl_malloc((*vf)->item_count * sizeof(hogl_vfi));
	(*vf)->buffer = hogl_malloc((*vf)->buffer_size);

	// Read data
	rsize = fread((*vf)->buffer, (*vf)->buffer_size, 1, fp);

	if (rsize != 1) {
		hogl_log_error("Failed to read data information from %s");
		hogl_vf_free((*vf));
		return HOGL_ERROR_BAD_READ;
	}

	// Parse data information
	uint64_t offset = 0;
	for (uint64_t i = 0; i < (*vf)->item_count; i++) {
		hogl_vfi* ivfi = &(*vf)->items[i];

		ivfi->type = (uint32_t*)((*vf)->buffer + offset);
		offset += sizeof(uint32_t);

		ivfi->name_length = (uint32_t*)((*vf)->buffer + offset);
		offset += sizeof(uint32_t);

		ivfi->name = (*vf)->buffer + offset;
		offset += *ivfi->name_length;

		ivfi->data_length = (uint64_t*)((*vf)->buffer + offset);
		offset += sizeof(uint64_t);

		ivfi->data = (*vf)->buffer + offset;
		offset += *ivfi->data_length;
	}

	// Close file
	fclose(fp);

	return HOGL_ERROR_NONE;
}

hogl_error hogl_vf_save(hogl_vf* vf, const char* path)
{
	FILE* fp = NULL;
	uint32_t echeck = ENDIAN_CHECK_VAL;
	size_t rsize = 0;
	char header_buffer[VF_HEADER_LEN];
	char* header_bp = &header_buffer[0];

	hogl_log_trace("Saving virtual file to %s", path);

	fp = fopen(path, "wb");

	if (fp == NULL) {
		hogl_log_error("Failed to open %s", path);
		perror("Cause: ");
		return HOGL_ERROR_BAD_PATH;
	}

	// Endian check
	rsize = fwrite(&echeck, sizeof(uint32_t), 1, fp);

	if (rsize != 1) {
		hogl_log_error("Failed to write endian information to %s");
		return HOGL_ERROR_BAD_WRITE;
	}

	// Header
	*(uint32_t*)(header_bp) = vf->version;
	*(uint64_t*)(header_bp + sizeof(uint32_t)) = vf->item_count;
	*(uint64_t*)(header_bp + sizeof(uint32_t) + sizeof(uint64_t)) = vf->buffer_size;

	rsize = fwrite(header_buffer, VF_HEADER_LEN, 1, fp);

	if (rsize != 1) {
		hogl_log_error("Failed to write header information to %s");
		return HOGL_ERROR_BAD_WRITE;
	}

	// Write data
	rsize = fwrite(vf->buffer, vf->buffer_size, 1, fp);

	if (rsize != 1) {
		hogl_log_error("Failed to write data information to %s");
		return HOGL_ERROR_BAD_WRITE;
	}

	// Close file
	fclose(fp);

	return HOGL_ERROR_NONE;
}

hogl_error hogl_vf_get_item_index(hogl_vf* vf, const char* name, size_t* index)
{
	for (*index = 0; *index < vf->item_count; (*index)++) {
		// Skip if non matching lengths
		if (strlen(name) != *vf->items[*index].name_length) {
			continue;
		}

		if (strncmp(name, vf->items[*index].name, *vf->items[*index].name_length) == 0) {
			return HOGL_ERROR_NONE;
		}
	}

	return HOGL_ERROR_VF_BAD_NAME;
}

uint32_t hogl_vf_version(hogl_vf* vf)
{
	return vf->version;
}

void hogl_vf_set_version(hogl_vf* vf, uint32_t version)
{
	if (version < vf->version) {
		hogl_log_warn("Setting new virtual file version to lower than the existing one");
	}

	vf->version = version;
}

hogl_error hogl_vf_map_item(hogl_vf* vf, size_t index, void** target)
{
	if (vf->item_count <= index) {
		hogl_log_warn("Tried to access invalid virtual file item");
		return HOGL_ERROR_OUT_OF_RANGE;
	}

	(*target) = vf->items[index].data;

	return HOGL_ERROR_NONE;
}

hogl_error hogl_vf_item_size(hogl_vf* vf, size_t index, uint64_t* target)
{
	if (vf->item_count <= index) {
		hogl_log_warn("Tried to access invalid virtual file item");
		return HOGL_ERROR_OUT_OF_RANGE;
	}

	(*target) = *vf->items[index].data_length;

	return HOGL_ERROR_NONE;
}

hogl_error hogl_vf_item_type(hogl_vf* vf, size_t index, uint32_t* target)
{
	if (vf->item_count <= index) {
		hogl_log_warn("Tried to access invalid virtual file item");
		return HOGL_ERROR_OUT_OF_RANGE;
	}

	(*target) = *vf->items[index].type;

	return HOGL_ERROR_NONE;
}

void hogl_vf_free(hogl_vf* vf) {
	hogl_free(vf->buffer);
	hogl_free(vf->items);
	hogl_free(vf);
}

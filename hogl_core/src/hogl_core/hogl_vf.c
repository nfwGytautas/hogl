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
#define VF_HEADER_LEN sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint64_t) + sizeof(uint32_t)

typedef struct _hogl_vfi {
	uint32_t* type;
	uint64_t* data_length;

	char* name;
	char* data;
} hogl_vfi;

typedef struct _hogl_vf {
	// Header
	uint32_t version;
	uint64_t item_count;
	uint64_t buffer_size;
	uint32_t max_name_len;

	// Data
	hogl_vfi* items;
	char* name_buffer;
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
	(*vf)->max_name_len = *(uint32_t*)(header_bp + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint64_t));

	// Allocate buffers
	(*vf)->items = NULL;
	(*vf)->name_buffer = hogl_malloc((*vf)->item_count * (*vf)->max_name_len);
	(*vf)->buffer = hogl_malloc((*vf)->buffer_size);

	// Read name
	rsize = fread((*vf)->name_buffer, (*vf)->item_count * (*vf)->max_name_len, 1, fp);

	if (rsize != 1) {
		hogl_log_error("Failed to read name information from %s");
		hogl_vf_free((*vf));
		return HOGL_ERROR_BAD_READ;
	}

	// Read data
	rsize = fread((*vf)->buffer, (*vf)->buffer_size, 1, fp);

	if (rsize != 1) {
		hogl_log_error("Failed to read data information from %s");
		hogl_vf_free((*vf));
		return HOGL_ERROR_BAD_READ;
	}

	// Create mappings
	hogl_vf_map_vfi((*vf));

	// Close file
	fclose(fp);

	return HOGL_ERROR_NONE;
}

hogl_vf* hogl_vf_new(uint32_t version, uint32_t max_name_len) {
	hogl_vf* vf = hogl_malloc(sizeof(hogl_vf));
	vf->buffer = NULL;
	vf->name_buffer = NULL;
	vf->items = NULL;
	vf->item_count = 0;
	vf->buffer_size = 0;
	vf->version = version;
	vf->max_name_len = max_name_len;
	return vf;
}

void hogl_vf_change_name_len(hogl_vf* vf, uint32_t new_name_len) {
	char* new_name_buffer = hogl_malloc(new_name_len * vf->item_count);

	for (uint64_t i = 0; i < vf->item_count; i++) {
		hogl_memcpy(new_name_buffer + i * new_name_len, vf->name_buffer + i * vf->max_name_len, vf->max_name_len);
	}

	hogl_free(vf->name_buffer);
	vf->name_buffer = new_name_buffer;
	vf->max_name_len = new_name_len;

	hogl_vf_map_vfi(vf);
}

hogl_error hogl_vf_add_item(hogl_vf* vf, const char* name, uint32_t type, void* data, uint64_t size) {
	uint64_t elment_stride = (sizeof(uint32_t) + sizeof(uint64_t) + size);
	uint64_t prev_size = vf->buffer_size;
	uint64_t new_size = prev_size + elment_stride;
	char* new_name_buffer = NULL;
	char* new_data_buffer = NULL;

	if (vf->max_name_len < strlen(name)) {
		hogl_log_error("Trying to assign name that doesn't fit inside a virtual file");
		return HOGL_ERROR_BAD_ARGUMENT;
	}

	// Need to expand memory
	new_name_buffer = hogl_realloc(vf->name_buffer, vf->max_name_len * (vf->item_count + 1) * sizeof(char));
	new_data_buffer = hogl_realloc(vf->buffer, new_size);

	if (new_name_buffer == NULL || new_data_buffer == NULL) {
		hogl_log_error("Failed to realloc virtual file buffers");
		return HOGL_ERROR_MEMORY;
	}

	// NULL memory inside the name buffer
	hogl_memset(new_name_buffer + vf->item_count * vf->max_name_len, 0, vf->max_name_len);

	vf->item_count++;
	vf->name_buffer = new_name_buffer;
	vf->buffer = new_data_buffer;
	vf->buffer_size = new_size;

	// Store name and data
	hogl_memcpy(vf->name_buffer + (vf->item_count - 1) * vf->max_name_len, name, strlen(name));
	
	*(uint32_t*)(vf->buffer + prev_size) = type;
	*(uint64_t*)(vf->buffer + prev_size + sizeof(uint32_t)) = size;
	hogl_smemcpy(vf->buffer + prev_size + sizeof(uint32_t) + sizeof(uint64_t), data, size);

	return HOGL_ERROR_NONE;
}

hogl_error hogl_vf_rename_item(hogl_vf* vf, size_t index, const char* new_name)
{
	if (vf->item_count <= index) {
		hogl_log_warn("Tried to access invalid virtual file item");
		return HOGL_ERROR_OUT_OF_RANGE;
	}

	if (vf->max_name_len < strlen(new_name)) {
		hogl_log_error("Name %s cannot be assigned cause length &ld exceeds the max name length %ld of virtual file", new_name, strlen(new_name), vf->max_name_len)
		return HOGL_ERROR_MEMORY;
	}

	if (vf->items == NULL) {
		hogl_log_warn("Ignoring unmapped virtual file item change");
		return HOGL_ERROR_VF_VFI_MAP;
	}

	// Delete the original name
	hogl_memset(vf->items[index].name, 0, vf->max_name_len);

	// Write the new name
	hogl_smemcpy(vf->items[index].name, new_name, vf->max_name_len);

	return HOGL_ERROR_NONE;
}

void hogl_vf_map_vfi(hogl_vf* vf) {
	if (vf->items != NULL) {
		hogl_free(vf->items);
	}

	vf->items = hogl_malloc(sizeof(hogl_vfi) * vf->item_count);

	// Parse data information
	uint64_t offset = 0;
	for (uint64_t i = 0; i < vf->item_count; i++) {
		hogl_vfi* ivfi = &vf->items[i];

		ivfi->type = (uint32_t*)(vf->buffer + offset);
		offset += sizeof(uint32_t);

		ivfi->name = vf->name_buffer + (i * vf->max_name_len);

		ivfi->data_length = (uint64_t*)(vf->buffer + offset);
		offset += sizeof(uint64_t);

		ivfi->data = vf->buffer + offset;
		offset += *ivfi->data_length;
	}
}

hogl_error hogl_vf_save(hogl_vf* vf, const char* path) {
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
	*(uint32_t*)(header_bp + sizeof(uint32_t) + sizeof(uint64_t) + sizeof(uint64_t)) = vf->max_name_len;

	rsize = fwrite(header_buffer, VF_HEADER_LEN, 1, fp);

	if (rsize != 1) {
		hogl_log_error("Failed to write header information to %s");
		return HOGL_ERROR_BAD_WRITE;
	}

	// Write name buffer
	rsize = fwrite(vf->name_buffer, vf->item_count * vf->max_name_len, 1, fp);

	if (rsize != 1) {
		hogl_log_error("Failed to write name information to %s");
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

hogl_error hogl_vf_get_item_index(hogl_vf* vf, const char* name, size_t* index) {
	if (vf->items == NULL) {
		hogl_log_warn("Ignoring unmapped virtual file item change");
		return HOGL_ERROR_VF_VFI_MAP;
	}

	for (*index = 0; *index < vf->item_count; (*index)++) {
		if (strcmp(name, vf->items[*index].name) == 0) {
			return HOGL_ERROR_NONE;
		}
	}

	return HOGL_ERROR_VF_BAD_NAME;
}

uint32_t hogl_vf_version(hogl_vf* vf) {
	return vf->version;
}

void hogl_vf_set_version(hogl_vf* vf, uint32_t version) {
	if (version < vf->version) {
		hogl_log_warn("Setting new virtual file version to lower than the existing one");
	}

	vf->version = version;
}

hogl_error hogl_vf_map_item(hogl_vf* vf, size_t index, void** target) {
	if (vf->item_count <= index) {
		hogl_log_warn("Tried to access invalid virtual file item");
		return HOGL_ERROR_OUT_OF_RANGE;
	}

	if (vf->items == NULL) {
		hogl_log_warn("Ignoring unmapped virtual file item change");
		return HOGL_ERROR_VF_VFI_MAP;
	}

	(*target) = vf->items[index].data;

	return HOGL_ERROR_NONE;
}

hogl_error hogl_vf_item_size(hogl_vf* vf, size_t index, uint64_t* target) {
	if (vf->item_count <= index) {
		hogl_log_warn("Tried to access invalid virtual file item");
		return HOGL_ERROR_OUT_OF_RANGE;
	}

	if (vf->items == NULL) {
		hogl_log_warn("Ignoring unmapped virtual file item change");
		return HOGL_ERROR_VF_VFI_MAP;
	}

	(*target) = *vf->items[index].data_length;

	return HOGL_ERROR_NONE;
}

hogl_error hogl_vf_item_type(hogl_vf* vf, size_t index, uint32_t* target) {
	if (vf->item_count <= index) {
		hogl_log_warn("Tried to access invalid virtual file item");
		return HOGL_ERROR_OUT_OF_RANGE;
	}

	if (vf->items == NULL) {
		hogl_log_warn("Ignoring unmapped virtual file item change");
		return HOGL_ERROR_VF_VFI_MAP;
	}

	(*target) = *vf->items[index].type;

	return HOGL_ERROR_NONE;
}

void hogl_vf_free(hogl_vf* vf) {
	hogl_free(vf->buffer);
	hogl_free(vf->items);
	hogl_free(vf->name_buffer);
	hogl_free(vf);
}

/**
* @brief hogl virtual file header contains functionality for hogl vf API
*/

#ifndef _HOGL_VF_
#define _HOGL_VF_

#include <stdint.h>
#include "hogl_core/shared/hogl_def.h"

/**
 * @brief Virtual file object containing the data use hogl_vf_ and hogl_vfe_ API to manipulate,
 * its not smart to try and store very small data as separate items since an item storage meta data
 * is 32 bytes + max_name_len long and storing anything less than this becomes wasteful
*/
typedef struct _hogl_vf hogl_vf;

/**
 * @brief Reads the specified virtual file pointed to by path
 * @param vf The result will be stored inside the vf pointer
 * @param path Path to the virtual file to read
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if reading was successful
 *		HOGL_ERROR_BAD_PATH			if the specified path does not resolve to a vf file (e.g. doesn't exist)
 *		HOGL_ERROR_BAD_READ			if a bad value was encountered when reading the file
 *		HOGL_ERROR_ENDIAN_MISMATCH	if endianess of the machine and virtual file don't match
*/
HOGL_API hogl_error hogl_vf_read(hogl_vf** vf, const char* path);

/**
 * @brief Creates a new empty virtual file
 * @param version Version of the virtual file
 * @param max_name_len The maximum amount of characters for an item this can be changed later, but the operation is costly
 * @return Virtual file pointer free using hogl_vf_free
*/
HOGL_API hogl_vf* hogl_vf_new(uint32_t version, uint32_t max_name_len);

/**
 * @brief Changes the maximum name length for a virtual file, if the new length is smaller then the previous one then
 * the name will be cutoff and internal buffer shrunk
 * @param vf Virtual file to edit
 * @param new_name_len New length for the virtual file name
*/
HOGL_API void hogl_vf_change_name_len(hogl_vf* vf, uint32_t new_name_len);

/**
 * @brief Adds an item to the virtual file, causes a realloc in a virtual file.
 * Name parameter must return a valid length from strlen, that means it needs to be a null terminated string, 
 * data is stored as is so checking struct padding is up to the user. The internal vfi data is invalidated and should
 * be regenerated using hogl_vf_map_vfi
 * @param vf Virtual file
 * @param name Name of the new item
 * @param type Type of the new item
 * @param data Data of the item
 * @param size Size of the data parameter
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if item was added successfully
 *		HOGL_ERROR_MEMORY			if reallocs failed, contents stay unchanged
*/
HOGL_API hogl_error hogl_vf_add_item(hogl_vf* vf, const char* name, uint32_t type, void* data, uint64_t size);

/**
 * @brief Renames the specified item by index to the new name
 * @param vf Virtual file to rename
 * @param index Index of the item
 * @param new_name New name of the item
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if item was added successfully
 *		HOGL_ERROR_OUT_OF_RANGE		if index was out of range in virtual file
 *		HOGL_ERROR_MEMORY			if the name doesn't fit
 *		HOGL_ERROR_VF_VFI_MAP		if the item mapping has not be built, should call hogl_vf_map_vfi before
*/
HOGL_API hogl_error hogl_vf_rename_item(hogl_vf* vf, size_t index, const char* new_name);

/**
 * @brief Remaps the item pointers for the virtual file, this is needed when an internal mapping changes for example when
 * new items are added or data size changed
 * @param vf Virtual file to remap
*/
HOGL_API void hogl_vf_map_vfi(hogl_vf* vf);

/**
 * @brief Saves the specified virtual file to the specified path
 * @param vf Virtual file to save
 * @param path File to save to
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if saving was successful
 *		HOGL_ERROR_BAD_PATH			if the specified path does not resolve to a vf file (e.g. doesn't exist)
 *		HOGL_ERROR_BAD_WRITE		if a bad value was encountered when writing the file
*/
HOGL_API hogl_error hogl_vf_save(hogl_vf* vf, const char* path);

/**
 * @brief Gets the index of the item with the specified name and stores it in index
 * @param vf Virtual file
 * @param name Name of the item
 * @param index Target of the operation
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if operation was successful
 *		HOGL_ERROR_VF_BAD_NAME		if an item with the specified name was not found
 * 		HOGL_ERROR_VF_VFI_MAP		if the item mapping has not be built, should call hogl_vf_map_vfi before
*/
HOGL_API hogl_error hogl_vf_get_item_index(hogl_vf* vf, const char* name, size_t* index);

/**
 * @brief Gets the virtual file version
 * @param vf Virtual file
 * @return Version number
*/
HOGL_API uint32_t hogl_vf_version(hogl_vf* vf);

/**
 * @brief Sets the virtual file version
 * @param vf Virtual file
 * @param version New vf version
*/
HOGL_API void hogl_vf_set_version(hogl_vf* vf, uint32_t version);

/**
 * @brief Get data of the specified item from virtual file to the specified pointer, no memcpy happens changing it
 * changes the item directly
 * @param vf Virtual file
 * @param index Index of the item
 * @param target Target pointer
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if mapping was successful
 *		HOGL_ERROR_OUT_OF_RANGE		if index was out of range in virtual file
 * 		HOGL_ERROR_VF_VFI_MAP		if the item mapping has not be built, should call hogl_vf_map_vfi before
*/
HOGL_API hogl_error hogl_vf_map_item(hogl_vf* vf, size_t index, void** target);

/**
 * @brief Get the item data size of the specified item and stores it inside target
 * @param vf Virtual file
 * @param index Index of the item
 * @param target Target pointer
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if mapping was successful
 *		HOGL_ERROR_OUT_OF_RANGE		if index was out of range in virtual file
 * 		HOGL_ERROR_VF_VFI_MAP		if the item mapping has not be built, should call hogl_vf_map_vfi before
*/
HOGL_API hogl_error hogl_vf_item_size(hogl_vf* vf, size_t index, uint64_t* target);

/**
 * @brief Gets the item type of the specified item and store it inside target
 * @param vf Virtual file
 * @param index Index of the item
 * @param target Target pointer
 * @return Returns error codes:
 *		HOGL_ERROR_NONE				if mapping was successful
 *		HOGL_ERROR_OUT_OF_RANGE		if index was out of range in virtual file
 * 		HOGL_ERROR_VF_VFI_MAP		if the item mapping has not be built, should call hogl_vf_map_vfi before
*/
HOGL_API hogl_error hogl_vf_item_type(hogl_vf* vf, size_t index, uint32_t* target);

/**
 * @brief Frees memory associated with the virtual file
 * @param vf Virtual file to free
*/
HOGL_API void hogl_vf_free(hogl_vf* vf);

#endif
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#include "fs.h"
#include "disk.h"

#define MAX_FILE_COUNT 64
#define MAX_FD_COUNT 32
#define MAX_NAME_LENGTH 16

// Mutex Lock
pthread_mutex_t dont_touch_my_mutex;

// Disk Data Structures
char block_belong_list[DISK_BLOCKS]; // size = 1 BYTE * DISK_BLOCKS = 8192 BYTE = 2 BLOCK
// -1 : not belong to any file
// 0 or above : belong to file
int block_page_number[DISK_BLOCKS]; // size = 4 BYTE * DISK_BLOCKS = 8192 BYTE = 8 BLOCK

bool file_free_list[MAX_FILE_COUNT]; // size = 1 BYTE * MAX_FILE_COUNT (64 BYTE)
// false : used
// true  : free
char file_name_list[MAX_NAME_LENGTH * MAX_FILE_COUNT]; // size = 1 BYTE * MAX_NAME_LENGTH * MAX_FILE_COUNT (1024 BYTE)
int file_size_list[MAX_FILE_COUNT]; // size = 4 BYTE * MAX_FILE_COUNT (256 BYTE)
int file_block_count_list[MAX_FILE_COUNT]; // size = 4 BYTE * MAX_FILE_COUNT (256 BYTE)
                                     // subtotal = 10 BLOCK + 1600 BYTE < 11 BLOCK
                                     // This means BLOCK 0 ~ 10 is used for metadata.

// Memory Data Structures
bool fd_free_list[MAX_FD_COUNT];
// false : used
// true  : free
int fd_file_list[MAX_FD_COUNT];
// which file this fd corresponds to
int fd_seek_pointer_list[MAX_FD_COUNT];

// Own Function(s)
void fs_init()
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        pthread_mutex_init(&dont_touch_my_mutex, NULL);
    }
}

// Functions Required to Implement
int make_fs(const char *disk_name)
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        fs_init();
    }
    pthread_mutex_lock(&dont_touch_my_mutex);
    
    // open this disk and write/initialize the necessary meta-information for your file system so
    // that it can be later used (mounted)

    // -1 if the disk disk_name could not be created
    int retval = make_disk(disk_name);
    if (retval)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    // -1 if the disk disk_name could not be opened
    retval = open_disk(disk_name);
    if (retval)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    // -1 if the disk disk_name could not be properly initialized
    char *temp_block = (char*)calloc(BLOCK_SIZE, sizeof(char));
    // write block_belong_list
    for (int i = 0; i < BLOCK_SIZE; i++)
    {
        temp_block[i] = -1;
    }
    for (int i = 0; i < 10; i++)
    {
        retval = block_write(i, temp_block);
        if (retval)
        {
            free(temp_block);
            pthread_mutex_unlock(&dont_touch_my_mutex);
            return -1;
        }
    }
    // write file_free_list, file_name_list, file_size_list, file_block_count_list
    int file_name_list_offset = 1 * MAX_FILE_COUNT;
    int file_size_list_offset = file_name_list_offset + 1 * MAX_NAME_LENGTH * MAX_FILE_COUNT;
    int file_block_count_list_offset = file_size_list_offset + 4 * MAX_FILE_COUNT;
    int super_block_end_offset = file_block_count_list_offset + 4 * MAX_FILE_COUNT;
    for (int i = 0; i < MAX_FILE_COUNT; i++)
    {
        // file_free_list
        temp_block[i] = (char)true;
        // file_name_list
        for (int j = 0; j < MAX_NAME_LENGTH; j++)
        {
            temp_block[file_name_list_offset + i * MAX_NAME_LENGTH + j] = '\0';
        }
        // file_size_list
        temp_block[file_size_list_offset + i] = -1;
        // file_block_count_list
        temp_block[file_block_count_list_offset + i] = -1;
    }
    for (int i = super_block_end_offset; i < BLOCK_SIZE; i++)
    {
        temp_block[i] = 0;
    }
    retval = block_write(10, temp_block);
    if (retval)
    {
        free(temp_block);
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    retval = close_disk();
    if (retval)
    {
        free(temp_block);
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    free(temp_block);
    pthread_mutex_unlock(&dont_touch_my_mutex);
    return 0;
}

int mount_fs(const char *disk_name)
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        fs_init();
    }
    pthread_mutex_lock(&dont_touch_my_mutex);

    // -1 when the disk disk_name could not be opened
    int retval = open_disk(disk_name);
    if (retval)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    // -1 when the disk does not contain a valid file system
    char *temp_block = (char*)calloc(BLOCK_SIZE, sizeof(char));
    int block_belong_list_index = 0;
    for (int i = 0; i < 2; i++)
    {
        retval = block_read(i, temp_block);
        if (retval)
        {
            free(temp_block);
            pthread_mutex_unlock(&dont_touch_my_mutex);
            return -1;
        }
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            block_belong_list[block_belong_list_index] = temp_block[j];
            block_belong_list_index++;
        }
    }
    for (int i = 0; i < 10; i++)
    {
        if (block_belong_list[i] != -1)
        {
            free(temp_block);
            pthread_mutex_unlock(&dont_touch_my_mutex);
            return -1;
        }
    }
    unsigned char tempChar0, tempChar1, tempChar2, tempChar3;
    int block_page_number_index = 0;
    for (int i = 2; i < 10; i++)
    {
        retval = block_read(i, temp_block);
        if (retval)
        {
            free(temp_block);
            pthread_mutex_unlock(&dont_touch_my_mutex);
            return -1;
        }
        for (int j = 0; j < BLOCK_SIZE; j += 4)
        {
            tempChar0 = (unsigned char)temp_block[j];
            tempChar1 = (unsigned char)temp_block[j + 1];
            tempChar2 = (unsigned char)temp_block[j + 2];
            tempChar3 = (unsigned char)temp_block[j + 3];
            block_page_number[block_page_number_index] =
                tempChar3 * 256 * 256 * 256 + tempChar2 * 256 * 256 + tempChar1 * 256 + tempChar0;
            block_page_number_index++;
        }
    }
    retval = block_read(10, temp_block);
    if (retval)
    {
        free(temp_block);
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    int file_name_list_offset = 1 * MAX_FILE_COUNT;
    int file_size_list_offset = file_name_list_offset + 1 * MAX_NAME_LENGTH * MAX_FILE_COUNT;
    int file_block_count_list_offset = file_size_list_offset + 4 * MAX_FILE_COUNT;
    // int super_block_end_offset = file_block_count_list_offset + 4 * MAX_FILE_COUNT;
    for (int i = 0; i < MAX_FILE_COUNT; i++)
    {
        file_free_list[i] = temp_block[i];
        for (int j = 0; j < MAX_NAME_LENGTH; j++)
        {
            file_name_list[i * MAX_NAME_LENGTH + j] = temp_block[file_name_list_offset + i * MAX_NAME_LENGTH + j];
        }
        file_size_list[i] = temp_block[file_size_list_offset + i];
        file_block_count_list[i] = temp_block[file_block_count_list_offset + i];
    }
    free(temp_block);
    for (int i = 0; i < MAX_FD_COUNT; i++)
    {
        fd_free_list[i] = true;
        fd_file_list[i] = -1;
        fd_seek_pointer_list[i] = -1;
    }
    pthread_mutex_unlock(&dont_touch_my_mutex);
    return 0;
}

int umount_fs(const char *disk_name)
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        fs_init();
    }
    pthread_mutex_lock(&dont_touch_my_mutex);

    char *temp_block = (char*)calloc(BLOCK_SIZE, sizeof(char));
    int retval;
    for (int i = 0; i < 2; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j++)
        {
            temp_block[j] = block_belong_list[j];
        }
        retval = block_write(i, temp_block);
        if (retval)
        {
            free(temp_block);
            pthread_mutex_unlock(&dont_touch_my_mutex);
            return -1;
        }
    }
    unsigned char tempChar0, tempChar1, tempChar2, tempChar3;
    int block_page_number_index = 0;
    for (int i = 2; i < 10; i++)
    {
        for (int j = 0; j < BLOCK_SIZE; j += 4)
        {
            tempChar0 = (unsigned char)(block_page_number[block_page_number_index] % 256);
            tempChar1 = (unsigned char)(block_page_number[block_page_number_index] / 256 % 256);
            tempChar2 = (unsigned char)(block_page_number[block_page_number_index] / 256 / 256 % 256);
            tempChar3 = (unsigned char)(block_page_number[block_page_number_index] / 256 / 256 / 256 % 256);
            temp_block[j] = (char)tempChar0;
            temp_block[j + 1] = (char)tempChar1;
            temp_block[j + 2] = (char)tempChar2;
            temp_block[j + 3] = (char)tempChar3;
            block_page_number_index++;
        }
        retval = block_write(i, temp_block);
        if (retval)
        {
            free(temp_block);
            pthread_mutex_unlock(&dont_touch_my_mutex);
            return -1;
        }
    }
    int file_name_list_offset = 1 * MAX_FILE_COUNT;
    int file_size_list_offset = file_name_list_offset + 1 * MAX_NAME_LENGTH * MAX_FILE_COUNT;
    int file_block_count_list_offset = file_size_list_offset + 4 * MAX_FILE_COUNT;
    int super_block_end_offset = file_block_count_list_offset + 4 * MAX_FILE_COUNT;
    for (int i = 0; i < MAX_FILE_COUNT; i++)
    {
        temp_block[i] = (char)file_free_list[i];
        for (int j = 0; j < MAX_NAME_LENGTH; j++)
        {
            temp_block[file_name_list_offset + i * MAX_NAME_LENGTH + j] = file_name_list[i * MAX_NAME_LENGTH + j];
        }
        temp_block[file_size_list_offset + i] = file_size_list[i];
        temp_block[file_block_count_list_offset + i] = file_block_count_list[i];
    }
    for (int i = super_block_end_offset; i < BLOCK_SIZE; i++)
    {
        temp_block[i] = 0;
    }
    retval = block_write(10, temp_block);
    if (retval)
    {
        free(temp_block);
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    // -1 when the disk disk_name could not be closed
    retval = close_disk(disk_name);
    if (retval)
    {
        free(temp_block);
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    free(temp_block);
    pthread_mutex_unlock(&dont_touch_my_mutex);
    return 0;
}



// Following functions require the disk to be at mounted state.

int fs_open(const char *name)
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        fs_init();
    }
    pthread_mutex_lock(&dont_touch_my_mutex);

    // Fail when the file with name cannot be found.
    int file_index = -1;
    for (int i = 0; i < MAX_FILE_COUNT; i++)
    {
        if (!file_free_list[i])
        {
            for (int j = 0; j < MAX_NAME_LENGTH; j++)
            {
                if (name[j] != file_name_list[i * MAX_NAME_LENGTH + j])
                {
                    break;
                }
                else if (name[j] == '\0' && file_name_list[i * MAX_NAME_LENGTH + j] == '\0')
                {
                    file_index = i;
                    goto fs_open_find_name_loop;
                }
            }
        }
    }
fs_open_find_name_loop:
    if (file_index == -1)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    // Fail when there are already 32 file descriptors active.
    int fd_index = -1;
    for (int i = 0; i < MAX_FD_COUNT; i++)
    {
        if (fd_free_list[i])
        {
            fd_free_list[i] = false;
            fd_file_list[i] = file_index;
            fd_seek_pointer_list[i] = 0;
            fd_index = i;
            break;
        }
    }
    if (fd_index == -1)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    // If successful, returns a non-negative integer, which is a file descriptor that can be used
    //     to subsequently access this file.
    // If opened multiple times, provide multiple independent file descriptors.
    // File offset should be set to 0.
    pthread_mutex_unlock(&dont_touch_my_mutex);
    return fd_index;
}

int fs_close(int fildes)
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        fs_init();
    }
    pthread_mutex_lock(&dont_touch_my_mutex);

    // Fail when the fildes does not exist.
    if (fildes < 0 || fildes >= MAX_FD_COUNT)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    // Fail when the fildes is not open.
    if (fd_free_list[fildes])
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    // A closed file descriptor can no longer be used to access the corresponding file.
    fd_free_list[fildes] = true;
    fd_file_list[fildes] = -1;
    fd_seek_pointer_list[fildes] = -1;
    pthread_mutex_unlock(&dont_touch_my_mutex);
    return 0;
}

int fs_create(const char *name)
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        fs_init();
    }
    pthread_mutex_lock(&dont_touch_my_mutex);

    // Fail when file with name already exist.
    for (int i = 0; i < MAX_FILE_COUNT; i++)
    {
        if (!file_free_list[i])
        {
            for (int j = 0; j < MAX_NAME_LENGTH; j++)
            {
                if (name[j] != file_name_list[i * MAX_NAME_LENGTH + j])
                {
                    break;
                }
                else if (name[j] == '\0' && file_name_list[i * MAX_NAME_LENGTH + j] == '\0')
                {
                    pthread_mutex_unlock(&dont_touch_my_mutex);
                    return -1;
                }
            }
        }
    }

    // Fail when file name exceeds 15 char.
    bool name_invalid = true;
    for (int i = 0; i < MAX_NAME_LENGTH; i++)
    {
        if (name[i] == '\0')
        {
            name_invalid = false;
        }
    }
    if (name_invalid)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    // Fail when root dir is full.
    bool free_file_index_not_found = true;
    for (int i = 0; i < MAX_FILE_COUNT; i++)
    {
        if (file_free_list[i])
        {
            file_free_list[i] = false;
            for (int j = 0; j < MAX_NAME_LENGTH; j++)
            {
                file_name_list[i * MAX_NAME_LENGTH + j] = name[j];
                if (name[j] == '\0')
                {
                    break;
                }
            }
            file_size_list[i] = 0;
            file_block_count_list[i] = 0;
            free_file_index_not_found = false;
            break;
        }
    }
    if (free_file_index_not_found)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    // File initially empty.
    // Max file name length is 15 char.
    pthread_mutex_unlock(&dont_touch_my_mutex);
    return 0;
}

int fs_delete(const char *name)
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        fs_init();
    }
    pthread_mutex_lock(&dont_touch_my_mutex);

    // Fail when the file with name does not exist.
    int file_index = -1;
    for (int i = 0; i < MAX_FILE_COUNT; i++)
    {
        if (!file_free_list[i])
        {
            for (int j = 0; j < MAX_NAME_LENGTH; j++)
            {
                if (name[j] != file_name_list[i * MAX_NAME_LENGTH + j])
                {
                    break;
                }
                else if (name[j] == '\0' && file_name_list[i * MAX_NAME_LENGTH + j] == '\0')
                {
                    file_index = i;
                    goto fs_delete_find_name_loop;
                }
            }
        }
    }
fs_delete_find_name_loop:
    if (file_index == -1)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    // Fail when the file is open.
    for (int i = 0; i < MAX_FD_COUNT; i++)
    {
        if (!fd_free_list[i])
        {
            if (fd_file_list[i] == file_index)
            {
                pthread_mutex_unlock(&dont_touch_my_mutex);
                return -1;
            }
        }
    }

    // Free all data blocks
    for (int i = 0; i < DISK_BLOCKS; i++)
    {
        if (block_belong_list[i] == file_index)
        {
            block_belong_list[i] = -1;
            block_page_number[i] = -1;
        }
    }
    // Free all meta information
    file_free_list[file_index] = true;
    for (int i = 0; i < MAX_NAME_LENGTH; i++)
    {
        file_name_list[file_index * MAX_NAME_LENGTH + i] = '\0';
    }
    file_size_list[file_index] = -1;
    file_block_count_list[file_index] = -1;
    pthread_mutex_unlock(&dont_touch_my_mutex);
    return 0;
}

int fs_read(int fildes, void *buf, size_t nbyte)
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        fs_init();
    }
    pthread_mutex_lock(&dont_touch_my_mutex);

    char *cbuf = (char*)buf;
    // Fail when fildes is not valid.
    if (fildes < 0 || fildes >= MAX_FD_COUNT)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    if (fd_free_list[fildes])
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    // Read nbytes of data from fildes into buffer pointed by buf.
    int current_file_number = fd_file_list[fildes];
    if (file_free_list[current_file_number])
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    int cache_seek_pointer = fd_seek_pointer_list[fildes];
    if (cache_seek_pointer > file_size_list[current_file_number])
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return 0;
    }
    int current_page_index = cache_seek_pointer / BLOCK_SIZE;
    int block_index = -1;
    for (int i = 11; i < DISK_BLOCKS; i++)
    {
        if ((int)block_belong_list[i] == current_file_number)
        {
            if (block_page_number[i] == current_page_index)
            {
                block_index = i;
                break;
            }
        }
    }
    if (block_index == -1)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    char *temp_block = (char*)calloc(BLOCK_SIZE, sizeof(char));
    int retval = block_read(block_index, temp_block);
    if (retval)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    int current_inpage_index = cache_seek_pointer % BLOCK_SIZE;
    int successful_read_byte_count = 0;
    for (int i = 0; i < nbyte; i++)
    {
        if (current_inpage_index >= BLOCK_SIZE)
        {
            current_page_index++;
            current_inpage_index = 0;
            block_index = -1;
            for (int j = 11; j < DISK_BLOCKS; j++)
            {
                if ((int)block_belong_list[j] == current_file_number)
                {
                    if (block_page_number[j] == current_page_index)
                    {
                        block_index = j;
                        break;
                    }
                }
            }
            if (block_index == -1)
            {
                free(temp_block);
                pthread_mutex_unlock(&dont_touch_my_mutex);
                return -1;
            }
            retval = block_read(block_index, temp_block);
            if (retval)
            {
                free(temp_block);
                pthread_mutex_unlock(&dont_touch_my_mutex);
                return -1;
            }
        }
        cbuf[i] = temp_block[current_inpage_index];
        current_inpage_index++;
        cache_seek_pointer++;
        successful_read_byte_count++;
        if (cache_seek_pointer >= file_size_list[fildes])
        {
            break;
        }
    }
    // If nbytes exceed end of file, read until end of file.
    // Implicitly increments the file pointer by number of bytes actually read.
    fd_seek_pointer_list[fildes] = cache_seek_pointer;
    //cbuf[successful_read_byte_count] = '\0';
    free(temp_block);
    pthread_mutex_unlock(&dont_touch_my_mutex);
    return 0;
}

int fs_write(int fildes, void *buf, size_t nbyte)
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        fs_init();
    }
    pthread_mutex_lock(&dont_touch_my_mutex);

    char *cbuf = (char*)buf;
    // Fail when fildes is not valid.
    if (fildes < 0 || fildes >= MAX_FD_COUNT)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    if (fd_free_list[fildes])
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    int current_file_number = fd_file_list[fildes];
    if (file_free_list[current_file_number])
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    int cache_seek_pointer = fd_seek_pointer_list[fildes];
    if (cache_seek_pointer >= file_size_list[fildes]) //This shouldn't happen but just in case...
    {
        int previous_page_count = file_size_list[fildes] / BLOCK_SIZE + 1;
        int current_page_count = cache_seek_pointer / BLOCK_SIZE + 1;
        for (int i = previous_page_count; i < current_page_count; i++)
        {
            bool block_not_found = true;
            for (int j = 11; j < DISK_BLOCKS; j++)
            {
                if (block_belong_list[j] == -1)
                {
                    block_belong_list[j] = (char)current_file_number;
                    block_page_number[j] = i;
                    block_not_found = false;
                    break;
                }
            }
            if (block_not_found)
            {
                file_size_list[current_file_number] = (i - 1 + 1) * BLOCK_SIZE;
                pthread_mutex_unlock(&dont_touch_my_mutex);
                return 0;
            }
        }
    }
    // File automatically extended if nbytes exceed end of file.
    // Write as many bytes as possible until disk full.
    // Implicitly increments the file pointer by number of byes actually written.
    int current_page_index = cache_seek_pointer / BLOCK_SIZE;
    int block_index = -1;
    for (int i = 11; i < DISK_BLOCKS; i++)
    {
        if ((int)block_belong_list[i] == current_file_number)
        {
            if (block_page_number[i] == current_page_index)
            {
                block_index = i;
                break;
            }
        }
    }
    char *temp_block = (char*)calloc(BLOCK_SIZE, sizeof(char));
    int retval;
    if (block_index == -1)
    {
        for (int j = 11; j < DISK_BLOCKS; j++)
        {
            if ((int)block_belong_list[j] == -1)
            {
                block_belong_list[j] = (char)current_file_number;
                block_page_number[j] = current_page_index;
                block_index = j;
                for (int k = 0; k < BLOCK_SIZE; k++)
                {
                    temp_block[k] = (char)0;
                }
                break;
            }
        }
        if (block_index == -1)
        {
            file_size_list[current_file_number] = (current_page_index - 1 + 1) * BLOCK_SIZE;
            free(temp_block);
            pthread_mutex_unlock(&dont_touch_my_mutex);
            return 0;
        }
    }
    else
    {
        retval = block_read(block_index, temp_block);
        if (retval)
        {
            free(temp_block);
            pthread_mutex_unlock(&dont_touch_my_mutex);
            return -1;
        }
    }
    int current_inpage_index = cache_seek_pointer % BLOCK_SIZE;
    for (int i = 0; i < nbyte; i++)
    {
        if (current_inpage_index >= BLOCK_SIZE)
        {
            retval = block_write(block_index, temp_block);
            if (retval)
            {
                free(temp_block);
                pthread_mutex_unlock(&dont_touch_my_mutex);
                return -1;
            }
            current_page_index++;
            current_inpage_index = 0;
            block_index = -1;
            for (int j = 11; j < DISK_BLOCKS; j++)
            {
                if ((int)block_belong_list[j] == current_file_number)
                {
                    if (block_page_number[j] == current_page_index)
                    {
                        block_index = j;
                        retval = block_read(j, temp_block);
                        if (retval)
                        {
                            free(temp_block);
                            pthread_mutex_unlock(&dont_touch_my_mutex);
                            return -1;
                        }
                        break;
                    }
                }
            }
            if (block_index == -1)
            {
                for (int j = 11; j < DISK_BLOCKS; j++)
                {
                    if ((int)block_belong_list[j] == -1)
                    {
                        block_belong_list[j] = (char)current_file_number;
                        block_page_number[j] = current_page_index;
                        block_index = j;
                        for (int k = 0; k < BLOCK_SIZE; k++)
                        {
                            temp_block[k] = (char)0;
                        }
                        break;
                    }
                }
                if (block_index == -1)
                {
                    file_size_list[current_file_number] = (current_page_index - 1 + 1) * BLOCK_SIZE;
                    free(temp_block);
                    pthread_mutex_unlock(&dont_touch_my_mutex);
                    return 0;
                }
            }
        }
        temp_block[current_inpage_index] = cbuf[i];
        current_inpage_index++;
        cache_seek_pointer++;
    }
    fd_seek_pointer_list[fildes] = cache_seek_pointer;
    file_size_list[current_file_number] = cache_seek_pointer;
    retval = block_write(block_index, temp_block);
    free(temp_block);
    if (retval)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    pthread_mutex_unlock(&dont_touch_my_mutex);
    return 0;
}

int fs_get_filesize(int fildes)
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        fs_init();
    }
    pthread_mutex_lock(&dont_touch_my_mutex);

    // Fail when fildes is not valid.
    if (fildes < 0 || fildes >= MAX_FD_COUNT)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    if (fd_free_list[fildes])
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    int current_file_number = fd_file_list[fildes];
    pthread_mutex_unlock(&dont_touch_my_mutex);
    return file_size_list[current_file_number];
}

int fs_listfiles(char ***files)
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        fs_init();
    }
    pthread_mutex_lock(&dont_touch_my_mutex);

    int file_count = 0;
    for (int i = 0; i < MAX_FILE_COUNT; i++)
    {
        if (!file_free_list[i])
        {
            file_count++;
        }
    }
    char **temp_name_list = (char**)calloc(file_count + 1, sizeof(char*));
    int temp_name_list_index = 0;
    for (int i = 0; i < MAX_FILE_COUNT; i++)
    {
        if (!file_free_list[i])
        {
            temp_name_list[temp_name_list_index] = (char*)calloc(MAX_NAME_LENGTH, sizeof(char));
            for (int j = 0; j < MAX_NAME_LENGTH; j++)
            {
                temp_name_list[temp_name_list_index][j] = file_name_list[i * MAX_NAME_LENGTH + j];
            }
            temp_name_list_index++;
        }
    }
    temp_name_list[file_count] = NULL;
    *files = temp_name_list;
    pthread_mutex_unlock(&dont_touch_my_mutex);
    return 0;
}

int fs_lseek(int fildes, off_t offset)
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        fs_init();
    }
    pthread_mutex_lock(&dont_touch_my_mutex);

    // Fail when fildes is not valid.
    if (fildes < 0 || fildes >= MAX_FD_COUNT)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    if (fd_free_list[fildes])
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    // Fail when offset is negative
    if ((int)offset < 0)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    // Fail when offset larger than file size
    int current_file_number = fd_file_list[fildes];
    if ((int)offset > file_size_list[current_file_number])
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }
    
    fd_seek_pointer_list[fildes] = (int)offset;
    pthread_mutex_unlock(&dont_touch_my_mutex);
    return 0;
}

int fs_truncate(int fildes, off_t length)
{
    static bool is_first_call = true;
    if (is_first_call)
    {
        is_first_call = false;
        fs_init();
    }
    pthread_mutex_lock(&dont_touch_my_mutex);

    // Fail when fildes is not valid.
    if (fildes < 0 || fildes >= MAX_FD_COUNT)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    if (fd_free_list[fildes])
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    // Fail when file size smaller than length.
    int current_file_number = fd_file_list[fildes];
    if (file_size_list[current_file_number] < (int)length)
    {
        pthread_mutex_unlock(&dont_touch_my_mutex);
        return -1;
    }

    int previous_page_count = file_size_list[current_file_number] / BLOCK_SIZE;
    if (file_size_list[current_file_number] % BLOCK_SIZE)
    {
        previous_page_count++;
    }
    int current_page_count = (int)length / BLOCK_SIZE;
    if ((int)length % BLOCK_SIZE)
    {
        current_page_count++;
    }
    bool block_not_found;
    for (int i = previous_page_count - 1; i >= current_page_count; i--)
    {
        block_not_found = true;
        for (int j = 11; j < DISK_BLOCKS; j++)
        {
            if (block_belong_list[j] == current_file_number)
            {
                if (block_page_number[j] == i)
                {
                    block_belong_list[j] = -1;
                    block_page_number[j] = -1;
                    block_not_found = false;
                    break;
                }
            }
        }
        if (block_not_found)
        {
            pthread_mutex_unlock(&dont_touch_my_mutex);
            return -1;
        }
    }
    file_size_list[current_file_number] = (int)length;
    // When seek pointer larger than length, set it to length
    if (fd_seek_pointer_list[fildes] > (int)length)
    {
        fd_seek_pointer_list[fildes] = (int)length;
    }
    pthread_mutex_unlock(&dont_touch_my_mutex);
    return 0;
}

// Debug Functions

int get_seek_pointer(int fildes)
{
    return fd_seek_pointer_list[fildes];
}

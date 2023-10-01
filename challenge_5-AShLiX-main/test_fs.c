#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "fs.h"

int main()
{
    char lorem_ipsum[] = "0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ \
    0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char abcd[] = "abcdefghijklmnopqrstuvwxyz";

    char disk_name[] = "my_disk";

    int retval;
    retval = make_fs(disk_name);
    if (retval)
    {
        perror("make_fs fail\n");
        exit(-1);
    }
    else
    {
        printf("make_fs success\n");
    }
    retval = mount_fs(disk_name);
    if (retval)
    {
        perror("mount_fs fail\n");
        exit(-1);
    }
    else
    {
        printf("mount_fs success\n");
    }
    retval = fs_create("lorem_ipsum");
    if (retval)
    {
        perror("fs_create - lorem_ipsum - fail\n");
        exit(-1);
    }
    else
    {
        printf("fs_create - lorem_ipsum - success\n");
    }
    int fd_lorem_ipsum = fs_open("lorem_ipsum");
    if (fd_lorem_ipsum == -1)
    {
        perror("fs_open - lorem_ipsum - fail\n");
        exit(-1);
    }
    else
    {
        printf("fs_open - lorem_ipsum - success\n");
    }
    retval = fs_write(fd_lorem_ipsum, lorem_ipsum, strlen(lorem_ipsum));
    if (retval)
    {
        perror("fs_write - lorem_ipsum - fail\n");
        exit(-1);
    }
    else
    {
        printf("fs_write - lorem_ipsum - success\n");
    }
    retval = fs_write(fd_lorem_ipsum, lorem_ipsum, strlen(lorem_ipsum));
    if (retval)
    {
        perror("fs_write - lorem_ipsum - fail\n");
        exit(-1);
    }
    else
    {
        printf("fs_write - lorem_ipsum - success\n");
    }
    retval = fs_close(fd_lorem_ipsum);
    if (retval)
    {
        perror("fs_close - lorem_ipsum - fail\n");
        exit(-1);
    }
    else
    {
        printf("fs_close - lorem_ipsum - success\n");
    }
    fd_lorem_ipsum = fs_open("lorem_ipsum");
    if (fd_lorem_ipsum == -1)
    {
        perror("fs_open - lorem_ipsum - fail\n");
        exit(-1);
    }
    else
    {
        printf("fs_open - lorem_ipsum - success\n");
    }
    char lorem_ipsum_read[strlen(lorem_ipsum) + 1];
    retval = fs_read(fd_lorem_ipsum, lorem_ipsum_read, strlen(lorem_ipsum));
    if (retval)
    {
        perror("fs_read - lorem_ipsum - fail\n");
        exit(-1);
    }
    else
    {
        lorem_ipsum_read[strlen(lorem_ipsum)] = '\0';
        if (strcmp(lorem_ipsum_read, lorem_ipsum))
        {
            perror("fs_read - lorem_ipsum - unexpected result\n");
            exit(-1);
        }
        else
        {
            printf("fs_read - lorem_ipsum - success and expected\n");
        }
    }
    retval = fs_read(fd_lorem_ipsum, lorem_ipsum_read, strlen(lorem_ipsum));
    if (retval)
    {
        perror("fs_read - lorem_ipsum - fail\n");
        exit(-1);
    }
    else
    {
        lorem_ipsum_read[strlen(lorem_ipsum)] = '\0';
        if (strcmp(lorem_ipsum_read, lorem_ipsum))
        {
            perror("fs_read - lorem_ipsum - unexpected result\n");
            exit(-1);
        }
        else
        {
            printf("fs_read - lorem_ipsum - success and expected\n");
        }
    }
    retval = fs_get_filesize(fd_lorem_ipsum);
    if (retval == -1)
    {
        perror("fs_get_filesize - lorem_ipsum - fail\n");
        exit(-1);
    }
    else
    {
        if (retval != strlen(lorem_ipsum) * 2)
        {
            perror("fs_get_filesize - lorem_ipsum - unexpected result\n");
            exit(-1);
        }
        else
        {
            printf("fs_get_filesize - lorem_ipsum - success and expected\n");
            printf("file size: %d\n", retval);
        }
    }
    retval = fs_create("abcd");
    if (retval)
    {
        perror("fs_create - abcd - fail\n");
        exit(-1);
    }
    else
    {
        printf("fs_create - abcd - success\n");
    }
    int fd_abcd = fs_open("abcd");
    if (fd_abcd == -1)
    {
        perror("fs_open - abcd - fail\n");
        exit(-1);
    }
    else
    {
        printf("fs_open - abcd - success\n");
    }
    retval = fs_write(fd_abcd, abcd, strlen(abcd));
    if (retval)
    {
        perror("fs_write - abcd - fail\n");
        exit(-1);
    }
    else
    {
        printf("fs_write - abcd - success\n");
    }
    retval = fs_lseek(fd_abcd, (off_t)0);
    if (retval)
    {
        perror("fs_lseek - abcd - fail\n");
        exit(-1);
    }
    else
    {
        if (get_seek_pointer(fd_abcd) != 0)
        {
            perror("fs_lseek - abcd - unexpected result\n");
            exit(-1);
        }
        else
        {
            printf("fs_lseek - abcd - success and expected\n");
        }
    }
    char abcd_read[strlen(abcd) + 1];
    retval = fs_read(fd_abcd, abcd_read, strlen(abcd));
    if (retval)
    {
        perror("fs_read - abcd - fail\n");
        exit(-1);
    }
    else
    {
        abcd_read[strlen(abcd)] = '\0';
        if (strcmp(abcd_read, abcd))
        {
            perror("fs_read - abcd - unexpected result\n");
            exit(-1);
        }
        else
        {
            printf("fs_read - abcd - success and expected\n");
        }
    }
    retval = fs_get_filesize(fd_abcd);
    if (retval == -1)
    {
        perror("fs_get_filesize - abcd - fail\n");
        exit(-1);
    }
    else
    {
        if (retval != strlen(abcd))
        {
            perror("fs_get_filesize - abcd - unexpected result\n");
            exit(-1);
        }
        else
        {
            printf("fs_get_filesize - abcd - success and expected\n");
        }
    }
    char **temp_name_list;
    retval = fs_listfiles(&temp_name_list);
    int list_index = 0;
    while (true)
    {
        if (temp_name_list[list_index] != NULL)
        {
            printf("%s\n", temp_name_list[list_index]);
            list_index++;
        }
        else
        {
            break;
        }
    }
    int temp_name_list_counter = 0;
    while (true)
    {
        if (temp_name_list[temp_name_list_counter] == NULL)
        {
            break;
        }
        free(temp_name_list[temp_name_list_counter]);
        temp_name_list_counter++;
    }
    if (retval)
    {
        perror("fs_listfiles - fail\n");
        exit(-1);
    }
    else
    {
        if (list_index != 2)
        {
            perror("fs_listfiles - unexpected result\n");
            exit(-1);
        }
        else
        {
            printf("fs_listfiles - success and expected\n");
        }
    }
    retval = fs_lseek(fd_lorem_ipsum, (off_t)20);
    if (retval)
    {
        perror("fs_lseek - lorem_ipsum - fail\n");
        exit(-1);
    }
    else
    {
        if (get_seek_pointer(fd_lorem_ipsum) != 20)
        {
            perror("fs_lseek - lorem_ipsum - unexpected result\n");
            exit(-1);
        }
        else
        {
            printf("fs_lseek - lorem_ipsum - success and expected\n");
        }
    }
    retval = fs_truncate(fd_lorem_ipsum, (off_t)10);
    if (retval)
    {
        perror("fs_truncate - lorem_ipsum - fail\n");
        exit(-1);
    }
    else
    {
        if (fs_get_filesize(fd_lorem_ipsum) != 10)
        {
            perror("fs_truncate - lorem_ipsum - unexpected result - filesize\n");
            exit(-1);
        }
        else
        {
            if (get_seek_pointer(fd_lorem_ipsum) != 10)
            {
                perror("fs_truncate - lorem_ipsum - unexpected result - seekpointer\n");
                exit(-1);
            }
            else
            {
                retval = fs_lseek(fd_lorem_ipsum, (off_t)0);
                if (retval)
                {
                    perror("fs_lseek - lorem_ipsum - second time fail\n");
                    exit(-1);
                }
                else
                {
                    if (get_seek_pointer(fd_lorem_ipsum))
                    {
                        perror("fs_lseek - lorem_ipsum - second time unexpected result\n");
                        exit(-1);
                    }
                    else
                    {
                        printf("fs_lseek - lorem_ipsum - second time success\n");
                    }
                }
                char *temp_string = (char*)calloc(11, sizeof(char));
                fs_read(fd_lorem_ipsum, temp_string, 10);
                temp_string[10] = '\0';
                printf("%s\n", temp_string);
                free(temp_string);
            }
        }
    }
    retval = fs_close(fd_lorem_ipsum);
    if (retval)
    {
        perror("fs_close - lorem_ipsum - fail\n");
        exit(-1);
    }
    else
    {
        printf("fs_close - lorem_ipsum - success\n");
    }
    retval = fs_close(fd_abcd);
    if (retval)
    {
        perror("fs_close - abcd - fail\n");
        exit(-1);
    }
    else
    {
        printf("fs_close - abcd - success\n");
    }
    retval = fs_delete("lorem_ipsum");
    if (retval)
    {
        perror("fs_delete - lorem_ipsum - fail\n");
        exit(-1);
    }
    else
    {
        printf("fs_delete - lorem_ipsum - success\n");
    }
    retval = fs_delete("abcd");
    if (retval)
    {
        perror("fs_delete - abcd - fail\n");
        exit(-1);
    }
    else
    {
        printf("fs_delete - abcd - success\n");
    }
    retval = umount_fs(disk_name);
    if (retval)
    {
        perror("umount_disk - fail\n");
        exit(-1);
    }
    else
    {
        printf("umount_disk - success\n");
    }
    return 0;
}

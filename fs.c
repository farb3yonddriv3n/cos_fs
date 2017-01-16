#include <stdio.h>
#include <memory.h>
#include <stdlib.h>

#include <util.h>
#include <fs.h>

static struct fs_file_s *fs_file_next(char **disk, const char *diskEnd)
{
    struct fs_file_s *f;

    f = (struct fs_file_s *)(*disk);
    if(f->used) {
        (*disk) += f->dataSize + sizeof(*f);
    }

    return (f->used ? f : NULL);
}

static int fs_add(const char *diskName, const char *fileName)
{
    struct fs_file_s newfile;
    FILE *disk = NULL;
    struct b_s f;
    struct b_s fd;

    char *data = NULL;
    f.s = NULL;
    fd.s = NULL;

    memset(&newfile, 0, sizeof(newfile));

    if(strlen(fileName) > (sizeof(newfile.name) - 1)) {
        printf("Error: Filename %ld longer than %d chars\n", strlen(fileName), (int)sizeof(newfile.name) - 1);
        return E_FS_FILESIZE;
    }

    disk = fopen(diskName, "r+b");
    if(disk == NULL) {
        printf("Error: Unable to open disk '%s'\n", diskName);
        goto exit;
    }

    f.n = fs_read(&f.s, fileName, "rb");
    if(f.n < 0) {
        goto exit;
    }

    strcpy(newfile.name, fileName);
    newfile.dataSize = f.n;
    newfile.used = 1;

    fd.n = fs_read(&fd.s, diskName, "rb");
    if(f.n < 0) {
        goto exit;
    }

    data = fd.s;

    while((fs_file_next(&fd.s, fd.s + fd.n - (fd.s - data))) != NULL);

    fseek(disk, fd.s - data, SEEK_SET);
    fwrite(&newfile, sizeof(newfile), 1, disk);
    fseek(disk, fd.s - data + sizeof(newfile), SEEK_SET);
    fwrite(f.s, f.n, 1, disk);

exit:
    if(f.s) free(f.s);
    if(disk) fclose(disk);
    if(data) free(data);

    return 0;
}

static void fs_dump(const char *diskName)
{
    struct b_s disk;
    char *data;

    disk.n = fs_read(&disk.s, diskName, "r+b");
    data = disk.s;

    if(disk.n <= 0) {
        printf("Error: reading diskname '%s'\n", diskName);
        return;
    }

    printf("====\t\t========\t\t==============\n");
    printf("file\t\tfilesize\t\tfirst 20 bytes\n");
    printf("====\t\t========\t\t==============\n");
    struct fs_file_s *file;
    while((file = fs_file_next(&disk.s, disk.s + disk.n)) != NULL) {
        printf("[%s]\t\t%d\t\t\t", file->name, file->dataSize);
        int i;
        for(i = 0; file->dataSize >= 20 ? (i < 20) : (i < file->dataSize); i++) {
            printf("0x%x ", (((char *)(file + 1)))[i]);
        }
        printf("\n");
    }

    free(data);
}

static void fs_format(const char *diskName)
{
    FILE *disk;

    disk = fopen(diskName, "r+b");
    if(disk == NULL) {
        printf("Error: Unable to open disk '%s'\n", diskName);
        return;
    }

    fseek(disk, 0, SEEK_END);
    int size = ftell(disk);
    rewind(disk);

    char *empty = malloc(size);
    memset(empty, 0, size);
    fwrite(empty, size, 1, disk);

    free(empty);

    fclose(disk);
}

int main(int argc, char *argv[])
{
    if(argc >= 3) {
        if(strcmp(argv[2], "create") == 0 && argc == 4) {
            fs_add(argv[1], argv[3]);
        } else if(strcmp(argv[2], "format") == 0) {
            fs_format(argv[1]);
        } else if(strcmp(argv[2], "dump") == 0) {
            fs_dump(argv[1]);
        }
    }

    return 0;
}

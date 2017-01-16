#ifndef FS_H_
#define FS_H_

struct b_s {
    char *s;
    int n;
};

struct fs_file_s {
    int used;
    char name[32];
    int dataSize;
};

enum err_e {
    E_FS_FILESIZE
};

#endif

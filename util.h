#ifndef UTIL_H_
#define UTIL_H_

#define MAX_FILE_SIZE		(1024 * 1024 * 1024)

inline static int fs_read(char **dst, const char *fname, const char *mode)
{
    FILE *pFile;
    int lSize;
    char *buffer;
    int result;

    pFile = fopen(fname, mode);
    if(pFile == NULL) {
        printf("Error: cannot open filename %s\n", fname);
        return -1; 
    }

    fseek(pFile , 0 , SEEK_END);
    lSize = ftell(pFile);
    rewind(pFile);

    if(lSize > MAX_FILE_SIZE) {
        fclose(pFile);
        printf("Error: file %s is too big %d\n", fname, lSize);
        return -1; 
    }

    buffer = malloc(sizeof(char) * lSize);
    if(buffer == NULL) {
        fclose(pFile);
        printf("Error: failed to allocate memory\n");
        return -1; 
    }

    result = fread(buffer, sizeof(char), lSize, pFile);
    if(result != lSize) {
        fclose(pFile);
        free(buffer);
        printf("Error: file read error\n");
        return -1; 
    }

    *dst = buffer;

    fclose (pFile);
    return result;
}

#endif

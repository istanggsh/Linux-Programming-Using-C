#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <sys/stat.h>
#include <sys/sysmacros.h>
#include <fcntl.h>
#include <time.h>
#include <string.h>

#define ERR_EXIT(s) \
    do { \
        perror(s); \
        exit(EXIT_FAILURE); \
    } while (0)

#define STRSIZ sizeof("-rwxrwxrwx")
#define GB (1024 * 1024 * 1024)
#define MB (1024 * 1024)
#define KB 1024
#define FILENAME 512

char str[STRSIZ];

void fileTypePermStr(mode_t perm) {
    memset(str, 0, STRSIZ);
    /* file type */
    switch (perm & S_IFMT) {
    case S_IFBLK:   str[0] = 'b';     break;
    case S_IFCHR:   str[0] = 'c';     break;
    case S_IFDIR:   str[0] = 'd';     break;
    case S_IFIFO:   str[0] = 'p';     break;
    case S_IFLNK:   str[0] = 'l';     break;
    case S_IFSOCK:  str[0] = 's';     break;
    case S_IFREG:   str[0] = '-';     break;
    default:        str[0] = '?';     break;
    }
    
    /* file perm */
    str[1] = (perm & S_IRUSR) ? 'r' : '-';
    str[2] = (perm & S_IWUSR) ? 'w' : '-';
    str[3] = (perm & S_IXUSR) ? 'x' : '-';
    str[4] = (perm & S_IRGRP) ? 'r' : '-';
    str[5] = (perm & S_IWGRP) ? 'w' : '-';
    str[6] = (perm & S_IXGRP) ? 'x' : '-';
    str[7] = (perm & S_IROTH) ? 'r' : '-';
    str[8] = (perm & S_IWOTH) ? 'w' : '-';
    str[9] = (perm & S_IXOTH) ? 'x' : '-';

    printf("%s ", str);
}

void displayStatInfo(const struct stat* sb, int flags) {
    /* file type and perm */
    fileTypePermStr(sb->st_mode);

    /* #hardlink */
    printf("%ld ", (long)sb->st_nlink);

    /* file ownership */
    printf("%s %s ", getpwuid(sb->st_uid)->pw_name, getgrgid(sb->st_gid)->gr_name);

    if (str[0] == 'b' || str[0] == 'c') /* device --> dev number */
        printf("%ld, %ld ", (long)major(sb->st_rdev), (long)minor(sb->st_rdev));
    else { /* file size */
        long long sz = (long long)sb->st_size;
        if (flags) { /* -h */
            if (sz >= GB) 
                printf("%2.1fG ", sz * 1.0 / GB);
            else if (sz >= MB)
                printf("%2.1fM ", sz * 1.0 / MB);
            else if (sz >= KB)
                printf("%2.1fK ", sz * 1.0 / KB);
            else
                printf("%lld ", sz);
        }
        else 
            printf("%lld ", sz);
    }

    /* Last file modification time */
    char* mtime = ctime(&sb->st_mtime);
    mtime[strlen(mtime) - 1] = '\0';    /* ctime返回的字符串末尾有个换行 */
    printf("%s ", mtime);
}

int main(int argc, char** argv) {    
    int flags = 0;  /* -h */
    int filePos = 1;
    struct stat sb;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-h] <pathname>", argv[0]);
        exit(EXIT_FAILURE);
    }

    if (argc == 3 && strcmp(argv[1], "-h") == 0) {
        flags = 1;
        filePos = 2;
    }
    
    if (lstat(argv[filePos], &sb) == -1)
        ERR_EXIT("lstat");

    displayStatInfo(&sb, flags);

    /* file name */
    if (str[0] == 'l') { /* symbolic link */
        char src[FILENAME];
        int ret;
        if ((ret = readlink(argv[filePos], src, FILENAME)) == -1)
            ERR_EXIT("readlink");
        src[ret] = '\0';
        printf("%s -> %s\n", argv[filePos], src);
    }
    else 
        printf("%s\n", argv[filePos]);
    return 0;
}


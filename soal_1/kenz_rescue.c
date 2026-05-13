#define FUSE_USE_VERSION 26

#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>

static char source_dir[1024];

static void fullpath(char fpath[1024], const char *path)
{
    strcpy(fpath, source_dir);
    strcat(fpath, path);
}

static int x_getattr(const char *path, struct stat *stbuf)
{
    char fpath[1024];
    fullpath(fpath, path);

    int res = lstat(fpath, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

static int x_readdir(const char *path,
                     void *buf,
                     fuse_fill_dir_t filler,
                     off_t offset,
                     struct fuse_file_info *fi)
{
    char fpath[1024];
    fullpath(fpath, path);

    DIR *dp;
    struct dirent *de;

    dp = opendir(fpath);

    if (dp == NULL)
        return -errno;

    while ((de = readdir(dp)) != NULL)
    {
        struct stat st;

        memset(&st, 0, sizeof(st));
        st.st_ino = de->d_ino;
        st.st_mode = de->d_type << 12;

        if (filler(buf, de->d_name, &st, 0))
            break;
    }

    closedir(dp);
    return 0;
}

static int x_open(const char *path, struct fuse_file_info *fi)
{
    char fpath[1024];
    fullpath(fpath, path);

    int res = open(fpath, fi->flags);

    if (res == -1)
        return -errno;

    close(res);
    return 0;
}

static int x_read(const char *path,
                  char *buf,
                  size_t size,
                  off_t offset,
                  struct fuse_file_info *fi)
{
    char fpath[1024];
    fullpath(fpath, path);

    int fd = open(fpath, O_RDONLY);

    if (fd == -1)
        return -errno;

    int res = pread(fd, buf, size, offset);

    if (res == -1)
        res = -errno;

    close(fd);
    return res;
}

static struct fuse_operations x_oper = {
    .getattr = x_getattr,
    .readdir = x_readdir,
    .open = x_open,
    .read = x_read,
};

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        fprintf(stderr, "Usage: %s <source_dir> <mount_point>\n", argv[0]);
        return 1;
    }

    realpath(argv[1], source_dir);

    char *fuse_argv[2];
    fuse_argv[0] = argv[0];
    fuse_argv[1] = argv[2];

    return fuse_main(2, fuse_argv, &x_oper, NULL);
}

# SISOP-4-2026-IT-078
# SISOP-4-2026-IT-078

**Deskripsi Soal**

Pada soal ini kita diminta untuk membuat filesystem virtual menggunakan FUSE dengan nama kenz_rescue. Filesystem tersebut melakukan mirror terhadap folder amba_files sehingga isi file pada folder asli dapat diakses melalui folder mount point mnt.
Program dibuat menggunakan bahasa C dan library FUSE. Filesystem harus dapat:
- membaca isi direktori
- membuka file
- membaca isi file
  
serta menampilkan isi folder asli melalui mount point.

untuk tahap pengerjaannya yang pertama membuat struktur folder yaotu, amba_files, mnt dan membaut file kenz_rescue.c , lalu menyiapkan file sumber yaitu file 1.txt sampai 7.txt dan dimasukkan dalam folder amba_files, lalu yang ketiga membuat program fuse. lalu complie jalankan system.

**Penjelasan Kode Kenz_sercue.c**
Menggunkan fuse yang digunakan untuk program 
```c
#define FUSE_USE_VERSION 26
```
ibrary yang digunakan
```c
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/stat.h>
```
Variable global yang digunakan untuk menyimpan path absolut dari folder sumber(amba_files)
```c
static char source_dir[1024];
```
Fungsi ini digunakan untuk menggabungkan:
path direktori asli
dengan path file dari FUSE.
```c
static void fullpath(char fpath[1024], const char *path)
{
    strcpy(fpath, source_dir);
    strcat(fpath, path);
}
```
Digunakan untuk mengambil atribut file seperti :
- ukuran file
- permission
- tipe file
```c
static int x_getattr(const char *path, struct stat *stbuf)
{
    char fpath[1024];
    fullpath(fpath, path);

    int res = lstat(fpath, stbuf);

    if (res == -1)
        return -errno;

    return 0;
}

```
untuk membaca isi direktori
```c
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
```
Digunakan untuk membuka file
```c
tatic int x_open(const char *path, struct fuse_file_info *fi)
{
    char fpath[1024];
    fullpath(fpath, path);

    int res = open(fpath, fi->flags);

    if (res == -1)
        return -errno;

    close(res);
    return 0;
}

```
untuk membaca isi file
```c
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
```
Digunakan untuk menghubungkan operasi fuse dengan fungsi yang dibuat
```c
static struct fuse_operations x_oper = {
    .getattr = x_getattr,
    .readdir = x_readdir,
    .open = x_open,
    .read = x_read,
};
```
getter = x_gettatr : menghubungkan operasi pengambilan atribut file
readdir = x_readdir : menghubungkan operasi pembacaan direktori
open = x_open :  menghubungkan operasi membuka file
read = x_read : menghubungkan operasi membaca isi file

Fungsi main()
```c
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
```
Merupakan fungsi utama program

` if (argc < 3)` mengecek apakah argumen program sudah benar
`fprintf(stderr, "Usage: %s <source_dir> <mount_point>\n", argv[0]);` menampilkan cara penggunaan program jika argumen kurang
` realpath(argv[1], source_dir);` mengubah path folder sumber menjadi path absolut
` char *fuse_argv[2];` array argumen yang dikirim ke fuse
` fuse_argv[0] = argv[0];` menyimpan nama program
` fuse_argv[1] = argv[2]; ` menyimpan mount point (mnt)
`return fuse_main(2, fuse_argv, &x_oper, NULL);` Menjalankan filesystem fuse operasi yang telah dibuat sebelumnya

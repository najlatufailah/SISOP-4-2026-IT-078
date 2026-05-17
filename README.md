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


## REVISI PENAMBAHAN SOAL 1

terdapat revisi pada soal 1 yaitu penambahan file virtual yang bernama `tujuan.txt` yang hanya muncul pada mount directory (mnt)  dan tidak terlihat secara fisik di dalam amba_file
File virtual ini ditambahkan melalui callback:
- x_getattr
- x_readdir
- x_open
- x_read
 output yang ditampilkan

` ls mnt `

<img width="685" height="50" alt="Tangkapan Layar 2026-05-17 pukul 02 14 57" src="https://github.com/user-attachments/assets/97526891-2610-47b5-9636-927453b17b12" />

`ls amba_files`

<img width="500" height="53" alt="Tangkapan Layar 2026-05-17 pukul 02 15 25" src="https://github.com/user-attachments/assets/d774efbf-ef65-485e-a331-3badb488e03a" />

- `cat mnt/tujuan.txt`
- menampilkan salah satu isi mnt 1.txt`cat mnt/1.txt`
- `stat mnt/tujuan.txt`
- `wc -c mnt/tujuan.txt`

<img width="723" height="251" alt="Tangkapan Layar 2026-05-17 pukul 02 15 53" src="https://github.com/user-attachments/assets/814d79fe-17f3-4f01-8c9a-dcc2f4764d2a" />

Jika di tree

<img width="700" height="316" alt="Tangkapan Layar 2026-05-17 pukul 02 20 17" src="https://github.com/user-attachments/assets/04f6c364-55a4-4913-9196-985f501baabd" />


#  PENAMBAHAN SOAL 2

Pada soal ini pembuatan sistem menajemn databas, yang sistem harus memiliki jaringan TCP Socket (Port 9000), mendukung operasi dasar DDL (CREATE DATABASE, CREATE TABLE) dan DML (INSERT INTO, SELECT * FROM), serta mengamankan seluruh berkas media penyimpanan menggunakan algoritma kriptografi bitwise XOR dengan kunci 0x76.

- Dockerfile: Mengisolasi environment Ubuntu, menginstal biner compiler gcc, menyalin berkas kode, mengompilasi program secara otomatis, dan mengekspos Port internal 9000.

- client.c: Menggunakan paradigma short-lived TCP connection (soket dibuka-tutup setiap kueri terkirim). Ini menjamin tidak adanya deadlock stream buffer saat menerima input interaktif dari user di prompt db > .

- server.c: Berperan sebagai jantung pangkalan data yang melakukan parsing perintah string, eksekusi pembuatan direktori fisik melalui fungsi low-level C (mkdir dan fopen), serta melakukan transformasi bitwise XOR 0x76 untuk mengamankan data tabel secara instan.

- Fungsi Kriptografi Simetris XOR 0x76 (server.c):
```c
void xor_cipher(const char *in, char *out, size_t size) {
    for (size_t i = 0; i < size; i++) {
        out[i] = in[i] ^ 0x76; // Kunci bitwise 0x76 sesuai lembar soal
    }
}
```

- Fungsi Penulisan Aman / Enkripsi (INSERT INTO):
```c
void save_to_encrypted_db(const char *table_name, const char *data) {
    char filepath[512];
    sprintf(filepath, "/app/encrypted_storage/%s.csv.enc", table_name);
    size_t len = strlen(data);
    char *encrypted_data = malloc(len);
    
    xor_cipher(data, encrypted_data, len); // Enkripsi sebelum masuk storage

    FILE *f = fopen(filepath, "ab"); // Append Binary
    if (f != NULL) {
        fwrite(encrypted_data, 1, len, f);
        char nl = '\n' ^ 0x76; // Newline ikut di-XOR sebagai delimiter
        fputc(nl, f);
        fclose(f);
    }
    free(encrypted_data);
}
```

- Fungsi Pembacaan Transparan / Dekripsi (SELECT * FROM):
```c
void read_and_decrypt_db(const char *table_name, char *output_buffer) {
    char filepath[512];
    sprintf(filepath, "/app/encrypted_storage/%s.csv.enc", table_name);
    FILE *f = fopen(filepath, "rb"); // Read Binary
    if (f == NULL) { return; }

    fseek(f, 0, SEEK_END); long size = ftell(f); fseek(f, 0, SEEK_SET);
    if (size > 0) {
        char *encrypted_data = malloc(size);
        char *decrypted_data = malloc(size + 1);
        fread(encrypted_data, 1, size, f);
        
        xor_cipher(encrypted_data, decrypted_data, size); // Mengembalikan ke teks asli
        decrypted_data[size] = '\0';
        strcpy(output_buffer, decrypted_data);
        
        free(encrypted_data); free(decrypted_data);
    }
    fclose(f);
}
```

Untuk menjalankan program
 1. Bikin folder jembatan sinkronisasi
mkdir -p fuse_mount

    Fungsi : Membuat folder fisik fuse_mount di komputer host (Mac). Folder ini       disinkronisasikan secara langsung dengan folder /app/db di dalam Ubuntu container. File Dockerfile, server.c, dan client.c wajib diletakkan langsung di dalam folder induk soal_2 ini

2. Kompilasi citra sistem Docker container
docker build -t soal-2-modul-4-sisop .

    Fungsi :Memerintahkan Docker Engine mengunduh base image Ubuntu, menginstal compiler gcc, menyalin file C, serta membundelnya menjadi sebuah image biner bernama soal_2.

3. Jalankan container dengan bind-mount volume aktif
docker run -d --name db_app -p 9000:9000 -v "$(pwd)/fuse_mount:/app/db" soal-2-modul-4-sisop

    Fungsi :Menjalankan kontainer db_app di latar belakang (detached mode), mem-forward jalur soket komunikasi ke port 9000, serta melakukan pengikatan volume agar folder kontainer tersambung langsung ke folder host Mac. Folder /app/encrypted_storage otomatis terbuat secara internal di dalam kontainer pada tahap ini sebagai brankas terenkripsi yang aman.

4. Masuk ke prompt konsol klien interaktif
docker exec -it db_app ./db_client

    Fungsi :Mengeksekusi biner ./db_client secara interaktif di dalam kontainer, memicu munculnya prompt interaktif pangkalan data berupa tulisan db > .

5. membersihkan alokasi memori dan kontainer.
   docker stop db_app && docker rm db_app

   Fungsi :Menghentikan background proses engine database dan menghapus kontainer db_app agar tidak membebani penggunaan resource laptop.

Lalu  menjalankan urutan kueri oengujian yang ada di dalam db> 
```
db > HELP
--- COMMAND LIST ---
1. CREATE DATABASE <name>
2. CREATE TABLE <name>
3. INSERT INTO <tbl> <data>
4. SELECT * FROM <tbl>
5. EXIT

db > CREATE DATABASE pokemon_db
DATABASE pokemon_db CREATED SUCCESSFULLY

db > CREATE TABLE pokedex
TABLE pokedex CREATED SUCCESSFULLY

db > INSERT INTO pokedex 001,Bulbasaur,Grass,90
SUCCESS: Data inserted into pokedex and encrypted securely.

db > INSERT INTO pokedex 004,Charmander,Fire,95
SUCCESS: Data inserted into pokedex and encrypted securely.

db > SELECT * FROM pokedex
001,Bulbasaur,Grass,90
004,Charmander,Fire,95

db > exit
Disconnecting...
```

<img width="577" height="127" alt="Tangkapan Layar 2026-05-17 pukul 17 22 15" src="https://github.com/user-attachments/assets/8aa575c8-cad8-41c4-ba22-676c23200a29" />

<img width="572" height="238" alt="Tangkapan Layar 2026-05-17 pukul 17 22 36" src="https://github.com/user-attachments/assets/a2872a74-221d-4c85-a120-2cd7e37ddc7b" />



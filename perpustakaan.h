#ifndef PERPUSTAKAAN_H
#define PERPUSTAKAAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>   // Untuk denda & tanggal
#include <ctype.h>  // Untuk tolower()
#ifdef _WIN32
    #define CLEAR "cls"
#else
    #define CLEAR "clear"
#endif

// --- NAMA FILE GLOBAL ---
#define FILE_BUKU "buku.txt"
#define FILE_ANGGOTA "anggota.txt"
#define FILE_TRANSAKSI "transaksi.txt"
#define FILE_PUSTAKAWAN "pustakawan.txt"
#define FILE_ADMIN "admin.txt"
#define FILE_TEMP "temp.txt" // Untuk proses edit/hapus

// --- DEKLARASI ENUM & STRUCT ---

typedef enum {
    FIKSI, NON_FIKSI, KOMIK, MAJALAH, REFERENSI
} KategoriBuku;

typedef enum {
    TERSEDIA, DIPINJAM
} StatusBuku;

typedef struct {
    int idBuku;
    char judul[100];
    char penulis[100];
    KategoriBuku kategori;
    StatusBuku status;
} Buku;

typedef struct {
    int idAnggota;
    char nama[100];
    char kontak[50];
} Anggota;

typedef struct {
    int idTransaksi;
    int idBuku;
    int idAnggota;
    time_t tanggalPinjam;
    time_t tanggalKembali;
    double denda;
} Transaksi;

// --- PROTOTYPE FUNGSI ---

// --- Dari main.c ---
void tampilkanMenuUtama();

// --- Dari login.c ---
void loginPustakawan();

// --- Dari util.c (Fungsi Bantuan) ---
void setupAwal(); // Membuat file .txt jika belum ada
void bersihkanLayar();
void tungguEnter();
void inputString(const char* prompt, char* buffer, int size);
int inputInt(const char* prompt);
int getNextId(const char* filename); // Helper untuk auto-increment ID
const char* getKategoriString(KategoriBuku k);
const char* getStatusString(StatusBuku s);
KategoriBuku inputKategori();
int my_strcasecmp(const char *s1, const char *s2);
void getBukuJudul(int idBuku, char* judulBuffer);
void getAnggotaNama(int idAnggota, char* namaBuffer);
int isAnggotaValid(int idAnggota);
int isBukuAvailable(int idBuku);
void kelolaDurasiPeminjaman();
int dapatkanDurasiMaksimal();

// --- Dari buku.c (Manajemen Buku) ---
void tambahBuku();
void hapusBuku();
void editBuku();
void lihatDaftarBuku();
void cariBuku();
int ubahStatusBuku(int idBuku, StatusBuku statusBaru); // Helper

// --- Dari anggota.c (Manajemen Anggota) ---
void daftarkanAnggota();
void lihatDaftarAnggota(); // Tambahan logis
void lihatRiwayatPeminjaman(); // Perlu ID Anggota

// --- Dari transaksi.c (Manajemen Peminjaman) ---
void peminjamanBuku();
void pengembalianBuku();
double perhitunganDendaOtomatis(time_t tanggalPinjam, time_t tanggalKembali);
void tampilkanRankingBuku();

#endif // PERPUSTAKAAN_H
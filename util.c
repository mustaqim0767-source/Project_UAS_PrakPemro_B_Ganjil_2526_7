#include "perpustakaan.h"

// fungsi cross-platform untuk case-insensitive string compare
int my_strcasecmp(const char *s1, const char *s2) {
    while (*s1 && *s2) {
        if (tolower((unsigned char)*s1) != tolower((unsigned char)*s2)) {
            break;
        }
        s1++;
        s2++;
    }
    return tolower((unsigned char)*s1) - tolower((unsigned char)*s2);
}

// fungsi untuk membuat file .txt default jika belum ada
void setupAwal() {
    FILE *fp;
    
    // cek file pustakawan
    if ((fp = fopen(FILE_PUSTAKAWAN, "r")) == NULL) {
        // File tidak ada, buat baru
        fp = fopen(FILE_PUSTAKAWAN, "w");
        fprintf(fp, "admin;admin123\n");
        fclose(fp);
    } else {
        fclose(fp);
    }

    // cek file admin (durasi)
    if ((fp = fopen(FILE_ADMIN, "r")) == NULL) {
        fp = fopen(FILE_ADMIN, "w");
        fprintf(fp, "7\n"); // Default 7 hari
        fclose(fp);
    } else {
        fclose(fp);
    }

    // buat file lain jika belum ada (kosong)
    if ((fp = fopen(FILE_BUKU, "r")) == NULL) {
        fp = fopen(FILE_BUKU, "w"); fclose(fp);
    } else { fclose(fp); }
    
    if ((fp = fopen(FILE_ANGGOTA, "r")) == NULL) {
        fp = fopen(FILE_ANGGOTA, "w"); fclose(fp);
    } else { fclose(fp); }

    if ((fp = fopen(FILE_TRANSAKSI, "r")) == NULL) {
        fp = fopen(FILE_TRANSAKSI, "w"); fclose(fp);
    } else { fclose(fp); }
}

void bersihkanLayar() {
    system(CLEAR);
}

void tungguEnter() {
    printf("\nTekan Enter untuk melanjutkan...");
    // membersihkan buffer input
    int c;
    while ((c = getchar()) != '\n' && c != EOF); // Handle sisa newline
    getchar(); // menunggu user menekan Enter
}

void inputString(const char* prompt, char* buffer, int size) {
    printf("%s", prompt);
    
    fgets(buffer, size, stdin);
    
    // Hapus newline di akhir string
    buffer[strcspn(buffer, "\r\n")] = 0; 
}

// Helper input integer (aman dari input salah)
int inputInt(const char* prompt) {
    int val;
    char buffer[100];
    while (1) {
        printf("%s", prompt);
        if (fgets(buffer, sizeof(buffer), stdin) != NULL) {
            if (sscanf(buffer, "%d", &val) == 1) {
                // Pastikan tidak ada sisa karakter non-numerik setelah angka
                int c = 0;
                sscanf(buffer, "%d %c", &val, &c);
                if (isspace(c) || c == 0) {
                     return val; // Input valid
                }
            }
        }
        printf("Input tidak valid. Harap masukkan angka.\n");
    }
}


// Helper untuk mendapatkan ID unik berikutnya
int getNextId(const char* filename) {
    FILE *fp = fopen(filename, "r");
    if (fp == NULL) {
        // Jika file tidak ada/kosong, mulai dari 1001 (atau 2001, dll)
        if (strcmp(filename, FILE_BUKU) == 0) return 1001;
        if (strcmp(filename, FILE_ANGGOTA) == 0) return 2001;
        if (strcmp(filename, FILE_TRANSAKSI) == 0) return 3001;
        return 1;
    }

    int id = 0, maxId = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        sscanf(line, "%d", &id); // Baca ID di awal baris
        if (id > maxId) {
            maxId = id;
        }
    }
    fclose(fp);
    
    // Jika file kosong, mulai dari ID awal
    if (maxId == 0) {
        if (strcmp(filename, FILE_BUKU) == 0) return 1001;
        if (strcmp(filename, FILE_ANGGOTA) == 0) return 2001;
        if (strcmp(filename, FILE_TRANSAKSI) == 0) return 3001;
    }
    
    return maxId + 1;
}

// --- helper konversi enum ke String ---

const char* getKategoriString(KategoriBuku k) {
    switch (k) {
        case FIKSI: return "Fiksi";
        case NON_FIKSI: return "Non-Fiksi";
        case KOMIK: return "Komik";
        case MAJALAH: return "Majalah";
        case REFERENSI: return "Referensi";
        default: return "Tidak Diketahui";
    }
}

const char* getStatusString(StatusBuku s) {
    return (s == TERSEDIA) ? "Tersedia" : "Dipinjam";
}

KategoriBuku inputKategori() {
    int k;
    while(1) {
        printf("Pilih Kategori:\n");
        printf("0. Fiksi\n1. Non-Fiksi\n2. Komik\n3. Majalah\n4. Referensi\n");
        k = inputInt("Pilihan Kategori (0-4): ");
        if (k >= 0 && k <= 4) {
            return (KategoriBuku)k;
        }
        printf("Pilihan tidak valid.\n");
    }
}


// --- HELPER BARU UNTUK TRANSAKSI ---

// Mencari judul buku berdasarkan ID
void getBukuJudul(int idBuku, char* judulBuffer) {
    FILE* fp = fopen(FILE_BUKU, "r");
    Buku b;
    int kat, stat;
    strcpy(judulBuffer, "Judul Tidak Ditemukan"); // Default

    if (fp == NULL) return;

    while(fscanf(fp, "%d;%99[^;];%99[^;];%d;%d\n", 
           &b.idBuku, b.judul, b.penulis, &kat, &stat) != EOF) 
    {
        if (b.idBuku == idBuku) {
            strcpy(judulBuffer, b.judul);
            break;
        }
    }
    fclose(fp);
}

// Validasi Anggota
int isAnggotaValid(int idAnggota) {
    FILE* fp = fopen(FILE_ANGGOTA, "r");
    Anggota a;
    int valid = 0;
    if (fp == NULL) return 0;

    while(fscanf(fp, "%d;%99[^;];%49[^\n]\n", &a.idAnggota, a.nama, a.kontak) != EOF) {
        if (a.idAnggota == idAnggota) {
            valid = 1;
            break;
        }
    }
    fclose(fp);
    return valid;
}

// Validasi Buku dan Ketersediaan
int isBukuAvailable(int idBuku) {
    FILE* fp = fopen(FILE_BUKU, "r");
    Buku b;
    int kat, stat;
    int available = 0;
    if (fp == NULL) return 0;

    while(fscanf(fp, "%d;%99[^;];%99[^;];%d;%d\n", 
           &b.idBuku, b.judul, b.penulis, &kat, &stat) != EOF) 
    {
        if (b.idBuku == idBuku) {
            if (stat == TERSEDIA) {
                available = 1; // Buku ditemukan dan tersedia
            }
            break; // Buku ditemukan (meski mungkin dipinjam)
        }
    }
    fclose(fp);
    return available;
}

// Mencari nama anggota berdasarkan ID
void getAnggotaNama(int idAnggota, char* namaBuffer) {
    FILE* fp = fopen(FILE_ANGGOTA, "r");
    Anggota a;
    strcpy(namaBuffer, "Nama Tidak Ditemukan"); // Default

    if (fp == NULL) return;

    while(fscanf(fp, "%d;%99[^;];%49[^\n]\n", &a.idAnggota, a.nama, a.kontak) != EOF) {
        if (a.idAnggota == idAnggota) {
            strcpy(namaBuffer, a.nama);
            break;
        }
    }
    fclose(fp);
}

// --- Kode dari admin.c yang dipindahkan ke util.c ---

void kelolaDurasiPeminjaman() {
    bersihkanLayar();
    printf("==========================\n");
    printf("  KELOLA DURASI PINJAM MAKS\n");
    printf("==========================\n");
    
    int durasiLama = dapatkanDurasiMaksimal();
    printf("Durasi maksimal peminjaman saat ini: %d hari\n\n", durasiLama);
    
    int durasiBaru = inputInt("Masukkan durasi baru (hari): ");
    
    FILE *fp = fopen(FILE_ADMIN, "w"); // Mode "w" untuk menimpa
    if (fp == NULL) {
        printf("Error: Gagal membuka file admin.\n");
    } else {
        fprintf(fp, "%d\n", durasiBaru);
        fclose(fp);
        printf("Durasi peminjaman berhasil diubah menjadi %d hari.\n", durasiBaru);
    }
    tungguEnter();
}

int dapatkanDurasiMaksimal() {
    int durasi = 7; // Default jika file error
    FILE *fp = fopen(FILE_ADMIN, "r");
    if (fp != NULL) {
        fscanf(fp, "%d", &durasi);
        fclose(fp);
    }
    return durasi;
}

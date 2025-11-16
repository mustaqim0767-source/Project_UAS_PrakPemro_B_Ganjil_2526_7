#include "perpustakaan.h"

void tambahBuku() {
    Buku b;
    FILE *fp;

    bersihkanLayar();
    printf("==========================\n");
    printf("     TAMBAH BUKU BARU\n");
    printf("==========================\n");

    // 1. Dapatkan ID unik baru
    b.idBuku = getNextId(FILE_BUKU);
    printf("ID Buku Baru: %d\n", b.idBuku);

    // 2. Minta input data
    inputString("Judul Buku: ", b.judul, 100);
    inputString("Penulis: ", b.penulis, 100);
    b.kategori = inputKategori();
    b.status = TERSEDIA; // Buku baru pasti tersedia

    // 3. Buka file dalam mode "append" (menambahkan di akhir)
    fp = fopen(FILE_BUKU, "a");
    if (fp == NULL) {
        printf("Error: Gagal membuka file %s\n", FILE_BUKU);
        tungguEnter();
        return;
    }

    // 4. Tulis data baru ke file dengan format
    fprintf(fp, "%d;%s;%s;%d;%d\n", 
            b.idBuku, b.judul, b.penulis, b.kategori, b.status);

    // 5. Tutup file
    fclose(fp);

    printf("\nBuku '%s' berhasil ditambahkan!\n", b.judul);
    tungguEnter();
}

void lihatDaftarBuku() {
    Buku b;
    int kategoriInt, statusInt;
    FILE *fp;

    bersihkanLayar();
    printf("============================================================================================\n");
    printf("                                   DAFTAR SEMUA BUKU\n");
    printf("============================================================================================\n");
    printf("| %-4s | %-30s | %-20s | %-12s | %-11s |\n", 
           "ID", "Judul", "Penulis", "Kategori", "Status");
    printf("--------------------------------------------------------------------------------------------\n");

    fp = fopen(FILE_BUKU, "r");
    if (fp == NULL) {
        printf("Gagal membuka file buku.\n");
    } else {
        int empty = 1;
        // 1. Baca file baris per baris
        while(fscanf(fp, "%d;%99[^;];%99[^;];%d;%d", 
               &b.idBuku, b.judul, b.penulis, &kategoriInt, &statusInt) == 5) 
        {
            empty = 0;
            b.kategori = (KategoriBuku)kategoriInt;
            b.status = (StatusBuku)statusInt;

            // 2. Tampilkan data
            printf("| %-4d | %-30s | %-20s | %-12s | %-11s |\n",
                   b.idBuku, b.judul, b.penulis, 
                   getKategoriString(b.kategori), 
                   getStatusString(b.status));
        }
        fclose(fp);
        if (empty) {
            printf("|                 Belum ada data buku. Silakan tambahkan terlebih dahulu.                |\n");
        }
    }
    printf("--------------------------------------------------------------------------------------------\n");
    tungguEnter();
}

void hapusBuku() {
    int idHapus, ditemukan = 0;
    Buku b;
    FILE *fp, *temp;

    bersihkanLayar();
    printf("==========================\n");
    printf("        HAPUS BUKU\n");
    printf("==========================\n");
    
    idHapus = inputInt("Masukkan ID Buku yang akan dihapus: ");

    fp = fopen(FILE_BUKU, "r");
    temp = fopen(FILE_TEMP, "w"); // File sementara

    if (fp == NULL || temp == NULL) {
        printf("Error: Gagal membuka file.\n");
        tungguEnter();
        return;
    }

    // Baca file asli, tulis ulang ke file temp KECUALI data yang mau dihapus
    while(fscanf(fp, "%d;%99[^;];%99[^;];%d;%d", 
           &b.idBuku, b.judul, b.penulis, (int*)&b.kategori, (int*)&b.status) == 5)
    {
        if (b.idBuku == idHapus) {
            ditemukan = 1; // Tandai data ditemukan
            // Jangan tulis (skip) data ini
        } else {
            // Tulis data lain ke file temp
            fprintf(temp, "%d;%s;%s;%d;%d\n", 
                    b.idBuku, b.judul, b.penulis, b.kategori, b.status);
        }
    }

    fclose(fp);
    fclose(temp);

    // Hapus file asli
    remove(FILE_BUKU);
    // Ganti nama file temp menjadi nama file asli
    rename(FILE_TEMP, FILE_BUKU);

    if (ditemukan) {
        printf("\nBuku dengan ID %d berhasil dihapus.\n", idHapus);
    } else {
        printf("\nBuku dengan ID %d tidak ditemukan.\n", idHapus);
    }
    tungguEnter();
}

void editBuku() {
    int idEdit, ditemukan = 0;
    Buku b;
    FILE *fp, *temp;

    bersihkanLayar();
    printf("==========================\n");
    printf("     EDIT INFORMASI BUKU\n");
    printf("==========================\n");

    idEdit = inputInt("Masukkan ID Buku yang akan diedit: ");

    fp = fopen(FILE_BUKU, "r");
    temp = fopen(FILE_TEMP, "w");

    if (fp == NULL || temp == NULL) {
        printf("Error: Gagal membuka file.\n");
        tungguEnter();
        return;
    }

    // Baca file asli, tulis ulang ke file temp
    while(fscanf(fp, "%d;%99[^;];%99[^;];%d;%d", 
           &b.idBuku, b.judul, b.penulis, (int*)&b.kategori, (int*)&b.status) == 5)
    {
        if (b.idBuku == idEdit) {
            ditemukan = 1;
            
            // Tampilkan data lama
            printf("\nData Lama (ID: %d):\n", b.idBuku);
            printf("Judul: %s\n", b.judul);
            printf("Penulis: %s\n", b.penulis);
            printf("Kategori: %s\n", getKategoriString(b.kategori));
            
            // Minta input data baru
            printf("\nMasukkan Data Baru:\n");
            inputString("Judul Baru: ", b.judul, 100);
            inputString("Penulis Baru: ", b.penulis, 100);
            b.kategori = inputKategori();
            // Status tidak diubah di sini, tapi di menu transaksi
        }
        
        // Tulis data (yang lama atau yang baru diedit) ke file temp
        fprintf(temp, "%d;%s;%s;%d;%d\n", 
                b.idBuku, b.judul, b.penulis, b.kategori, b.status);
    }

    fclose(fp);
    fclose(temp);

    remove(FILE_BUKU);
    rename(FILE_TEMP, FILE_BUKU);

    if (ditemukan) {
        printf("\nBuku dengan ID %d berhasil diperbarui.\n", idEdit);
    } else {
        printf("\nBuku dengan ID %d tidak ditemukan.\n", idEdit);
    }
    tungguEnter();
}

void cariBuku() {
    int pilihan, ditemukan = 0;
    char keyword[100];
    KategoriBuku katKeyword;
    
    Buku b;
    int kategoriInt, statusInt;
    FILE *fp;

    bersihkanLayar();
    printf("==========================\n");
    printf("        CARI BUKU\n");
    printf("==========================\n");
    printf("Cari berdasarkan:\n");
    printf("1. Judul\n");
    printf("2. Penulis\n");
    printf("3. Kategori\n");
    pilihan = inputInt("Pilihan (1-3): ");

    if (pilihan == 1) {
        inputString("Masukkan judul yang dicari: ", keyword, 100);
    } else if (pilihan == 2) {
        inputString("Masukkan penulis yang dicari: ", keyword, 100);
    } else if (pilihan == 3) {
        katKeyword = inputKategori();
    } else {
        printf("Pilihan tidak valid.\n");
        tungguEnter();
        return;
    }

    // Tampilkan header hasil pencarian
    printf("\n============================================================================================\n");
    printf("                                      HASIL PENCARIAN\n");
    printf("============================================================================================\n");
    printf("| %-4s | %-30s | %-20s | %-12s | %-11s |\n", 
           "ID", "Judul", "Penulis", "Kategori", "Status");
    printf("--------------------------------------------------------------------------------------------\n");

    fp = fopen(FILE_BUKU, "r");
    if (fp == NULL) {
        printf("Gagal membuka file buku.\n");
    } else {
        while(fscanf(fp, "%d;%99[^;];%99[^;];%d;%d", 
               &b.idBuku, b.judul, b.penulis, &kategoriInt, &statusInt) == 5) 
        {
            b.kategori = (KategoriBuku)kategoriInt;
            b.status = (StatusBuku)statusInt;
            
            int cocok = 0;
            if (pilihan == 1 && my_strcasecmp(strstr(b.judul, keyword) ? b.judul : "", keyword) == 0) {
                cocok = 1; // Cari substring judul (case-insensitive)
            } else if (pilihan == 2 && my_strcasecmp(strstr(b.penulis, keyword) ? b.penulis : "", keyword) == 0) {
                cocok = 1; // Cari substring penulis (case-insensitive)
            } else if (pilihan == 3 && b.kategori == katKeyword) {
                cocok = 1; // Cari kategori
            }

            if (cocok) {
                ditemukan = 1;
                printf("| %-4d | %-30s | %-20s | %-12s | %-11s |\n",
                       b.idBuku, b.judul, b.penulis, 
                       getKategoriString(b.kategori), 
                       getStatusString(b.status));
            }
        }
        fclose(fp);
    }

    if (!ditemukan) {
        printf("|                             Tidak ada buku yang cocok dengan kriteria.                         |\n");
    }
    printf("--------------------------------------------------------------------------------------------\n");
    tungguEnter();
}


// Fungsi helper untuk dipanggil oleh modul 'transaksi.c'
int ubahStatusBuku(int idBuku, StatusBuku statusBaru) {
    int ditemukan = 0;
    Buku b;
    FILE *fp, *temp;

    fp = fopen(FILE_BUKU, "r");
    temp = fopen(FILE_TEMP, "w");

    if (fp == NULL || temp == NULL) {
        return 0; // Gagal
    }

    while(fscanf(fp, "%d;%99[^;];%99[^;];%d;%d", 
           &b.idBuku, b.judul, b.penulis, (int*)&b.kategori, (int*)&b.status) == 5)
    {
        if (b.idBuku == idBuku) {
            ditemukan = 1;
            b.status = statusBaru; // Ubah statusnya
        }
        
        // Tulis data ke file temp
        fprintf(temp, "%d;%s;%s;%d;%d\n", 
                b.idBuku, b.judul, b.penulis, b.kategori, b.status);
    }

    fclose(fp);
    fclose(temp);

    remove(FILE_BUKU);
    rename(FILE_TEMP, FILE_BUKU);

    return ditemukan; // 1 jika sukses, 0 jika gagal
}

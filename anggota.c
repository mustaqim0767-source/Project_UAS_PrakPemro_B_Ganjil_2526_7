#include "perpustakaan.h"

void daftarkanAnggota() {
    Anggota a;
    FILE *fp;

    bersihkanLayar();
    printf("==========================\n");
    printf("   DAFTARKAN ANGGOTA BARU\n");
    printf("==========================\n");

    // 1. Dapatkan ID unik baru
    a.idAnggota = getNextId(FILE_ANGGOTA);
    printf("ID Anggota Baru: %d\n", a.idAnggota);

    // 2. Minta input data
    inputString("Nama Lengkap: ", a.nama, 100);
    inputString("Kontak (No. HP/Email): ", a.kontak, 50);

    // 3. Buka file dalam mode "append"
    fp = fopen(FILE_ANGGOTA, "a");
    if (fp == NULL) {
        printf("Error: Gagal membuka file %s\n", FILE_ANGGOTA);
        tungguEnter();
        return;
    }

    // 4. Tulis data baru ke file
    fprintf(fp, "%d;%s;%s\n", a.idAnggota, a.nama, a.kontak);

    // 5. Tutup file
    fclose(fp);

    printf("\nAnggota '%s' berhasil didaftarkan!\n", a.nama);
    tungguEnter();
}

void lihatDaftarAnggota() {
    Anggota a;
    FILE *fp;

    bersihkanLayar();
    printf("=================================================================\n");
    printf("                       DAFTAR SEMUA ANGGOTA\n");
    printf("=================================================================\n");
    printf("| %-6s | %-30s | %-20s |\n", "ID", "Nama Lengkap", "Kontak");
    printf("-----------------------------------------------------------------\n");

    fp = fopen(FILE_ANGGOTA, "r");
    if (fp == NULL) {
        printf("Gagal membuka file anggota.\n");
    } else {
        int empty = 1;
        // 1. Baca file baris per baris
        while(fscanf(fp, "%d;%99[^;];%49[^\n]\n", &a.idAnggota, a.nama, a.kontak) != EOF) {
            empty = 0;
            // 2. Tampilkan data
            printf("| %-6d | %-30s | %-20s |\n", a.idAnggota, a.nama, a.kontak);
        }
        fclose(fp);
        if (empty) {
            printf("|            Belum ada data anggota. Silakan daftarkan.         |\n");
        }
    }
    printf("-----------------------------------------------------------------\n");
    tungguEnter();
}

void lihatRiwayatPeminjaman() {
    int idAnggota, ditemukan = 0;
    char namaAnggota[100];
    Transaksi t;
    long tglPinjamLong, tglKembaliLong;
    char judulBuku[100];
    
    FILE *fp;

    bersihkanLayar();
    printf("==========================\n");
    printf("  RIWAYAT PEMINJAMAN ANGGOTA\n");
    printf("==========================\n");
    
    idAnggota = inputInt("Masukkan ID Anggota: ");

    // Validasi anggota dan dapatkan nama
    if (!isAnggotaValid(idAnggota)) {
        printf("Anggota dengan ID %d tidak ditemukan.\n", idAnggota);
        tungguEnter();
        return;
    }
    getAnggotaNama(idAnggota, namaAnggota);
    
    printf("Menampilkan riwayat untuk: %s (ID: %d)\n\n", namaAnggota, idAnggota);
    printf("| %-4s | %-30s | %-12s | %-12s | %-10s |\n", 
           "ID Trx", "Judul Buku", "Tgl Pinjam", "Tgl Kembali", "Denda (Rp)");
    printf("-----------------------------------------------------------------------------------\n");

    fp = fopen(FILE_TRANSAKSI, "r");
    if (fp == NULL) {
        printf("Gagal membuka file transaksi.\n");
    } else {
        // Format: IDTrx;IDBuku;IDAnggota;TglPinjam;TglKembali;Denda
        while(fscanf(fp, "%d;%d;%d;%ld;%ld;%lf\n", 
               &t.idTransaksi, &t.idBuku, &t.idAnggota, 
               &tglPinjamLong, &tglKembaliLong, &t.denda) != EOF) 
        {
            if (t.idAnggota == idAnggota) {
                ditemukan = 1;
                t.tanggalPinjam = (time_t)tglPinjamLong;
                t.tanggalKembali = (time_t)tglKembaliLong;

                getBukuJudul(t.idBuku, judulBuku);
                
                char strPinjam[11], strKembali[11];
                // Format tanggal
                strftime(strPinjam, sizeof(strPinjam), "%d-%m-%Y", localtime(&t.tanggalPinjam));
                
                if (t.tanggalKembali == 0) {
                    strcpy(strKembali, "Dipinjam");
                } else {
                    strftime(strKembali, sizeof(strKembali), "%d-%m-%Y", localtime(&t.tanggalKembali));
                }

                printf("| %-4d | %-30s | %-12s | %-12s | %-10.0f |\n",
                       t.idTransaksi, judulBuku, strPinjam, strKembali, t.denda);
            }
        }
        fclose(fp);
    }
    
    if (!ditemukan) {
        printf("|                      Anggota ini belum memiliki riwayat peminjaman.                     |\n");
    }
    printf("-----------------------------------------------------------------------------------\n");
    tungguEnter();
}
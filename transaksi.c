#include "perpustakaan.h"

// helper lokal untuk menampilkan transaksi yang masih aktif (belum kembali)
void lihatTransaksiAktif() {
    FILE* fp = fopen(FILE_TRANSAKSI, "r");
    Transaksi t;
    long tglPinjam, tglKembali;
    int ada = 0;
    char namaAnggota[100];
    char judulBuku[100];

    printf("\n--- DAFTAR PEMINJAMAN AKTIF ---\n");
    printf("| %-6s | %-15s | %-25s | %-12s |\n", "ID Trx", "Peminjam", "Judul Buku", "Tgl Pinjam");
    printf("--------------------------------------------------------------------\n");

    if (fp == NULL) {
        printf("| Gagal membaca data transaksi.                                    |\n");
        return;
    }

    while(fscanf(fp, "%d;%d;%d;%ld;%ld;%lf", 
           &t.idTransaksi, &t.idBuku, &t.idAnggota, 
           &tglPinjam, &tglKembali, &t.denda) == 6) 
    {
        if (tglKembali == 0) { // Belum dikembalikan
            ada = 1;
            t.tanggalPinjam = (time_t)tglPinjam;
            getAnggotaNama(t.idAnggota, namaAnggota);
            getBukuJudul(t.idBuku, judulBuku);
            
            char strPinjam[11];
            strftime(strPinjam, sizeof(strPinjam), "%d-%m-%Y", localtime(&t.tanggalPinjam));

            printf("| %-6d | %-15s | %-25s | %-12s |\n", 
                   t.idTransaksi, namaAnggota, judulBuku, strPinjam);
        }
    }
    fclose(fp);
    if (!ada) {
        printf("| Tidak ada peminjaman yang sedang aktif.                          |\n");
    }
    printf("--------------------------------------------------------------------\n");
}


void peminjamanBuku() {
    int idAnggota, idBuku;
    
    bersihkanLayar();
    printf("==========================\n");
    printf("   CATAT PEMINJAMAN BUKU\n");
    printf("==========================\n");

    idAnggota = inputInt("Masukkan ID Anggota Peminjam: ");
    if (!isAnggotaValid(idAnggota)) {
        printf("Error: Anggota dengan ID %d tidak ditemukan.\n", idAnggota);
        tungguEnter();
        return;
    }

    idBuku = inputInt("Masukkan ID Buku yang dipinjam: ");
    if (!isBukuAvailable(idBuku)) {
        printf("Error: Buku dengan ID %d tidak ditemukan atau sedang dipinjam.\n", idBuku);
        tungguEnter();
        return;
    }

    // --- Semua valid, proses transaksi ---

    // 1. ubah status buku menjadi DIPINJAM
    if (!ubahStatusBuku(idBuku, DIPINJAM)) {
        printf("Error: Gagal mengubah status buku.\n");
        tungguEnter();
        return;
    }

    // 2. tambahkan data ke file transaksi
    FILE* fp = fopen(FILE_TRANSAKSI, "a");
    if (fp == NULL) {
        printf("Error: Gagal membuka file transaksi.\n");
        // Kembalikan status buku jika gagal catat
        ubahStatusBuku(idBuku, TERSEDIA); 
        tungguEnter();
        return;
    }

    Transaksi t;
    t.idTransaksi = getNextId(FILE_TRANSAKSI);
    t.idBuku = idBuku;
    t.idAnggota = idAnggota;
    t.tanggalPinjam = time(NULL); // Waktu saat ini
    t.tanggalKembali = 0;         // Belum kembali
    t.denda = 0.0;

    fprintf(fp, "%d;%d;%d;%ld;%ld;%lf\n",
            t.idTransaksi, t.idBuku, t.idAnggota, 
            (long)t.tanggalPinjam, (long)t.tanggalKembali, t.denda);
    
    fclose(fp);

    printf("\nPeminjaman berhasil dicatat (ID Transaksi: %d)\n", t.idTransaksi);
    tungguEnter();
}

void pengembalianBuku() {
    int idTransaksi, ditemukan = 0;
    Transaksi t;
    long tglPinjam, tglKembali;
    FILE *fp, *temp;

    bersihkanLayar();
    printf("==========================\n");
    printf("  CATAT PENGEMBALIAN BUKU\n");
    printf("==========================\n");

    // tampilkan transaksi aktif untuk memudahkan
    lihatTransaksiAktif();

    idTransaksi = inputInt("\nMasukkan ID Transaksi yang dikembalikan: ");

    fp = fopen(FILE_TRANSAKSI, "r");
    temp = fopen(FILE_TEMP, "w");

    if (fp == NULL || temp == NULL) {
        printf("Error: Gagal membuka file.\n");
        tungguEnter();
        return;
    }

    while(fscanf(fp, "%d;%d;%d;%ld;%ld;%lf", 
           &t.idTransaksi, &t.idBuku, &t.idAnggota, 
           &tglPinjam, &tglKembali, &t.denda) == 6) 
    {
        if (t.idTransaksi == idTransaksi && tglKembali == 0) { // Cocok & masih aktif
            ditemukan = 1;
            
            t.tanggalPinjam = (time_t)tglPinjam;
            t.tanggalKembali = time(NULL); // Set waktu kembali
            
            // menghitung denda
            t.denda = perhitunganDendaOtomatis(t.tanggalPinjam, t.tanggalKembali);
            
            // mengubah status buku kembali jadi TERSEDIA
            ubahStatusBuku(t.idBuku, TERSEDIA);
            
            printf("\n--- Detail Pengembalian ---\n");
            if (t.denda > 0) {
                printf("Status: Terlambat\n");
                printf("Total Denda: Rp %.0f\n", t.denda);
            } else {
                printf("Status: Tepat waktu\n");
                printf("Denda: Rp 0\n");
            }

            // tulis data yang sudah diupdate ke temp
            fprintf(temp, "%d;%d;%d;%ld;%ld;%lf\n",
                    t.idTransaksi, t.idBuku, t.idAnggota, 
                    (long)t.tanggalPinjam, (long)t.tanggalKembali, t.denda);
        } else {
            // tulis data lain apa adanya
            fprintf(temp, "%d;%d;%d;%ld;%ld;%lf\n",
                    t.idTransaksi, t.idBuku, t.idAnggota, 
                    tglPinjam, tglKembali, t.denda);
        }
    }

    fclose(fp);
    fclose(temp);

    remove(FILE_TRANSAKSI);
    rename(FILE_TEMP, FILE_TRANSAKSI);

    if (ditemukan) {
        printf("\nPengembalian buku berhasil dicatat.\n");
    } else {
        printf("\nID Transaksi %d tidak ditemukan atau sudah dikembalikan.\n", idTransaksi);
    }
    tungguEnter();
}


double perhitunganDendaOtomatis(time_t tanggalPinjam, time_t tanggalKembali) {
    int durasiMaks = dapatkanDurasiMaksimal(); // Dalam hari
    double selisihDetik = difftime(tanggalKembali, tanggalPinjam);
    
    // konversi selisih detik ke hari
    // tambahkan 1 detik untuk pembulatan ke atas (misal 2.1 hari jadi 3 hari)
    int selisihHari = (selisihDetik / (60 * 60 * 24)) + 1; 

    printf("\nDurasi Pinjam Maks: %d hari\n", durasiMaks);
    printf("Lama Peminjaman: %d hari\n", selisihHari);

    if (selisihHari > durasiMaks) {
        int hariTerlambat = selisihHari - durasiMaks;
        double denda = hariTerlambat * 1000.0; // Misal denda 1000/hari
        printf("Terlambat: %d hari\n", hariTerlambat);
        return denda;
    }
    return 0.0; // Tidak ada denda
}

// struct helper untuk ranking
typedef struct {
    int idBuku;
    int jumlahPinjam;
    char judulBuku[100];
} Ranking;

// fungsi pembanding untuk qsort
int compareRanking(const void *a, const void *b) {
    Ranking *rankA = (Ranking*)a;
    Ranking *rankB = (Ranking*)b;
    return (rankB->jumlahPinjam - rankA->jumlahPinjam); // Descending
}

void tampilkanRankingBuku() {
    Ranking daftarRanking[200]; // asumsi maks 200 buku berbeda
    int count = 0;
    
    FILE* fp = fopen(FILE_TRANSAKSI, "r");
    Transaksi t;
    long tglPinjam, tglKembali;

    bersihkanLayar();
    printf("==========================================\n");
    printf("   RANKING BUKU PALING SERING DIPINJAM\n");
    printf("==========================================\n");

    if (fp == NULL) {
        printf("Belum ada riwayat transaksi.\n");
        tungguEnter();
        return;
    }

    // 1. baca semua transaksi dan hitung frekuensi
    while(fscanf(fp, "%d;%d;%d;%ld;%ld;%lf", 
           &t.idTransaksi, &t.idBuku, &t.idAnggota, 
           &tglPinjam, &tglKembali, &t.denda) == 6) 
    {
        int ditemukan = 0;
        // cek apakah buku sudah ada di daftar ranking
        for (int i = 0; i < count; i++) {
            if (daftarRanking[i].idBuku == t.idBuku) {
                daftarRanking[i].jumlahPinjam++;
                ditemukan = 1;
                break;
            }
        }
        
        // jika buku belum ada, tambahkan baru
        if (!ditemukan && count < 200) {
            daftarRanking[count].idBuku = t.idBuku;
            daftarRanking[count].jumlahPinjam = 1;
            getBukuJudul(t.idBuku, daftarRanking[count].judulBuku);
            count++;
        }
    }
    fclose(fp);

    if (count == 0) {
        printf("Belum ada riwayat transaksi.\n");
        tungguEnter();
        return;
    }

    // 2. urutkan (Sorting) daftar ranking
    qsort(daftarRanking, count, sizeof(Ranking), compareRanking);

    // 3. tampilkan hasil
    printf("| %-5s | %-30s | %-10s |\n", "Rank", "Judul Buku", "Jumlah Pinjam");
    printf("------------------------------------------------------\n");
    
    for (int i = 0; i < count; i++) {
        printf("| %-5d | %-30s | %-13d |\n", 
               i + 1, 
               daftarRanking[i].judulBuku, 
               daftarRanking[i].jumlahPinjam);
    }
    printf("------------------------------------------------------\n");
    tungguEnter();
}

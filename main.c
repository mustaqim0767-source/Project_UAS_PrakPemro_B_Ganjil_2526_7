#include "perpustakaan.h"

void tampilkanMenuUtama() {
    int pilihan = 0;
    while (pilihan != 14) {
        bersihkanLayar();
        printf("==================================\n");
        printf("    SISTEM PERPUSTAKAAN MINI\n");
        printf("==================================\n");
        printf("--- MANAJEMEN BUKU ---\n");
        printf(" 1. Tambah Buku Baru\n");
        printf(" 2. Hapus Buku\n");
        printf(" 3. Edit Informasi Buku\n");
        printf(" 4. Lihat Daftar Buku\n");
        printf(" 5. Cari Buku\n");
        printf("\n--- MANAJEMEN ANGGOTA ---\n");
        printf(" 6. Daftarkan Anggota Baru\n");
        printf(" 7. Lihat Daftar Anggota\n");
        printf(" 8. Lihat Riwayat Peminjaman Anggota\n");
        printf("\n--- TRANSAKSI ---\n");
        printf(" 9. Catat Peminjaman Buku\n");
        printf(" 10. Catat Pengembalian Buku\n");
        printf(" 11. Tampilkan Ranking Buku Terpopuler\n");
        printf("\n--- ADMIN ---\n");
        printf(" 12. Kelola Durasi Peminjaman\n");
        printf(" 13. Logout\n");
        printf(" 14. Keluar Program\n");
        printf("----------------------------------\n");
        
        pilihan = inputInt("Masukkan pilihan Anda: ");

        switch (pilihan) {
            case 1: tambahBuku(); break;
            case 2: hapusBuku(); break;
            case 3: editBuku(); break;
            case 4: lihatDaftarBuku(); break;
            case 5: cariBuku(); break;
            case 6: daftarkanAnggota(); break;
            case 7: lihatDaftarAnggota(); break;
            case 8: lihatRiwayatPeminjaman(); break;
            case 9: peminjamanBuku(); break;
            case 10: pengembalianBuku(); break;
            case 11: tampilkanRankingBuku(); break;
            case 12: kelolaDurasiPeminjaman(); break;
            case 13: 
                printf("Logout berhasil.\n");
                tungguEnter();
                loginPustakawan(); // Kembali ke layar login
                break; 
            case 14: 
                printf("Terima kasih. Keluar dari program...\n"); 
                break;
            default:
                printf("Pilihan tidak valid. Silakan coba lagi.\n");
                tungguEnter();
                break;
        }
    }
}

int main() {
    // 1. Buat file .txt jika belum ada
    setupAwal();

    // 2. Lakukan login pustakawan
    loginPustakawan();

    // 3. Jika login berhasil, tampilkan menu utama
    tampilkanMenuUtama();

    return 0;
}
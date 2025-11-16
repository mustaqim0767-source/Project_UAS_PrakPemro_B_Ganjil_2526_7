#include "perpustakaan.h"

// Fungsi loginPustakawan
void loginPustakawan() {
    char username[50], password[50];
    char line[100]; // Buffer untuk membaca baris
    char *fileUser, *filePass; // Pointer untuk token
    int loginSukses = 0;
    FILE *fp;

    while (!loginSukses) {
        bersihkanLayar();
        printf("==========================\n");
        printf("   LOGIN PUSTAKAWAN\n");
        printf("==========================\n");
        
        // Membersihkan sisa input buffer
        fflush(stdin); 
        inputString("Username: ", username, 50);

        fflush(stdin); // Bersihkan lagi untuk input password
        inputString("Password: ", password, 50);

        if ((fp = fopen(FILE_PUSTAKAWAN, "r")) == NULL) {
            printf("Error: Gagal membuka file login. Hubungi admin.\n");
            tungguEnter();
            exit(1); // Keluar program jika file login error
        }

        loginSukses = 0; // Reset status login untuk setiap loop file
        while (fgets(line, sizeof(line), fp) != NULL) {
            
            // 1. Bersihkan newline/carriage return dari baris
            line[strcspn(line, "\r\n")] = 0;

            // 2. Parse baris menggunakan strtok
            fileUser = strtok(line, ";");
            filePass = strtok(NULL, ";"); // Ambil token kedua (setelah ';')

            // 3. Pastikan kedua token (user & pass) ada
            if (fileUser != NULL && filePass != NULL) {
                // 4. Bandingkan
                if (strcmp(username, fileUser) == 0 && strcmp(password, filePass) == 0) {
                    loginSukses = 1;
                    break; // Keluar dari loop 'while (fgets...)'
                }
            }
        }
        fclose(fp); // Tutup file setelah selesai membaca

        if (loginSukses) {
            printf("\nLogin berhasil! Selamat datang, %s.\n", username);
            tungguEnter();
        } else {
            printf("\nUsername atau password salah. Coba lagi.\n");
            tungguEnter();
        }
    }
}

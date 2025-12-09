#include <iostream>
#include <string.h>
#include <iomanip>
#include <fstream>
using namespace std;

struct List
{
  char *tanggal;
  char *deskripsi;
  char *kategori;
  long long nominal;
  int currency; // 1: IDR, 2:USD
  struct List *next;
};

struct List *pengeluaran = NULL;
long long nilaiKursDollar = 16000; // nilai tukar awal

// ini deklarasi fungsinya, nanti kalian ubah sendiri sesuai kebutuhan
// BUAT TAMPILAN
void showHeader();
void printRupiah(long long amount);
void aturKurs();

// BUAT FILE MANAGEMENT
void loadFile();
void simpanFile();

void tambahData(const char *tgl, const char *desc, const char *kat, long long nominal, int currency);
void tampilkanData();
void editData();
void hapusData();
void sortingData();
void cariData();
void bersihkanMemori();
void tambahPengeluaran();

int main()
{
  loadFile();
  int pilihan;

  do
  {
    showHeader();
    cout << " [1] Tambah Pengeluaran\n";
    cout << " [2] Edit Pengeluaran\n";
    cout << " [3] Hapus Pengeluaran\n";
    cout << " [4] Lihat Tabel & Total\n";
    cout << " [5] Urutkan Data (Sort)\n";
    cout << " [6] Cari Data\n";
    cout << " [7] Simpan & Keluar\n";
    cout << " [8] Atur Kurs Dollar (Saat Ini: Rp " << nilaiKursDollar << ")\n";
    cout << " -----------------------------------\n";
    cout << " Pilih Menu: ";
    cin >> pilihan;
    switch (pilihan)
    {
    case 1:
      tambahPengeluaran();
      break;
    case 2:
      tampilkanData();
      editData();
      break;
    case 3:
      tampilkanData();
      hapusData();
      break;
    case 4:
      tampilkanData();
      break;
    case 5:
      sortingData();
      break;
    case 6:
      cariData();
      break;
    case 7:
      simpanFile();
      cout << " Terima kasih! Sampai jumpa.\n";
      return 0;
    case 8:
      aturKurs();
      break;
    default:
      cout << " Pilihan tidak valid.\n";
    }

    if (pilihan != 4 && pilihan != 6 && pilihan != 7)
    {
      cout << " Tekan Enter...";
      cin.ignore();
      cin.get();
    }

  } while (pilihan != 7);

  return 0;
}

void showHeader()
{
  cout << "=======================================================================================================" << endl;
  cout << R"(
    e   e                 888'Y88                                                   
   d8b d8b    Y8b Y888P   888 ,'Y  Y8b Y8Y 888 88e   ,e e,  888 8e   dP"Y  ,e e,    
  e Y8b Y8b    Y8b Y8P    888C8     Y8b Y  888 888b d88 88b 888 88b C88b  d88 88b   
 d8b Y8b Y8b    Y8b Y     888 ",d  e Y8b   888 888P 888   , 888 888  Y88D 888   ,   
d888b Y8b Y8b    888      888,d88 d8b Y8b  888 88"   "YeeP" 888 888 d,dP   "YeeP"   
                 888                       888                                      
                 888                       888                                      
          88P'888'Y88                         888                                   
          P'  888  'Y 888,8,  ,"Y88b  e88'888 888 ee  ,e e,  888,8,                 
              888     888 "  "8" 888 d888  '8 888 P  d88 88b 888 "                  
              888     888    ,ee 888 Y888   , 888 b  888   , 888                    
              888     888    "88 888  "88,e8' 888 8b  "YeeP" 888                    
                                                                                    
                                                                                    
    )" << endl;
  cout << "======================================================================================================" << endl;
  cout << "                          APLIKASI PENCATAT KEUANGAN MAHASISWA                                      \n";
  cout << "======================================================================================================" << endl;
}

void loadFile()
{
  // TODO: Implement file loading functionality
}

void simpanFile()
{
  if (pengeluaran == NULL)
  {
    cout << "Data kosong, tidak ada yang perlu disimpan.\n";
    return;
  }

  int formatPilihan;
  cout << "\n --- PILIH FORMAT PENYIMPANAN ---\n";
  cout << " [1] File Teks (.txt)\n";
  cout << " [2] File Excel (.csv)\n";
  cout << " [3] SIMPAN DI KEDUANYA\n";
  cout << " [0] Batal\n";
  cout << " --------------------------------\n";
  cout << " Pilihanmu: ";
  cin >> formatPilihan;

  if (formatPilihan == 1 || formatPilihan == 3)
  {
    ofstream fileTxt("database_keuangan.txt");
    if (fileTxt.is_open())
    {
      struct List *temp = pengeluaran;
      while (temp != NULL)
      {
        fileTxt << temp->tanggal << "|"
                << temp->deskripsi << "|"
                << temp->kategori << "|"
                << temp->nominal << "|"
                << temp->currency << endl;
        temp = temp->next;
      }
      fileTxt.close();
      cout << "Data tersimpan di 'database_keuangan.txt'\n";
    }
    else
    {
      cout << "Gagal membuat file .txt\n";
    }
  }

  if (formatPilihan == 2 || formatPilihan == 3)
  {
    ofstream fileCsv("database_keuangan.csv");
    if (fileCsv.is_open())
    {
      fileCsv << "TANGGAL;DESKRIPSI;KATEGORI;NOMINAL;MATA_UANG\n";

      struct List *temp = pengeluaran;
      while (temp != NULL)
      {
        fileCsv << temp->tanggal << ";"
                << temp->deskripsi << ";"
                << temp->kategori << ";"
                << temp->nominal << ";"
                << (temp->currency) << endl;
        temp = temp->next;
      }
      fileCsv.close();
      cout << "Data tersimpan di 'database_keuangan.csv'\n";
    }
    else
    {
      cout << "Gagal membuat file .csv\n";
    }
  }

  if (formatPilihan == 0)
  {
    cout << "Penyimpanan dibatalkan.\n";
  }
  else if (formatPilihan > 3 || formatPilihan < 0)
  {
    cout << " [Error] Pilihan format tidak valid.\n";
  }
}

void tambahData(const char *tgl, const char *desc, const char *kat, long long nominal, int currency)
{
  // TODO: Implement adding data functionality
}

void printRupiah(long long amount)
{
  if (amount < 0)
  {
    cout << "-";
    amount = -amount;
  }
  if (amount < 1000)
  {
    cout << amount;
  }
  else
  {
    printRupiah(amount / 1000);
    cout << "." << setfill('0') << setw(3) << (amount % 1000);
  }
}

void aturKurs()
{
  showHeader();
  cout << "\n --- PENGATURAN KURS DOLLAR --- \n";
  cout << " Kurs saat ini: 1 USD = Rp " << nilaiKursDollar << "\n\n";
  cout << " Masukkan nilai kurs baru: Rp ";
  cin >> nilaiKursDollar;
  cout << " [Sukses] Kurs berhasil diupdate menjadi Rp " << nilaiKursDollar << "!\n";

  cout << "\n Tekan Enter kembali ke menu...";
  cin.ignore();
  cin.get();
}

void tampilkanData()
{
  showHeader();
  if (pengeluaran == NULL)
  {
    cout << "\n\t[ DATA KOSONG ]\n\n";
    return;
  }

  cout << " ----------------------------------------------------------------------------------------- \n";
  cout << "| No | " << left << setw(12) << "TANGGAL"
       << "| " << left << setw(25) << "DESKRIPSI"
       << "| " << left << setw(10) << "KATEGORI"
       << "| " << right << setw(18) << "NOMINAL" << " |\n";
  cout << " ----------------------------------------------------------------------------------------- \n";

  struct List *temp = pengeluaran;
  int no = 1;
  long long totalEstimasiRupiah = 0; // Total gabungan (USD dikonversi ke IDR)

  while (temp != NULL)
  {
    cout << "| " << setw(2) << no << " | "
         << left << setw(12) << temp->tanggal
         << "| " << left << setw(25) << temp->deskripsi
         << "| " << left << setw(10) << temp->kategori
         << "| ";

    // --- LOGIKA MATA UANG ---
    if (temp->currency == 1)
    {
      // Jika Rupiah: Tampil Rata Kanan, Panggil Format Rp
      cout << right << setw(15);
      printRupiah(temp->nominal);
      cout << "   |"; // Spasi penutup

      // Tambah ke total langsung
      totalEstimasiRupiah += temp->nominal;
    }
    else if (temp->currency == 2)
    {
      // Jika Dollar: Tampil "$ 100"
      cout << right << setw(12) << "$ " << temp->nominal << "     |";

      // Tambah ke total (KONVERSI DULU)
      totalEstimasiRupiah += (temp->nominal * nilaiKursDollar);
    }

    cout << "\n";
    temp = temp->next;
    no++;
  }

  cout << " ----------------------------------------------------------------------------------------- \n";
  cout << "  ESTIMASI TOTAL PENGELUARAN (Dalam Rupiah): Rp ";
  printRupiah(totalEstimasiRupiah);
  cout << "\n  (Asumsi Kurs $1 = Rp " << nilaiKursDollar << ")\n";
  cout << " ----------------------------------------------------------------------------------------- \n";

  cout << "\n Tekan Enter...";
  cin.ignore();
  cin.get();
}

void editData()
{
  if (pengeluaran == NULL)
  {
    cout << "\n [Error] Data kosong, tidak ada yang bisa diedit.\n";
    return;
  }

  int index;
  cout << "\n Masukkan nomor data yang ingin diedit: ";
  cin >> index;

  if (index < 1)
  {
    cout << " [Error] Nomor tidak valid.\n";
    return;
  }

  // Cari node berdasarkan index
  struct List *temp = pengeluaran;
  int current = 1;
  while (temp != NULL && current < index)
  {
    temp = temp->next;
    current++;
  }

  if (temp == NULL)
  {
    cout << " [Error] Data dengan nomor " << index << " tidak ditemukan.\n";
    return;
  }

  // Tampilkan data yang akan diedit
  cout << "\n --- DATA YANG AKAN DIEDIT ---\n";
  cout << " Tanggal   : " << temp->tanggal << endl;
  cout << " Deskripsi : " << temp->deskripsi << endl;
  cout << " Kategori  : " << temp->kategori << endl;
  cout << " Nominal   : " << temp->nominal << endl;
  cout << " Mata Uang : " << (temp->currency == 1 ? "IDR" : "USD") << endl;

  // Menu pilihan field yang ingin diedit
  int pilihan;
  cout << "\n --- PILIH FIELD YANG INGIN DIEDIT ---\n";
  cout << " [1] Tanggal\n";
  cout << " [2] Deskripsi\n";
  cout << " [3] Kategori\n";
  cout << " [4] Nominal\n";
  cout << " [5] Mata Uang\n";
  cout << " [6] Edit Semua\n";
  cout << " [0] Batal\n";
  cout << " ------------------------------------\n";
  cout << " Pilihanmu: ";
  cin >> pilihan;
  cin.ignore();

  char buffer[100];

  switch (pilihan)
  {
  case 1:
  {
    cout << " Masukkan tanggal baru (DD/MM/YYYY): ";
    cin.getline(buffer, 100);
    delete[] temp->tanggal;
    temp->tanggal = new char[strlen(buffer) + 1];
    strcpy(temp->tanggal, buffer);
    cout << " [Sukses] Tanggal berhasil diupdate!\n";
    break;
  }
  case 2:
  {
    cout << " Masukkan deskripsi baru: ";
    cin.getline(buffer, 100);
    delete[] temp->deskripsi;
    temp->deskripsi = new char[strlen(buffer) + 1];
    strcpy(temp->deskripsi, buffer);
    cout << " [Sukses] Deskripsi berhasil diupdate!\n";
    break;
  }
  case 3:
  {
    cout << " Masukkan kategori baru: ";
    cin.getline(buffer, 100);
    delete[] temp->kategori;
    temp->kategori = new char[strlen(buffer) + 1];
    strcpy(temp->kategori, buffer);
    cout << " [Sukses] Kategori berhasil diupdate!\n";
    break;
  }
  case 4:
  {
    long long nominalBaru;
    cout << " Masukkan nominal baru: ";
    cin >> nominalBaru;
    temp->nominal = nominalBaru;
    cout << " [Sukses] Nominal berhasil diupdate!\n";
    break;
  }
  case 5:
  {
    int currencyBaru;
    cout << " Pilih mata uang baru [1] IDR [2] USD: ";
    cin >> currencyBaru;
    if (currencyBaru == 1 || currencyBaru == 2)
    {
      temp->currency = currencyBaru;
      cout << " [Sukses] Mata uang berhasil diupdate!\n";
    }
    else
    {
      cout << " [Error] Pilihan mata uang tidak valid.\n";
    }
    break;
  }
  case 6:
  {
    // Edit semua field
    cout << " Masukkan tanggal baru (DD/MM/YYYY): ";
    cin.getline(buffer, 100);
    delete[] temp->tanggal;
    temp->tanggal = new char[strlen(buffer) + 1];
    strcpy(temp->tanggal, buffer);

    cout << " Masukkan deskripsi baru: ";
    cin.getline(buffer, 100);
    delete[] temp->deskripsi;
    temp->deskripsi = new char[strlen(buffer) + 1];
    strcpy(temp->deskripsi, buffer);

    cout << " Masukkan kategori baru: ";
    cin.getline(buffer, 100);
    delete[] temp->kategori;
    temp->kategori = new char[strlen(buffer) + 1];
    strcpy(temp->kategori, buffer);

    long long nominalBaru;
    cout << " Masukkan nominal baru: ";
    cin >> nominalBaru;
    temp->nominal = nominalBaru;

    int currencyBaru;
    cout << " Pilih mata uang [1] IDR [2] USD: ";
    cin >> currencyBaru;
    if (currencyBaru == 1 || currencyBaru == 2)
    {
      temp->currency = currencyBaru;
    }
    else
    {
      cout << " [Warning] Mata uang tidak valid, tetap menggunakan nilai sebelumnya.\n";
    }

    cout << " [Sukses] Semua data berhasil diupdate!\n";
    break;
  }
  case 0:
    cout << " Edit dibatalkan.\n";
    break;
  default:
    cout << " [Error] Pilihan tidak valid.\n";
  }
}

void hapusData()
{
  // TODO: Implement delete data functionality
}

void sortingData()
{
  // TODO: Implement sorting functionality
}

void cariData()
{
  // TODO: Implement search data functionality
}

void bersihkanMemori()
{
  // TODO: Implement memory cleanup functionality
}

//SEBASTIAN
void tambahPengeluaran()
{
  // TODO: Implement add expense functionality
  int tanggal_beli, barang, kategori, nominal;
}

#include <iostream>
#include <string.h>
#include <string>
#include <iomanip>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <cstdio>
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
string printRupiah(long long amount);
void aturKurs();
void printPotong(const char *teks, int maxLebar);

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
void groupingPengeluaranKategori();
void toLowerCase(char *str);
void toProperCase(char *str);
void persentaseExpensePerBulan();

// Fungsi Validasi Input
bool isLeapYear(int year);
int getDaysInMonth(int month, int year);
bool validateTanggal(const char *tgl, bool checkFuture = true);
bool validateDeskripsi(const char *desc);
bool validateKategori(const char *kat);
bool validateNominal(const char *nomStr, long long &nominal);
void getTanggalHariIni(int &hari, int &bulan, int &tahun);
bool inputTanggal(char *tgl, int maxLen);
bool inputDeskripsi(char *desc, int maxLen);
bool inputKategori(char *kat, int maxLen);
bool inputNominal(long long &nominal);
int inputCurrency();

// Fungsi untuk Merge Sort Linked List
struct List *getMiddle(struct List *head);
struct List *mergeSortedByNominal(struct List *left, struct List *right, bool ascending);
struct List *mergeSortedByTanggal(struct List *left, struct List *right, bool ascending);
struct List *mergeSortByNominal(struct List *head, bool ascending);
struct List *mergeSortByTanggal(struct List *head, bool ascending);
int compareTanggal(const char *tgl1, const char *tgl2);

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
    cout << " [7] Simpan\n";
    cout << " [8] Atur Kurs Dollar (Saat Ini: Rp " << nilaiKursDollar << ")\n";
    cout << " [9] Grouping Pengeluaran per Kategori\n";
    cout << " [10] Persentase Expense per Bulan\n";
    cout << " [11] Bersihkan Memori\n";
    cout << " [12] Keluar\n";
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
      break;
    case 8:
      aturKurs();
      break;
    case 9:
      groupingPengeluaranKategori();
      break;
    case 10:
      persentaseExpensePerBulan();
      break;
    case 11:
      bersihkanMemori();
      break;
    case 12:
      cout << "Keluar program\n";
      cout << "Terima kasih telah menggunakan aplikasi ini!\n";
      return 0;

    default:
      cout << " Pilihan tidak valid.\n";
    }

    // if (pilihan != 4 && pilihan != 6 && pilihan != 7 && pilihan != 5 && pilihan != 9 && pilihan != 10)

  } while (pilihan != 12);

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
  ifstream fileCsv("database_keuangan.csv");

  if (fileCsv.is_open())
  {
    char buffer[500];
    fileCsv.getline(buffer, 500);

    while (fileCsv.getline(buffer, 500))
    {
      char *tgl, *desc, *kat, *nomStr, *curStr;

      tgl = strtok(buffer, ";");
      desc = strtok(NULL, ";");
      kat = strtok(NULL, ";");
      nomStr = strtok(NULL, ";");
      curStr = strtok(NULL, ";");

      if (tgl && desc && kat && nomStr && curStr)
      {
        tambahData(tgl, desc, kat, atoll(nomStr), atoi(curStr));
      }
    }
    fileCsv.close();
    cout << " [System] Data berhasil dimuat dari 'database_keuangan.csv'.\n";
    cout << " Tekan Enter...";
    cin.get();
    return;
  }

  ifstream fileTxt("database_keuangan.txt");

  if (fileTxt.is_open())
  {
    char buffer[500];

    while (fileTxt.getline(buffer, 500))
    {
      char *tgl, *desc, *kat, *nomStr, *curStr;

      tgl = strtok(buffer, "|");
      desc = strtok(NULL, "|");
      kat = strtok(NULL, "|");
      nomStr = strtok(NULL, "|");
      curStr = strtok(NULL, "|");

      if (tgl && desc && kat && nomStr && curStr)
      {
        tambahData(tgl, desc, kat, atoll(nomStr), atoi(curStr));
      }
    }
    fileTxt.close();
    cout << " [System] Data berhasil dimuat dari 'database_keuangan.txt'.\n";
    cout << " Tekan Enter...";
    cin.get();
  }
}

void simpanFile()
{
  if (pengeluaran == NULL)
  {
    cout << "Data kosong, tidak ada yang perlu disimpan.\n";
    cout << " Tekan Enter...";
    cin.ignore();
    cin.get();
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
      cout << " Tekan Enter...";
      cin.get();
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
  struct List *nodeBaru = new List;

  nodeBaru->tanggal = new char[strlen(tgl) + 1];
  strcpy(nodeBaru->tanggal, tgl);

  nodeBaru->deskripsi = new char[strlen(desc) + 1];
  strcpy(nodeBaru->deskripsi, desc);

  nodeBaru->kategori = new char[strlen(kat) + 1];
  strcpy(nodeBaru->kategori, kat);

  nodeBaru->nominal = nominal;
  nodeBaru->currency = currency;
  nodeBaru->next = NULL;

  if (pengeluaran == NULL)
  {
    pengeluaran = nodeBaru;
  }
  else
  {
    struct List *temp = pengeluaran;
    while (temp->next != NULL)
    {
      temp = temp->next;
    }
    temp->next = nodeBaru;
  }
}

string printRupiah(long long number)
{
if (number < 0) return "-" + printRupiah(-number);
    
    string numStr = to_string(number);
    string result = "";
    int count = 0;
    
    for (int i = numStr.length() - 1; i >= 0; i--) {
        result = numStr[i] + result;
        count++;
        if (count % 3 == 0 && i > 0) {
            result = "." + result;
        }
    }
    return result;
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
  if (pengeluaran == NULL)
  {
    cout << "\n\t[ DATA KOSONG ]\n\n";
    cout << " Tekan Enter...";
    cin.ignore();
    cin.get();
    return;
  }
  
  cout << setfill(' '); 
  
  cout << " ----------------------------------------------------------------------------------------- \n";
  cout << "| No | " << left << setw(12) << "TANGGAL"
       << "| " << left << setw(25) << "DESKRIPSI"
       << "| " << left << setw(20) << "KATEGORI"
       << "| " << right << setw(19) << "NOMINAL" << " |\n"; 
  cout << " ----------------------------------------------------------------------------------------- \n";

  struct List *temp = pengeluaran;
  int no = 1;
  long long totalEstimasiRupiah = 0;

  while (temp != NULL)
  {
    cout << "| " << setw(2) << no << " | "
         << left << setw(12) << temp->tanggal
         << "| ";
    printPotong(temp->deskripsi, 25);
    cout << "| ";
    printPotong(temp->kategori, 20);
    cout << "| ";

    string nominalStr = printRupiah(temp->nominal);
    
    int maxLebarNominal = 18; 

    if (temp->currency == 1) // IDR
    {
      string idrStr = "Rp " + nominalStr;
      
       if (idrStr.length() > maxLebarNominal) {
           cout << right << setw(maxLebarNominal) << (idrStr.substr(0, maxLebarNominal-3) + "...");
       } else {
           cout << right << setw(maxLebarNominal) << idrStr;
       }
       cout << "  |";
       totalEstimasiRupiah += temp->nominal;
    }
    else if (temp->currency == 2) // USD
    {
       string usdStr = "$ " + nominalStr;
       
       if (usdStr.length() > maxLebarNominal) {
           cout << right << setw(maxLebarNominal) << (usdStr.substr(0, maxLebarNominal-3) + "...");
       } else {
           cout << right << setw(maxLebarNominal) << usdStr;
       }
       cout << "  |"; // Spasi penutup
       totalEstimasiRupiah += (temp->nominal * nilaiKursDollar);
    }

    cout << "\n";
    temp = temp->next;
    no++;
  }

  cout << " ----------------------------------------------------------------------------------------- \n";
  cout << "  ESTIMASI TOTAL PENGELUARAN (Dalam Rupiah): Rp " << printRupiah(totalEstimasiRupiah) << "\n";
  cout << "  (Asumsi Kurs $1 = Rp " << printRupiah(nilaiKursDollar) << ")\n";
  cout << " ----------------------------------------------------------------------------------------- \n";
  
  cout << "\n Tekan Enter...";
  cin.ignore();
  cin.get();
}

void editData()
{
  if (pengeluaran == NULL)
  {
    // cout << "\n [Error] Data kosong, tidak ada yang bisa diedit.\n";
    // cout << " Tekan Enter...";
    // cin.ignore();
    // cin.get();
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

  char buffer[200];
  long long nominalBaru;
  int currencyBaru;

  switch (pilihan)
  {
  case 1:
  {
    // Input tanggal dengan validasi
    int hariIni, bulanIni, tahunIni;
    getTanggalHariIni(hariIni, bulanIni, tahunIni);
    cout << " (Hari ini: " << setfill('0') << setw(2) << hariIni << "/"
         << setw(2) << bulanIni << "/" << tahunIni << ")\n";
    cout << setfill(' ');
    while (true)
    {
      cout << " Masukkan tanggal baru (DD/MM/YYYY): ";
      cin.getline(buffer, 100);
      if (validateTanggal(buffer, true))
      {
        delete[] temp->tanggal;
        temp->tanggal = new char[strlen(buffer) + 1];
        strcpy(temp->tanggal, buffer);
        cout << " [Sukses] Tanggal berhasil diupdate!\n";
        break;
      }
      cout << " Silakan coba lagi.\n";
    }
    break;
  }
  case 2:
  {
    // Input deskripsi dengan validasi
    while (true)
    {
      cout << " Masukkan deskripsi baru: ";
      cin.getline(buffer, 200);
      if (validateDeskripsi(buffer))
      {
        delete[] temp->deskripsi;
        temp->deskripsi = new char[strlen(buffer) + 1];
        strcpy(temp->deskripsi, buffer);
        cout << " [Sukses] Deskripsi berhasil diupdate!\n";
        break;
      }
      cout << " Silakan coba lagi.\n";
    }
    break;
  }
  case 3:
  {
    // Input kategori dengan validasi
    while (true)
    {
      cout << " Masukkan kategori baru: ";
      cin.getline(buffer, 100);
      if (validateKategori(buffer))
      {
        toProperCase(buffer);
        delete[] temp->kategori;
        temp->kategori = new char[strlen(buffer) + 1];
        strcpy(temp->kategori, buffer);
        cout << " [Sukses] Kategori berhasil diupdate!\n";
        break;
      }
      cout << " Silakan coba lagi.\n";
    }
    break;
  }
  case 4:
  {
    // Input nominal dengan validasi
    char nomStr[50];
    while (true)
    {
      cout << " Masukkan nominal baru: ";
      cin.getline(nomStr, 50);
      if (validateNominal(nomStr, nominalBaru))
      {
        temp->nominal = nominalBaru;
        cout << " [Sukses] Nominal berhasil diupdate!\n";
        break;
      }
      cout << " Silakan coba lagi.\n";
    }
    break;
  }
  case 5:
  {
    // Input currency dengan validasi
    char input[10];
    while (true)
    {
      cout << " Pilih mata uang baru [1] IDR [2] USD: ";
      cin.getline(input, 10);
      if (strlen(input) == 1 && (input[0] == '1' || input[0] == '2'))
      {
        temp->currency = input[0] - '0';
        cout << " [Sukses] Mata uang berhasil diupdate!\n";
        break;
      }
      cout << " [Error] Pilihan mata uang tidak valid! Masukkan 1 atau 2.\n";
      cout << " Silakan coba lagi.\n";
    }
    break;
  }
  case 6:
  {
    // Edit semua field dengan validasi

    // Tanggal
    int hariIni, bulanIni, tahunIni;
    getTanggalHariIni(hariIni, bulanIni, tahunIni);
    cout << " (Hari ini: " << setfill('0') << setw(2) << hariIni << "/"
         << setw(2) << bulanIni << "/" << tahunIni << ")\n";
    cout << setfill(' ');
    while (true)
    {
      cout << " Masukkan tanggal baru (DD/MM/YYYY): ";
      cin.getline(buffer, 100);
      if (validateTanggal(buffer, true))
      {
        delete[] temp->tanggal;
        temp->tanggal = new char[strlen(buffer) + 1];
        strcpy(temp->tanggal, buffer);
        break;
      }
      cout << " Silakan coba lagi.\n";
    }

    // Deskripsi
    while (true)
    {
      cout << " Masukkan deskripsi baru: ";
      cin.getline(buffer, 200);
      if (validateDeskripsi(buffer))
      {
        delete[] temp->deskripsi;
        temp->deskripsi = new char[strlen(buffer) + 1];
        strcpy(temp->deskripsi, buffer);
        break;
      }
      cout << " Silakan coba lagi.\n";
    }

    // Kategori
    while (true)
    {
      cout << " Masukkan kategori baru: ";
      cin.getline(buffer, 100);
      if (validateKategori(buffer))
      {
        toProperCase(buffer);
        delete[] temp->kategori;
        temp->kategori = new char[strlen(buffer) + 1];
        strcpy(temp->kategori, buffer);
        break;
      }
      cout << " Silakan coba lagi.\n";
    }

    // Nominal
    char nomStr[50];
    while (true)
    {
      cout << " Masukkan nominal baru: ";
      cin.getline(nomStr, 50);
      if (validateNominal(nomStr, nominalBaru))
      {
        temp->nominal = nominalBaru;
        break;
      }
      cout << " Silakan coba lagi.\n";
    }

    // Currency
    char input[10];
    while (true)
    {
      cout << " Pilih mata uang [1] IDR [2] USD: ";
      cin.getline(input, 10);
      if (strlen(input) == 1 && (input[0] == '1' || input[0] == '2'))
      {
        temp->currency = input[0] - '0';
        break;
      }
      cout << " [Error] Pilihan mata uang tidak valid! Masukkan 1 atau 2.\n";
      cout << " Silakan coba lagi.\n";
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
  if (pengeluaran == NULL)
  {
    // cout << "\n[DATA KOSONG] Tidak ada data yang bisa dihapus.\n";
    // cout << " Tekan Enter...";
    // cin.ignore();
    // cin.get();
    return;
  }

  int nomorHapus;
  cout << "\n Masukkan nomor data yang ingin dihapus: ";
  cin >> nomorHapus;

  struct List *temp = pengeluaran;
  struct List *prev = NULL;

  int currentIndex = 1;
  while (temp != NULL && currentIndex < nomorHapus)
  {
    prev = temp;
    temp = temp->next;
    currentIndex++;
  }

  if (temp == NULL)
  {
    cout << "[Error] Nomor data tidak ditemukan.\n";
    return;
  }

  // Kalau yang dihapus HEAD
  if (prev == NULL)
  {
    pengeluaran = temp->next;
  }
  else
  {
    prev->next = temp->next;
  }

  delete[] temp->tanggal;
  delete[] temp->deskripsi;
  delete[] temp->kategori;
  delete temp;

  cout << "[Sukses] Data nomor " << nomorHapus << " telah dihapus.\n";
}

void sortingData()
{
  if (pengeluaran == NULL)
  {
    cout << "\n\t[ DATA KOSONG ]\n\n";
    cout << " Tekan Enter...";
    cin.ignore();
    cin.get();
    return;
  }

  showHeader();
  int pilihan;
  cout << "\n --- SORTING PENGELUARAN ---\n";
  cout << " [1] Sort berdasarkan Nominal\n";
  cout << " [2] Sort berdasarkan Tanggal\n";
  cout << " [0] Batal\n";
  cout << " ---------------------------\n";
  cout << " Pilihanmu: ";
  cin >> pilihan;

  if (pilihan == 0)
  {
    cout << " Sorting dibatalkan.\n";
    return;
  }

  if (pilihan != 1 && pilihan != 2)
  {
    cout << " [Error] Pilihan tidak valid.\n";
    return;
  }

  int urutan;
  cout << "\n --- PILIH URUTAN ---\n";
  cout << " [1] Ascending (Kecil ke Besar)\n";
  cout << " [2] Descending (Besar ke Kecil)\n";
  cout << " Pilihanmu: ";
  cin >> urutan;

  bool ascending = (urutan == 1);

  if (pilihan == 1)
  {
    // Sort by Nominal
    pengeluaran = mergeSortByNominal(pengeluaran, ascending);
    cout << "\n [Sukses] Data berhasil diurutkan berdasarkan Nominal!\n";
  }
  else if (pilihan == 2)
  {
    // Sort by Tanggal
    pengeluaran = mergeSortByTanggal(pengeluaran, ascending);
    cout << "\n [Sukses] Data berhasil diurutkan berdasarkan Tanggal!\n";
  }

  // Tampilkan hasil sorting
  tampilkanData();
}

// Fungsi untuk mendapatkan node tengah dari linked list
struct List *getMiddle(struct List *head)
{
  if (head == NULL)
    return head;

  struct List *slow = head;
  struct List *fast = head->next;

  while (fast != NULL)
  {
    fast = fast->next;
    if (fast != NULL)
    {
      slow = slow->next;
      fast = fast->next;
    }
  }
  return slow;
}

// Fungsi untuk membandingkan tanggal (format DD/MM/YYYY)
// Return: negatif jika tgl1 < tgl2, 0 jika sama, positif jika tgl1 > tgl2
int compareTanggal(const char *tgl1, const char *tgl2)
{
  // Parse tanggal 1
  int day1, month1, year1;
  sscanf(tgl1, "%d/%d/%d", &day1, &month1, &year1);

  // Parse tanggal 2
  int day2, month2, year2;
  sscanf(tgl2, "%d/%d/%d", &day2, &month2, &year2);

  // Bandingkan tahun dulu
  if (year1 != year2)
    return year1 - year2;
  // Lalu bulan
  if (month1 != month2)
    return month1 - month2;
  // Terakhir hari
  return day1 - day2;
}

// Merge dua linked list yang sudah terurut berdasarkan Nominal
struct List *mergeSortedByNominal(struct List *left, struct List *right, bool ascending)
{
  if (left == NULL)
    return right;
  if (right == NULL)
    return left;

  struct List *result = NULL;

  // Konversi ke nilai yang sama (IDR) untuk perbandingan
  long long leftValue = (left->currency == 1) ? left->nominal : left->nominal * nilaiKursDollar;
  long long rightValue = (right->currency == 1) ? right->nominal : right->nominal * nilaiKursDollar;

  bool condition;
  if (ascending)
  {
    condition = (leftValue <= rightValue);
  }
  else
  {
    condition = (leftValue >= rightValue);
  }

  if (condition)
  {
    result = left;
    result->next = mergeSortedByNominal(left->next, right, ascending);
  }
  else
  {
    result = right;
    result->next = mergeSortedByNominal(left, right->next, ascending);
  }

  return result;
}

// Merge dua linked list yang sudah terurut berdasarkan Tanggal
struct List *mergeSortedByTanggal(struct List *left, struct List *right, bool ascending)
{
  if (left == NULL)
    return right;
  if (right == NULL)
    return left;

  struct List *result = NULL;

  int cmp = compareTanggal(left->tanggal, right->tanggal);

  bool condition;
  if (ascending)
  {
    condition = (cmp <= 0);
  }
  else
  {
    condition = (cmp >= 0);
  }

  if (condition)
  {
    result = left;
    result->next = mergeSortedByTanggal(left->next, right, ascending);
  }
  else
  {
    result = right;
    result->next = mergeSortedByTanggal(left, right->next, ascending);
  }

  return result;
}

// Merge Sort untuk Nominal
struct List *mergeSortByNominal(struct List *head, bool ascending)
{
  // Base case: jika list kosong atau hanya 1 node
  if (head == NULL || head->next == NULL)
  {
    return head;
  }

  // Cari node tengah
  struct List *middle = getMiddle(head);
  struct List *nextOfMiddle = middle->next;

  // Putus linked list menjadi dua bagian
  middle->next = NULL;

  // Rekursif sort kedua bagian
  struct List *left = mergeSortByNominal(head, ascending);
  struct List *right = mergeSortByNominal(nextOfMiddle, ascending);

  // Merge kedua bagian yang sudah terurut
  return mergeSortedByNominal(left, right, ascending);
}

// Merge Sort untuk Tanggal
struct List *mergeSortByTanggal(struct List *head, bool ascending)
{
  // Base case: jika list kosong atau hanya 1 node
  if (head == NULL || head->next == NULL)
  {
    return head;
  }

  // Cari node tengah
  struct List *middle = getMiddle(head);
  struct List *nextOfMiddle = middle->next;

  // Putus linked list menjadi dua bagian
  middle->next = NULL;

  // Rekursif sort kedua bagian
  struct List *left = mergeSortByTanggal(head, ascending);
  struct List *right = mergeSortByTanggal(nextOfMiddle, ascending);

  // Merge kedua bagian yang sudah terurut
  return mergeSortedByTanggal(left, right, ascending);
}

void cariData()
{
  if (pengeluaran == NULL)
  {
    cout << "\n [DATA KOSONG] Tidak ada yang bisa dicari.\n";
    cout << " Tekan Enter...";
    cin.ignore();
    cin.get();
    return;
  }

  cin.ignore(); // bersihkan buffer input

  int pilihan;
  char keyword[200];

  cout << "\n --- MENU PENCARIAN DATA ---\n";
  cout << " [1] Cari berdasarkan Deskripsi\n";
  cout << " [2] Cari berdasarkan Kategori\n";
  cout << " [0] Batal\n";
  cout << " --------------------------------\n";
  cout << " Pilihan: ";
  cin >> pilihan;

  if (pilihan == 0)
  {
    cout << " Pencarian dibatalkan.\n";
    return;
  }

  cin.ignore();

  long long cariNominal = 0;

  if (pilihan == 1 || pilihan == 2)
  {
    cout << " Masukkan kata kunci: ";
    cin.getline(keyword, 200);

    toLowerCase(keyword);
  }
  else
  {
    cout << " [Error] Pilihan tidak valid.\n";
    return;
  }

  cout << "\n --- HASIL PENCARIAN ---\n";

  cout << " ----------------------------------------------------------------------------------------- \n";
  cout << "| No | " << left << setw(12) << "TANGGAL"
       << "| " << left << setw(25) << "DESKRIPSI"
       << "| " << left << setw(10) << "KATEGORI"
       << "| " << right << setw(18) << "NOMINAL" << " |\n";
  cout << " ----------------------------------------------------------------------------------------- \n";

  struct List *temp = pengeluaran;
  int no = 1;
  bool ditemukan = false;

  while (temp != NULL)
  {
    bool match = false;

    // buffer lowercase
    char lowTanggal[200], lowDeskripsi[200], lowKategori[200];

    strcpy(lowTanggal, temp->tanggal);
    strcpy(lowDeskripsi, temp->deskripsi);
    strcpy(lowKategori, temp->kategori);

    toLowerCase(lowTanggal);
    toLowerCase(lowDeskripsi);
    toLowerCase(lowKategori);

    // cek sesuai pilihan pencarian
    if (pilihan == 1 && strstr(lowDeskripsi, keyword))
      match = true;
    if (pilihan == 2 && strstr(lowKategori, keyword))
      match = true;
    
    if (match)
    {
      ditemukan = true;

      cout << "| " << setw(2) << no << " | "
           << left << setw(12) << temp->tanggal
           << "| " << left << setw(25) << temp->deskripsi
           << "| " << left << setw(10) << temp->kategori
           << "| ";

      if (temp->currency == 1)
      {
        cout << right << setw(15);
        printRupiah(temp->nominal);
        cout << "   |";
      }
      else
      {
        cout << right << setw(12) << "$ " << temp->nominal << "     |";
      }

      cout << "\n";
    }

    temp = temp->next;
    no++;
  }

  cout << " ----------------------------------------------------------------------------------------- \n";

  if (!ditemukan)
  {
    cout << "\n   [Tidak ada data yang cocok dengan pencarian]\n\n";
  }

  cout << " Tekan Enter...";
  cin.get();
}

void bersihkanMemori()
{
  // 1. Cek dulu apakah data memang ada? Kalau kosong, tidak perlu drama
  if (pengeluaran == NULL)
  {
    cout << "\n [Info] Memori sudah bersih (Data Kosong).\n";
    cout << " Tekan Enter...";
    cin.ignore();
    cin.get();
    return;
  }

  // 2. Tampilkan PERINGATAN KERAS
  showHeader(); // Opsional, biar header tetap ada
  cout << "\n ======================================================== \n";
  cout << "                      !!! PERINGATAN !!!                  \n";
  cout << " ======================================================== \n";

  cout << " Apakah Anda yakin ingin melanjutkan? (y/n): ";

  char konfirmasi;
  cin >> konfirmasi;

  if (konfirmasi == 'y' || konfirmasi == 'Y')
  {
    struct List *current = pengeluaran;
    struct List *next = NULL;
    int jumlahDihapus = 0;

    while (current != NULL)
    {
      next = current->next;

      if (current->tanggal != NULL) delete[] current->tanggal;
      if (current->deskripsi != NULL) delete[] current->deskripsi;
      if (current->kategori != NULL) delete[] current->kategori;

      delete current;

      current = next;
      jumlahDihapus++;
    }

    pengeluaran = NULL;
    
    cout << "\n [SUKSES] Memori telah dibersihkan.\n";
    cout << "          " << jumlahDihapus << " data berhasil dihapus dari memori.\n";
  }
  else
  {
    cout << "\n [BATAL] Operasi dibatalkan. Data Anda masih aman.\n";
  }

  cout << "\n Tekan Enter kembali ke menu...";
  cin.ignore();
  cin.get();
}

// ==================== FUNGSI VALIDASI INPUT ====================

// Cek apakah tahun kabisat
bool isLeapYear(int year)
{
  return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

// Mendapatkan jumlah hari dalam bulan tertentu
int getDaysInMonth(int month, int year)
{
  int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
  if (month == 2 && isLeapYear(year))
  {
    return 29;
  }
  return daysInMonth[month - 1];
}

// Mendapatkan tanggal hari ini
void getTanggalHariIni(int &hari, int &bulan, int &tahun)
{
  time_t now = time(0);
  tm *ltm = localtime(&now);
  hari = ltm->tm_mday;
  bulan = ltm->tm_mon + 1;
  tahun = 1900 + ltm->tm_year;
}

// Validasi format dan nilai tanggal
// checkFuture = true -> tidak boleh lebih dari hari ini
bool validateTanggal(const char *tgl, bool checkFuture)
{
  if (tgl == NULL || strlen(tgl) == 0)
  {
    cout << " [Error] Tanggal tidak boleh kosong!\n";
    return false;
  }

  int day, month, year;
  int count = sscanf(tgl, "%d/%d/%d", &day, &month, &year);

  // Cek apakah format valid (harus ada 3 angka)
  if (count != 3)
  {
    cout << " [Error] Format tanggal tidak valid! Gunakan format DD/MM/YYYY\n";
    return false;
  }

  // Validasi tahun (minimal 2000, maksimal tahun sekarang + 1)
  int hariIni, bulanIni, tahunIni;
  getTanggalHariIni(hariIni, bulanIni, tahunIni);

  if (year < 2000 || year > tahunIni + 1)
  {
    cout << " [Error] Tahun tidak valid! (2000 - " << tahunIni << ")\n";
    return false;
  }

  // Validasi bulan
  if (month < 1 || month > 12)
  {
    cout << " [Error] Bulan tidak valid! (1-12)\n";
    return false;
  }

  // Validasi hari berdasarkan bulan
  int maxDay = getDaysInMonth(month, year);
  if (day < 1 || day > maxDay)
  {
    cout << " [Error] Hari tidak valid untuk bulan " << month << "! (1-" << maxDay << ")\n";
    return false;
  }

  // Cek apakah tanggal di masa depan
  if (checkFuture)
  {
    if (year > tahunIni ||
        (year == tahunIni && month > bulanIni) ||
        (year == tahunIni && month == bulanIni && day > hariIni))
    {
      cout << " [Error] Tanggal tidak boleh lebih dari hari ini!\n";
      return false;
    }
  }

  return true;
}

// Validasi deskripsi
bool validateDeskripsi(const char *desc)
{
  if (desc == NULL || strlen(desc) == 0)
  {
    cout << " [Error] Deskripsi tidak boleh kosong!\n";
    return false;
  }
  if (strlen(desc) < 3)
  {
    cout << " [Error] Deskripsi minimal 3 karakter!\n";
    return false;
  }
  if (strlen(desc) > 100)
  {
    cout << " [Error] Deskripsi maksimal 100 karakter!\n";
    return false;
  }
  return true;
}

// Validasi kategori
bool validateKategori(const char *kat)
{
  if (kat == NULL || strlen(kat) == 0)
  {
    cout << " [Error] Kategori tidak boleh kosong!\n";
    return false;
  }
  if (strlen(kat) < 2)
  {
    cout << " [Error] Kategori minimal 2 karakter!\n";
    return false;
  }
  if (strlen(kat) > 50)
  {
    cout << " [Error] Kategori maksimal 50 karakter!\n";
    return false;
  }
  return true;
}

// Validasi nominal dari string
bool validateNominal(const char *nomStr, long long &nominal)
{
  if (nomStr == NULL || strlen(nomStr) == 0)
  {
    cout << " [Error] Nominal tidak boleh kosong!\n";
    return false;
  }

  // Cek apakah semua karakter adalah digit
  for (int i = 0; nomStr[i]; i++)
  {
    if (nomStr[i] < '0' || nomStr[i] > '9')
    {
      cout << " [Error] Nominal harus berupa angka positif!\n";
      return false;
    }
  }

  nominal = atoll(nomStr);

  if (nominal <= 0)
  {
    cout << " [Error] Nominal harus lebih dari 0!\n";
    return false;
  }

  if (nominal > 999999999999LL)
  {
    cout << " [Error] Nominal terlalu besar! Maksimal 999.999.999.999\n";
    return false;
  }

  return true;
}

// Input tanggal dengan validasi
bool inputTanggal(char *tgl, int maxLen)
{
  int hariIni, bulanIni, tahunIni;
  getTanggalHariIni(hariIni, bulanIni, tahunIni);

  cout << " (Hari ini: " << setfill('0') << setw(2) << hariIni << "/"
       << setw(2) << bulanIni << "/" << tahunIni << ")\n";

  while (true)
  {
    cout << " Tanggal (DD/MM/YYYY)  : ";
    cin.getline(tgl, maxLen);

    if (validateTanggal(tgl, true))
    {
      return true;
    }
    cout << " Silakan coba lagi.\n";
  }
}

// Input deskripsi dengan validasi
bool inputDeskripsi(char *desc, int maxLen)
{
  while (true)
  {
    cout << " Deskripsi             : ";
    cin.getline(desc, maxLen);

    if (validateDeskripsi(desc))
    {
      return true;
    }
    cout << " Silakan coba lagi.\n";
  }
}

// Input kategori dengan validasi
bool inputKategori(char *kat, int maxLen)
{
  while (true)
  {
    cout << " Kategori              : ";
    cin.getline(kat, maxLen);

    if (validateKategori(kat))
    {
      toProperCase(kat);
      return true;
    }
    cout << " Silakan coba lagi.\n";
  }
}

// Input nominal dengan validasi
bool inputNominal(long long &nominal)
{
  char nomStr[50];
  while (true)
  {
    cout << " Nominal               : ";
    cin.getline(nomStr, 50);

    if (validateNominal(nomStr, nominal))
    {
      return true;
    }
    cout << " Silakan coba lagi.\n";
  }
}

// Input currency dengan validasi
int inputCurrency()
{
  char input[10];
  int currency;
  while (true)
  {
    cout << " Mata Uang [1] IDR [2] USD: ";
    cin.getline(input, 10);

    if (strlen(input) == 1 && (input[0] == '1' || input[0] == '2'))
    {
      currency = input[0] - '0';
      return currency;
    }
    cout << " [Error] Pilihan mata uang tidak valid! Masukkan 1 atau 2.\n";
    cout << " Silakan coba lagi.\n";
  }
}

// ==================== END FUNGSI VALIDASI ====================

void tambahPengeluaran()
{
  showHeader();
  cin.ignore(); // bersihkan buffer agar getline tidak ter-skip

  char tgl[50], desc[200], kat[100];
  long long nominal;
  int currency;

  cout << "\n --- TAMBAH PENGELUARAN ---\n";

  // Input dengan validasi
  inputTanggal(tgl, 50);
  inputDeskripsi(desc, 200);
  inputKategori(kat, 100);
  inputNominal(nominal);
  currency = inputCurrency();

  // Panggil fungsi penambah node
  tambahData(tgl, desc, kat, nominal, currency);

  cout << "\n [Sukses] Pengeluaran berhasil ditambahkan!\n";
    cout << " Tekan Enter...";
    cin.get();
}

void toLowerCase(char *str)
{
  for (int i = 0; str[i]; i++)
  {
    if (str[i] >= 'A' && str[i] <= 'Z')
    {
      str[i] += 32; // jadi huruf kecil
    }
  }
}

void toProperCase(char *str)
{
  bool startWord = true;
  for (int i = 0; str[i]; i++)
  {
    char c = str[i];

    if (c == ' ' || c == '\t')
    {
      startWord = true;
    }
    else
    {
      if (startWord)
      {
        // huruf pertama kata → BESAR
        if (c >= 'a' && c <= 'z')
        {
          str[i] = c - 32;
        }
        startWord = false;
      }
      else
      {
        // huruf setelahnya → kecil
        if (c >= 'A' && c <= 'Z')
        {
          str[i] = c + 32;
        }
      }
    }
  }
}

void groupingPengeluaranKategori()
{
  showHeader();
  if (pengeluaran == NULL)
  {
    cout << "\n\t[ DATA KOSONG ]\n\n";
    cout << " Tekan Enter...";
    cin.ignore();
    cin.get();
    return;
  }

  char kategoriList[50][100];
  long long totalKategori[50] = {0};
  int jumlahKategori = 0;
  struct List *temp = pengeluaran;

  while (temp != NULL)
  {
    long long NominalInIDR = (temp->currency == 1) ? temp->nominal : temp->nominal * nilaiKursDollar;
    bool found = false;
    for (int i = 0; i < jumlahKategori; i++)
    {
      if (strcmp(kategoriList[i], temp->kategori) == 0)
      {
        totalKategori[i] += NominalInIDR;
        found = true;
        break;
      }
    }
    if (!found)
    {
      strcpy(kategoriList[jumlahKategori], temp->kategori);
      totalKategori[jumlahKategori] = NominalInIDR;
      jumlahKategori++;
    }
    temp = temp->next;
  }
  cout << "\n--- TOTAL PENGELUARAN PER KATEGORI ---\n";
  cout << "Semua nominal ditampilkan dalam Rupiah (IDR)\n\n";

  for (int i = 0; i < jumlahKategori; i++)
  {
    cout << left << setw(20) << kategoriList[i] << ": Rp ";
    printRupiah(totalKategori[i]);
    cout << "\n";
  }
  cout << "\nKurs yang digunakan: $1 = Rp " << nilaiKursDollar << endl;
  cout << "---------------------------------\n";
   cout << " Tekan Enter...";
      cin.ignore();
      cin.get();
}

void printPotong(const char *teks, int maxLebar)
{
  int panjang = strlen(teks);

  if (panjang <= maxLebar)
  {
    cout << left << setw(maxLebar) << teks;
  }
  else
  {
    for (int i = 0; i < maxLebar - 3; i++)
    {
      cout << teks[i];
    }
    cout << "...";
  }
}

// Fungsi untuk mendapatkan nama bulan dari nomor bulan
const char *getNamaBulan(int bulan)
{
  const char *namaBulan[] = {
      "Januari", "Februari", "Maret", "April", "Mei", "Juni",
      "Juli", "Agustus", "September", "Oktober", "November", "Desember"};
  if (bulan >= 1 && bulan <= 12)
  {
    return namaBulan[bulan - 1];
  }
  return "Unknown";
}

void persentaseExpensePerBulan()
{
  showHeader();
  if (pengeluaran == NULL)
  {
    cout << "\n\t[ DATA KOSONG ]\n\n";
    cout << " Tekan Enter...";
    cin.ignore();
    cin.get();
    return;
  }

  // Struktur untuk menyimpan data per bulan
  // Format: bulanTahun[i] = bulan * 10000 + tahun (misal: Januari 2025 = 12025)
  int bulanTahunList[100]; // Daftar kombinasi bulan-tahun unik
  int jumlahBulanTahun = 0;

  // Kumpulkan semua bulan-tahun unik
  struct List *temp = pengeluaran;
  while (temp != NULL)
  {
    int day, month, year;
    sscanf(temp->tanggal, "%d/%d/%d", &day, &month, &year);
    int bulanTahun = month * 10000 + year;

    // Cek apakah sudah ada di list
    bool found = false;
    for (int i = 0; i < jumlahBulanTahun; i++)
    {
      if (bulanTahunList[i] == bulanTahun)
      {
        found = true;
        break;
      }
    }
    if (!found)
    {
      bulanTahunList[jumlahBulanTahun++] = bulanTahun;
    }
    temp = temp->next;
  }

  // Sort bulanTahunList (bubble sort sederhana)
  for (int i = 0; i < jumlahBulanTahun - 1; i++)
  {
    for (int j = 0; j < jumlahBulanTahun - i - 1; j++)
    {
      int year1 = bulanTahunList[j] % 10000;
      int month1 = bulanTahunList[j] / 10000;
      int year2 = bulanTahunList[j + 1] % 10000;
      int month2 = bulanTahunList[j + 1] / 10000;

      if (year1 > year2 || (year1 == year2 && month1 > month2))
      {
        int tempVal = bulanTahunList[j];
        bulanTahunList[j] = bulanTahunList[j + 1];
        bulanTahunList[j + 1] = tempVal;
      }
    }
  }

  cout << "\n============================================" << endl;
  cout << "   PERSENTASE EXPENSE PER BULAN & KATEGORI" << endl;
  cout << "============================================" << endl;
  cout << "(Semua nominal dalam Rupiah, Kurs $1 = Rp " << nilaiKursDollar << ")\n"
       << endl;

  // Untuk setiap bulan-tahun, hitung persentase per kategori
  for (int bt = 0; bt < jumlahBulanTahun; bt++)
  {
    int bulan = bulanTahunList[bt] / 10000;
    int tahun = bulanTahunList[bt] % 10000;

    // Hitung total dan per kategori untuk bulan ini
    char kategoriList[50][100];
    long long totalKategori[50] = {0};
    int jumlahKategori = 0;
    long long totalBulan = 0;

    temp = pengeluaran;
    while (temp != NULL)
    {
      int day, month, year;
      sscanf(temp->tanggal, "%d/%d/%d", &day, &month, &year);

      if (month == bulan && year == tahun)
      {
        long long nominalIDR = (temp->currency == 1) ? temp->nominal : temp->nominal * nilaiKursDollar;
        totalBulan += nominalIDR;

        // Cari atau tambah kategori
        bool found = false;
        for (int i = 0; i < jumlahKategori; i++)
        {
          if (strcmp(kategoriList[i], temp->kategori) == 0)
          {
            totalKategori[i] += nominalIDR;
            found = true;
            break;
          }
        }
        if (!found)
        {
          strcpy(kategoriList[jumlahKategori], temp->kategori);
          totalKategori[jumlahKategori] = nominalIDR;
          jumlahKategori++;
        }
      }
      temp = temp->next;
    }

    // Tampilkan hasil untuk bulan ini
    cout << "--------------------------------------------" << endl;
    cout << " " << getNamaBulan(bulan) << " " << tahun << endl;
    cout << "--------------------------------------------" << endl;
    cout << " Total: Rp ";
    printRupiah(totalBulan);
    cout << "\n"
         << endl;

    // Sort kategori berdasarkan nominal (descending) untuk tampilan
    for (int i = 0; i < jumlahKategori - 1; i++)
    {
      for (int j = 0; j < jumlahKategori - i - 1; j++)
      {
        if (totalKategori[j] < totalKategori[j + 1])
        {
          // Swap nominal
          long long tempNom = totalKategori[j];
          totalKategori[j] = totalKategori[j + 1];
          totalKategori[j + 1] = tempNom;
          // Swap kategori
          char tempKat[100];
          strcpy(tempKat, kategoriList[j]);
          strcpy(kategoriList[j], kategoriList[j + 1]);
          strcpy(kategoriList[j + 1], tempKat);
        }
      }
    }

    // Tampilkan persentase per kategori
    cout << left << setw(20) << " Kategori" << right << setw(12) << "Nominal" << setw(10) << "Persen" << endl;
    cout << " " << string(40, '-') << endl;

    for (int i = 0; i < jumlahKategori; i++)
    {
      double persen = (totalBulan > 0) ? ((double)totalKategori[i] / totalBulan) * 100.0 : 0.0;

      cout << " " << left << setw(19) << kategoriList[i];
      cout << "Rp ";
      // Tampilkan nominal dengan format
      cout << right << setw(9);
      printRupiah(totalKategori[i]);
      cout << fixed << setprecision(1) << setw(8) << persen << "%";

      // Tampilkan bar visual
      cout << "  [";
      int barLen = (int)(persen / 5); // Setiap 5% = 1 karakter
      for (int b = 0; b < barLen; b++)
        cout << "#";
      for (int b = barLen; b < 20; b++)
        cout << " ";
      cout << "]" << endl;
    }
    cout << endl;
  }
  cout << "============================================" << endl;
   cout << " Tekan Enter...";
      cin.ignore();
      cin.get();
}
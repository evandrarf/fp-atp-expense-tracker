#include <iostream>
#include <string.h>
#include <iomanip>
#include <fstream>
#include <cstdlib>
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
    cout << " [7] Simpan & Keluar\n";
    cout << " [8] Atur Kurs Dollar (Saat Ini: Rp " << nilaiKursDollar << ")\n";
    cout << " [9] Grouping Pengeluaran per Kategori\n";
    cout << " [10] Persentase Expense per Bulan\n";
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
    case 9:
      groupingPengeluaranKategori();
      break;
    case 10:
      persentaseExpensePerBulan();
      break;
    default:
      cout << " Pilihan tidak valid.\n";
    }

    if (pilihan != 4 && pilihan != 6 && pilihan != 7 && pilihan != 5)
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
  }
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
    cout << setfill(' ');
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
  if (pengeluaran == NULL)
  {
    cout << "\n\t[ DATA KOSONG ]\n\n";
    cout << " Tekan Enter...";
    cin.ignore();
    cin.get();
    return;
  }

  cout << " ----------------------------------------------------------------------------------------- \n";
  cout << "| No | " << left << setw(12) << "TANGGAL"
       << "| " << left << setw(25) << "DESKRIPSI"
       << "| " << left << setw(20) << "KATEGORI"
       << "| " << right << setw(20) << "NOMINAL" << " |\n";
  cout << " ----------------------------------------------------------------------------------------- \n";

  struct List *temp = pengeluaran;
  int no = 1;
  long long totalEstimasiRupiah = 0; // Total gabungan (USD dikonversi ke IDR)

  while (temp != NULL)
  {
    cout << "| " << setw(2) << no << " | "
         << left << setw(12) << temp->tanggal
         << "| ";
    printPotong(temp->deskripsi, 25);
    cout << "| ";
    printPotong(temp->kategori, 20);
    cout << "| ";

    if (temp->currency == 1)
    {
      cout << right << setw(15);
      printRupiah(temp->nominal);
      cout << "   |";

      totalEstimasiRupiah += temp->nominal;
    }
    else if (temp->currency == 2)
    {
      cout << right << setw(12) << "$ " << temp->nominal << "     |";

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
  if (pengeluaran == NULL)
  {
    cout << "\n[DATA KOSONG] Tidak ada data yang bisa dihapus.\n";
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
    return;
  }

  cin.ignore(); // bersihkan buffer input

  int pilihan;
  char keyword[200];

  cout << "\n --- MENU PENCARIAN DATA ---\n";
  cout << " [1] Cari berdasarkan Tanggal\n";
  cout << " [2] Cari berdasarkan Deskripsi\n";
  cout << " [3] Cari berdasarkan Kategori\n";
  cout << " [4] Cari berdasarkan Nominal tepat\n";
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

  if (pilihan >= 1 && pilihan <= 3)
  {
    cout << " Masukkan kata kunci: ";
    cin.getline(keyword, 200);

    // ubah ke lowercase untuk pencarian
    toLowerCase(keyword);
  }
  else if (pilihan == 4)
  {
    cout << " Masukkan nominal (angka): ";
    cin >> cariNominal;
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
    if (pilihan == 1 && strstr(lowTanggal, keyword))
      match = true;
    if (pilihan == 2 && strstr(lowDeskripsi, keyword))
      match = true;
    if (pilihan == 3 && strstr(lowKategori, keyword))
      match = true;
    if (pilihan == 4 && temp->nominal == cariNominal)
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
  cin.ignore();
  cin.get();
}

void bersihkanMemori()
{
  // TODO: Implement memory cleanup functionality
}

void tambahPengeluaran()
{
  showHeader();
  cin.ignore(); // bersihkan buffer agar getline tidak ter-skip

  char tgl[50], desc[200], kat[100];
  long long nominal;
  int currency;

  cout << "\n --- TAMBAH PENGELUARAN ---\n";

  cout << " Tanggal (DD/MM/YYYY): ";
  cin.getline(tgl, 50);

  cout << " Deskripsi             : ";
  cin.getline(desc, 200);

  cout << " Kategori              : ";
  cin.getline(kat, 100);

  toProperCase(kat);

  cout << " Nominal               : ";
  cin >> nominal;

  do
  {
    cout << " Mata Uang [1] IDR  [2] USD : ";
    cin >> currency;

    if (currency != 1 && currency != 2)
    {
      cout << " [Error] Pilihan mata uang tidak valid. Silakan coba lagi.\n";
    }
  } while (currency != 1 && currency != 2);
  // Panggil fungsi penambah node
  tambahData(tgl, desc, kat, nominal, currency);

  cout << "\n [Sukses] Pengeluaran berhasil ditambahkan!\n";
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
}
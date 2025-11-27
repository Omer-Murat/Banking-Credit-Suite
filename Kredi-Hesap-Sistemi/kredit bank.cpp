// --- EN TEPEDE BU OLMALI ---
#define NOMINMAX 

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib> 
#include <ctime>   
#include <windows.h> 
#include <algorithm> 

using namespace std;

// --- RENK KODLARI ---
#define RESET   "\033[0m"
#define RED     "\033[31m"
#define GREEN   "\033[32m"
#define YELLOW  "\033[33m"
#define CYAN    "\033[36m"
#define WHITE   "\033[97m" 
#define BOLD    "\033[1m"

// --- YARDIMCI FONKSIYONLAR ---
void EkranTemizle() {
    system("cls");
}

void Bekle() {
    cout << "\n" << WHITE << "Devam etmek icin Enter'a basin..." << RESET;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cin.get();
}

string RastgeleHesapNoUret() {
    int r = 10000 + rand() % 90000;
    return "KREDI-" + to_string(r);
}

// Akilli Sayi Alma (100.000 gibi noktalilari anlar)
double SayiAl(string mesaj) {
    string giris;
    cout << mesaj;
    cin >> giris;
    string temizGiris = "";
    for (char c : giris) {
        if (c == '.') continue;
        else if (c == ',') temizGiris += '.';
        else temizGiris += c;
    }
    try { return stod(temizGiris); }
    catch (...) { return -1.0; }
}

// =============================================================
// 1. BASE CLASS (ANA SINIF)
// =============================================================
class BankaHesabi {
protected:
    string hesapNo;
    string sahipAd;
    double bakiye; // Kredi hesabinda bu "BORC" demektir.

public:
    BankaHesabi(string no, string ad, double baslangicBakiye) {
        hesapNo = no;
        sahipAd = ad;
        bakiye = baslangicBakiye;
    }

    virtual ~BankaHesabi() {}

    // Kredi hesabina para yatirmak borcu dusurur
    virtual void ParaYatir(double miktar) {
        if (miktar <= 0) return;
        bakiye -= miktar; // Borctan dusuyoruz
        if (bakiye < 0) bakiye = 0; // Fazla yatirirsa 0 olur
        cout << GREEN << ">>> ODEME YAPILDI. Kalan Borc: " << fixed << setprecision(2) << bakiye << " TL" << RESET << endl;
    }

    // Kredi hesabindan para cekmek (Borcu artirmak/Ek kredi)
    virtual void ParaCek(double miktar) {
        if (miktar <= 0) return;
        bakiye += miktar; // Borc artiyor
        cout << RED << ">>> EK KREDI CEKILDI. Yeni Borc: " << fixed << setprecision(2) << bakiye << " TL" << RESET << endl;
    }

    virtual void BilgiGoster() {
        cout << CYAN << "No: " << WHITE << setw(10) << left << hesapNo
            << CYAN << "| Sahip: " << WHITE << setw(15) << left << sahipAd
            << CYAN << "| Guncel Borc: " << RED << fixed << setprecision(2) << bakiye << " TL" << RESET;
    }

    string getHesapNo() { return hesapNo; }
    string getAd() { return sahipAd; }
    double getBakiye() { return bakiye; }
};

// =============================================================
// 2. KREDI HESABI (SENIN ISTEDIGIN MANTIK BURADA)
// =============================================================
class KrediHesabi : public BankaHesabi {
private:
    double standartFaiz; // 3.5
    double gecikmeFaizi; // 3.8

public:
    KrediHesabi(string no, string ad, double borc)
        : BankaHesabi(no, ad, borc) {
        standartFaiz = 3.5;
        gecikmeFaizi = 3.8;
    }

    // --- KRITIK FONKSIYON: FAIZ VE VERGI HESAPLAMA ---
    void FaizIslet() {
        cout << YELLOW << "\n--- ODEME DURUMU KONTROLU ---" << RESET << endl;
        cout << "Odeme zamaninda yapildi mi? (E/H): ";
        char cevap;
        cin >> cevap;

        double guncelOran;
        string durumMesaji;

        // 1. FAIZ ORANI BELIRLEME (3.5 mu 3.8 mi?)
        if (cevap == 'E' || cevap == 'e') {
            guncelOran = standartFaiz; // 3.5
            durumMesaji = "Zamaninda Odeme";
        }
        else {
            guncelOran = gecikmeFaizi; // 3.8
            durumMesaji = "GECIKME CEZASI!";
        }

        // 2. MATEMATIKSEL HESAPLAMALAR
        double faizTutari = bakiye * (guncelOran / 100);
        double vergiTutari = faizTutari * 0.40; // Faizin %40'i kadar vergi
        double toplamEk = faizTutari + vergiTutari; // Borca eklenecek toplam tutar

        // Borcu Guncelle
        bakiye += toplamEk;

        // 3. DETAYLI RAPOR YAZDIRMA
        cout << "\n" << WHITE << "======================================" << RESET << endl;
        cout << (cevap == 'h' || cevap == 'H' ? RED : GREEN) << "Durum: " << durumMesaji << RESET << endl;
        cout << "Uygulanan Faiz Orani: %" << guncelOran << endl;
        cout << "--------------------------------------" << endl;
        cout << "Anapara (Borc):      " << fixed << setprecision(2) << (bakiye - toplamEk) << " TL" << endl;
        cout << "Isleyen Faiz:        " << faizTutari << " TL" << endl;
        cout << "Vergi (%40):         " << vergiTutari << " TL" << endl;
        cout << YELLOW << "--------------------------------------" << RESET << endl;
        cout << "TOPLAM EKLENEN:      " << toplamEk << " TL" << endl;
        cout << RED << "YENI TOPLAM BORC:    " << bakiye << " TL" << RESET << endl;
        cout << WHITE << "======================================" << RESET << endl;
    }

    void BilgiGosterDetayli() {
        BankaHesabi::BilgiGoster();
        cout << YELLOW << " (KREDI HESABI)" << RESET << endl;
    }
};

// --- ARAYUZ ---
void LogoGoster() {
    EkranTemizle();
    cout << BOLD << RED << "==========================================================" << RESET << endl;
    cout << BOLD << RED << R"(
  _  _   ___   ___   ___    _     _____     ___     _     _  _   _  __
 | |/ / | _ \ | __| |   \  | |   |_   _|   | _ )   /_\   | \| | | |/ /
 | ' <  |   / | _|  | |) | | |     | |     | _ \  / _ \  | .` | | ' < 
 |_|\_\ |_|_\ |___| |___/  |_|     |_|     |___/ /_/ \_\ |_|\_| |_|\_\
    )" << RESET << endl;
    cout << BOLD << RED << "==========================================================" << RESET << endl;
    cout << "           " << WHITE << "KREDI TAKIP SISTEMI v6.0" << RESET << endl;
    cout << endl;
}

// --- ANA PROGRAM ---
int main() {
    SetConsoleOutputCP(65001);
    srand(time(0));

    vector<KrediHesabi*> hesaplar;

    // Ornek bir kredi hesabi
    hesaplar.push_back(new KrediHesabi(RastgeleHesapNoUret(), "Musteri 1", 10000.0));

    while (true) {
        LogoGoster();
        cout << BOLD << WHITE << "1. YENI KREDI CEK (Hesap Olustur)" << RESET << endl;
        cout << BOLD << WHITE << "2. KREDI ISLEMLERI (Odeme/Faiz Hesapla)" << RESET << endl;
        cout << BOLD << RED << "0. CIKIS" << RESET << endl;
        cout << "\nSeciminiz: ";

        int anaSecim;
        if (!(cin >> anaSecim)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        if (anaSecim == 0) break;

        // --- YENI KREDI ---
        if (anaSecim == 1) {
            EkranTemizle();
            LogoGoster();
            cout << GREEN << "--- KREDI BASVURUSU ---" << RESET << endl;

            string ad;
            double miktar;
            cout << "Ad Soyad: "; cin.ignore(); getline(cin, ad);

            miktar = SayiAl("Cekilecek Kredi Tutari (Orn: 100.000): ");

            string yeniNo = RastgeleHesapNoUret();
            hesaplar.push_back(new KrediHesabi(yeniNo, ad, miktar));

            cout << "\n" << GREEN << ">>> KREDI ONAYLANDI! Hesap No: " << YELLOW << yeniNo << RESET << endl;
            Bekle();
        }

        // --- ISLEMLER ---
        else if (anaSecim == 2) {
            EkranTemizle();
            if (hesaplar.empty()) { cout << RED << "Aktif kredi yok!" << endl; Bekle(); continue; }

            cout << CYAN << "--- KREDI LISTESI ---" << RESET << endl;
            for (size_t i = 0; i < hesaplar.size(); i++) {
                cout << BOLD << WHITE << i + 1 << ". " << RESET;
                hesaplar[i]->BilgiGosterDetayli();
            }

            int hSecim;
            cout << "\nIslem yapilacak hesap (0 iptal): "; cin >> hSecim;

            if (hSecim > 0 && hSecim <= (int)hesaplar.size()) {
                KrediHesabi* aktifHesap = hesaplar[hSecim - 1];
                bool altMenu = true;
                while (altMenu) {
                    EkranTemizle();
                    cout << YELLOW << "MUSTERI: " << WHITE << aktifHesap->getAd() << RESET << endl;
                    cout << "Guncel Borc: " << RED << fixed << setprecision(2) << aktifHesap->getBakiye() << " TL" << RESET << endl;
                    cout << "---------------------------" << endl;
                    cout << "1. Borc Ode (Para Yatir)" << endl;
                    cout << "2. Ek Kredi Cek (Borc Artir)" << endl;
                    cout << BOLD << YELLOW << "3. DONEM FAIZI HESAPLA (Vergi Dahil)" << RESET << endl; // OZEL ISLEM
                    cout << "0. Geri Don" << endl;
                    cout << "---------------------------" << endl;
                    cout << "Islem: ";

                    int islem; cin >> islem;
                    double miktar;

                    if (islem == 1) {
                        miktar = SayiAl("Odenecek Tutar: ");
                        aktifHesap->ParaYatir(miktar);
                        Bekle();
                    }
                    else if (islem == 2) {
                        miktar = SayiAl("Ek Cekilecek Tutar: ");
                        aktifHesap->ParaCek(miktar);
                        Bekle();
                    }
                    else if (islem == 3) {
                        // --- SENIN ISTEDIGIN MATEMATIK BURADA ---
                        aktifHesap->FaizIslet();
                        Bekle();
                    }
                    else if (islem == 0) altMenu = false;
                }
            }
        }
    }

    for (auto h : hesaplar) delete h;
    return 0;
}
// --- EN TEPEDE BU OLMALI ---
#define NOMINMAX 

#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <cstdlib> 
#include <ctime>   
#include <windows.h> 
#include <algorithm> // String islemleri icin

using namespace std;

// --- RENK TANIMLARI ---
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
    return "TR-" + to_string(r);
}

// --- YENI: AKILLI SAYI ALMA FONKSIYONU ---
// Bu fonksiyon "100.000" girilse bile noktalari temizleyip 100000 yapar.
double SayiAl(string mesaj) {
    string giris;
    cout << mesaj;
    cin >> giris;

    string temizGiris = "";

    // Girilen yazinin icindeki harfleri ve noktalari ayikla
    for (char c : giris) {
        if (c == '.') {
            continue; // Noktayi gormezden gel (100.000 -> 100000 olsun)
        }
        else if (c == ',') {
            temizGiris += '.'; // Virgulu noktaya cevir (C++ ondalik sistemi icin)
        }
        else {
            temizGiris += c; // Rakamlari ekle
        }
    }

    // String'i Double sayiya cevir (Hata olursa -1 don)
    try {
        return stod(temizGiris);
    }
    catch (...) {
        return -1.0; // Gecersiz giris (Harf vs girilirse)
    }
}

// =============================================================
// 1. BASE CLASS 
// =============================================================
class BankaHesabi {
protected:
    string hesapNo;
    string sahipAd;
    double bakiye;

public:
    BankaHesabi(string no, string ad, double baslangicBakiye) {
        hesapNo = no;
        sahipAd = ad;
        bakiye = baslangicBakiye;
    }

    virtual ~BankaHesabi() {}

    void ParaYatir(double miktar) {
        if (miktar <= 0) {
            cout << RED << "!!! HATA: Gecersiz tutar." << RESET << endl; return;
        }
        bakiye += miktar;
        cout << GREEN << ">>> ISLEM BASARILI! Yeni Bakiye: " << fixed << setprecision(2) << bakiye << " TL" << RESET << endl;
    }

    virtual void ParaCek(double miktar) {
        if (miktar <= 0) { cout << RED << "HATA: Gecersiz tutar." << endl; return; }

        if (miktar > bakiye) {
            cout << BOLD << RED << "!!! HATA: Hesabinizda o kadar miktar yok!" << RESET << endl;
            cout << RED << ">> Cekilmek istenen: " << fixed << setprecision(2) << miktar << " TL" << endl;
            cout << ">> Mevcut Bakiye:    " << bakiye << " TL" << RESET << endl;
        }
        else {
            bakiye -= miktar;
            cout << YELLOW << ">>> CEKIM ISLEMI TAMAMLANDI. Kalan: " << fixed << setprecision(2) << bakiye << " TL" << RESET << endl;
        }
    }

    void BilgiGoster() {
        cout << CYAN << "No: " << WHITE << setw(8) << left << hesapNo
            << CYAN << "| Sahip: " << WHITE << setw(15) << left << sahipAd
            << CYAN << "| Bakiye: " << YELLOW << fixed << setprecision(2) << bakiye << " TL" << RESET;
    }

    string getHesapNo() { return hesapNo; }
    string getAd() { return sahipAd; }
    double getBakiye() { return bakiye; }
};

// =============================================================
// 2. VADESIZ HESAP 
// =============================================================
class VadesizHesap : public BankaHesabi {
public:
    VadesizHesap(string no, string ad, double bakiye) : BankaHesabi(no, ad, bakiye) {}

    void BilgiGosterDetayli() {
        BankaHesabi::BilgiGoster();
        cout << GREEN << " (Vadesiz)" << RESET << endl;
    }
};

// =============================================================
// 3. VADELI HESAP
// =============================================================
class VadeliHesap : public BankaHesabi {
private:
    double faizOrani;
public:
    VadeliHesap(string no, string ad, double bakiye, double oran)
        : BankaHesabi(no, ad, bakiye) {
        faizOrani = oran;
    }

    void ParaCek(double miktar) override {
        double ceza = 50.0;
        double toplam = miktar + ceza;

        if (miktar <= 0) { cout << RED << "Gecersiz Tutar." << endl; return; }
        cout << RED << "!!! UYARI: Vadeli bozuluyor. 50 TL ceza kesilecek." << RESET << endl;

        if (toplam > bakiye) {
            cout << BOLD << RED << "!!! HATA: Yetersiz Bakiye (Ceza Dahil)!" << RESET << endl;
            cout << RED << ">> Gereken: " << fixed << setprecision(2) << toplam << " TL | Mevcut: " << bakiye << " TL" << RESET << endl;
        }
        else {
            bakiye -= toplam;
            cout << YELLOW << ">>> CEKIM BASARILI. Kalan: " << fixed << setprecision(2) << bakiye << " TL" << RESET << endl;
        }
    }

    void FaizEkle() {
        double kazanc = bakiye * (faizOrani / 100);
        bakiye += kazanc;
        cout << GREEN << ">> %" << faizOrani << " Faiz islendi! Yeni Bakiye: " << fixed << setprecision(2) << bakiye << " TL" << endl;
    }

    void BilgiGosterDetayli() {
        BankaHesabi::BilgiGoster();
        cout << RED << " (Vadeli - %" << (int)faizOrani << ")" << RESET << endl;
    }
};

// --- ARAYUZ ---
void LogoGoster() {
    EkranTemizle();
    cout << BOLD << YELLOW << "==========================================================" << RESET << endl;
    cout << BOLD << YELLOW << R"(
  __  __   _   _   ___     _     _____     ___     _     _  _   _  __
 |  \/  | | | | | | _ \   /_\   |_   _|   | _ )   /_\   | \| | | |/ /
 | |\/| | | |_| | |   /  / _ \    | |     | _ \  / _ \  | .` | | ' < 
 |_|  |_|  \___/  |_|_\ /_/ \_\   |_|     |___/ /_/ \_\ |_|\_| |_|\_\
    )" << RESET << endl;
    cout << BOLD << YELLOW << "==========================================================" << RESET << endl;
    cout << "           " << WHITE << "GUVENLI BANKACILIK SISTEMI v5.0" << RESET << endl;
    cout << endl;
}

// --- ANA PROGRAM ---
int main() {
    SetConsoleOutputCP(65001);
    srand(time(0));

    vector<BankaHesabi*> hesaplar;
    hesaplar.push_back(new VadesizHesap(RastgeleHesapNoUret(), "Admin Test", 1000.0));

    while (true) {
        LogoGoster();
        cout << BOLD << WHITE << "1. YENI HESAP AC" << RESET << endl;
        cout << BOLD << WHITE << "2. ISLEM YAP (Para Yatir/Cek)" << RESET << endl;
        cout << BOLD << RED << "3. HESAP KAPAT (SIL)" << RESET << endl;
        cout << BOLD << RED << "0. CIKIS" << RESET << endl;
        cout << "\nSeciminiz: ";

        int anaSecim;
        if (!(cin >> anaSecim)) { cin.clear(); cin.ignore(1000, '\n'); continue; }

        if (anaSecim == 0) break;

        // --- YENI HESAP ---
        if (anaSecim == 1) {
            EkranTemizle();
            LogoGoster();
            cout << GREEN << "--- YENI HESAP KAYDI ---" << RESET << endl;

            string ad;
            double ilkBakiye;
            int tur;
            cout << "Ad Soyad: "; cin.ignore(); getline(cin, ad);

            // YENI FONKSIYON KULLANIMI:
            ilkBakiye = SayiAl("Baslangic Bakiyesi (Orn: 100.000): ");

            cout << "\n1. Vadesiz\n2. Vadeli\nSecim: "; cin >> tur;

            string yeniNo = RastgeleHesapNoUret();
            if (tur == 1) hesaplar.push_back(new VadesizHesap(yeniNo, ad, ilkBakiye));
            else {
                double oran; cout << "Faiz (%): "; cin >> oran;
                hesaplar.push_back(new VadeliHesap(yeniNo, ad, ilkBakiye, oran));
            }
            cout << "\n" << GREEN << ">>> HESAP ACILDI! No: " << YELLOW << yeniNo << RESET << endl;
            Bekle();
        }

        // --- ISLEM YAPMA ---
        else if (anaSecim == 2) {
            EkranTemizle();
            if (hesaplar.empty()) { cout << RED << "Hesap yok!" << endl; Bekle(); continue; }

            cout << CYAN << "--- HESAP SECIMI ---" << RESET << endl;
            for (size_t i = 0; i < hesaplar.size(); i++) {
                cout << BOLD << WHITE << i + 1 << ". " << RESET;
                if (VadeliHesap* v = dynamic_cast<VadeliHesap*>(hesaplar[i])) v->BilgiGosterDetayli();
                else if (VadesizHesap* v = dynamic_cast<VadesizHesap*>(hesaplar[i])) v->BilgiGosterDetayli();
            }

            int hSecim;
            cout << "\nSecim (0 iptal): "; cin >> hSecim;

            if (hSecim > 0 && hSecim <= (int)hesaplar.size()) {
                BankaHesabi* aktifHesap = hesaplar[hSecim - 1];
                bool altMenu = true;
                while (altMenu) {
                    EkranTemizle();
                    cout << YELLOW << "SECILEN: " << WHITE << aktifHesap->getAd() << " (" << aktifHesap->getHesapNo() << ")" << RESET << endl;
                    // Bakiye gosteriminde de noktali gosterim icin fixed ve setprecision kullaniyoruz
                    cout << "Mevcut Bakiye: " << BOLD << WHITE << fixed << setprecision(2) << aktifHesap->getBakiye() << " TL" << RESET << endl;

                    cout << "-----------------------" << endl;
                    cout << "1. Para Yatir (+)\n2. Para Cek (-)\n";
                    if (dynamic_cast<VadeliHesap*>(aktifHesap)) cout << "3. Faiz Ekle\n";
                    cout << "0. Geri Don\nIslem: ";

                    int islem; cin >> islem;
                    double miktar;

                    if (islem == 1) {
                        // YENI FONKSIYON: 100.000 girilebilir
                        miktar = SayiAl("Yatirilacak Tutar: ");
                        aktifHesap->ParaYatir(miktar);
                        Bekle();
                    }
                    else if (islem == 2) {
                        // YENI FONKSIYON: 100.000 girilebilir
                        miktar = SayiAl("Cekilecek Tutar: ");
                        aktifHesap->ParaCek(miktar);
                        Bekle();
                    }
                    else if (islem == 3 && dynamic_cast<VadeliHesap*>(aktifHesap)) {
                        ((VadeliHesap*)aktifHesap)->FaizEkle(); Bekle();
                    }
                    else if (islem == 0) altMenu = false;
                }
            }
        }

        // --- HESAP SILME ---
        else if (anaSecim == 3) {
            EkranTemizle();
            if (hesaplar.empty()) { cout << RED << "Silinecek hesap yok!" << endl; Bekle(); continue; }

            LogoGoster();
            cout << RED << "--- HESAP KAPATMA ---" << RESET << endl;
            for (size_t i = 0; i < hesaplar.size(); i++) {
                cout << BOLD << WHITE << i + 1 << ". " << RESET;
                hesaplar[i]->BilgiGoster(); cout << endl;
            }

            int silSecim;
            cout << "\nSilinecek Sira No (0 iptal): "; cin >> silSecim;

            if (silSecim > 0 && silSecim <= (int)hesaplar.size()) {
                int index = silSecim - 1;
                BankaHesabi* silinecek = hesaplar[index];

                cout << "\n" << RED << "DIKKAT: " << silinecek->getAd() << " hesabi (" << silinecek->getHesapNo() << ") silinecek!" << RESET << endl;
                if (silinecek->getBakiye() > 0) cout << YELLOW << "UYARI: Icinde para var!" << RESET << endl;

                char onay; cout << "Onayliyor musunuz? (E/H): "; cin >> onay;
                if (onay == 'e' || onay == 'E') {
                    delete silinecek;
                    hesaplar.erase(hesaplar.begin() + index);
                    cout << "\n" << GREEN << ">>> HESAP SILINDI." << RESET << endl;
                }
                else {
                    cout << "\n" << GREEN << ">>> Iptal edildi." << RESET << endl;
                }
                Bekle();
            }
        }
    }

    for (auto h : hesaplar) delete h;
    return 0;
}
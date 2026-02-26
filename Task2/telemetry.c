// =============================================================================
// telemetry.c — Telemetri Modülü Uygulama Dosyası
// =============================================================================
// Bu dosya, aracın TÜM durumunu (hız, batarya, sıcaklıklar) ve geçmiş
// verilerini tutan dizileri BURADA tanımlar. Fonksiyonlar bu verilere
// doğrudan erişerek günceller.
//
// ÖNEMLİ KAVRAM: "static" Anahtar Kelimesi
// ------------------------------------------
// C dilinde, bir değişkenin önüne "static" koyduğumuzda:
//   1. Değişken SADECE bu dosyadan (.c) erişilebilir (dışarıdan görünmez).
//   2. Programın ömrü boyunca bellekte kalır (fonksiyon bitince silinmez).
//
// Bu sayede:
//   - main.c bu değişkenlere doğrudan erişemez (Kapsülleme / Encapsulation).
//   - Global değişken kullanmamış oluruz (Gömülü Sistem standardı).
//   - Veriye sadece bu dosyadaki fonksiyonlar aracılığıyla erişilir.
//
// Encapsulation için struct kullanmakta mümkün, ama ödevin kapsamından dol
// Neden Pointer Kullanmıyoruz?
// ----------------------------
// Pointer'lar güçlü ama tehlikelidir (Dangling Pointer, Memory Leak vb.).
// Bu ödevde pointer kullanmak YASAKTIR. Bunun yerine static değişkenlerle
// "Module-Level State" yaklaşımını kullanıyoruz.
// =============================================================================

#include "telemetry.h" // Sabitler (MAX_KAYIT) ve fonksiyon prototipleri
#include <stdio.h>     // printf, scanf
#include <stdlib.h>    // rand, srand (rastgele sayı üretimi)

// ---------------------------------------------------------------------------
// MODÜL SEVİYESİ DURUM DEĞİŞKENLERİ (Module-Level State)
// ---------------------------------------------------------------------------
// "static" → Bu değişkenler SADECE telemetry.c dosyasından erişilebilir.
// main.c veya başka bir dosya bunları göremez, değiştiremez.

// Aracın anlık durumu
static float hiz = 0.0f;               // Güncel hız (km/s), başlangıçta sıfır
static int batarya = 100;              // Batarya yüzdesi (%)
static float motor_sicaklik = 35.0f;   // Motor sıcaklığı (°C)
static float batarya_sicaklik = 35.0f; // Batarya sıcaklığı (°C)

// ---------------------------------------------------------------------------
// İSTATİSTİK DİZİLERİ (Arrays for Session History)
// ---------------------------------------------------------------------------
// Her gaza basışta, frende ve rejen frende oluşan hız değişimini kaydediyoruz.
// Dizi boyutu MAX_KAYIT (100) ile sınırlı — taşma (overflow) önlenir.
//
// Neden dizi kullanıyoruz?
// Çünkü ortalama hesaplamak için geçmiş değerlere ihtiyacımız var.
// Her seferinde toplam ve sayaç tutmak yerine, diziye kaydedip sonradan
// ortalamasını hesaplıyoruz. Bu, gömülü sistemlerde "veri loglama" mantığıdır.

static float hizlanma_kayitlari[MAX_KAYIT]; // Gaza basınca oluşan hız artışları
static float
    yavaslama_kayitlari[MAX_KAYIT];      // Fren basınca oluşan hız düşüşleri
static float rejen_kayitlari[MAX_KAYIT]; // Rejen frende oluşan hız düşüşleri

// Dizilerde kaç eleman olduğunu takip eden sayaçlar (counter)
static int hizlanma_sayisi = 0;
static int yavaslama_sayisi = 0;
static int rejen_sayisi = 0;

// =============================================================================
// FONKSİYON: sistemi_kur
// =============================================================================
// Amaç: Hafta 1'deki Start-Up'tan gelen batarya sıcaklığı ve şarj değerlerini
//        alarak modülün iç durumunu başlatır.
//
// Parametre: baslangic_sicaklik → Sensörden okunan batarya sıcaklığı
//            baslangic_sarj     → Sensörden okunan şarj yüzdesi
//
// Not: Motor sıcaklığını da batarya sıcaklığıyla eşitliyoruz (araç henüz
//      çalışmaya başladığında motor ve batarya aynı ortam sıcaklığındadır).
// =============================================================================
void sistemi_kur(float baslangic_sicaklik, int baslangic_sarj) {
  hiz = 0.0f;
  batarya = baslangic_sarj;
  motor_sicaklik = baslangic_sicaklik;
  batarya_sicaklik = baslangic_sicaklik;

  // Sayaçları sıfırla (yeni sürüş oturumu)
  hizlanma_sayisi = 0;
  yavaslama_sayisi = 0;
  rejen_sayisi = 0;

  printf("\n[SİSTEM] Telemetri modülü kuruldu. Araç sürüşe hazır.\n");
  printf("[SİSTEM] Başlangıç Batarya: %%%d | Sıcaklık: %.1f°C\n\n", batarya,
         batarya_sicaklik);
}

// =============================================================================
// FONKSİYON: gaza_bas
// =============================================================================
// Amaç: Aracı hızlandırır.
//
// Mekanik:
//   - Hız artışı: 2.0 ile 10.0 km/s arası rastgele (0.1 adımlarla)
//   - Batarya tüketimi: %2
//   - Termal etki: Motor +5°C, Batarya +2°C
//
// Sınır Durumlar (Edge Cases):
//   - Batarya %0 ise gaza basılamaz (araç hareket edemez)
//   - Hız 70.0 km/s'yi geçemez (maksimum hız sınırı)
//   - Dizi doluysa (100 kayıt) yeni kayıt eklenmez
//
// Rastgele Sayı Üretimi:
//   rand() % 81  → 0 ile 80 arası tam sayı üretir
//   Bunu 10.0'a bölüp 2.0 eklersek → 2.0 ile 10.0 arası float elde ederiz
//   Formül: artis = 2.0 + (rand() % 81) / 10.0
// =============================================================================
void gaza_bas(void) {
  // Sınır Durum: Batarya boşsa gaza basılamaz
  if (batarya <= 0) {
    printf("[UYARI] Batarya tükendi! Gaza basamazsınız.\n");
    return; // Fonksiyondan erken çıkış (Early Return)
  }

  // Sınır Durum: Araç zaten maksimum hızdaysa
  if (hiz >= 70.0f) {
    printf("[UYARI] Maksimum hıza (70.0 km/s) ulaşıldı! Daha fazla "
           "hızlanamazsınız.\n");
    return;
  }

  // Rastgele hız artışı hesapla: 2.0 - 10.0 km/s (0.1 adımlarla)
  // rand() % 81 → 0..80 arası üretir, /10.0 → 0.0..8.0, +2.0 → 2.0..10.0
  float artis = 2.0f + (rand() % 81) / 10.0f;
  float eski_hiz = hiz;

  // Hızı artır
  hiz = hiz + artis;

  // Sınır Durum: Maksimum hız kontrolü (70.0 km/s)
  if (hiz > 70.0f) {
    hiz = 70.0f; // Hızı üst sınıra sabitle
  }

  // Gerçekten ne kadar hızlandığını hesapla (sınır aşılmış olabilir)
  artis = hiz - eski_hiz;

  // Batarya tüketimi: Gaza basınca yüksek akım çekilir → %2 azalır
  batarya = batarya - 2;
  if (batarya < 0) {
    batarya = 0; // Batarya eksiye düşemez
  }

  // Termal etkiler: Motor ve batarya yük altında ısınır
  motor_sicaklik = motor_sicaklik + 5.0f;
  batarya_sicaklik = batarya_sicaklik + 2.0f;

  // İstatistik: Hız artışını diziye kaydet (taşma kontrolü ile)
  if (hizlanma_sayisi < MAX_KAYIT) {
    hizlanma_kayitlari[hizlanma_sayisi] = artis;
    hizlanma_sayisi = hizlanma_sayisi + 1;
  }

  // Kullanıcıya bilgi ver
  printf("[BİLGİ] Gaza basıldı. Araç %.1f km/s hızlandı. "
         "Motor: %.1f°C, Batarya: %.1f°C\n",
         artis, motor_sicaklik, batarya_sicaklik);
}

// =============================================================================
// FONKSİYON: frene_bas
// =============================================================================
// Amaç: Aracı yavaşlatır (klasik mekanik fren).
//
// Mekanik:
//   - Hız düşüşü: 5.0 ile 15.0 km/s arası rastgele
//   - Batarya etkisi: Yok (mekanik fren enerji geri kazanmaz)
//   - Termal etki: Motor -3°C, Batarya -1°C (rüzgar soğutması)
//
// Sınır Durumlar:
//   - Araç zaten durmuşsa (hız = 0) uyarı ver
//   - Hız negatife düşemez (minimum 0.0 km/s)
//   - Soğuma en fazla 20°C'ye kadar olabilir. Yani sıcaklık 20°C'nin altına
//   düşemez.
// =============================================================================
void frene_bas(void) {
  // Sınır Durum: Araç zaten duruyorsa frene basmanın anlamı yok
  if (hiz <= 0.0f) {
    printf("[UYARI] Araç zaten duruyor. Frene basmanıza gerek yok.\n");
    return;
  }

  // Rastgele hız düşüşü: 5.0 - 15.0 km/s (0.1 adımlarla)
  // rand() % 101 → 0..100, /10.0 → 0.0..10.0, +5.0 → 5.0..15.0
  float dusus = 5.0f + (rand() % 101) / 10.0f;
  float eski_hiz = hiz;

  // Hızı azalt
  hiz = hiz - dusus;

  // Sınır Durum: Hız negatif olamaz
  if (hiz < 0.0f) {
    hiz = 0.0f;
  }

  // Sınır Durum: Soğuma en fazla 20°C'ye kadar olabilir. Yani sıcaklık 20°C'nin
  // altına düşemez.
  if (motor_sicaklik < 20.0f) {
    motor_sicaklik = 20.0f;
  }
  if (batarya_sicaklik < 20.0f) {
    batarya_sicaklik = 20.0f;
  }

  // Gerçekten ne kadar yavaşladığını hesapla (sınır aşılmış olabilir)
  dusus = eski_hiz - hiz;

  // Termal etki: Motor yük binmediği ve rüzgar soğutması olduğu için soğur
  motor_sicaklik = motor_sicaklik - 3.0f;
  batarya_sicaklik = batarya_sicaklik - 1.0f;

  // İstatistik: Yavaşlama miktarını diziye kaydet
  if (yavaslama_sayisi < MAX_KAYIT) {
    yavaslama_kayitlari[yavaslama_sayisi] = dusus;
    yavaslama_sayisi = yavaslama_sayisi + 1;
  }

  printf("[BİLGİ] Frene basıldı. Araç %.1f km/s yavaşladı. "
         "Motor: %.1f°C, Batarya: %.1f°C\n",
         dusus, motor_sicaklik, batarya_sicaklik);
}

// =============================================================================
// FONKSİYON: rejen_fren
// =============================================================================
// Amaç: Rejeneratif (geri kazanımlı) frenleme uygular.
//
// Rejeneratif Frenleme Nedir?
//   Elektrikli araçlarda motor aynı zamanda jeneratör olarak çalışabilir.
//   Fren yapılırken kinetik enerji elektrik enerjisine dönüştürülür ve
//   bataryaya geri verilir. Bu sayede menzil artar!
//
// Mekanik:
//   - Hız düşüşü: 2.0 ile 10.0 km/s arası rastgele
//   - Batarya şarjı: +1% (enerji geri kazanımı)
//   - Termal etki: Motor -2°C, Batarya +1°C (enerji dönüşümü ısı üretir)
//
// Sınır Durumlar:
//   - Araç duruyorsa rejen çalışmaz
//   - Batarya %100'ü geçemez
//   - Hız negatife düşemez
//   - Sıcaklık 20°C'nin altına düşemez
// =============================================================================
void rejen_fren(void) {
  // Sınır Durum: Araç duruyorsa kinetik enerji yok, rejen çalışmaz
  if (hiz <= 0.0f) {
    printf("[UYARI] Araç duruyor. Rejeneratif frenleme yapılamaz.\n");
    return;
  }
  // Sınır Durum: Batarya %100'ü geçemez
  if (batarya >= 100) {
    printf("[UYARI] Batarya %%100 dolu. Rejeneratif frenleme yapılamaz.\n");
    return;
  }

  // Sınır Durum: Sıcaklık 20°C'nin altına düşemez
  if (motor_sicaklik < 20.0f) {
    motor_sicaklik = 20.0f;
  }
  if (batarya_sicaklik < 20.0f) {
    batarya_sicaklik = 20.0f;
  }

  // Rastgele hız düşüşü: 2.0 - 10.0 km/s (0.1 adımlarla)
  float dusus = 2.0f + (rand() % 81) / 10.0f;
  float eski_hiz = hiz;

  // Hızı azalt
  hiz = hiz - dusus;
  if (hiz < 0.0f) {
    hiz = 0.0f;
  }

  // Gerçekten ne kadar yavaşladığını hesapla (sınır aşılmış olabilir)
  dusus = eski_hiz - hiz;

  // Bataryayı şarj et: Kinetik enerji → Elektrik enerjisi
  batarya = batarya + 1;
  if (batarya > 100) {
    batarya = 100; // Batarya %100'ü aşamaz
  }

  // Termal etki: Enerji dönüşümü her zaman batarya üzerinde biraz ısı üretir
  //              Motor sıcaklığı düşer
  motor_sicaklik = motor_sicaklik - 2.0f;
  batarya_sicaklik = batarya_sicaklik + 1.0f;

  // İstatistik: Rejen frenleme kaydı
  if (rejen_sayisi < MAX_KAYIT) {
    rejen_kayitlari[rejen_sayisi] = dusus;
    rejen_sayisi = rejen_sayisi + 1;
  }

  printf("[BİLGİ] Rejeneratif fren devrede. Araç %.1f km/s yavaşladı. "
         "Batarya şarj oluyor.\n",
         dusus);
}

// =============================================================================
// FONKSİYON: telemetri_ve_istatistik_yazdir
// =============================================================================
// Amaç: Anlık araç durumunu ve sürüş istatistiklerini yazdırır.
//
// Ortalama Hesaplama:
//   Dizideki tüm değerleri toplam bir for döngüsüyle toplarız,
//   sonra eleman sayısına böleriz.
//   Örnek: 3 kayıt: [5.4, 8.2, 6.7] → Toplam = 20.3 → Ortalama = 20.3/3 = 6.77
//
// Not: Eğer hiç işlem yapılmamışsa (sayaç = 0), ortalama 0.0 olarak gösterilir.
//      Sıfıra bölme hatasını (division by zero) önlemek için sayaç > 0 kontrolü
//      yapılır.
// =============================================================================
void telemetri_ve_istatistik_yazdir(void) {
  // ----- BÖLÜM 1: Anlık Telemetri -----
  printf("\n--- ANLIK TELEMETRİ ---\n");
  printf("Güncel Hız: %.1f km/s\n", hiz);
  printf("Batarya: %%%d\n", batarya);
  printf("Motor Sıcaklığı: %.1f °C\n", motor_sicaklik);
  printf("Batarya Sıcaklığı: %.1f °C\n", batarya_sicaklik);

  // ----- BÖLÜM 2: Sürüş İstatistikleri -----
  printf("-------------------------\n");
  printf("--- SÜRÜŞ İSTATİSTİKLERİ ---\n");

  // --- Hızlanma (Gaza Basma) İstatistiği ---
  // Ortalama hesabı: Dizideki tüm değerleri topla, sayısına böl
  float hizlanma_toplam = 0.0f;
  int i; // Döngü değişkeni (C89/C90 uyumluluğu için döngü dışında tanımlanır)

  for (i = 0; i < hizlanma_sayisi; i++) {
    hizlanma_toplam = hizlanma_toplam + hizlanma_kayitlari[i];
  }
  // Sıfıra bölme kontrolü: Hiç gaza basılmadıysa ortalama 0 olur
  float hizlanma_ort = 0.0f;
  if (hizlanma_sayisi > 0) {
    hizlanma_ort = hizlanma_toplam / hizlanma_sayisi;
  }
  printf("Gaza Basma Sayısı: %d (Ortalama Artış: %.2f km/s)\n", hizlanma_sayisi,
         hizlanma_ort);

  // --- Frenleme İstatistiği ---
  float yavaslama_toplam = 0.0f;
  for (i = 0; i < yavaslama_sayisi; i++) {
    yavaslama_toplam = yavaslama_toplam + yavaslama_kayitlari[i];
  }
  float yavaslama_ort = 0.0f;
  if (yavaslama_sayisi > 0) {
    yavaslama_ort = yavaslama_toplam / yavaslama_sayisi;
  }
  printf("Frene Basma Sayısı: %d (Ortalama Düşüş: %.2f km/s)\n",
         yavaslama_sayisi, yavaslama_ort);

  // --- Rejeneratif Frenleme İstatistiği ---
  float rejen_toplam = 0.0f;
  for (i = 0; i < rejen_sayisi; i++) {
    rejen_toplam = rejen_toplam + rejen_kayitlari[i];
  }
  float rejen_ort = 0.0f;
  if (rejen_sayisi > 0) {
    rejen_ort = rejen_toplam / rejen_sayisi;
  }
  printf("Rejeneratif Frenleme Sayısı: %d (Ortalama Düşüş: %.2f km/s)\n",
         rejen_sayisi, rejen_ort);

  printf("-------------------------\n\n");
}

// =============================================================================
// FONKSİYON: sistemi_kapat
// =============================================================================
// Amaç: Simülasyonu güvenli bir şekilde kapatır.
//
// Gerçek hayat karşılığı: Elektrikli araçlarda "Shutdown Sequence" denilen
// süreç; inverter kapatılır, batarya kontaktörü açılır, tüm alt sistemlere
// "uyku" komutu gönderilir. Burada sadece bir bilgi mesajı veriyoruz.
// =============================================================================
void sistemi_kapat(void) {
  printf("\n[SİSTEM] Araç sistemi kapatılıyor...\n");
  printf("[SİSTEM] Motor durduruldu. Batarya bağlantısı kesildi.\n");
  printf("[SİSTEM] Son Durum → Hız: %.1f km/s | Batarya: %%%d | "
         "Motor: %.1f°C | Batarya Sıc.: %.1f°C\n",
         hiz, batarya, motor_sicaklik, batarya_sicaklik);
  printf("[SİSTEM] Güvenli kapanma tamamlandı. Hoşça kalın!\n\n");
}

// =============================================================================
// FONKSİYON: asiri_isinma_kontrol
// =============================================================================
// Amaç: Motor veya batarya sıcaklığının kritik eşiği aşıp aşmadığını kontrol
//        eder.
//
// Eşik Değerler:
//   - Motor sıcaklığı > 90°C  → ACİL DURUM
//   - Batarya sıcaklığı > 70°C → ACİL DURUM
//
// Dönüş Değeri:
//   1 → Aşırı ısınma var (sadece "Sistemi Kapat" seçeneği sunulmalı)
//   0 → Normal (tüm menü seçenekleri kullanılabilir)
//
// Bu fonksiyon bir "predicate" (koşul fonksiyonu) örneğidir — sadece evet/hayır
// (1/0) döndürür, bir şey yazdırmaz veya değiştirmez.
// =============================================================================
int asiri_isinma_kontrol(void) {
  if (motor_sicaklik > 90.0f || batarya_sicaklik > 70.0f) {
    return 1; // ACİL DURUM: Aşırı ısınma tespit edildi
  }
  return 0; // Normal: Sıcaklıklar güvenli aralıkta
}

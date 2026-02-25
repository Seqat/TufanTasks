// =============================================================================
// main.c — Ana Program Dosyası
// =============================================================================
// Bu dosya iki ana görev üstlenir:
//   1. Hafta 1'den gelen "Start-Up" güvenlik kontrollerini uygular.
//   2. Başarılı start-up sonrası sürüş menüsünü (switch-case) çalıştırır.
//
// ÖNEMLİ: Bu dosyada araç durumunu tutan DEĞİŞKEN tanımlanmaz!
// Tüm araç verileri (hız, batarya vb.) telemetry.c içinde "static" olarak
// tutulur. main.c sadece fonksiyonları ÇAĞIRIR.
//
// Modüler Yapı:
//   main.c ──(#include)──> telemetry.h ──(prototip)──> telemetry.c
//   Ana program             Arayüz                     Uygulama
// =============================================================================

#include "telemetry.h" // Telemetri fonksiyon prototipleri ve sabitleri
#include <stdio.h>     // printf, scanf
#include <stdlib.h>    // srand, rand (rastgele sayı üretimi için)
#include <time.h>      // time (rastgele sayı tohumlaması için)

int main() {
  // ====================================================================
  // RASTGELE SAYI TOHUMLAMASI (Random Seed)
  // ====================================================================
  // srand(time(NULL)); → Rastgele sayı üretecine (RNG) şu anki zamanı
  // "tohum" (seed) olarak verir. Böylece her çalıştırmada farklı rastgele
  // değerler üretilir. Eğer bunu yapmazsak, rand() her seferinde aynı
  // "rastgele" sayıları üretir!
  //
  // time(NULL) → 1 Ocak 1970'den bu yana geçen saniye sayısını döndürür
  //              (Unix Epoch). Her saniye farklı olduğu için iyi bir tohumdur.
  srand(time(NULL));

  // ====================================================================
  // HAFTA 1: START-UP GÜVENLİK KONTROLLERİ
  // ====================================================================
  // Aşağıdaki değişkenler SADECE start-up aşamasında kullanılır.
  // Araç sürüşe başladıktan sonra tüm durum telemetry.c'ye aktarılır.

  // 1. Değişken Tanımlamaları ve İlk Değer Atamaları (Initialization)
  // Garbage value (çöp değer) okumamak için başlangıç değerlerini atıyoruz.
  float batarya_sicakligi = 0.0f;
  int sarj_yuzdesi = 0;
  char kapi_durumu = 'K';
  int fren_pedali = 1;

  printf("--- ELEKTROMOBİL GÜVENLİK KONTROLÜ ---\n");

  // ====================================================================
  // AŞAMA 1: VERİ TOPLAMA VE SINIR DURUM (EDGE CASE) KONTROLLERİ
  // ====================================================================
  // Sensörlerden gelen verinin "fiziksel olarak mümkün" olup olmadığını test
  // ediyoruz. Hatalı/İmkansız bir veri varsa "Erken Çıkış (Early Return)"
  // yaparak programı yormuyoruz.

  // Sıcaklık Girişi
  printf("Batarya Sıcaklığı (°C): ");
  scanf("%f", &batarya_sicakligi);
  if (batarya_sicakligi < -100.0f || batarya_sicakligi > 200.0f) {
    printf("SONUÇ: KRİTİK HATA: İmkansız sıcaklık verisi. Sensör arızası "
           "olabilir.\n");
    return 1; // Programı hata koduyla (1) sonlandır.
  }

  // Şarj Girişi
  printf("Şarj Yüzdesi (%%): ");
  scanf("%d", &sarj_yuzdesi);
  if (sarj_yuzdesi < 0 || sarj_yuzdesi > 100) {
    printf("SONUÇ: KRİTİK HATA: İmkansız şarj seviyesi (%d). Lütfen sensör "
           "verilerini kontrol edin.\n",
           sarj_yuzdesi);
    return 1;
  }

  // Kapı Durumu Girişi
  printf("Kapı Durumu (A/K): ");
  // NOT: %c'den önceki BOŞLUK karakteri, bir önceki enter (newline) tuşunu
  // atlamasını sağlar!
  scanf(" %c", &kapi_durumu);
  if (kapi_durumu != 'A' && kapi_durumu != 'K') {
    printf("SONUÇ: KRİTİK HATA: Geçersiz kapı durumu. Sadece 'A' veya 'K' "
           "girilmelidir.\n");
    return 1;
  }

  // Fren Pedalı Girişi
  printf("Fren Pedalı (1:Basılı, 0:Değil): ");
  scanf("%d", &fren_pedali);
  if (fren_pedali != 0 && fren_pedali != 1) {
    printf("SONUÇ: KRİTİK HATA: Geçersiz fren verisi. Sadece 1 veya 0 "
           "girilmelidir.\n");
    return 1;
  }

  // ====================================================================
  // AŞAMA 2: İŞ MANTIĞI VE ÖNCELİK SIRALAMASI (BUSINESS LOGIC)
  // ====================================================================
  // Sensör verileri fiziksel olarak geçerli. Şimdi yarış kurallarını
  // istenilen öncelik sırasına göre (Sıcaklık -> Şarj -> Kapı -> Fren) test
  // ediyoruz.

  printf("\n");

  // Sistemde uyarı olup olmadığını takip edeceğimiz FLAG (Bayrak) değişkeni
  // 0 = Uyarı yok (Temiz)
  // 1 = Uyarı var (Kirli)
  int uyari_var = 0;

  // Dikkat: Burada "else if" yerine sadece "if" kullanıyoruz.
  // Böylece program her bir şartı tek tek, birbirinden bağımsız test edecek.

  if (batarya_sicakligi > 60.0f) {
    printf("SONUÇ: UYARI: Motor Aşırı Isındı! Sürüş Engellendi.\n");
    uyari_var = 1;
  }

  if (sarj_yuzdesi < 10) {
    printf("SONUÇ: UYARI: Batarya Kritik Seviyede! Sürüş Başlatılamaz.\n");
    uyari_var = 1;
  }

  if (kapi_durumu == 'A') {
    printf("SONUÇ: UYARI: Kapılar Açık! Lütfen Kapatınız.\n");
    uyari_var = 1;
  }

  if (fren_pedali == 0) {
    printf("SONUÇ: UYARI: Güvenlik için frene basarak tekrar deneyin.\n");
    uyari_var = 1;
  }

  // Eğer uyari_var bayrağı kalkmamışsa (hala 0 ise)
  if (!uyari_var) {
    printf("SONUÇ: BAŞARILI: SİSTEM HAZIR. MOTOR BAŞLATILIYOR...\n");
  } else {
    // Eğer uyari_var 1 olduysa
    printf("SİSTEM MESAJI: Lütfen motoru başlatmadan önce yukarıdaki uyarıları "
           "giderin.\n");
    return 1; // Uyarı varsa sürüş menüsüne geçmeden çık
  }

  // ====================================================================
  // HAFTA 2: SÜRÜŞ SİMÜLASYONU (Driving Simulation)
  // ====================================================================
  // Start-Up başarılı! Şimdi telemetri modülünü başlat ve sürüş menüsüne geç.
  //
  // sistemi_kur() fonksiyonu, start-up'tan gelen değerleri telemetry.c'nin
  // iç durumuna kopyalar. Bu noktadan sonra main.c artık batarya_sicakligi
  // ve sarj_yuzdesi değişkenlerini KULLANMAZ — her şey telemetry.c'de.
  sistemi_kur(batarya_sicakligi, sarj_yuzdesi);

  // ====================================================================
  // ANA MENÜ DÖNGÜSÜ (Main Menu Loop)
  // ====================================================================
  // while(1) → Sonsuz döngü: Kullanıcı "Sistemi Kapat" seçene kadar çalışır.
  //
  // switch-case yapısı, if-else if zincirinin daha okunabilir halidir.
  // Her "case" bir menü seçeneğini temsil eder.
  // "break" olmadan bir sonraki case'e düşer ("fall-through") — dikkat!

  int secim = 0; // Kullanıcının menü seçimi

  while (1) {
    // Aşırı ısınma durumunu kontrol et
    // asiri_isinma_kontrol() → 1 dönerse ACİL DURUM
    if (asiri_isinma_kontrol()) {
      // ============================================================
      // ACİL DURUM MENÜSÜ
      // ============================================================
      // Motor veya batarya kritik sıcaklığı aştığında, güvenlik
      // nedeniyle sadece "Sistemi Kapat" seçeneği sunulur.
      // Gerçek araçlarda bu duruma "Limp Mode" veya "Thermal
      // Shutdown" denir.
      printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("!!! ACİL DURUM: AŞIRI ISINMA TESPİT EDİLDİ !!!\n");
      printf("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n");
      printf("Motor veya Batarya sıcaklığı kritik seviyede!\n");
      printf("Güvenlik nedeniyle sadece sistemi kapatabilirsiniz.\n\n");
      printf("5. Sistemi Kapat\n");
      printf("Seçiminiz: ");
      scanf("%d", &secim);

      if (secim == 5) {
        sistemi_kapat();
        return 0;
      } else {
        printf("[UYARI] Aşırı ısınma nedeniyle bu işlem yapılamaz!\n");
      }
    } else {
      // ============================================================
      // NORMAL SÜRÜŞ MENÜSÜ
      // ============================================================
      printf("\n--- TUFAN ELEKTROMOBİL SÜRÜŞ SİMÜLASYONU ---\n");
      printf("1. Gaza Bas\n");
      printf("2. Frene Bas\n");
      printf("3. Rejeneratif Frenleme Yap\n");
      printf("4. Anlık Telemetri ve İstatistikleri Oku\n");
      printf("5. Sistemi Kapat\n");
      printf("Seçiminiz: ");
      scanf("%d", &secim);

      // switch-case ile menü yönlendirmesi
      switch (secim) {
      case 1:
        gaza_bas();
        break; // break olmadan case 2'ye düşer!

      case 2:
        frene_bas();
        break;

      case 3:
        rejen_fren();
        break;

      case 4:
        telemetri_ve_istatistik_yazdir();
        break;

      case 5:
        sistemi_kapat();
        return 0; // Programı başarıyla (0) sonlandır

      // switch-case yapısında default, yukarıdaki case'lerden
      // hiçbiri eşleşmezse çalışır.
      default:
        // Kullanıcı 1-5 dışında bir şey girdiyse
        printf("[UYARI] Geçersiz seçim! Lütfen 1-5 arası bir "
               "değer giriniz.\n");
        break;
      }
    }
  }

  // Soru: Bu satıra asla ulaşılmaz - Neden?
  return 0;
}

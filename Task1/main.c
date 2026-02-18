#include <stdio.h>

int main() {
  // 1. Değişken Tanımlamaları ve İlk Değer Atamaları (Initialization)
  // Garbage value (çöp değer) okumamak için başlangıç değerlerini atıyoruz.
  float batarya_sicakligi = 0.0f;
  int sarj_yuzdesi = 0;
  char kapi_durumu = 'K';
  int fren_pedali = 1;

  printf("--- ELEKTROMOBİL GÜVENLİK KONTROLÜ ---\n");

  // ====================================================================
  // AŞAMA 1: VERİ TOPLAMA VE SINIR DURUM (EDGE CASE) KONTROLLERİ
  // Sensörlerden gelen verinin "fiziksel olarak mümkün" olup olmadığını test
  // ediyoruz. Hatalı/İmkansız bir veri varsa "Erken Çıkış (Early Return)"
  // yaparak programı yormuyoruz.
  // ====================================================================

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
  // Sensör verileri artık fiziksel olarak geçerli. Şimdi yarış kurallarını
  // istenilen öncelik sırasına göre (Sıcaklık -> Şarj -> Kapı -> Fren) test
  // ediyoruz.
  // ====================================================================

  printf("\n");

  // Sistemde uyarı olup olmadığını takip edeceğimiz FLAG (Bayrak) değişkeni
  // 0 = Uyarı yok (Temiz)
  // 1 = Uyarı var (Kirli)
  int uyari_var = 0;

  // Dikkat: Burada "else if" yerine sadece "if" kullanıyoruz.
  // Böylece program her bir şartı tek tek, birbirinden bağımsız test edecek.

  if (batarya_sicakligi > 60.0f) {
    printf("SONUÇ: UYARI: Motor Aşırı Isındı! Sürüş Engellendi.\n");
    uyari_var = 1; // Uyarı bulduğumuz için bayrağı kaldırdık (raised the flag)
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
    // Eğer uyari_var 1 (veya herhangi bir sayı) olduysa
    printf("SİSTEM MESAJI: Lütfen motoru başlatmadan önce yukarıdaki uyarıları "
           "giderin.\n");
  }

  return 0;
}
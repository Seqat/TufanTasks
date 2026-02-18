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
    printf("SONUÇ: HATA: İmkansız sıcaklık verisi. Sensör arızası olabilir.\n");
    return 1; // Programı hata koduyla (1) sonlandır.
  }

  // Şarj Girişi
  printf("Şarj Yüzdesi (%%): ");
  scanf("%d", &sarj_yuzdesi);
  if (sarj_yuzdesi < 0 || sarj_yuzdesi > 100) {
    printf("SONUÇ: HATA: İmkansız şarj seviyesi (%%%d). Lütfen sensör "
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
    printf("SONUÇ: HATA: Geçersiz kapı durumu. Sadece 'A' veya 'K' "
           "girilmelidir.\n");
    return 1;
  }

  // Fren Pedalı Girişi
  printf("Fren Pedalı (1:Basılı, 0:Değil): ");
  scanf("%d", &fren_pedali);
  if (fren_pedali != 0 && fren_pedali != 1) {
    printf(
        "SONUÇ: HATA: Geçersiz fren verisi. Sadece 1 veya 0 girilmelidir.\n");
    return 1;
  }

  return 0;
}
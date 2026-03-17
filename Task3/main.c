#include "can_queue.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
  int secim = 0, mesaj_id = 0, kapasite = 0;
  float veri = 0.0f;
  char gonderen[30];

  // Kullanicidan kuyruk kapasitesini al
  printf("[SISTEM] Lutfen CAN Bus mesaj kuyrugu kapasitesini giriniz: ");
  if (scanf("%d", &kapasite) != 1) {
    printf("[HATA] Gecersiz giris! Bir sayi giriniz.\n");
    return 1;
  }

  if (kapasite <= 0) {
    printf("[HATA] Gecersiz kapasite! Pozitif bir sayi giriniz.\n");
    return 1;
  }

  // Dinamik bellek ile kuyruk olustur
  MesajKuyrugu *kuyruk = kuyrukOlustur(kapasite);
  if (kuyruk == NULL) {
    return 1;
  }

  // Ana menu dongusu
  do {
    printf("--- CAN BUS ILETISIM SISTEMI ---\n");
    printf("1. Sensorden Mesaj Uret (Kuyruga Ekle)\n");
    printf("2. Bekleyen Ilk Mesaji Isle (Kuyruktan Cikar)\n");
    printf("3. Bekleyen Mesajlari Goster\n");
    printf("4. Cikis\n");
    printf("Seciminiz: ");
    if (scanf("%d", &secim) != 1) {
      printf("\n[HATA] Gecersiz giris! Lutfen bir sayi giriniz.\n");
      while (getchar() != '\n')
        ; // Gecersiz girisi temizle
      secim = 0;
      continue;
    }
    printf("\n");

    switch (secim) { 
    case 1:
      // Sensorden mesaj uret (Enqueue)
      printf("Sensor Adi: ");
      scanf(" %29[^\n]", gonderen); // Bosluklu metin okuma (max 29 karakter).
                                    // Bu sayede buffer overflow onlenir.
      printf("Mesaj ID: ");
      scanf("%d", &mesaj_id);
      printf("Veri: ");
      scanf("%f", &veri);
      enqueue(kuyruk, mesaj_id, veri, gonderen);
      break;

    case 2:
      // Bekleyen ilk mesaji isle (Dequeue)
      dequeue(kuyruk);
      break;

    case 3:
      // Bekleyen mesajlari goster
      kuyrukYazdir(kuyruk);
      break;

    case 4:
      // Cikis - bellegi temizle
      kuyrukSil(kuyruk);
      kuyruk = NULL;
      break;

    default:
      printf("[HATA] Gecersiz secim! Lutfen 1-4 arasinda bir sayi giriniz.\n");
      break;
    }
    printf("\n");
  } while (secim != 4);

  return 0;
}

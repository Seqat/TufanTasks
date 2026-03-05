#include "can_queue.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Dinamik bellek ile kuyruk olusturma
MesajKuyrugu *kuyrukOlustur(int kapasite) {
  MesajKuyrugu *kuyruk = (MesajKuyrugu *)malloc(sizeof(MesajKuyrugu));
  if (kuyruk == NULL) {
    printf("[HATA] Bellek tahsisi basarisiz!\n");
    return NULL;
  }

  kuyruk->mesajlar = (CAN_Mesaj *)malloc(sizeof(CAN_Mesaj) * (size_t)kapasite);
  if (kuyruk->mesajlar == NULL) {
    printf("[HATA] Mesaj dizisi icin bellek tahsisi basarisiz!\n");
    free(kuyruk);
    return NULL;
  }

  kuyruk->kapasite = kapasite;
  kuyruk->front = 0;
  kuyruk->rear = 0;
  kuyruk->count = 0;

  printf(
      "[BILGI] %d kapasiteli dinamik mesaj kuyrugu bellekte olusturuldu!\n\n",
      kapasite);
  return kuyruk;
}

// Kuyruga mesaj ekleme (Enqueue)
int enqueue(MesajKuyrugu *kuyruk, int mesaj_id, float veri,
            const char *gonderen) {
  // Kuyruk dolu mu kontrol et
  if (kuyruk->count >= kuyruk->kapasite) {
    printf("[HATA] CAN Bus hatti dolu! Kapasite (%d) asildi. Yeni mesaj "
           "eklenemez.\n",
           kuyruk->kapasite);
    return -1;
  }

  // Yeni mesaji rear konumuna ekle
  kuyruk->mesajlar[kuyruk->rear].mesaj_id = mesaj_id;
  kuyruk->mesajlar[kuyruk->rear].veri = veri;
  strncpy(kuyruk->mesajlar[kuyruk->rear].gonderen, gonderen, 29);
  kuyruk->mesajlar[kuyruk->rear].gonderen[29] = '\0';

  // Dairesel kuyruk: rear indeksini guncelle
  kuyruk->rear = (kuyruk->rear + 1) % kuyruk->kapasite;
  kuyruk->count++;

  printf("[BILGI] Mesaj kuyruga eklendi. (Siradaki mesaj sayisi: %d)\n",
         kuyruk->count);
  return 0;
}

// Kuyruktan mesaj cikarma (Dequeue) - FIFO
int dequeue(MesajKuyrugu *kuyruk) {
  // Kuyruk bos mu kontrol et
  if (kuyruk->count == 0) {
    printf("[BILGI] Islenecek yeni mesaj bulunmuyor.\n");
    return -1;
  }

  // Front konumundaki mesaji isle
  CAN_Mesaj *mesaj = &kuyruk->mesajlar[kuyruk->front];
  printf("[ISLENDI] %s (ID: %d) sensorunden gelen '%.2f' verisi ana "
         "bilgisayara gonderildi.\n",
         mesaj->gonderen, mesaj->mesaj_id, mesaj->veri);

  // Dairesel kuyruk: front indeksini guncelle
  kuyruk->front = (kuyruk->front + 1) % kuyruk->kapasite;
  kuyruk->count--;

  printf("[BILGI] Kuyrukta kalan mesaj sayisi: %d\n", kuyruk->count);
  return 0;
}

// Kuyruktaki mesajlari yazdirma
void kuyrukYazdir(const MesajKuyrugu *kuyruk) {
  if (kuyruk->count == 0) {
    printf("[BILGI] Islenecek yeni mesaj bulunmuyor.\n");
    return;
  }

  printf("[BEKLEYEN MESAJLAR - Toplam: %d]\n", kuyruk->count);

  int indeks = kuyruk->front;
  for (int i = 0; i < kuyruk->count; i++) {
    printf("  %d. %s (ID: %d) - Veri: %.2f\n", i + 1,
           kuyruk->mesajlar[indeks].gonderen, kuyruk->mesajlar[indeks].mesaj_id,
           kuyruk->mesajlar[indeks].veri);
    // Dairesel dizi icinde ilerleme
    indeks = (indeks + 1) % kuyruk->kapasite;
  }
}

// Dinamik bellegi serbest birakma
void kuyrukSil(MesajKuyrugu *kuyruk) {
  if (kuyruk != NULL) {
    printf("[BILGI] Dinamik bellek (RAM) temizleniyor...\n");
    if (kuyruk->mesajlar != NULL) {
      free(kuyruk->mesajlar);
      kuyruk->mesajlar = NULL;
    }
    free(kuyruk);
    printf("[BILGI] Sistem guvenli bir sekilde kapatildi.\n");
  }
}

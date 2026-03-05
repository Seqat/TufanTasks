#ifndef CAN_QUEUE_H
#define CAN_QUEUE_H

// 1. Tek bir sensor mesajinin yapisi
typedef struct {
  int mesaj_id;      // Orn: 101 (Hiz), 202 (Batarya)
  float veri;        // Orn: 45.5 (Derece veya km/s)
  char gonderen[30]; // Orn: "Motor_Sicaklik_Sensoru"
} CAN_Mesaj;

// 2. Mesajlarin bekledigi kuyruk yapisi (Dinamik Dizi ile)
typedef struct {
  CAN_Mesaj *mesajlar; // Dinamik olarak tahsis edilecek dizi pointer'i
  int kapasite;        // Kullanicinin belirledigi maksimum kapasite
  int front;           // Kuyruk basi (Islenecek ilk mesajin indeksi)
  int rear;            // Kuyruk sonu (Yeni eklenecek mesajin indeksi)
  int count;           // Kuyruktaki anlik mesaj sayisi
} MesajKuyrugu;

// Fonksiyon prototipleri
MesajKuyrugu *kuyrukOlustur(int kapasite);
int enqueue(MesajKuyrugu *kuyruk, int mesaj_id, float veri,
            const char *gonderen);
int dequeue(MesajKuyrugu *kuyruk);
void kuyrukYazdir(const MesajKuyrugu *kuyruk);
void kuyrukSil(MesajKuyrugu *kuyruk);

#endif // CAN_QUEUE_H

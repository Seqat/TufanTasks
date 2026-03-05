# Açıklama

Bu proje, C dilinde geliştirilmiş bir **CAN Bus Mesaj Kuyruğu** simülasyonudur. Gerçek bir elektromobilde sensörlerden (hız, batarya, motor sıcaklığı vb.) gelen verilerin CAN Bus hattı üzerinden ana bilgisayara (ECU) iletilmesini modellemektedir. Mesajlar FIFO (First In, First Out) prensibiyle sıraya alınır ve işlenir.

## Özellikler

- **Dinamik Bellek Yönetimi:** Kuyruk kapasitesi program başlangıcında kullanıcıdan alınır ve `malloc` ile dinamik olarak bellekte oluşturulur.
- **Dairesel Kuyruk (Circular Queue):** `front` ve `rear` indeksleri modüler aritmetik ile yönetilir, böylece dizi sınırlarında verimli bir şekilde sarmalama yapılır.
- **Mesaj Ekleme (Enqueue):** Sensör adı, mesaj ID ve veri değeri girilerek kuyruğa yeni mesaj eklenir.
- **Mesaj İşleme (Dequeue):** Kuyruktan FIFO sırasıyla ilk mesaj çıkarılır ve işlenmiş olarak ekrana yazdırılır.
- **Hata Yönetimi:** Kuyruk taşması (overflow) ve boş kuyruk (underflow) durumları kontrol edilerek uygun hata/bilgi mesajları verilir.
- **Bellek Temizliği:** Program sonlandırılmadan önce `free` ile dinamik bellek serbest bırakılır.

## Proje Dosyaları

- `can_queue.h`: `CAN_Mesaj` ve `MesajKuyrugu` struct tanımları, include korumaları (`#ifndef`) ve fonksiyon prototipleri.
- `can_queue.c`: Kuyruk fonksiyonlarının (`kuyrukOlustur`, `enqueue`, `dequeue`, `kuyrukYazdir`, `kuyrukSil`) implementasyonu.
- `main.c`: Kullanıcıdan kapasite alma, interaktif menü döngüsü ve bellek temizliği.

## Kurulum ve Çalıştırma

Projenin derlenmesi ve çalıştırılması için sisteminizde `gcc` compiler kurulu olmalıdır.

### Derleme

Terminali açın ve proje dizinine gidin, ardından tüm dosyaları derlemek için şu komutu çalıştırın:

```bash
gcc -Wall -Wextra -o can_bus_sim main.c can_queue.c
```

### Çalıştırma

Derlenen projeyi çalıştırmak için:

```bash
./can_bus_sim
```

## Örnek Kullanım

Program ilk çalıştığında kuyruk kapasitesini sorar:

```text
[SISTEM] Lutfen CAN Bus mesaj kuyrugu kapasitesini giriniz: 2
[BILGI] 2 kapasiteli dinamik mesaj kuyrugu bellekte olusturuldu!
```

Ardından 4 seçenekli ana menü ekrana gelir:

1. Sensörden Mesaj Üret (Kuyruğa Ekle)
2. Bekleyen İlk Mesajı İşle (Kuyruktan Çıkar)
3. Bekleyen Mesajları Göster
4. Çıkış

**Örnek Çıktı:**

```text
--- CAN BUS ILETISIM SISTEMI ---
Seciminiz: 1
Sensor Adi: Batarya_Sicaklik
Mesaj ID: 201
Veri: 45.5
[BILGI] Mesaj kuyruga eklendi. (Siradaki mesaj sayisi: 1)

Seciminiz: 3
[BEKLEYEN MESAJLAR - Toplam: 1]
  1. Batarya_Sicaklik (ID: 201) - Veri: 45.50

Seciminiz: 2
[ISLENDI] Batarya_Sicaklik (ID: 201) sensorunden gelen '45.50' verisi ana bilgisayara gonderildi.
[BILGI] Kuyrukta kalan mesaj sayisi: 0

Seciminiz: 4
[BILGI] Dinamik bellek (RAM) temizleniyor...
[BILGI] Sistem guvenli bir sekilde kapatildi.
```

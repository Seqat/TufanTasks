# Açıklama

Bu proje, C dilinde geliştirilmiş modüler bir elektrikli araç (elektromobil) sürüş simülasyonudur. Sistem, start-up güvenlik kontrollerini geçerse, kullanıcıya interaktif bir sürüş menüsü sunar ve telemetri verilerini anlık olarak hesaplar.

## Özellikler

- **Start-Up Güvenlik Kontrolü:** Motor başlatılmadan önce batarya sıcaklığı, şarj yüzdesi, kapı ve fren durumu gibi kritik güvenlik kontrolleri yapılır. (Hafta 1)
- **Modüler Yapı (Kapsülleme):** `main.c`, `telemetry.h` ve `telemetry.c` olmak üzere 3 farklı dosya kullanılmıştır. `telemetry.c` içindeki durum değişkenleri, "Module-Level State" (Encapsulation) prensibi ile dışarıdan gizlenmiştir.
- **Dinamik Sürüş Deneyimi:**
  - **Gaza Basma:** Araç hızlanır, batarya azalır ve sıcaklıklar artar. Maksimum hız 70.0 km/s ile sınırlandırılmıştır.
  - **Frene Basma:** Araç klasik frenleme ile yavaşlar ve rüzgar etkisiyle soğur.
  - **Rejeneratif Frenleme:** Araç yavaşlarken kinetik enerji geri kazanılır ve batarya şarj edilir. Isı üretimi devam eder.
- **Sürüş İstatistikleri Loglama:** Yapılan tüm hızlanma ve yavaşlama eylemleri, dizi (array) yapılarında geçmiş veri olarak saklanır. Ortalama hızlanma ve yavaşlama değerleri anında hesaplanıp gösterilebilir.
- **Aşırı Isınma (Overheat) Koruması:** Motor ( > 90°C ) veya bataryada ( > 70°C ) kritik sıcaklıklara ulaşıldığında sistem "Acil Durum" moduna geçer ve sadece güvenli kapanışa izin verir.

## Proje Dosyaları

- `main.c`: Start-up güvenlik kontrollerinin yapıldığı ve interaktif menünün döndürüldüğü ana döngü dosyası.
- `telemetry.h`: Sabitlerin (`MAX_KAYIT`) tanımlandığı ve telemetri alt sisteminin fonksiyon prototiplerini içeren başlık dosyası.
- `telemetry.c`: Aracın iç durumunun (`hiz`, `batarya`, `motor_sicaklik`, `batarya_sicaklik`) tutulduğu ve sürüş fonksiyonlarının uygulandığı modül dosyası ("static" C değişkenleri ile).

## Kurulum ve Çalıştırma

Projenin derlenmesi ve çalıştırılması için sisteminizde `gcc` compiler kurulu olmalıdır.

### Derleme

Terminali açın ve proje dizinine gidin, ardından tüm dosyaları derlemek için şu komutu çalıştırın:

```bash
gcc -Wall -Wextra -o simulation main.c telemetry.c -lm
```

*(Eğer "Wall" ve "Wextra" kullanmak istemezseniz doğrudan `gcc -o simulation main.c telemetry.c` de yazabilirsiniz.)*

### Çalıştırma

Derlenen projeyi çalıştırmak için:

```bash
./simulation
```

## Örnek Kullanım

Program ilk çalıştığında güvenlik için araç sensör verilerinin simüle edilmesi istenir:

```text
Batarya Sıcaklığı (°C): 35.0
Şarj Yüzdesi (%): 85
Kapı Durumu (A/K): K
Fren Pedalı (1:Basılı, 0:Değil): 1
```

Değerler kritik eşikleri taşımaz ise araç çalışır:

```text
SONUÇ: BAŞARILI: SİSTEM HAZIR. MOTOR BAŞLATILIYOR...
```

Ardından 5 seçenekli ana menü ekrana gelir ve sürüş boyunca işlemleriniz kayıt altına alınır:

1. Gaza Bas
2. Frene Bas
3. Rejeneratif Frenleme Yap
4. Anlık Telemetri ve İstatistikleri Oku
5. Sistemi Kapat

**Örnek Sürüş Çıktısı:**

```text
--- TUFAN ELEKTROMOBİL SÜRÜŞ SİMÜLASYONU ---
1. Gaza Bas
2. Frene Bas
3. Rejeneratif Frenleme Yap
...
Seçiminiz: 1
[BİLGİ] Gaza basıldı. Araç 5.7 km/s hızlandı. Motor: 40.0°C, Batarya: 37.0°C

...
Seçiminiz: 4

--- ANLIK TELEMETRİ ---
Güncel Hız: 5.7 km/s
Batarya: %83
Motor Sıcaklığı: 40.0 °C
Batarya Sıcaklığı: 37.0 °C
-------------------------
--- SÜRÜŞ İSTATİSTİKLERİ ---
Gaza Basma Sayısı: 1 (Ortalama Artış: 5.70 km/s)
Frene Basma Sayısı: 0 (Ortalama Düşüş: 0.00 km/s)
Rejeneratif Frenleme Sayısı: 0 (Ortalama Düşüş: 0.00 km/s)
-------------------------

...
Seçiminiz: 5

[SİSTEM] Araç sistemi kapatılıyor...
[SİSTEM] Motor durduruldu. Batarya bağlantısı kesildi.
[SİSTEM] Son Durum → Hız: 5.7 km/s | Batarya: %83 | Motor: 40.0°C | Batarya Sıc.: 37.0°C
[SİSTEM] Güvenli kapanma tamamlandı. Hoşça kalın!
```

# Elektromobil Güvenlik Kontrol Sistemi

## Açıklama

Bu proje, bir elektromobilin (elektrikli araç) motoru çalıştırılmadan önce yapılması gereken güvenlik kontrollerini simüle eden bir C programıdır.

Program, kullanıcıdan 4 farklı sensör verisini alır ve belirli güvenlik kurallarına göre motorun çalıştırılıp çalıştırılamayacağına karar verir:

| Sensör               | Değişken             | Tür     | Açıklama                          |
| -------------------- | -------------------- | ------- | --------------------------------- |
| Batarya Sıcaklığı    | `batarya_sicakligi`  | `float` | Derece cinsinden (ör: 45.5)       |
| Şarj Yüzdesi         | `sarj_yuzdesi`       | `int`   | 0–100 arası yüzde değeri          |
| Kapı Durumu          | `kapi_durumu`        | `char`  | `'A'` = Açık, `'K'` = Kapalı     |
| Fren Pedalı          | `fren_pedali`        | `int`   | `1` = Basılı, `0` = Basılı Değil |

## Kontrol Kuralları (Öncelik Sırasına Göre)

1. **Sıcaklık > 60°C** → Motor aşırı ısındı, sürüş engellenir.
2. **Şarj < %10** → Batarya kritik seviyede, sürüş başlatılamaz.
3. **Kapı Açık (`'A'`)** → Kapıların kapatılması gerekir.
4. **Fren Basılı Değil (`0`)** → Frene basılması istenir.
5. **Tümü uygunsa** → Motor başlatılır.

Program ayrıca sensör verilerinin fiziksel olarak mümkün olup olmadığını kontrol eder (ör: sıcaklık -100°C ile 200°C arasında olmalıdır, şarj %0–100 arasında olmalıdır). Geçersiz bir veri girildiğinde program hata mesajı vererek sonlanır.

## Nasıl Çalıştırılır?

### Gereksinimler

- GCC derleyicisi (veya herhangi bir C derleyicisi)

### Derleme ve Çalıştırma

```bash
gcc main.c -o odev1
./odev1
```

## Örnek Çıktılar

### Başarılı Senaryo

```
--- ELEKTROMOBİL GÜVENLİK KONTROLÜ ---
Batarya Sıcaklığı (°C): 42.8
Şarj Yüzdesi (%): 85
Kapı Durumu (A/K): K
Fren Pedalı (1:Basılı, 0:Değil): 1

SONUÇ: BAŞARILI: SİSTEM HAZIR. MOTOR BAŞLATILIYOR...
```

### Geçersiz Giriş Senaryosu (Sınır değerler kontrolü)

```
--- ELEKTROMOBİL GÜVENLİK KONTROLÜ ---
Batarya Sıcaklığı (°C): 25.0
Şarj Yüzdesi (%): 120
SONUÇ: KRİTİK HATA: İmkansız şarj seviyesi (%120). Lütfen sensör verilerini kontrol edin.
```

### Kritik Isınma Senaryosu (İş akışına kontrolü)

```
--- ELEKTROMOBİL GÜVENLİK KONTROLÜ ---
Batarya Sıcaklığı (°C): 65.0
Şarj Yüzdesi (%): 50
Kapı Durumu (A/K): K
Fren Pedalı (1:Basılı, 0:Değil): 1

SONUÇ: UYARI: Motor Aşırı Isındı! Sürüş Engellendi.
SİSTEM MESAJI: Lütfen motoru başlatmadan önce yukarıdaki uyarıları giderin.
```
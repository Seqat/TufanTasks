// =============================================================================
// telemetry.h — Telemetri Modülü Başlık Dosyası
// =============================================================================
// Bu dosya, telemetry.c içindeki fonksiyonların PROTOTİPLERİNİ (imzalarını)
// ve projenin SABİTLERİNİ içerir.
//
// Neden ayrı bir .h dosyası?
// --------------------------
// main.c dosyasının, telemetry.c'de yazdığımız fonksiyonları çağırabilmesi
// için bu fonksiyonların "var olduğunu" bilmesi gerekir. #include "telemetry.h"
// satırı, derleyiciye "bu fonksiyonlar başka bir dosyada tanımlı, merak etme"
// der. Bu yapıya "Modüler Programlama" denir.
//
// #ifndef / #define / #endif (Include Guard) Nedir?
// --------------------------------------------------
// Eğer aynı .h dosyası yanlışlıkla iki kez #include edilirse, derleyici
// "fonksiyon zaten tanımlı" hatası verir. Include Guard bunu önler:
// İlk #include'da TELEMETRY_H tanımlanır, ikinci #include'da #ifndef
// kontrolü başarısız olur ve dosya tekrar okunmaz.
// =============================================================================

#ifndef TELEMETRY_H
#define TELEMETRY_H

// ---------------------------------------------------------------------------
// SABİTLER (Constants)
// ---------------------------------------------------------------------------
// Dizilerimizin (array) maksimum boyutu. 100'den fazla kayıt tutmayacağız.
// Neden #define? Çünkü C dilinde dizi boyutu derleme zamanında (compile-time)
// bilinmelidir. #define, ön-işlemci (preprocessor) tarafından derleme
// öncesinde sayıyla değiştirilir.
#define MAX_KAYIT 100

// ---------------------------------------------------------------------------
// FONKSİYON PROTOTİPLERİ (Function Prototypes / Signatures)
// ---------------------------------------------------------------------------
// Aşağıdaki satırlar fonksiyonların GÖVDESİNİ (body) içermez — sadece
// isimlerini ve parametre tiplerini bildirir. Asıl kod telemetry.c'dedir.
//
// Parametre almayan fonksiyonlara dikkat: Araç verileri (hız, batarya vb.)
// telemetry.c dosyasının İÇİNDE static değişkenler olarak tutulduğu için
// fonksiyonlara parametre olarak geçmemize gerek yoktur. Bu yaklaşıma
// "Module-Level State" veya "Kapsülleme (Encapsulation)" denir.

// Sistemi başlangıç değerleriyle kurar.
// baslangic_sicaklik: Sensörden okunan batarya sıcaklığı (°C)
// baslangic_sarj:     Sensörden okunan şarj yüzdesi (%)
void sistemi_kur(float baslangic_sicaklik, int baslangic_sarj);

// Gaza basarak aracı hızlandırır.
void gaza_bas(void);

// Frene basarak aracı yavaşlatır.
void frene_bas(void);

// Rejeneratif frenleme uygular (yavaşlatır + batarya şarj eder).
void rejen_fren(void);

// Anlık telemetri verilerini ve sürüş istatistiklerini ekrana yazdırır.
void telemetri_ve_istatistik_yazdir(void);

// Sistemi güvenli bir şekilde kapatır.
void sistemi_kapat(void);

// Aşırı ısınma kontrolü yapar.
// Dönüş: 1 = Motor veya Batarya aşırı ısındı (ACİL DURUM)
//         0 = Sıcaklıklar normal aralıkta
int asiri_isinma_kontrol(void);

#endif // TELEMETRY_H

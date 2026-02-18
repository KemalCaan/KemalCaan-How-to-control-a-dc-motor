# STM32 ile DC Motor Hız ve Yön Kontrol Sistemi
# (DC Motor Speed & Direction Control using STM32 PWM)

![Platform](https://img.shields.io/badge/Platform-STM32%20F1%2F%20F4-blue?style=for-the-badge&logo=stmicroelectronics)
![Language](https://img.shields.io/badge/Language-C%20%2F%20C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B)
![Driver](https://img.shields.io/badge/Driver-L298N%20%2F%20L293D-red?style=for-the-badge&logo=arduino)
![License](https://img.shields.io/badge/License-MIT-orange?style=for-the-badge)

## 📖 Proje Özeti (Abstract)

Bu proje, **Gömülü Sistemler ve Motor Sürücü** uygulamaları kapsamında **Kemal Can Güngör** tarafından geliştirilmiştir.

Proje, fırçalı bir DC motorun (Brushed DC Motor) hızını ve dönüş yönünü **STM32** mikrodenetleyicisi kullanarak hassas bir şekilde kontrol etmeyi amaçlar. Hız kontrolü için **Timer (TIM)** birimleri üzerinden üretilen **PWM (Pulse Width Modulation)** sinyalleri kullanılırken, yön kontrolü H-Köprüsü (H-Bridge) topolojisine sahip bir sürücü entegresi (L298N veya L293D) üzerinden GPIO pinleri ile sağlanır.

Sistem, endüstriyel robotik kollar, otonom araçlar (RC Car) ve fan kontrol sistemleri için temel bir altyapı sunar.

---

## ✨ Temel Özellikler (Key Features)

* **Hassas Hız Kontrolü:** Timer çevre birimi ile %0-%100 arasında ayarlanabilir PWM Duty Cycle (Görev Döngüsü).
* **Çift Yönlü Hareket:** H-Köprüsü sürücüsü sayesinde Saat Yönü (CW) ve Saat Yönünün Tersi (CCW) dönüş yeteneği.
* **Yumuşak Başlangıç (Soft Start):** Ani akım çekimlerini (Inrush Current) engellemek için kademeli hızlanma algoritması.
* **Donanım Koruması:** Sürücü entegresi ile mikrodenetleyici arasında optik veya diyot tabanlı geri besleme koruması (Back-EMF Protection).
* **Modüler Yapı:** Farklı motor sürücüler (L298N, TB6612FNG) ile uyumlu esnek kod mimarisi.

---

## 🛠️ Sistem Mimarisi (System Architecture)

Sistem, kontrol sinyallerini üreten dijital katman ve yüksek akımı yöneten güç katmanından oluşur.

### 1. Donanım Bileşenleri (Hardware Stack)
| Bileşen | Teknik Detay | Görevi |
| :--- | :--- | :--- |
| **Mikrodenetleyici** | STM32 Serisi (F103/F407) | PWM sinyal üretimi ve lojik kontrol. |
| **Motor Sürücü** | L298N / L293D Modülü | Düşük güçlü PWM sinyalini yüksek akımlı motor sürüşüne çevirir. |
| **Eyleyici** | 12V/24V DC Motor | Kontrol edilen mekanik yük. |
| **Güç Kaynağı** | Harici Batarya / Adaptör | Motorlar için izole güç sağlar. |

#### Donanım Bağlantı Şeması (Wiring Diagram)
Aşağıdaki şema, STM32 ile Motor Sürücü arasındaki sinyal akışını göstermektedir:

```mermaid
graph LR
    subgraph KONTROL_ÜNİTESİ
    STM32[STM32 MCU]
    GPIO1[GPIO - IN1]
    GPIO2[GPIO - IN2]
    PWM[TIMx_CHx - ENA]
    end
    
    subgraph GÜÇ_KATI
    DRIVER[L298N Motor Sürücü]
    PSU[Harici Güç Kaynağı]
    end
    
    subgraph EYLEYİCİ
    MOTOR((DC Motor))
    end

    %% Bağlantılar
    STM32 -- Yön Sinyali --> GPIO1 --> DRIVER
    STM32 -- Yön Sinyali --> GPIO2 --> DRIVER
    STM32 -- PWM Hız Sinyali --> PWM --> DRIVER
    
    PSU -- 12V/24V Besleme --> DRIVER
    DRIVER -- Yüksek Akım --> MOTOR

    %% Renk Tanımlamaları (Mor/Pembe - Turuncu Tonlar)
    style MOTOR fill:#E1BEE7,stroke:#8E24AA,stroke-width:2px,color:#000
    style DRIVER fill:#FFCCBC,stroke:#D84315,stroke-width:2px
```

### 2. Yazılım Teknolojileri (Software Stack)
* **Dil:** Embedded C
* **Sürücüler:** STM32 HAL (Hardware Abstraction Layer)
* **Periferik:** TIM (Timer) PWM Generation, GPIO Output
* **IDE:** STM32CubeIDE

---

## 🔄 Algoritma Akış Şeması (Methodology & Workflow)

Yazılım, kullanıcıdan gelen hız talebini işleyerek uygun PWM sinyalini üretir.

**(Kontrol Algoritması)**

```mermaid
graph TD
    %% Başlangıç
    Start([Başlangıç]) --> Init["Sistem Kurulumu<br>(GPIO & Timer PWM)"]
    
    %% Girdi Okuma
    Init --> Input{"Hız Talebi Oku<br>(ADC / UART)"}
    
    %% Yön Kararı
    Input --> Direction{"Hız > 0 mı?"}
    
    %% İleri Yön
    Direction -- Evet (Pozitif) --> SetCW["Yön: İLERİ (CW)<br>IN1=1, IN2=0"]
    SetCW --> CalcPWM1["Duty Cycle Hesapla<br>CCR = Hız Değeri"]
    
    %% Geri Yön
    Direction -- Hayır (Negatif) --> SetCCW["Yön: GERİ (CCW)<br>IN1=0, IN2=1"]
    SetCCW --> CalcPWM2["Duty Cycle Hesapla<br>CCR = |Hız Değeri|"]
    
    %% Motor Sürüşü
    CalcPWM1 --> Update["Timer CCR Güncelle<br>__HAL_TIM_SET_COMPARE"]
    CalcPWM2 --> Update
    
    Update --> MotorRun["Motor Dönüyor"]
    MotorRun --> Input

    %% --- Profesyonel Renk Tanımlamaları ---
    classDef process fill:#ECEFF1,stroke:#546E7A,stroke-width:1px,color:#263238;
    classDef decision fill:#FFF8E1,stroke:#FFC107,stroke-width:1px,color:#263238;
    classDef output fill:#E8F5E9,stroke:#43A047,stroke-width:2px,color:#1B5E20;
    classDef init fill:#F5F5F5,stroke:#9E9E9E,stroke-width:1px,color:#616161;

    class Init,Input init;
    class SetCW,SetCCW,CalcPWM1,CalcPWM2,Update process;
    class Direction decision;
    class MotorRun output;
```

---

## 💻 Kod Örneği (Code Snippet)

Motor hızını ayarlamak için kullanılan temel HAL fonksiyonu:

```c
// Motor Yönünü Ayarla (İleri)
HAL_GPIO_WritePin(GPIOA, IN1_PIN, GPIO_PIN_SET);
HAL_GPIO_WritePin(GPIOA, IN2_PIN, GPIO_PIN_RESET);

// PWM Sinyalini Başlat ve Hızı Ayarla (0-1000 arası)
HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
__HAL_TIM_SET_COMPARE(&htim2, TIM_CHANNEL_1, 500); // %50 Hız
```

---

## 💻 Kurulum ve Kullanım (Build & Flash)

Projeyi çalıştırmak için:

1.  **Depoyu Klonlayın:**
    ```bash
    git clone [https://github.com/KemalCaan/KemalCaan-How-to-control-a-dc-motor.git](https://github.com/KemalCaan/KemalCaan-How-to-control-a-dc-motor.git)
    ```

2.  **STM32CubeIDE ile Açın:**
    * `.ioc` dosyasını açarak Timer ayarlarını (Prescaler/Period) motorunuza göre düzenleyin.
    
3.  **Derleme ve Yükleme:**
    * Kodları derleyin ve ST-Link ile yükleyin.
    * PWM pinini sürücünün `ENA` veya `ENB` pinine bağlamayı unutmayın.

---

## 👨‍💻 Geliştirici (Developer)

**Kemal Can Güngör**
* **Unvan:** Elektrik-Elektronik Mühendisi
* **Uzmanlık:** Gömülü Yazılım, Motor Kontrol, Güç Elektroniği
* **İletişim:** [LinkedIn Profilim](https://www.linkedin.com/in/kemal-can-g%C3%BCng%C3%B6r-4598b4234/)

---

## 📄 Lisans

Bu proje **MIT Lisansı** altında açık kaynak olarak sunulmuştur.

# Mini Kabuk (Shell) Uygulaması

## Proje Hakkında
Bu proje, bir **mini kabuk (shell)** uygulaması geliştirmeyi amaçlar. Linux üzerinde çalışan bu kabuk, kullanıcıdan komut alır, bu komutları yorumlar ve yürütür. Proje kapsamında, **proses yönetimi**, **I/O yönlendirme**, **boru (pipe)** ve **arka plan işlemleri** gibi temel işletim sistemi mekanizmaları uygulanmıştır.

---

## Özellikler
- **Tekli Komut Çalıştırma**: `ls -l`
- **Giriş/Çıkış Yönlendirme**: 
  - `echo "Deneme" > test.txt`
  - `cat < test.txt`
- **Boru Kullanımı**: `echo "12" | wc -l`
- **Arka Plan İşlemleri**: `sleep 5 &`
- **Çoklu Komut Çalıştırma**: `echo 12; sleep 2; echo 13`
- **Kabuktan Çıkış**: `quit`

---

## Dosya Yapısı

project/
├── bin/           # Çalıştırılabilir dosya
├── doc/           
├── include/       # Başlık dosyaları (.h)
├── lib/           # Makine kodları (.o)
├── src/           # Kaynak dosyalar (.c)
├── Makefile       # Derleme için Makefile
└── README.md      # Bu dosya
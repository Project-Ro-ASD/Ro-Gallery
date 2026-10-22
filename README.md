# Ro-Gallery

Ro-ASD için C++17 ve Qt 6 Widgets ile geliştirilen yerel fotoğraf kitaplığı ve
görüntüleyici. Proje geliştirme aşamasındadır; henüz Ro-Repo üzerinden yayınlanmaz.

Fotoğrafları seçilen klasörlerden tarar, tarihe göre gruplar, dosya adına göre
arar ve favorilere ekler. Görüntüleyicide yakınlaştırma, kaydırma ve klavye ile
gezinme bulunur. Fotoğraflar bilgisayarda kalır; **Upload** eylemi dosyaları ilk
kaynak klasöre kopyalar. Video ve bazı yardımcı gezinme öğeleri henüz uygulanmamıştır.

## Fedora 44 üzerinde geliştirme

```bash
sudo dnf install gcc-c++ cmake ninja-build qt6-qtbase-devel qt6-qtimageformats \
    python3 desktop-file-utils appstream rpm-build
./run.sh
```

```bash
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release
cmake --build build --parallel 2
ctest --test-dir build --output-on-failure
./build/ro-gallery
```

Desteklenen görüntü biçimleri kurulu Qt görüntü eklentilerine bağlıdır.
`./install.sh` uygulamayı mevcut kullanıcı için `~/.local` altına kurar.
Paket kurulumunda CMake, `/usr` öneki ve `DESTDIR` kullanılır; derleme sırasında
kullanıcının ev dizinine veya sistemine dosya yüklenmez.

## Kullanım

- **Sources +** ile fotoğraf klasörlerini ekleyin; alt klasörler de taranır.
- **Years / Months / Days** ile gruplamayı, sürgü ile küçük resim boyutunu değiştirin.
- Fotoğrafa çift tıklayarak görüntüleyiciyi açın. Tekerlek yakınlaştırır;
  sürükleme görüntüyü kaydırır; `←` / `→` gezinir, `Esc` kapatır.
- `F` favori durumunu, `Del` uygulama içindeki silindi işaretini değiştirir.

Silme işlemi dosyayı fiziksel olarak silmez veya sistem çöp kutusuna taşımaz.
**Deleted** görünümünde uygulama içi işaretleme kullanılır.

| Veri | Varsayılan konum |
| --- | --- |
| Ayarlar, kaynaklar ve favoriler | `~/.config/Ro-ASD/ro-gallery.ini` |
| Küçük resim önbelleği | `~/.cache/ro-gallery/thumbs/` |

XDG ortam değişkenleri desteklenir. Önceki prototipin ayarları otomatik taşınmaz.

## Kontroller ve paketleme

```bash
bash tools/project-ci
bash tools/build-rpm 0.1.0 x86_64 "$PWD/release-rpms"
```

RPM derlemesi Fedora 44 üzerinde, hedef mimaride çalıştırılır; `x86_64` ve
`aarch64` desteklenir. Çıktı dizini boş olmalıdır. Her derleme bir `ro-gallery`
ikili RPM'si ve ona karşılık gelen SRPM üretir. Yerel araçlar paket yüklemez;
CI konteynerlerinde bağımlılıklar otomatik kurulur.

GitHub CI'nin sabit sonuç kontrolü **ro-app-gate** olarak korunur.
`tools/project-ci` sözleşme, masaüstü/AppStream doğrulaması, derleme, izole
uygulama açılış testi ve geçici dizine kurulum kontrolünü çalıştırır.
Testler kişisel fotoğrafları veya mevcut kullanıcı ayarlarını okumaz.

`vX.Y.Z` etiketi mevcut sürüm iş akışını başlatır: iki mimari için Fedora 44
RPM'leri, SRPM, `SHA256SUMS`, `component-artifact-manifest-v1.json` ve GitHub
attestation üretilir. Üretim imzalama anahtarları Ro-Repo'da kalır.
Etiket oluşturmak yayın işlemidir; geliştirme aktarımı için etiket gerekmez.

[Ro-Repo uyumu ve kalan kabul adımları](docs/RO-REPO.md).

## Kaynak düzeni

| Dizin / dosya | Görev |
| --- | --- |
| `src/` | Qt arayüzü, kitaplık, ayarlar ve küçük resim önbelleği |
| `assets/ro-gallery.svg` | Uygulama simgesi |
| `ro-gallery.desktop`, `data/` | Linux masaüstü ve AppStream entegrasyonu |
| `packaging/ro-gallery.spec` | Fedora RPM tarifi |
| `.roasd/app.json` | Ro-ASD uygulama sözleşmesi |
| `tools/` | CI, sözleşme doğrulaması ve RPM üretimi |
| `tests/` | İzole uygulama açılış ve görüntüleme testi |

Yerel tasarım referansları, kök dizindeki raster görseller, örnek fotoğraf
üreticisi ve derleme çıktıları `.gitignore` ile dışlanır. SRPM kaynağı yalnız
uygulama için açıkça seçilmiş dosyalardan hazırlanır.

Lisans: [GPL-3.0-or-later](LICENSE).

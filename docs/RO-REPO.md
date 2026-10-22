# Ro-Repo entegrasyonu

Bu uygulamanın kaynak reposu `Project-Ro-ASD/Ro-Gallery`, paket ve bileşen
kimliği `ro-gallery`, görünen adı **Ro-Gallery** olarak tanımlanır.
Fedora 44 hedeflenir; mimariler `x86_64` ve `aarch64`'tür.

## Kaynak repo sözleşmesi

Ro-Gallery'nin mevcut şablon geçmişi, `ro-app-gate`, sabitlenmiş GitHub Actions
sürümleri ve `.github/workflows/release.yml` yolu korunur. Şablondaki boş
`tools/project-ci`, `tools/build-rpm` ve uygulama metaverisi uygulanmıştır.
CMake, masaüstü girdisini, simgeyi ve AppStream metaverisini standart konumlara
kurar. RPM üretimi kullanıcının kişisel dosyalarını kaynağa dahil etmez.
Ro-Repo ile uyum için lisans GPL-3.0-or-later olarak belirtilmiştir.

## Üretici kaydı hâlâ gerekli

23 Ekim 2026 tarihinde incelenen Ro-Repo `config/producers-v2.json` içinde
Ro-Gallery bulunmuyor. Kaynak aktarımı bu uygulamayı otomatik olarak güvenilir
üretici yapmaz. Yeni kabul işlemleri V2 üretici kaydını kullanır; şablonun eski
V1 kayıt açıklaması yeni üretici eklemek için esas alınmamalıdır.

Ro-Repo'da incelenerek eklenmesi gereken kayıt önerisi:

```json
{
  "repository": "Project-Ro-ASD/Ro-Gallery",
  "owner": "Project-Ro-ASD/Ro-Gallery maintainers",
  "components": [{
    "component": "ro-gallery",
    "package_names": ["ro-gallery"],
    "architectures": ["x86_64", "aarch64"],
    "risk_class": "normal-app",
    "promotion_group": "ro-gallery",
    "required_tests": ["dependency-solve", "clean-install", "upgrade", "file-conflict", "rpmlint", "smoke"],
    "srpm_required": true,
    "sbom_required": false,
    "allow_fedora_override": false,
    "trusted_signer_workflow": "Project-Ro-ASD/Ro-Gallery/.github/workflows/release.yml",
    "require_complete_architecture_set": true
  }]
}
```

Bu belge canlı üretici kaydı veya test sonucu değildir. Kayıt, yetkili Ro-Repo
bakımcılarının incelemesiyle yapılmalıdır.

## Yayına geçiş sırası

1. Kaynak değişiklikleri ve GitHub `ro-app-gate` başarılı olmalı.
2. İki mimaride RPM derlemesi ve uygulama testi doğrulanmalı.
3. Üretici kaydı Ro-Repo'da incelenip kabul edilmeli.
4. Sürüm etiketiyle üretilen sabit release, manifest, hash ve provenance kabul
   araçlarıyla doğrulanmalı; bağımlılık çözümü, temiz kurulum, yükseltme, dosya
   çakışması, rpmlint ve uygulama açılış kanıtları tamamlanmalı.
5. Merkezi imzalama ve beta kabulü Ro-Repo üzerinden yapılmalı. Stable geçişi
   Ro-Repo'nun yürürlükteki onay ve bekleme kurallarına tabi olmalı.

Ro-Repo'nun kökündeki tarihsel `x86_64/`, `aarch64/`, `noarch/` ve `SRPMS/`
dizinlerine paket kopyalanmaz. İmzalama anahtarları uygulama reposuna eklenmez.
Bu geliştirme aktarımı üretim kanalını değiştirmez.

## Esas alınan kaynaklar

- [Ro-Gallery sürüm iş akışı](https://github.com/Project-Ro-ASD/Ro-Gallery/blob/main/.github/workflows/release.yml)
- [Ro-Repo genel sözleşmesi](https://github.com/Project-Ro-ASD/Ro-Repo/blob/main/README.md)
- [Üretici kayıt biçimi](https://github.com/Project-Ro-ASD/Ro-Repo/blob/main/docs/PRODUCER-REGISTRY-V2.md)
- [Güncel üretici kaydı](https://github.com/Project-Ro-ASD/Ro-Repo/blob/main/config/producers-v2.json)
- [Bileşen bazlı terfi sözleşmesi](https://github.com/Project-Ro-ASD/Ro-Repo/blob/main/docs/V3-COMPONENT-PROMOTION-CONTRACT.md)

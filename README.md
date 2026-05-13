<p align="center">
   <img src="https://github.com/user-attachments/assets/5cd63373-e806-474f-94ec-6e04963bf90f"
        alt="Video2X: Framework do super rozdzielczości wideo i interpolacji klatek oparty na uczeniu maszynowym."/>
   </br>
   <img src="https://img.shields.io/github/v/release/k4yt3x/video2x?style=flat-square"/>
   <img src="https://img.shields.io/github/downloads/k4yt3x/video2x/total?style=flat-square"/>
   <img src="https://img.shields.io/github/license/k4yt3x/video2x?style=flat-square"/>
   <img src="https://img.shields.io/github/sponsors/k4yt3x?style=flat-square&link=https%3A%2F%2Fgithub.com%2Fsponsors%2Fk4yt3x"/>
   <img src="https://img.shields.io/badge/dynamic/json?color=%23e85b46&label=Patreon&query=data.attributes.patron_count&suffix=%20patrons&url=https%3A%2F%2Fwww.patreon.com%2Fapi%2Fcampaigns%2F4507807&style=flat-square"/>
</p>

## 🌟 Wersja 6.0.0

Najważniejsze zmiany w Video2X 6.0.0:

- Całkowite przepisanie projektu Video2X w C/C++.
- Szybsza i bardziej wydajna architektura.
- Wsparcie dla wielu platform: Windows i Linux.
- Znacznie lepsza jakość wyjściowa.
- Nowy interfejs graficzny (GUI) i instalator dla łatwej konfiguracji w systemie Windows.

<details>
<summary>Kliknij, aby zobaczyć więcej szczegółów</summary>

Wersja 6.0.0 to całkowite przepisanie projektu w C/C++. Ona:

- naprawdę działa tym razem, z znacznie mniejszymi problemami w porównaniu do bety 5.0.0;
- jest błyskawicznie szybka, dzięki nowemu zoptymalizowanemu potokowi przetwarzania i wydajności C/C++;
- jest wieloplatformowa, dostępna teraz zarówno dla Windows, jak i Linux;
- oferuje znacznie lepszą jakość wyjściową dzięki Anime4K v4, Real-ESRGAN, Real-CUGAN i RIFE;
- obsługuje dwa tryby: filtrowanie (skalowanie w górę) i interpolację klatek;
- obsługuje Anime4K v4 i wszystkie niestandardowe shadery GLSL zgodne z MPV;
- obsługuje Real-ESRGAN, Real-CUGAN i RIFE (wszystkie modele) przez ncnn i Vulkan;
- nie wymaga dodatkowego miejsca na dysku podczas przetwarzania – tylko miejsce na końcowy plik wyjściowy.

</details>

![6.4.0-screenshot](https://github.com/user-attachments/assets/9b1cc8a7-2903-4d2c-80a2-8d81f007e45b)

## 🖥️ Wymagania sprzętowe

Twój system musi spełniać poniższe minimalne wymagania sprzętowe, aby uruchomić Video2X.

- **Procesor (CPU)**
  - Prekompilowane pliki binarne wymagają procesorów z obsługą AVX2.
  - **Intel**: Haswell (Q2 2013) lub nowszy
  - **AMD**: Excavator (Q2 2015) lub nowszy
- **Karta graficzna (GPU)**
  - GPU musi obsługiwać Vulkan.
  - **NVIDIA**: Kepler (seria GTX 600, Q2 2012) lub nowszy
  - **AMD**: GCN 1.0 (seria Radeon HD 7000, Q1 2012) lub nowszy
  - **Intel**: HD Graphics 4000 (Q2 2012) lub nowszy

## [🪟 Instalacja w systemie Windows](https://docs.video2x.org/installing/windows-qt6.html)

**[Pobierz najnowszy instalator dla systemu Windows (6.4.0)](https://github.com/k4yt3x/video2x/releases/download/6.4.0/video2x-qt6-windows-amd64-installer.exe)**

Najnowszą wersję dla systemu Windows możesz pobrać ze [strony wydań](https://github.com/k4yt3x/video2x/releases/latest). Aby zapoznać się z podstawową obsługą interfejsu graficznego, przejdź do [dokumentacji](https://docs.video2x.org/running/desktop.html). Jeśli nie możesz pobrać bezpośrednio z GitHub, spróbuj skorzystać z [serwera lustrzanego](https://files.k4yt3x.com). Interfejs graficzny obsługuje obecnie następujące języki:

- English (United States)
- 简体中文（中国）
- 日本語（日本）
- Português (Portugal)
- Français (France)
- Deutsch (Deutschland)

## [🐧 Instalacja w systemie Linux](https://docs.video2x.org/installing/linux.html)

Pakiety Video2X są dostępne dla dystrybucji Linuksa wymienionych poniżej. Dla innych dystrybucji dostępny jest również uniwersalny AppImage. Jeśli chcesz zbudować program ze źródeł, zapoznaj się z plikiem [PKGBUILD](packaging/arch/PKGBUILD), aby uzyskać ogólny przegląd wymaganych zależności i poleceń.

- Arch Linux: pakiety AUR, utrzymywane przez [@K4YT3X](https://github.com/k4yt3x).
  - [aur/video2x](https://aur.archlinux.org/packages/video2x)
  - [aur/video2x-git](https://aur.archlinux.org/packages/video2x-git)
  - [aur/video2x-qt6](https://aur.archlinux.org/packages/video2x-qt6)
  - [aur/video2x-qt6-git](https://aur.archlinux.org/packages/video2x-qt6-git)
- Arch Linux (kontynentalne Chiny): pakiety archlinuxcn, utrzymywane przez [@Integral-Tech](https://github.com/Integral-Tech).
  - [archlinuxcn/video2x](https://github.com/archlinuxcn/repo/tree/master/archlinuxcn/video2x)
  - [archlinuxcn/video2x-git](https://github.com/archlinuxcn/repo/tree/master/archlinuxcn/video2x-git)
  - [archlinuxcn/video2x-qt6](https://github.com/archlinuxcn/repo/tree/master/archlinuxcn/video2x-qt6)
  - [archlinuxcn/video2x-qt6-git](https://github.com/archlinuxcn/repo/tree/master/archlinuxcn/video2x-qt6-git)
- Inne dystrybucje: `Video2X-x86_64.AppImage` dostępny na [stronie wydań](https://github.com/k4yt3x/video2x/releases/latest).

## [📦 Obraz kontenera](https://docs.video2x.org/running/container.html)

[Obrazy kontenerów](https://github.com/k4yt3x/video2x/pkgs/container/video2x) Video2X są dostępne w GitHub Container Registry, co ułatwia wdrożenie na systemach Linux i macOS. Jeśli masz już zainstalowany Docker/Podman, wystarczy jedno polecenie, aby rozpocząć skalowanie wideo. Więcej informacji na temat korzystania z obrazu Docker Video2X znajdziesz w [dokumentacji](https://docs.video2x.org/running/container.html).

## [📔 Google Colab](https://colab.research.google.com/drive/1gWEwcA9y57EsxwOjmLNmNMXPsafw0kGo)

Możesz używać Video2X na [Google Colab](https://colab.research.google.com/) **bezpłatnie**, jeśli nie posiadasz wydajnej karty graficznej. Możesz korzystać z mocnego GPU (NVIDIA T4, L4 lub A100) na serwerach Google bezpłatnie przez maksymalnie 12 godzin na sesję. **Prosimy o uczciwe korzystanie z darmowych zasobów** – nie twórz sesji jedna po drugiej i nie uruchamiaj skalowania 24/7. Może to skutkować zablokowaniem konta. Możesz wykupić [Colab Pro/Pro+](https://colab.research.google.com/signup/pricing), jeśli chcesz korzystać z lepszych GPU i dłuższych sesji. Instrukcje użytkowania są zawarte w [notatniku Colab](https://colab.research.google.com/drive/1gWEwcA9y57EsxwOjmLNmNMXPsafw0kGo).

## [💬 Grupa dyskusyjna na Telegramie](https://t.me/video2x)

Dołącz do naszej grupy dyskusyjnej na Telegramie, aby zadać pytania dotyczące Video2X, porozmawiać bezpośrednio z deweloperami lub dyskutować o technologiach super rozdzielczości, interpolacji klatek lub przyszłości Video2X.

## [📖 Dokumentacja](https://docs.video2x.org/)

Pełna dokumentacja Video2X jest dostępna pod adresem [https://docs.video2x.org/](https://docs.video2x.org/). Zawiera szczegółowe instrukcje dotyczące [budowania](https://docs.video2x.org/building/index.html), [instalacji](https://docs.video2x.org/installing/index.html), [użytkowania](https://docs.video2x.org/running/index.html) i [rozwijania](https://docs.video2x.org/developing/index.html) tego programu.

## 📽️ Dema wideo (przestarzałe)

![Spirited Away Demo](https://user-images.githubusercontent.com/21986859/49412428-65083280-f73a-11e8-8237-bb34158a545e.png)\
_Demo skalowania: zwiastun filmu „Spirited Away"_

- **Spirited Away**: [YouTube](https://youtu.be/mGEfasQl2Zo) | [Bilibili](https://www.bilibili.com/video/BV1V5411471i/)
  - Z 360P do 4K
  - Prawa autorskie do [oryginalnego wideo](https://www.youtube.com/watch?v=ByXuk9QqQkk) należą do 株式会社スタジオジブリ
- **Bad Apple!!**: [YouTube](https://youtu.be/A81rW_FI3cw) | [Bilibili](https://www.bilibili.com/video/BV16K411K7ue)
  - Z 384P 30 FPS do 4K 120 FPS przy użyciu waifu2x i DAIN
  - Prawa autorskie do [oryginalnego wideo](https://www.nicovideo.jp/watch/sm8628149) należą do あにら
- **The Pet Girl of Sakurasou**: [YouTube](https://youtu.be/M0vDI1HH2_Y) | [Bilibili](https://www.bilibili.com/video/BV14k4y167KP/)
  - Z 240P 29.97 do 1080P 60 FPS przy użyciu waifu2x i DAIN
  - Prawa autorskie do oryginalnego wideo należą do ASCII Media Works

### Standardowy klip testowy

Poniższy klip może być użyty do sprawdzenia, czy Twoja konfiguracja działa poprawnie. Jest to również standardowy klip używany do testów wydajności.

- [Standardowy klip testowy (240P)](https://files.k4yt3x.com/resources/videos/standard-test.mp4) 4,54 MiB
- [Przykład skalowania Real-CUGAN (1704P)](https://files.k4yt3x.com/resources/videos/standard-realcugan.mp4) 3,5 MiB
- [Przykład skalowania Real-ESRGAN (1704P)](https://files.k4yt3x.com/resources/videos/standard-realesrgan.mp4) 3,1 MiB
- [Przykład skalowania waifu2x (1080P)](https://files.k4yt3x.com/resources/videos/standard-waifu2x.mp4) 4,54 MiB
- [Oryginał w wysokiej rozdzielczości (1080P)](https://files.k4yt3x.com/resources/videos/standard-original.mp4) 22,2 MiB

Oryginalny klip pochodzi z anime „さくら荘のペットな彼女."\
Prawa autorskie do tego klipu należą do 株式会社アニプレックス.

## ⚖️ Licencja

Ten projekt jest licencjonowany na warunkach [GNU AGPL w wersji 3](https://www.gnu.org/licenses/agpl-3.0.txt).\
Copyright (C) 2018-2025 K4YT3X i [współtwórcy](https://github.com/k4yt3x/video2x/graphs/contributors).

![AGPLv3](https://www.gnu.org/graphics/agplv3-155x51.png)

Ten projekt zawiera lub jest zależny od następujących projektów:

| Projekt                                                                               | Licencja        |
| ------------------------------------------------------------------------------------- | --------------- |
| [FFmpeg/FFmpeg](https://www.ffmpeg.org/)                                              | LGPLv2.1, GPLv2 |
| [Tencent/ncnn](https://github.com/Tencent/ncnn)                                       | BSD 3-Clause    |
| [bloc97/Anime4K](https://github.com/bloc97/Anime4K)                                   | MIT License     |
| [nihui/realcugan-ncnn-vulkan](https://github.com/nihui/realcugan-ncnn-vulkan)         | MIT License     |
| [nihui/rife-ncnn-vulkan](https://github.com/nihui/rife-ncnn-vulkan)                   | MIT License     |
| [xinntao/Real-ESRGAN-ncnn-vulkan](https://github.com/xinntao/Real-ESRGAN-ncnn-vulkan) | MIT License     |

Więcej informacji o licencjach można znaleźć w pliku [NOTICE](NOTICE).

## 🌺 Podziękowania

Szczególne podziękowania dla następujących osób za ich znaczący wkład w projekt, wymienionych w kolejności alfabetycznej.

- [@ArchieMeng](https://github.com/archiemeng)
- [@BrianPetkovsek](https://github.com/BrianPetkovsek)
- [@Integral-Tech](https://github.com/Integral-Tech)
- [@ddouglas87](https://github.com/ddouglas87)
- [@lhanjian](https://github.com/lhanjian)
- [@nihui](https://github.com/nihui)
- [@sat3ll](https://github.com/sat3ll)

#include "argparse.h"

#include <iostream>

#if _WIN32
#include <Windows.h>
#include <cwchar>
#endif

#include <libvideo2x/logger_manager.h>
#include <libvideo2x/version.h>
#include <spdlog/spdlog.h>
#include <boost/program_options.hpp>

#include "validators.h"
#include "vulkan_utils.h"

#ifdef _WIN32
#define BOOST_PROGRAM_OPTIONS_WCHAR_T
#define PO_STR_VALUE po::wvalue
#else
#define PO_STR_VALUE po::value
#endif

namespace po = boost::program_options;

#ifdef _WIN32
std::string wstring_to_u8string(const std::wstring& wstr) {
    if (wstr.empty()) {
        return std::string();
    }
    int size_needed = WideCharToMultiByte(
        CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()), nullptr, 0, nullptr, nullptr
    );
    std::string converted_str(size_needed, 0);
    WideCharToMultiByte(
        CP_UTF8,
        0,
        wstr.data(),
        static_cast<int>(wstr.size()),
        &converted_str[0],
        size_needed,
        nullptr,
        nullptr
    );
    return converted_str;
}
#else
std::string wstring_to_u8string(const std::string& str) {
    return str;
}
#endif

int parse_args(
    int argc,
#ifdef _WIN32
    wchar_t* argv[],
#else
    char* argv[],
#endif
    Arguments& arguments,
    video2x::processors::ProcessorConfig& proc_cfg,
    video2x::encoder::EncoderConfig& enc_cfg
) {
    try {
        // clang-format off
        po::options_description all_opts("Opcje ogólne");
        all_opts.add_options()
            ("help", "Wyświetl tę stronę pomocy")
            ("version,V", "Wydrukuj wersję programu i wyjdź")
            ("log-level", PO_STR_VALUE<video2x::fsutils::StringType>()
                ->default_value(STR("info"), "info"),
                "Ustaw poziom szczegółowości (trace, debug, info, warn, error, critical, none)")
            ("no-progress", po::bool_switch(&arguments.no_progress),
                "Nie wyświetlaj paska postępu")
            ("list-devices,l", "Wyświetl dostępne urządzenia Vulkan (GPU)")

            // Ogólne opcje przetwarzania
            ("input,i", PO_STR_VALUE<video2x::fsutils::StringType>()->required(),
                "Ścieżka do wejściowego pliku wideo")
            ("output,o", PO_STR_VALUE<video2x::fsutils::StringType>()->required(),
                "Ścieżka do wyjściowego pliku wideo")
            ("processor,p", PO_STR_VALUE<video2x::fsutils::StringType>()->required(),
                "Procesor do użycia (libplacebo, realesrgan, realcugan, rife)")
            ("hwaccel,a", PO_STR_VALUE<video2x::fsutils::StringType>()
                ->default_value(STR("none"), "none"), "Metoda akceleracji sprzętowej (dekodowanie)")
            ("device,d", po::value<uint32_t>(&arguments.vk_device_index)->default_value(0),
                "Indeks urządzenia Vulkan (ID GPU)")
            ("benchmark,b", po::bool_switch(&arguments.benchmark),
                "Odrzucaj przetworzone klatki i obliczaj średnie FPS; "
                "przydatne do wykrywania wąskich gardeł kodera")
        ;

        po::options_description encoder_opts("Opcje kodera");
        encoder_opts.add_options()
            ("codec,c", PO_STR_VALUE<video2x::fsutils::StringType>()
                ->default_value(STR("libx264"), "libx264"), "Kodek wyjściowy")
            ("no-recalculate-pts", "Nie przeliczaj znaczników czasu prezentacji")
            ("no-copy-audio-streams", "Nie kopiuj strumieni audio")
            ("no-copy-subtitle-streams", "Nie kopiuj strumieni napisów")
            ("pix-fmt", PO_STR_VALUE<video2x::fsutils::StringType>(), "Format pikseli wyjściowych")
            ("bit-rate", po::value<int64_t>(&enc_cfg.bit_rate)->default_value(0),
                "Szybkość transmisji w bitach na sekundę")
            ("rc-buffer-size", po::value<int>(&enc_cfg.rc_buffer_size)->default_value(0),
                "Rozmiar bufora kontroli przepływności w bitach")
            ("rc-min-rate", po::value<int>(&enc_cfg.rc_min_rate)->default_value(0),
                "Minimalna kontrola przepływności")
            ("rc-max-rate", po::value<int>(&enc_cfg.rc_max_rate)->default_value(0),
                "Maksymalna kontrola przepływności")
            ("qmin", po::value<int>(&enc_cfg.qmin)->default_value(-1), "Minimalny kwantyzator")
            ("qmax", po::value<int>(&enc_cfg.qmax)->default_value(-1), "Maksymalny kwantyzator")
            ("gop-size", po::value<int>(&enc_cfg.gop_size)->default_value(-1),
                "Rozmiar grupy obrazów")
            ("max-b-frames", po::value<int>(&enc_cfg.max_b_frames)->default_value(-1),
                "Maksymalna liczba klatek B")
            ("keyint-min", po::value<int>(&enc_cfg.keyint_min)->default_value(-1),
                "Minimalny odstęp między klatkami kluczowymi")
            ("refs", po::value<int>(&enc_cfg.refs)->default_value(-1),
                "Liczba klatek referencyjnych")
            ("thread-count", po::value<int>(&enc_cfg.thread_count)->default_value(0),
                "Liczba wątków do kodowania")
            ("delay", po::value<int>(&enc_cfg.delay)->default_value(0),
                "Opóźnienie w milisekundach dla kodera")

            // Dodatkowe opcje kodera (pary klucz-wartość)
            ("extra-encoder-option,e", PO_STR_VALUE<std::vector<video2x::fsutils::StringType>>()
                ->multitoken(), "Dodatkowe opcje AVOption dla kodera (format: -e klucz=wartość)")
        ;

        po::options_description upscale_opts("Opcje powiększania");
        upscale_opts.add_options()
            ("width,w", po::value<int>(&proc_cfg.width)
                ->notifier([](int v) { validate_greater_equal_one(v, "width"); }), "Szerokość wyjściowa")
            ("height,h", po::value<int>(&proc_cfg.height)
                ->notifier([](int v) { validate_greater_equal_one(v, "height"); }), "Wysokość wyjściowa")
            ("scaling-factor,s", po::value<int>(&proc_cfg.scaling_factor)
                ->notifier([](int v) { validate_min(v, "scaling-factor", 2); }), "Współczynnik skalowania")
            ("noise-level,n", po::value<int>(&proc_cfg.noise_level)
                ->notifier([](int v) { validate_min(v, "noise-level", -1); }), "Poziom szumu")
        ;

        po::options_description interp_opts("Opcje interpolacji klatek");
        interp_opts.add_options()
            ("frame-rate-mul,m", po::value<int>(&proc_cfg.frm_rate_mul)
                ->notifier([](int v) { validate_min(v, "frame-rate-mul", 2); }),
                "Mnożnik liczby klatek na sekundę")
            ("scene-thresh,t", po::value<float>(&proc_cfg.scn_det_thresh)->default_value(100.0f)
                ->notifier([](float v) { validate_range<float>(v, "scene-thresh", 0.0, 100.0); }),
                "Próg wykrywania scen (20 oznacza 20% różnicy między klatkami jako zmiana sceny)")
        ;

        po::options_description libplacebo_opts("Opcje libplacebo");
        libplacebo_opts.add_options()
            ("libplacebo-shader", PO_STR_VALUE<video2x::fsutils::StringType>()
                ->default_value(STR("anime4k-v4-a"), "anime4k-v4-a")
                ->notifier(validate_anime4k_shader_name),
                "Nazwa/ścieżka pliku shadera GLSL do użycia (wbudowane: anime4k-v4-a, anime4k-v4-a+a, "
                "anime4k-v4-b, anime4k-v4-b+b, anime4k-v4-c, anime4k-v4-c+a, anime4k-v4.1-gan)")
        ;

        po::options_description realesrgan_opts("Opcje Real-ESRGAN");
        realesrgan_opts.add_options()
            ("realesrgan-model", PO_STR_VALUE<video2x::fsutils::StringType>()
                ->default_value(STR("realesr-animevideov3"), "realesr-animevideov3")
                ->notifier(validate_realesrgan_model_name),
                "Nazwa modelu Real-ESRGAN do użycia (realesr-animevideov3, "
                "realesrgan-plus-anime, realesrgan-plus, realesr-generalv3)")
        ;

        po::options_description realcugan_opts("Opcje Real-CUGAN");
        realcugan_opts.add_options()
            ("realcugan-model", PO_STR_VALUE<video2x::fsutils::StringType>()
                ->default_value(STR("models-se"), "models-se")
                ->notifier(validate_realcugan_model_name),
                "Nazwa modelu Real-CUGAN do użycia (models-nose, models-pro, models-se)")
            ("realcugan-threads", po::value<int>()->default_value(1),
                "Liczba wątków do użycia dla Real-CUGAN")
            ("realcugan-syncgap", po::value<int>()->default_value(3),
                "Tryb synchronizacji; 0: bez synchronizacji, 1: dokładna, 2: przybliżona, 3: bardzo przybliżona")
        ;

        po::options_description rife_opts("Opcje RIFE");
        rife_opts.add_options()
            ("rife-model", PO_STR_VALUE<video2x::fsutils::StringType>()
                ->default_value(STR("rife-v4.26"), "rife-v4.26")
                ->notifier(validate_rife_model_name),
                "Nazwa modelu RIFE do użycia (rife, rife-HD, rife-UHD, rife-anime, rife-v2, "
                "rife-v2.3, rife-v2.4, rife-v3.0, rife-v3.1, rife-v4, rife-v4.6, rife-v4.25, "
                "rife-v4.25-lite, rife-v4.26)")
            ("rife-uhd", "Włącz tryb Ultra HD")
        ;
        // clang-format on

        // Combine all options
        all_opts.add(encoder_opts)
            .add(upscale_opts)
            .add(interp_opts)
            .add(libplacebo_opts)
            .add(realesrgan_opts)
            .add(realcugan_opts)
            .add(rife_opts);

        po::variables_map vm;
#ifdef _WIN32
        po::store(po::wcommand_line_parser(argc, argv).options(all_opts).run(), vm);
#else
        po::store(po::command_line_parser(argc, argv).options(all_opts).run(), vm);
#endif

        if (vm.count("help") || argc == 1) {
            std::cout
                << all_opts << std::endl
                << "Przykłady:" << std::endl
                << "  Powiększ wideo anime do 4K za pomocą libplacebo:" << std::endl
                << "    video2x -i input.mp4 -o output.mp4 -w 3840 -h 2160 \\" << std::endl
                << "      -p libplacebo --libplacebo-shader anime4k-v4-a+a" << std::endl
                << std::endl
                << "  Powiększ film 4x za pomocą Real-ESRGAN z niestandardowymi opcjami kodera:"
                << std::endl
                << "    video2x -i input.mkv -o output.mkv -s 4 \\" << std::endl
                << "      -p realesrgan --realesrgan-model realesrgan-plus \\" << std::endl
                << "      -c libx264rgb -e crf=17 -e preset=veryslow -e tune=film" << std::endl
                << std::endl
                << "  Interpoluj klatki wideo za pomocą RIFE do 4x oryginalnej liczby klatek:"
                << std::endl
                << "    video2x -i input.mp4 -o output.mp4 -m 4 -p rife --rife-model rife-v4.6"
                << std::endl;
            return 1;
        }

        if (vm.count("version")) {
            std::cout << "Video2X wersja " << LIBVIDEO2X_VERSION_STRING << std::endl;
            return 1;
        }

        if (vm.count("list-devices")) {
            if (list_vulkan_devices()) {
                return -1;
            }
            return 1;
        }

        // Run all notify functions and validations
        po::notify(vm);

        if (vm.count("log-level")) {
            if (!video2x::logger_manager::LoggerManager::instance().set_log_level(
                    wstring_to_u8string(vm["log-level"].as<video2x::fsutils::StringType>())
                )) {
                video2x::logger()->critical("Podano nieprawidłowy poziom dziennika.");
                return -1;
            }
        }
        video2x::logger_manager::LoggerManager::instance().hook_ffmpeg_logging();

        // Wydrukuj baner programu
        video2x::logger()->info("Video2X wersja {}", LIBVIDEO2X_VERSION_STRING);
        // video2x::logger()->info("Copyright (C) 2018-2024 K4YT3X i współpracownicy.");
        // video2x::logger()->info("Licencjonowane na warunkach GNU AGPL w wersji 3.");

        // Przypisz argumenty pozycyjne
        if (vm.count("input")) {
            arguments.in_fname =
                std::filesystem::path(vm["input"].as<video2x::fsutils::StringType>());
            video2x::logger()->info("Przetwarzanie pliku: {}", arguments.in_fname.u8string());
        } else {
            video2x::logger()->critical("Ścieżka do pliku wejściowego jest wymagana.");
            return -1;
        }

        if (vm.count("output")) {
            arguments.out_fname =
                std::filesystem::path(vm["output"].as<video2x::fsutils::StringType>());
        } else if (!arguments.benchmark) {
            video2x::logger()->critical("Ścieżka do pliku wyjściowego jest wymagana.");
            return -1;
        }

        // Parsuj typ procesora
        if (vm.count("processor")) {
            video2x::fsutils::StringType processor_type_str =
                vm["processor"].as<video2x::fsutils::StringType>();
            if (processor_type_str == STR("libplacebo")) {
                proc_cfg.processor_type = video2x::processors::ProcessorType::Libplacebo;
            } else if (processor_type_str == STR("realesrgan")) {
                proc_cfg.processor_type = video2x::processors::ProcessorType::RealESRGAN;
            } else if (processor_type_str == STR("realcugan")) {
                proc_cfg.processor_type = video2x::processors::ProcessorType::RealCUGAN;
            } else if (processor_type_str == STR("rife")) {
                proc_cfg.processor_type = video2x::processors::ProcessorType::RIFE;
            } else {
                video2x::logger()->critical("Podano nieprawidłowy procesor.");
                return -1;
            }
        } else {
            video2x::logger()->critical("Typ procesora jest wymagany.");
            return -1;
        }

        // Parsuj metodę akceleracji sprzętowej
        arguments.hw_device_type = AV_HWDEVICE_TYPE_NONE;
        if (vm.count("hwaccel")) {
            video2x::fsutils::StringType hwaccel_str =
                vm["hwaccel"].as<video2x::fsutils::StringType>();
            if (hwaccel_str != STR("none")) {
                arguments.hw_device_type =
                    av_hwdevice_find_type_by_name(wstring_to_u8string(hwaccel_str).c_str());
                if (arguments.hw_device_type == AV_HWDEVICE_TYPE_NONE) {
                    video2x::logger()->critical(
                        "Nieprawidłowy typ urządzenia sprzętowego '{}'.", wstring_to_u8string(hwaccel_str)
                    );
                    return -1;
                }
            }
        }

        // Parsuj kodek do AVCodec
        enc_cfg.codec = "libx264";
        if (vm.count("codec")) {
            std::string codec_str =
                wstring_to_u8string(vm["codec"].as<video2x::fsutils::StringType>());
            if (avcodec_find_encoder_by_name(codec_str.c_str()) == nullptr) {
                video2x::logger()->critical("Nieprawidłowy koder '{}'.", codec_str);
                return -1;
            }
            enc_cfg.codec = codec_str;
        }

        // Parse copy streams options
        enc_cfg.recalculate_pts = vm.count("no-recalculate-pts") == 0;
        enc_cfg.copy_audio_streams = vm.count("no-copy-audio-streams") == 0;
        enc_cfg.copy_subtitle_streams = vm.count("no-copy-subtitle-streams") == 0;

        // Parsuj format pikseli do AVPixelFormat
        enc_cfg.pix_fmt = AV_PIX_FMT_NONE;
        if (vm.count("pix-fmt")) {
            video2x::fsutils::StringType pix_fmt_str =
                vm["pix-fmt"].as<video2x::fsutils::StringType>();
            if (!pix_fmt_str.empty()) {
                enc_cfg.pix_fmt = av_get_pix_fmt(wstring_to_u8string(pix_fmt_str).c_str());
                if (enc_cfg.pix_fmt == AV_PIX_FMT_NONE) {
                    video2x::logger()->critical(
                        "Nieprawidłowy format pikseli '{}'.", wstring_to_u8string(pix_fmt_str)
                    );
                    return -1;
                }
            }
        }

        // Parsuj dodatkowe opcje AVOptions
        if (vm.count("extra-encoder-option")) {
            for (const auto& opt :
                 vm["extra-encoder-option"].as<std::vector<video2x::fsutils::StringType>>()) {
                size_t eq_pos = opt.find('=');
                if (eq_pos != video2x::fsutils::StringType::npos) {
                    video2x::fsutils::StringType key = opt.substr(0, eq_pos);
                    video2x::fsutils::StringType value = opt.substr(eq_pos + 1);
                    enc_cfg.extra_opts.push_back(
                        std::make_pair(wstring_to_u8string(key), wstring_to_u8string(value))
                    );
                } else {
                    video2x::logger()->critical(
                        "Nieprawidłowy format opcji AVOption: {}", wstring_to_u8string(opt)
                    );
                    return -1;
                }
            }
        }

        // Parsuj konfiguracje specyficzne dla procesora
        switch (proc_cfg.processor_type) {
            case video2x::processors::ProcessorType::Libplacebo: {
                if (!vm.count("libplacebo-shader")) {
                    video2x::logger()->critical("Nazwa/ścieżka shadera musi być ustawiona dla libplacebo.");
                    return -1;
                }
                if (proc_cfg.width <= 0 || proc_cfg.height <= 0) {
                    video2x::logger()->critical(
                        "Szerokość i wysokość wyjściowa muszą być ustawione dla libplacebo."
                    );
                    return -1;
                }

                proc_cfg.processor_type = video2x::processors::ProcessorType::Libplacebo;
                video2x::processors::LibplaceboConfig libplacebo_config;
                libplacebo_config.shader_path =
                    vm["libplacebo-shader"].as<video2x::fsutils::StringType>();
                proc_cfg.config = libplacebo_config;
                break;
            }
            case video2x::processors::ProcessorType::RealESRGAN: {
                if (!vm.count("realesrgan-model")) {
                    video2x::logger()->critical("Nazwa modelu musi być ustawiona dla Real-ESRGAN.");
                    return -1;
                }
                if (proc_cfg.scaling_factor < 2 || proc_cfg.scaling_factor > 4) {
                    video2x::logger()->critical(
                        "Współczynnik skalowania musi być ustawiony na 2, 3 lub 4 dla Real-ESRGAN."
                    );
                    return -1;
                }
                if (proc_cfg.noise_level < -1 || proc_cfg.noise_level > 1) {
                    video2x::logger()->critical("Poziom szumu musi wynosić 0 lub 1 dla Real-ESRGAN.");
                    return -1;
                }

                proc_cfg.processor_type = video2x::processors::ProcessorType::RealESRGAN;
                video2x::processors::RealESRGANConfig realesrgan_config;
                realesrgan_config.tta_mode = false;
                realesrgan_config.model_name =
                    vm["realesrgan-model"].as<video2x::fsutils::StringType>();
                proc_cfg.config = realesrgan_config;
                break;
            }
            case video2x::processors::ProcessorType::RealCUGAN: {
                if (!vm.count("realcugan-model")) {
                    video2x::logger()->critical("Nazwa modelu musi być ustawiona dla Real-CUGAN.");
                    return -1;
                }
                if (vm.count("realcugan-threads") && vm["realcugan-threads"].as<int>() < 1) {
                    video2x::logger()->critical(
                        "Liczba wątków musi wynosić co najmniej 1 dla Real-CUGAN."
                    );
                    return -1;
                }
                if (vm.count("realcugan-syncgap") && (vm["realcugan-syncgap"].as<int>() < 0 ||
                                                      vm["realcugan-syncgap"].as<int>() > 3)) {
                    video2x::logger()->critical(
                        "Tryb synchronizacji musi być ustawiony na 0, 1, 2 lub 3 dla Real-CUGAN."
                    );
                    return -1;
                }
                if (proc_cfg.scaling_factor < 2 || proc_cfg.scaling_factor > 4) {
                    video2x::logger()->critical(
                        "Współczynnik skalowania musi być ustawiony na 2, 3 lub 4 dla Real-CUGAN."
                    );
                    return -1;
                }
                if (proc_cfg.noise_level < -1 || proc_cfg.noise_level > 3) {
                    video2x::logger()->critical(
                        "Poziom szumu musi być ustawiony na -1, 0, 1, 2 lub 3 dla Real-CUGAN."
                    );
                    return -1;
                }

                proc_cfg.processor_type = video2x::processors::ProcessorType::RealCUGAN;
                video2x::processors::RealCUGANConfig realcugan_config;
                realcugan_config.tta_mode = false;
                realcugan_config.model_name =
                    vm["realcugan-model"].as<video2x::fsutils::StringType>();
                realcugan_config.num_threads = vm["realcugan-threads"].as<int>();
                realcugan_config.syncgap = vm["realcugan-syncgap"].as<int>();
                proc_cfg.config = realcugan_config;
                break;
            }
            case video2x::processors::ProcessorType::RIFE: {
                if (!vm.count("rife-model")) {
                    video2x::logger()->critical("Nazwa modelu musi być ustawiona dla RIFE.");
                    return -1;
                }
                if (proc_cfg.frm_rate_mul < 2) {
                    video2x::logger()->critical(
                        "Mnożnik liczby klatek musi być ustawiony na co najmniej 2 dla RIFE."
                    );
                    return -1;
                }

                proc_cfg.processor_type = video2x::processors::ProcessorType::RIFE;
                video2x::processors::RIFEConfig rife_config;
                rife_config.tta_mode = false;
                rife_config.tta_temporal_mode = false;
                rife_config.uhd_mode = vm.count("rife-uhd") > 0;
                rife_config.num_threads = 0;
                rife_config.model_name = vm["rife-model"].as<video2x::fsutils::StringType>();
                proc_cfg.config = rife_config;
                break;
            }
            default:
                video2x::logger()->critical("Nieprawidłowy typ procesora.");
                return -1;
        }
    } catch (const po::error& e) {
        video2x::logger()->critical("Błąd podczas analizowania argumentów: {}", e.what());
        return -1;
    } catch (const std::exception& e) {
        video2x::logger()->critical(
            "Nieoczekiwany wyjątek podczas analizowania opcji: {}", e.what()
        );
        return -1;
    }

    // Waliduj identyfikator urządzenia Vulkan
    VkPhysicalDeviceProperties dev_props;
    int get_vulkan_dev_ret = get_vulkan_device_prop(arguments.vk_device_index, &dev_props);
    if (get_vulkan_dev_ret != 0) {
        if (get_vulkan_dev_ret == -2) {
            video2x::logger()->critical("Podano nieprawidłowy identyfikator urządzenia Vulkan.");
            return -1;
        } else {
            video2x::logger()->warn("Nie można zweryfikować identyfikatora urządzenia Vulkan.");
            return -1;
        }
    } else {
        // Ostrzeż, jeśli wybrane urządzenie jest procesorem CPU
        video2x::logger()->info(
            "Używane urządzenie Vulkan: {} ({:#x})", dev_props.deviceName, dev_props.deviceID
        );
        if (dev_props.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU) {
            video2x::logger()->warn("Wybrane urządzenie Vulkan jest urządzeniem CPU.");
        }
    }
    return 0;
}

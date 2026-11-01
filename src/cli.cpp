#include "cli.hpp"
#include "process.hpp"
#include "display.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <filesystem>

const std::string CARPETA_MILO = "videos_milo";

void mostrarLogo() {
    std::cout << R"(
                                                           
     ______  _______    ____  ____                _____    
    |      \/       \  |    ||    |          ____|\    \   
   /          /\     \ |    ||    |         /     /\    \  
  /     /\   / /\     ||    ||    |        /     /  \    \ 
 /     /\ \_/ / /    /||    ||    |  ____ |     |    |    |
|     |  \|_|/ /    / ||    ||    | |    ||     |    |    |
|     |       |    |  ||    ||    | |    ||\     \  /    /|
|\____\       |____|  /|____||____\/____/|| \_____\/____/ |
| |    |      |    | / |    ||          || \ |    ||    | /
 \|____|      |____|/  |____||__________|/  \|____||____|/ 
                                                     v1.0
    )" << std::endl;
}

std::string SeleccionarVideoMilo() {
    // Verificamos si la carpeta existe
    if (!std::filesystem::exists(CARPETA_MILO)) {
        std::cout << "La carpeta '" << CARPETA_MILO << "' no existe. Todavia no hay videos convertidos.\n";
        return "";
    }

    std::vector<std::string> archivos;

    // Recorremos la carpeta buscando solo archivos con extensión .milo
    for (const auto& entrada : std::filesystem::directory_iterator(CARPETA_MILO)) {
        if (entrada.is_regular_file() && entrada.path().extension() == ".milo") {
            archivos.push_back(entrada.path().filename().string());
        }
    }

    if (archivos.empty()) {
        std::cout << "No se encontraron archivos .milo en la carpeta '" << CARPETA_MILO << "'.\n";
        return "";
    }

    // Mostramos el menú
    std::cout << "\n=== ARCHIVOS DISPONIBLES ===\n";
    for (size_t i = 0; i < archivos.size(); ++i) {
        std::cout << "[" << i + 1 << "] " << archivos[i] << "\n";
    }
    std::cout << "[0] Cancelar\n";
    std::cout << "============================\n";

    int opcion;
    std::cout << "Elige un numero para reproducir: ";
    
    // Validamos la entrada del usuario
    while (!(std::cin >> opcion) || opcion < 0 || opcion > archivos.size()) {
        std::cin.clear(); // Limpia el error de lectura
        std::cin.ignore(10000, '\n'); // Descarta la entrada invalida
        std::cout << "Opcion invalida. Intenta de nuevo: ";
    }

    if (opcion == 0) {
        std::cout << "Reproduccion cancelada.\n";
        return "";
    }

    // Armamos la ruta final
    std::string archivo_elegido = archivos[opcion - 1];
    std::filesystem::path ruta_final = std::filesystem::path(CARPETA_MILO) / archivo_elegido;
    
    return ruta_final.string();
}


std::string AgregarExtencion(std::string ruta){
    if(ruta.compare(ruta.length() - 5, 5, ".milo") != 0){
        ruta.reserve(ruta.size()+5);
        ruta +=".milo";
    }
    // 2. Si la carpeta no existe, la creamos
    if (!std::filesystem::exists(CARPETA_MILO)) {
        std::filesystem::create_directory(CARPETA_MILO);
    }

    // 3. Unimos la carpeta con el nombre del archivo (ej: videos_milo/video.milo)
    std::filesystem::path ruta_completa = std::filesystem::path(CARPETA_MILO) / ruta;
    
    return ruta_completa.string();
}

std::string limpiarRuta(std::string ruta) {
    // 1. Quitar comillas dobles '"'
    ruta.erase(std::remove(ruta.begin(), ruta.end(), '"'), ruta.end());

    // 2. Reemplazar '\' por '/'
    // Nota: Se escribe '\\' porque la barra invertida es un carácter de escape en C++
    std::replace(ruta.begin(), ruta.end(), '\\', '/');

    return ruta;
}

void iniciarCLI() {
    mostrarLogo();
    std::string comando;

    while (true) {
        std::cout << "\nmilo> ";
        if (!(std::cin >> comando)) break;

        if (comando == "encode") {
            std::string entrada, salida;
            std::cout << "Ruta del video (.mp4/.avi): ";
            std::cin >> entrada;
            entrada = limpiarRuta(entrada);
            std::cout << "Nombre de salida (.milo): ";
            std::cin >> salida;
            salida = limpiarRuta(salida);
            salida = AgregarExtencion(salida);
            std::cout<<"El nombre de archivo sera: "<<salida<<std::endl;

            std::cout << "[PROCESS] Iniciando codificación...\n";
            if (Process::codificarVideo(entrada, salida)==0) {
                std::cout << "[PROCESS] ¡Archivo " << salida << " generado con éxito!\n";
            } else {
                std::cout << "[ERROR] No se pudo procesar el video.\n";
            }
        } 
        else if (comando == "play") {
            // std::string archivoMilo;
            // std::cout << "Ruta del archivo (.milo): ";
            // std::cin >> archivoMilo;
            //archivoMilo = limpiarRuta(archivoMilo);
            std::string ruta_reproducir = SeleccionarVideoMilo();
    
             std::cout << "[DISPLAY] Abriendo visor ImGui...\n";
            if (!ruta_reproducir.empty()) {
                Display::reproducirMilo(ruta_reproducir);
            }
            // Llama a ImGui: detiene el CLI temporalmente hasta que se cierre la ventana
            std::cout << "[DISPLAY] Ventana cerrada. Volviendo a la consola.\n";
        } 
        else if (comando == "help") {
            std::cout << "Comandos disponibles:\n";
            std::cout << "  encode - Convierte un video a formato .milo\n";
            std::cout << "  play   - Abre la ventana ImGui para reproducir un .milo\n";
            std::cout << "  exit   - Sale del programa\n";
        } 
        else if (comando == "exit") {
            std::cout << "¡ Hasta luego !\n";
            break;
        } 
        else {
            std::cout << "Comando no reconocido. Escribe 'help' para ver la lista.\n";
        }
    }
}
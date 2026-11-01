#ifndef DISPLAY_HPP
#define DISPLAY_HPP

#include <string>

namespace Display {
    // Inicializa ImGui, abre la ventana, reproduce el archivo .milo y al cerrar limpia el contexto
    int reproducirMilo(const std::string& rutaMilo);
}

#endif
#ifndef PROCESS_HPP
#define PROCESS_HPP

#include <string>

namespace Process {
    // Procesa el video de entrada y genera el archivo binario .milo
    int codificarVideo(const std::string& rutaEntrada, const std::string& rutaSalida);
}

#endif
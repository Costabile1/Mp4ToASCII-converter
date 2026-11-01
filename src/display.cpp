#include "display.hpp"
#include <iostream>
#include <cmath>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <chrono>  
#include "imgui.h"
#include "imgui-SFML.h"

#include <cstdint> // Asegúrate de incluir esto arriba

// Empaquetado estricto: cero basura entre variables
#pragma pack(push, 1)
struct HeaderLILA {
    char firma[5]; 
    uint16_t columnas;                 
    uint16_t filas;                    
    uint8_t fps;                       
};

struct framehdr{
    int id;
    bool soy_copiado=false; //para saber si lo tengo que mantener en memoria
    int id_copiado=-1; //tengo filas o columas resumidas de tipo caracter*cantidad.
    uint16_t tamaño;
};
#pragma pack(pop)



int Display::reproducirMilo(const std::string& rutaMilo){
    std::ifstream archivo(rutaMilo, std::ios::binary);
    std::cout<<"Ruta de Archivo recibida: "<<rutaMilo<<std::endl;
    if(!archivo.is_open()){
        std::cout<<"Error al Leer el archivo"<<std::endl;
        return 1;
    }
    int largo_header=sizeof(struct HeaderLILA);
    char header_buf[largo_header];
    archivo.read(header_buf,sizeof(header_buf));
    struct HeaderLILA* header=(struct HeaderLILA*) header_buf;
    std::cout<<"LA FIRMA ES: "<<header->firma<<std::endl;
    if(strcmp(header->firma,"milo")!=0){
        
        std::cout<<"la firma no coincide. debe ingresar un archivo (.milo)"<<std::endl;
        return 1;
    }
    int columnas = header->columnas;
    int filas = header->filas;
    int fps=header->fps;
    std::cout<<"COMPROBACION"<<std::endl;
    std::cout<<"FIRMA: " <<header->firma<<std::endl;
    std::cout<<"Ancho HEADER "<<columnas<<std::endl;
    std::cout<<"Largo HEADER "<<filas<<std::endl;
    std::cout<<"SIZE: "<<columnas*filas+1<<std::endl;
    std::cout<<"FPS "<<fps<<std::endl;
    std::cout<<"SIZE HEader LILA: "<<sizeof(struct HeaderLILA)<<std::endl;
    std::cout<<"FPS header Frame"<<sizeof(struct framehdr)<<std::endl;
  
    sf::RenderWindow window(sf::VideoMode({1000,1000}), "Mi primera ventana SFML");
    ImGui::SFML::Init(window);
    ImGuiIO& io = ImGui::GetIO();
    //io.Fonts->AddFontFromFileTTF("C:/Users/santi/projects/ProyectoOpenCV/build/CONSOLA.TTF", 20.0f);
    //ImGui::SFML::UpdateFontTexture();
    window.setFramerateLimit(fps);

    //obtener dimenciones correcttas
    
    
    // sf::Font fuente;
    // if(!fuente.openFromFile("CONSOLA.ttf")){
    //     return 1;SS
    // }
    // sf::Text texto_ascii(fuente);
    // texto_ascii.setFont(fuente); // Asignarle la fuente
    // texto_ascii.setCharacterSize(8); // Tamaño de la letra en píxeles
    // texto_ascii.setFillColor(sf::Color::White); // Letras blancas
    char frameheader_buffer[sizeof(struct framehdr)];
    //char buffer[(header->columnas)*header->filas];
    std::string ref_frame;
    struct framehdr* ref_frameheader;
    std::string frame_aux;
    sf::Clock clock;
    // Bucle principal del programa

    while (window.isOpen()){
        // Procesar los eventos de la ventana (como cerrar la ventana)
        while (const std::optional event = window.pollEvent()){
            ImGui::SFML::ProcessEvent(window, *event);
            if (event->is<sf::Event::Closed>()){

                window.close();
            }
        }
        // Limpiar la pantalla con un color (en este caso, negro o gris por defecto)
        ImGui::SFML::Update(window, clock.restart());
        //Logica:
        if(!archivo.read(frameheader_buffer,sizeof(frameheader_buffer))) {
            ImGui::SFML::Render(window);
            return 1;
        }

        struct framehdr* frameheader=(struct framehdr*) frameheader_buffer;
        char buffer[frameheader->tamaño];
        archivo.read(buffer,sizeof(buffer));
        std::string frame(buffer,sizeof(buffer));
        frame_aux.clear();
        frame_aux.reserve((header->columnas*header->filas)+1);
        int tamaño_inicial = frame_aux.size();
        int i=0;
        int cout=0;
        auto inicio = std::chrono::steady_clock::now();

        while(frame[i]!='\x04'){
            char c = frame[i];
            if(frame[i]=='\x02'){
                char caracter = frame[i+1];
                uint8_t cantidad = frame[i+2];
                frame_aux.append(cantidad,caracter);
                i+=4; //que es lo que opcupa el oeprador repertido (inicio,caraceter,cantidarepetido,terminador)
            }else{
                frame_aux.push_back(frame[i]);
                i+=1;
                cout++;
            }
            
        }
        auto fin = std::chrono::steady_clock::now();
        auto duracionMs = std::chrono::duration_cast<std::chrono::milliseconds>(fin - inicio);
        auto duracionUs = std::chrono::duration_cast<std::chrono::microseconds>(fin - inicio);

        auto inicio1 = std::chrono::steady_clock::now();
        //texto_ascii.setString(frame_aux);
        ImGui::SetNextWindowPos(ImVec2(0.0f, 0.0f));

        // 2. Forzar el tamaño para que coincida exactamente con tu ventana de SFML
        float tam_X = static_cast<float>(window.getSize().x);
        float tam_Y = static_cast<float>(window.getSize().y);
        ImGui::SetNextWindowSize(ImVec2(tam_X,tam_Y));

        // 3. Banderas (Flags) para quitarle el "look" de ventana y hacerla una pantalla plana
        ImGuiWindowFlags banderas = ImGuiWindowFlags_NoTitleBar | 
                                    ImGuiWindowFlags_NoResize | 
                                    ImGuiWindowFlags_NoMove | 
                                    ImGuiWindowFlags_NoCollapse |
                                    ImGuiWindowFlags_NoScrollbar | // Opcional: quita las barras de scroll
                                    ImGuiWindowFlags_NoBackground; // Opcional: quita el fondo gris de ImGui

        // Abrimos la ventana aplicándole las banderas
        ImGui::Begin("Consola de Video", nullptr, banderas);

        // 4. Escalar la fuente (¡Aquí está la magia para que quepa todo!)
        // El 1.0f es el tamaño normal. Usa 0.5f para la mitad del tamaño, 0.3f para un tercio, etc.
        // Tendrás que jugar con este número decimal hasta que tu fotograma entre perfecto.
        ImVec2 espacio_disponible = ImGui::GetContentRegionAvail();
        ImGui::SetWindowFontScale(1.0f); 
        float ancho_base_letra = ImGui::CalcTextSize("A").x;
        float alto_base_letra  = ImGui::GetTextLineHeight();
        // 4. Calcular cuánto mediría todo el video en píxeles si la escala fuera 1.0
        float ancho_texto_total = columnas * ancho_base_letra;
        float alto_texto_total  = filas * alto_base_letra;

        // 5. Calcular la escala necesaria para cada eje
        float escala_x = espacio_disponible.x / ancho_texto_total;
        float escala_y = espacio_disponible.y / alto_texto_total;

        // 6. El truco maestro: Elegir la escala más pequeña (std::min)
        // Si no haces esto, el video cabrá de ancho pero se saldrá por abajo, o viceversa.
        float escala_perfecta = std::min(escala_x, escala_y);
        ImGui::SetWindowFontScale(escala_perfecta);
        ImGui::TextUnformatted(frame_aux.c_str(),frame_aux.c_str()+frame_aux.size());
        ImGui::End();

        // Mostrar en pantalla lo que se acaba de dibujar
        window.clear();
        ImGui::SFML::Render(window);
        window.display();
        
        auto fin1 = std::chrono::steady_clock::now();
        auto duracionMs1 = std::chrono::duration_cast<std::chrono::milliseconds>(fin1 - inicio1);
        auto duracionUs1 = std::chrono::duration_cast<std::chrono::microseconds>(fin1 - inicio1);
        double cpu_ms = duracionUs.count();
        double gpu_ms = duracionUs1.count();

        double relacion = gpu_ms / cpu_ms;
        //std::cout<<"RELACION PROCESO VENTANA: "<<relacion<<std::endl;
    }
    return 1;
}
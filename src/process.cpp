#include "process.hpp"
#include <iostream>
#include <cmath>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <string>
#include <cstdint> 
#include <cstdio>

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

#include <cstdio> // Para popen, pclose, fread
#include <vector>

// Macro para compatibilidad Windows/Linux de pipes
#ifdef _WIN32
    #define POPEN _popen
    #define PCLOSE _pclose
#else
    #define POPEN popen
    #define PCLOSE pclose
#endif

// Empaquetado estricto: cero basura entre variables
#pragma pack(push, 1)
struct HeaderLILA {
    char firma[5] = "milo"; 
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


int G_X[3][3] ={{-1,0,1},{-2,0,2},{-1,0,+1}};
//const cv::Mat G_X(3,3,CV_32S,matrixSobelX);
int G_Y[3][3]={{-1,-2,-1},{0,0,0},{1,2,1}};
//const cv::Mat G_Y(3,3,CV_32S,matrixSobelY);
int umbral_min=160;
int umbral_max=250;
int umbral_gradiente=600;
int umbral_min_gradiente=20;
int ancho_char=250;

std::vector<char> ASCCI = {' ', '.', ',', '-', '~', ':', ';', '=', '!', '*', 'i', 't', '1', 'l', 'o', 'w', 'm', '#','$','%','&', '@'};
std::vector<char> spinner = {'|', '/', '-', '\\'};


void MostrarSpinnerCLI(int frame_actual) {
    int animacion_idx = frame_actual % 4;
    std::cout << "\rConvertiendo a ASCII... [" << spinner[animacion_idx] << "] " << frame_actual << " frames procesados" << std::flush;
}

int BuscarRepetidos(std::string &current,struct HeaderLILA &header){
    /*
    #Buscamos cadenas de letras iguales mayores a estricto a 2, para optimizar espacio de memoria en el archivo.milo
    #como vamos a estar cambiando la logitud establecemos 2 variables,
    #c controla las filas de curren, se incremente a deforma normal
    # j que representa cada indece de una fila completa
    # i que maneja la cantidad de caracteres por fila, la incrementamos en header.columns pero cada vez que reducimos caracteres
    reducimos i por la cantidad de caracteres que encontramos repetidos osea count y le sumamos el largo de nuetra sintaxis para indicar que ahi se repiten letras
    SIntaxis : (operador,letra_repetida,cuantas vecez estaba repetida) actualemtne el operador es "!", podria ser R.
    */
    int cant_remplazos=0;
    char* linea= current.data();
    size_t c=0;
    bool primera=true;
    double tamaño = (current.size());
    double count=1;
    while(c<current.size()){   
        char current_char;
        double indice;
        if(primera){
            current_char=linea[c];
            indice=c;
            primera=false;
        }
        if(current_char==linea[c+1]){
            count++;
        }else{
            if(count>5 && count<255){
                char inicio=(char)2;
                std::string remplazo;
                remplazo.push_back(inicio);
                remplazo+=linea[c];
                remplazo+=(char)(count);
                remplazo+=(char) 3;
                double tamaño = (current.size());
                current.replace(indice,count,remplazo);
                c=c-(count)+remplazo.size(); 
                cant_remplazos++;
            }
            current_char=linea[c+1];
            indice=c+1;
            count=1;
        }
        c++;
    }
    return cant_remplazos;

}

int CompararLineas(std::string &buffer, std::string &current,struct HeaderLILA &header,struct framehdr &framehdr_buffer,struct framehdr &framehdr_current){
    /*
    # el Buffer jamas deberia estar cambiado, tiene los caracteres completos y no estar modificado
    tenemos 3 variables en juego c,indice_current,indice_buffer
    # c itera por a cantidad de filas, de forma normal, no cambia
    # indice_buffer inidica la cant de caracteres por linea y la aumentamos por hedear.columns que nos dice la cantidad de caracteres por linea, se incrementa d eform normla
    # indice_current va a variar, se incrementa al igual que indice_buffer con header.columns, pero cada vez que encontramos una linea igual, la remplazamos por una "X" y a indice_current
    le restamos todas las canitda de letras de una fila osea(header.columns) por que remplazamos toda la linea y le sumamos 1 por el luar qu eocupa "X", que indica que ahi va una linea del frame de referencia.
    */
    int res=0;
    int indice_current=0;
    int indice_buffer=0;
    for(int c=0;c<header.filas;c++,indice_buffer+=header.columnas,indice_current+=header.columnas){
        std::string linea_buffer= buffer.substr(indice_buffer,header.columnas); 
        std::string linea_current = current.substr(indice_current,header.columnas);
        if(linea_buffer==linea_current){
            int indice_fila=c;
            framehdr_current.id_copiado=framehdr_buffer.id;
            current.replace(indice_current,header.columnas,"C\n");
            indice_current=(indice_current-header.columnas)+1;
            res++;
        }
    }
    return res;
}


char ObtenerCaracter(int valor){
    double indice=((double) valor/255.0)*(ASCCI.size()-1);
    int i = (int) indice;
    return ASCCI[(int) indice];
}

int NormalizacionGradiente(int &G){
    if(G>255){
        G=255;
    }else if(G<0){
        G=0;
    }
    return G;
}

int GradienteX(int valores[3][3]){
    return (valores[0][0]*G_X[0][0] + valores[0][1]*G_X[0][1] +valores[0][2]*G_X[0][2]) +
    (valores[1][0]*G_X[1][0] + valores[1][1]*G_X[1][1] +valores[1][2]*G_X[1][2]) + 
    (valores[2][0]*G_X[2][0] + valores[2][1]*G_X[2][1] +valores[2][2]*G_X[2][2]);
}

int GradienteY(int valores[3][3]){
    return (valores[0][0]*G_Y[0][0] + valores[0][1]*G_Y[0][1] +valores[0][2]*G_Y[0][2]) +
    (valores[1][0]*G_Y[1][0] + valores[1][1]*G_Y[1][1] +valores[1][2]*G_Y[1][2]) + 
    (valores[2][0]*G_Y[2][0] + valores[2][1]*G_Y[2][1] +valores[2][2]*G_Y[2][2]);
}

int Process::codificarVideo(const std::string& rutaEntrada, const std::string& rutaSalida){
    bool hay_header=false;

    std::ofstream archivo_salida(rutaSalida, std::ios::binary);
    std::vector<int> repetidos;
    std::vector<double> tamaños_frames;
    if(!archivo_salida.is_open()){
        std::cout<<"Error al Crear el archivo"<<std::endl;
        return 1;
    }
    std::string string_ascii;
    std::string buffer;
    //Inicializamos la estrcutura con el tamaño del primer frame.
    struct HeaderLILA header;
    struct framehdr framehdr_buffer;

    int ancho_original=0, alto_original=0;
    std::string cmd_probe = "ffprobe -v error -select_streams v:0 -show_entries stream=width,height -of csv=s=x:p=0 \"" + rutaEntrada + "\"";
    FILE* probe = POPEN(cmd_probe.c_str(), "r");
    if (probe) {
        fscanf(probe, "%dx%d", &ancho_original, &alto_original);
        PCLOSE(probe);
    }

    if (ancho_original == 0 || alto_original == 0) {
        std::cout << "Error: ffprobe no pudo leer las dimensiones del video. Verifica la ruta." << std::endl;
        return 1;
    }

    double proporcion = (double)alto_original / ancho_original;
    int alto_char = (int)((ancho_char * proporcion) * 0.4);

    // 2. Definir tamaños de memoria
    size_t size_original = ancho_original * alto_original; // 1 byte por píxel (grises)
    size_t size_chico = ancho_char * alto_char;

    // Vectores crudos de C++ para reemplazar a cv::Mat
    std::vector<uint8_t> frame_original(size_original);
    std::vector<uint8_t> frame_chico(size_chico);

    
    std::string cmd_ffmpeg = "ffmpeg -i \"" + rutaEntrada + "\" -f image2pipe -pix_fmt gray -vcodec rawvideo - 2> NUL";
    FILE* pipe = POPEN(cmd_ffmpeg.c_str(), "rb");
    if (!pipe) {
        std::cout << "Error al iniciar FFmpeg." << std::endl;
        return 1;
    }

    std::cout<<"SIZE HEader LILA: "<<sizeof(struct HeaderLILA)<<std::endl;
    std::cout<<"FPS header Frame"<<sizeof(struct framehdr)<<std::endl;
    int largo_string_ascii=0;
    int ancho_string_ascii=0;
    int cant_frames=0;
    while(fread(frame_original.data(),1,size_original,pipe)==size_original){
        MostrarSpinnerCLI(cant_frames);

        // Achicar la imagen con STB (Reemplaza a cv::resize)
        stbir_resize_uint8_linear(
            frame_original.data(), ancho_original, alto_original, 0,
            frame_chico.data(), ancho_char, alto_char, 0, 
            STBIR_1CHANNEL
        );

        struct framehdr framehdr;
        string_ascii="";
        ancho_string_ascii=0;
        framehdr.id=cant_frames;
    

        if(!hay_header){
            int columnas= ancho_char-1;  //RESTAMOS 1 POR QUE EN NUETROS BUCLES ITERAMOS DEL 1 A N-1, LOS FRAMES DE LOS BORDES NO LOS MMIRAMOS, pero tnemos en cuenta que a cada liena le agregamos un /n
            int filas=alto_char-2;    //RESTAMOS 2 POR QUE EN NUETROS BUCLES ITERAMOS DEL 1 A N-1, LOS FRAMES DE LOS BORDES NO LOS MMIRAMOS
            header.columnas=columnas;
            header.filas=filas;
            header.fps=30;
            archivo_salida.write(reinterpret_cast<char*>(&header), sizeof(HeaderLILA));
            hay_header=true;
        }
        for (int i=1;i<alto_char-1;i++){
            ///ARMAMOS EL HEADER FRAME
            
            int GX,GY;
            int inicio_fila_sup = (i - 1) * ancho_char;
            int inicio_fila_cur = i * ancho_char;
            int inicio_fila_inf = (i + 1) * ancho_char;
        

            largo_string_ascii=0;
            for(int j=1;j<ancho_char-1;j++){
                int valores[3][3] = {
                    {frame_chico[inicio_fila_sup + j - 1], frame_chico[inicio_fila_sup + j], frame_chico[inicio_fila_sup + j + 1]},
                    {frame_chico[inicio_fila_cur + j - 1], frame_chico[inicio_fila_cur + j], frame_chico[inicio_fila_cur + j + 1]},
                    {frame_chico[inicio_fila_inf + j - 1], frame_chico[inicio_fila_inf + j], frame_chico[inicio_fila_inf + j + 1]}
                };
                GX = GradienteX(valores);
                GY = GradienteY(valores);
                int valor = std::sqrt(pow(NormalizacionGradiente(GX),2)+pow(NormalizacionGradiente(GY),2));
                string_ascii+=(ObtenerCaracter(NormalizacionGradiente(valor)));
                largo_string_ascii+=1;
            }
            string_ascii+="\n";
            ancho_string_ascii+=1;
        }
        ////Antes de LEER EL PROXIMO FRAME
        //Logica de comparacion
        
        repetidos.push_back(BuscarRepetidos(string_ascii,header));
        string_ascii+='\x04'; //caracter que termina la tramsicion del frame
        if(cant_frames==4){
            std::ofstream archivo_salida("prueba1frame.txt");
            archivo_salida.write(string_ascii.c_str(),string_ascii.size());
          
        }
        // if(buffer.empty()){
        //     buffer.asing(string_ascii,string_ascii.size());
        //     framehdr_buffer=framehdr;
        // }else{
        //     // if(CompararLineas(buffer,string_ascii,header,framehdr_buffer,framehdr)==0){
        //     //     buffer="";
        //     // }
        //     //framehdr_buffer.soy_copiado=true;
        //     BuscarRepetidos(string_ascii,header);
        // }
        
        //CompararColumna();
        
        //necesito almacenar todo mi frame.
        
        framehdr.tamaño=string_ascii.size();
        tamaños_frames.push_back(string_ascii.size());
        archivo_salida.write(reinterpret_cast<char*>(&framehdr), sizeof(framehdr));
        archivo_salida.write(string_ascii.c_str(),(string_ascii.size()));

        cant_frames++;
    }
    PCLOSE(pipe);
    archivo_salida.close();

    std::cout<<"COMPROBACION"<<std::endl;
    std::cout<<"Ancho HEADER "<<header.columnas<<std::endl;
    std::cout<<"Largo HEADER "<<header.filas<<std::endl;
    std::cout<<"SIZE Real:  "<<(header.filas*header.columnas)+1<<std::endl;
    std::cout<<"Ancho TEXTO "<<largo_string_ascii<<std::endl;
    std::cout<<"Largo HEADER "<<ancho_string_ascii<<std::endl;
    std::cout<<" REPETIDOOSSS "<<std::endl;
    // for(int i=0; i<repetidos.size();i++){
    //     std::cout<<"["<<i<<"]: "<<repetidos[i]<<" Tamanio Frame :"<<tamaños_frames[i]<<std::endl;
    //     std::cout<<"RELACION REPETIDOS/FRAMES: "<<(repetidos[i]/tamaños_frames[i])*100<<std::endl;
    // }
    
    

    //AnalizisGradiente();
    //cv::imshow("OpenCV Test", img);
    //cv::imshow("Gradiente", img_grad);
    

    return 0;
}
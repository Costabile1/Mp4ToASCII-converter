#include <opencv2/opencv.hpp>
#include <iostream>
#include <cmath>
#include <fstream>
#include <SFML/Graphics.hpp>


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
std::vector<cv::Vec2i> gradientes;

char ObtenerCaracter(int valor){
    double indice=((double) valor/255.0)*(ASCCI.size()-1);
    int i = (int) indice;
    return ASCCI[(int) indice];
}

void AnalizisGradiente(){
    int maxDifX=0;
    int maxDifY=0;
    float countX_grande=0;
    float countY_grande=0;
    float countX_chico=0;
    float countY_chico=0;
    for(int i=0;i<gradientes.size()-1;i++){
        int dif_X=abs(gradientes[i][0]);
        if(dif_X>maxDifX){
            maxDifX=dif_X;
        }
        int dif_Y = abs(gradientes[i][1]);
        if(dif_Y>maxDifY){
            maxDifY=dif_Y;
        }
        if(dif_X>umbral_gradiente){
            countX_grande++;
        }
        if(dif_Y>umbral_gradiente){
            countY_grande++;
        }
        if(dif_X<umbral_min_gradiente){
            countX_chico++;
        }
        if(dif_Y<umbral_min_gradiente){
            countY_chico++;
        }
    }
    std::cout<<"MAxima diferencia en X"<<maxDifX<<std::endl;
    std::cout<<"MAxima diferencia en Y"<<maxDifY<<std::endl;
    std::cout<<"Tamaño"<<gradientes.size()<<std::endl;
    std::cout<<"Cantidad de Gradientes EN X Mayores a "<<umbral_gradiente<< "Son: "<<countX_grande/gradientes.size()<<std::endl;
    std::cout<<"Cantidad de GradientesEN Y Mayores a "<<umbral_gradiente<< "Son: "<<countY_grande/gradientes.size()<<std::endl;
    std::cout<<"Cantidad de Gradientes EN X Menores a "<<umbral_min_gradiente<< "Son: "<<countX_chico/gradientes.size()<<std::endl;
    std::cout<<"Cantidad de GradientesEN Y Menores a "<<umbral_min_gradiente<< "Son: "<<countY_chico/gradientes.size()<<std::endl;
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

int main()
{
    std::cout << "OpenCV Version: " << CV_VERSION << std::endl;
    std::string ruta = "../Santi.jpg";

    cv::Mat img = cv::imread(ruta, cv::IMREAD_GRAYSCALE);
    cv::Mat img_grad = cv::Mat::zeros(img.rows, img.cols, CV_8U);
    //Ajuste para que la iamgen sea coherente
    double proporcion=(double)img.rows/img.cols;
    int alto_char = (ancho_char *  proporcion) * 0.4;
    cv::resize(img,img,cv::Size(ancho_char,alto_char));

    if (img.empty()) {
        std::cout << "Error: No se pudo cargar la imagen." << std::endl;
        return -1;
    }
    std::ofstream archivo("archivo_foto.txt");
    std::ofstream archivo_salida("Salida.milo", std::ios::binary);
    if(!archivo.is_open() || !archivo_salida.is_open()){
        std::cout<<"Error al abrir el archivo"<<std::endl;
        return 1;
    }
    std::string linea="";
    for (int i=1;i<img.rows-1;i++){
        int GX,GY;
        uchar *p_sup = img.ptr<uchar>(i-1);
        uchar *p_current = img.ptr<uchar>(i);
        uchar *p_inf = img.ptr<uchar>(i+1);
        uchar *p_destino = img_grad.ptr<uchar>(i);
        for(int j=1;j<img.cols-1;j++){
            int valores[3][3] ={
                {p_sup[j-1],p_sup[j],p_sup[j+1]},
                {p_current[j-1],p_current[j],p_current[j+1]},
                {p_inf[j-1],p_inf[j],p_inf[j+1]}
            };
            GX = GradienteX(valores);
            GY = GradienteY(valores);
            cv::Vec2i aux(GX,GY);
            gradientes.push_back(aux);
            int valor = std::sqrt(pow(NormalizacionGradiente(GX),2)+pow(NormalizacionGradiente(GY),2));
            p_destino[j]=valor;
            archivo<<ObtenerCaracter(NormalizacionGradiente(valor));
            linea+=(ObtenerCaracter(NormalizacionGradiente(valor)));
        }
        linea+="\n";
        archivo <<std::endl;
    }
    archivo_salida.write(linea.c_str(),linea.size());
    archivo_salida.close();
    archivo.close();
    //AnalizisGradiente();
    //cv::imshow("OpenCV Test", img);
    //cv::imshow("Gradiente", img_grad);
    cv::waitKey(0);
    std::cout<<"Hola"<<std::endl;
    sf::RenderWindow window(sf::VideoMode({1000, 1000}), "Mi primera ventana SFML");
    
    // Bucle principal del programa
    sf::Font fuente;
    if(!fuente.openFromFile("CONSOLA.ttf")){
        return 1;
    }
    sf::Text texto_ascii(fuente);
    texto_ascii.setFont(fuente); // Asignarle la fuente
    texto_ascii.setCharacterSize(8); // Tamaño de la letra en píxeles
    texto_ascii.setFillColor(sf::Color::White); // Letras blancas

    while (window.isOpen())
    {
        // Procesar los eventos de la ventana (como cerrar la ventana)
        while (const std::optional event = window.pollEvent())
        {
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }
        }

        // Limpiar la pantalla con un color (en este caso, negro o gris por defecto)
        window.clear();
        texto_ascii.setString(linea);
        window.draw(texto_ascii);

        // Mostrar en pantalla lo que se acaba de dibujar
        window.display();
    }

    return 0;
}


#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <string>
#include <iostream>
#include <cstdio>
#include<cstring>
#include <cstdlib>
#include <stdlib.h>


//definimos nuestra salida
#define 	IMAGEN_TRATADA	"output.bmp"

using namespace std;

typedef struct BMP{
    char bm[2];					//(2 Bytes) BM (Tipo de archivo)
    int tamano;					//(4 Bytes) Tamaño del archivo en bytes
    int reservado;					//(4 Bytes) Reservado
    int offset;						//(4 Bytes) offset, distancia en bytes entre la img y los píxeles
    int tamanoMetadatos;			//(4 Bytes) Tamaño de Metadatos (tamaño de esta estructura = 40)
    int ancho;						//(4 Bytes) Alto (numero de pixeles verticales)
    int alto;						//(4 Bytes) Ancho (numero de píxeles horizontales)
    short int numeroPlanos;			//(2 Bytes) Numero de planos de color
    short int profundidadColor;		//(2 Bytes) Profundidad de color (debe ser 24 para nuestro caso)
    int tipoCompresion;				//(4 Bytes) Tipo de compresión (Vale 0, ya que el bmp es descomprimido)
    int tamanoEstructura;			//(4 Bytes) Tamaño de la estructura Imagen (Paleta)
    int pxmh;					//(4 Bytes) Píxeles por metro horizontal
    int pxmv;					//(4 Bytes) Píxeles por metro vertical
    int coloresUsados;				//(4 Bytes) Cantidad de colores usados
    int coloresImportantes;		//(4 Bytes) Cantidad de colores importantes
    unsigned char **pixelB; 	//Puntero a una tabla dinamica de caracteres de 2 dimenciones almacenara el valor del pixel en escala de BLUE (0-255)
    unsigned char **pixelG;		//Puntero a una tabla dinamica de caracteres de 2 dimenciones almacenara el valor del pixel en escala de GREEN (0-255)
    unsigned char **pixelR;		//Puntero a una tabla dinamica de caracteres de 2 dimenciones almacenara el valor del pixel en escala de RED (0-255)
}BMP;


void abrir_imagen(BMP *imagen, char *ruta){
    FILE *archivo;	//Puntero FILE para el archivo de imágen a abrir
    int i,j;
    qDebug("ruta: %s\n", ruta);

    //Abrir el archivo de imágen
    archivo = fopen( ruta, "rb+" ); // rb rb+
    if(!archivo){
        //Si la imágen no se encuentra en la ruta dada
        qDebug( "La imágen %s no se encontro\n",ruta);
        exit(1);
    }

    //Leer la cabecera de la imagen y almacenarla en la
  // estructura a la que apunta la imagen

    fseek( archivo,0, SEEK_SET);
    fread(&imagen->bm,sizeof(char),2, archivo);
    fread(&imagen->tamano,sizeof(int),1, archivo);
    fread(&imagen->reservado,sizeof(int),1, archivo);
    fread(&imagen->offset,sizeof(int),1, archivo);
    fread(&imagen->tamanoMetadatos,sizeof(int),1, archivo);
    fread(&imagen->ancho,sizeof(int),1, archivo);
    fread(&imagen->alto,sizeof(int),1, archivo);
    fread(&imagen->numeroPlanos,sizeof(short int),1, archivo);
    fread(&imagen->profundidadColor,sizeof(short int),1, archivo);
    fread(&imagen->tipoCompresion,sizeof(int),1, archivo);
    fread(&imagen->tamanoEstructura,sizeof(int),1, archivo);
    fread(&imagen->pxmh,sizeof(int),1, archivo);
    fread(&imagen->pxmv,sizeof(int),1, archivo);
    fread(&imagen->coloresUsados,sizeof(int),1, archivo);
    fread(&imagen->coloresImportantes,sizeof(int),1, archivo);

    //Validar ciertos datos de la cabecera de la imágen
    if (imagen->bm[0]!='B'||imagen->bm[1]!='M')	{
        qDebug ("La imagen debe ser un bitmap.\n");
        exit(1);
    }
    if (imagen->profundidadColor!= 24) {
        qDebug ("La imagen debe ser de 24 bits.\n");
        exit(1);
    }

    //Reservar memoria para el arreglo que tendra la imágen en escala de BLUE (Arreglo de tamaño "img.ancho X img.alto")

    // imagen->pixelB=malloc (alto*sizeof(char*));
   imagen->pixelB = new unsigned char*[imagen->alto*sizeof(char*)];

  // cout<<"alto->"<<alt<<endl;


    for( i=0; i<imagen->alto; i++)
         // imagen->pixelB[i]=malloc (imagen->ancho* sizeof(char));
     imagen->pixelB[i] = new unsigned char[imagen->ancho* sizeof(char)];

    //Reservar memoria para el arreglo que tendra la imágen en escala de GREEN (Arreglo de tamaño "img.ancho X img.alto")
    // imagen->pixelG=malloc (imagen->alto* sizeof(char *));
  imagen->pixelG = new unsigned char*[imagen->alto* sizeof(char *)];
    for( i=0; i<imagen->alto; i++)
    imagen->pixelG[i] = new unsigned char[imagen->ancho* sizeof(char)];
        // imagen->pixelG[i]=malloc (imagen->ancho* sizeof(char));

    //Reservar memoria para el arreglo que tendra la imágen en escala de RED (Arreglo de tamaño "img.ancho X img.alto")
    // imagen->pixelR=malloc (imagen->alto* sizeof(char *));
  imagen->pixelR = new unsigned char*[imagen->alto* sizeof(char *)];
    for( i=0; i<imagen->alto; i++)
        imagen->pixelR[i]= new unsigned char[imagen->ancho* sizeof(char)];//malloc (imagen->ancho* sizeof(char));

    //Pasar la imágen al arreglo reservado en escala de BLUE,GREEN y RED
    unsigned char R,B,G;
    for (i=0;i<imagen->alto;i++){
        for (j=0;j<imagen->ancho;j++){
            fread(&B,sizeof(char),1, archivo);  //Byte Blue del pixel
            fread(&G,sizeof(char),1, archivo);  //Byte Green del pixel
            fread(&R,sizeof(char),1, archivo);  //Byte Red del pixel

            //imagen->pixel[i][j]=(unsigned char)((R*0.3)+(G*0.59)+ (B*0.11)); 	//Formula correcta
            //imagen->pixel[i][j]=(B+G+R)/3;								//Forma simple (Promedio)
            imagen->pixelB[i][j]=B;
            imagen->pixelG[i][j]=G;
            imagen->pixelR[i][j]=R;
        }
    }
    qDebug("imagen->alto: %d \n",imagen->alto);
    qDebug("imagen->ancho: %d \n",imagen->ancho);
    //Cerrrar el archivo
    fclose(archivo);

}

//****************************************************************************************************************************************************
//Función para crear una imagen BMP, a partir de la estructura imagen (Arreglo de bytes de alto*ancho  --- 1 Byte por pixel 0-255)
//Parametros de entrada: Referencia a un BMP (Estructura BMP), Referencia a la cadena ruta char ruta[]=char *ruta
//Parametro que devuelve: Ninguno
//****************************************************************************************************************************************************

void crear_imagen(BMP *imagen, char ruta[],int escala){
    FILE *archivo;	//Puntero FILE para el archivo de imágen a abrir

    int i,j;

    //Abrir el archivo de imágen
    archivo = fopen( ruta, "wb+" );
    if(!archivo)
    {
        //Si la imágen no se encuentra en la ruta dada
        qDebug( "La imágen %s no se pudo crear\n",ruta);
        exit(1);
    }
    //nueva img contenida en nueva estructura
    BMP imgNew;
    imgNew.bm[0]=imagen->bm[0];
    imgNew.bm[1]=imagen->bm[1];
    //imgNew.tamano=imagen->tamano;
    imgNew.reservado=imagen->reservado;
    imgNew.offset=imagen->offset;
    imgNew.tamanoMetadatos=imagen->tamanoMetadatos;
    //imgNew.alto=imagen->alto;
    //imgNew.ancho=imagen->ancho;
    imgNew.numeroPlanos=imagen->numeroPlanos;
    imgNew.profundidadColor=imagen->profundidadColor;
    imgNew.tipoCompresion=imagen->tipoCompresion;
    //imgNew.tamanoEstructura=imagen->tamanoEstructura;
    imgNew.pxmh=imagen->pxmh;
    imgNew.pxmv=imagen->pxmv;
    imgNew.coloresUsados=imagen->coloresUsados;
    imgNew.coloresImportantes=imagen->coloresImportantes;
    //
    imgNew.alto=(imagen->alto)*escala;
    imgNew.ancho=(imagen->ancho)*escala;
    imgNew.tamanoEstructura=imgNew.alto*imgNew.ancho*3;
    imgNew.tamano=54+imgNew.tamanoEstructura;


    qDebug("tipo imagNew: %c%c\n",imgNew.bm[0],imgNew.bm[1]);
    qDebug("tamano imagNew: %d\n",imgNew.tamano);
    qDebug("tamano alto: %d\n",imgNew.alto);
    qDebug("tamano ancho: %d\n",imgNew.ancho);
    qDebug("tamano tamanoMetadatos: %d\n",imgNew.tamanoMetadatos);
    qDebug("tamano tamanoEstructura: %d\n",imgNew.tamanoEstructura);
    qDebug("escala: %d\n",escala);
    qDebug("pxmh: %d\n",imgNew.pxmh);
    qDebug("pxmv: %d\n",imgNew.pxmv);
    //qDebug("sizeof(int): %u\n",sizeof(int));
    //qDebug("sizeof(short int): %u\n",sizeof(short int));
    //qDebug("sizeof(char): %u\n",sizeof(char));

    ////////////////////////////////////////////////////////////
    //Reservar memoria para el arreglo que tendra imgNew en escala de BLUE (Arreglo de tamaño "imgNew.ancho X imgNew.alto")
    // imgNew.pixelB=malloc (imgNew.alto* sizeof(char *));
  imgNew.pixelB = new unsigned char*[imgNew.alto* sizeof(char *)];

    for( i=0; i<imgNew.alto; i++)
        imgNew.pixelB[i]= new unsigned char[imgNew.ancho*sizeof(char)];//malloc (imgNew.ancho* sizeof(char));

    //Reservar memoria para el arreglo que tendra imgNew en escala de GREEN (Arreglo de tamaño "imgNew.ancho X imgNew.alto")
    // imgNew.pixelG=malloc (imgNew.alto* sizeof(char *));
  imgNew.pixelG = new unsigned char* [imgNew.alto* sizeof(char *)];
    for( i=0; i<imgNew.alto; i++)
        imgNew.pixelG[i]= new unsigned char[imgNew.ancho* sizeof(char)];//malloc (imgNew.ancho* sizeof(char));

    //Reservar memoria para el arreglo que tendra imgNew en escala de RED (Arreglo de tamaño "imgNew.ancho X imgNew.alto")
    imgNew.pixelR= new unsigned char*[imgNew.alto* sizeof(char *)];//malloc (imgNew.alto* sizeof(char *));

    for( i=0; i<imgNew.alto; i++)
        imgNew.pixelR[i]=new unsigned char[imgNew.ancho* sizeof(char)];//malloc (imgNew.ancho* sizeof(char));

    //Pasar la imágen al arreglo reservado en escala de BLUE,GREEN y RED
    for (i=0;i<imgNew.alto;i++){
        for (j=0;j<imgNew.ancho;j++){
            imgNew.pixelB[i][j]=(0);
            imgNew.pixelG[i][j]=(0);
            imgNew.pixelR[i][j]=(0);
        }
    }

    qDebug("xD-1.0\n");
    //paso de pixeles imagen a posiciones pares de imgNew
    for (i=0;i<imgNew.alto;i++){
        for (j=0;j<imgNew.ancho;j++){
            if((i%escala==0) && (j%escala==0)){
                imgNew.pixelB[i][j]=(imagen->pixelB[i/escala][j/escala]);
                imgNew.pixelG[i][j]=(imagen->pixelG[i/escala][j/escala]);
                imgNew.pixelR[i][j]=(imagen->pixelR[i/escala][j/escala]);

            }
        }
    }
    qDebug("imagen->ancho: %d\n",imagen->ancho);
    qDebug("imgNew.ancho%d\n",imgNew.ancho);
    qDebug("xD0.0\n");

    //interpolación horizontal de pixeles
    for (i=0;i<imgNew.alto;i++){
        for (j=1;j<imgNew.ancho;j++){
            if((i%escala==0) && (j%escala!=0)){
                imgNew.pixelB[i][j]=(imgNew.pixelB[i][j-1]+imgNew.pixelB[i][j+1])/2;
                imgNew.pixelG[i][j]=(imgNew.pixelG[i][j-1]+imgNew.pixelG[i][j+1])/2;
                imgNew.pixelR[i][j]=(imgNew.pixelR[i][j-1]+imgNew.pixelR[i][j+1])/2;
            }
        }
    }
    qDebug("%d\n",imgNew.ancho);
    qDebug("xD0.1\n");

    //interpolación vertical de pixeles
    for (i=0;i<imgNew.alto-1;i++){
        for (j=0;j<imgNew.ancho;j++){
            if(i%escala!=0){
                imgNew.pixelB[i][j]=(imgNew.pixelB[i-1][j]+imgNew.pixelB[i+1][j])/2;
                imgNew.pixelG[i][j]=(imgNew.pixelG[i-1][j]+imgNew.pixelG[i+1][j])/2;
                imgNew.pixelR[i][j]=(imgNew.pixelR[i-1][j]+imgNew.pixelR[i+1][j])/2;
            }
        }
    }

    qDebug("xD1\n");
    //Escribir la cabecera de la imagen en el archivo
    /*
    fseek( archivo,0, SEEK_SET);

    fwrite(&imgNew.bm,sizeof(char),2, archivo);
    fwrite(&imgNew.tamano,sizeof(int),1, archivo);
    fwrite(&imgNew.reservado,sizeof(int),1, archivo);
    fwrite(&imgNew.offset,sizeof(int),1, archivo);
    fwrite(&imgNew.tamanoMetadatos,sizeof(int),1, archivo);
    fwrite(&imgNew.ancho,sizeof(int),1, archivo);
    fwrite(&imgNew.alto,sizeof(int),1, archivo);

    fwrite(&imgNew.numeroPlanos,sizeof(short int),1, archivo);
    fwrite(&imgNew.profundidadColor,sizeof(short int),1, archivo);
    fwrite(&imgNew.tipoCompresion,sizeof(int),1, archivo);
    fwrite(&imgNew.tamanoEstructura,sizeof(int),1, archivo);
    fwrite(&imgNew.pxmh,sizeof(int),1, archivo);
    fwrite(&imgNew.pxmv,sizeof(int),1, archivo);
    fwrite(&imgNew.coloresUsados,sizeof(int),1, archivo);
    fwrite(&imgNew.coloresImportantes,sizeof(int),1, archivo);
    */

    qDebug("xD2\n");
    //Pasar la imágen del arreglo reservado en escala de BLUE,GREEN y RED a el archivo (Deben escribirse los valores BGR)
    for (i=0;i<imgNew.alto;i++){
        for (j=0;j<imgNew.ancho;j++){
            //Ecribir los 3 bytes BGR al archivo BMP, en este caso todos se igualan al mismo valor (Valor del pixel en la matriz de la estructura imagen)
            fwrite(&imgNew.pixelB[i][j],sizeof(char),1, archivo);  //Escribir el Byte Blue del pixel
            fwrite(&imgNew.pixelG[i][j],sizeof(char),1, archivo);  //Escribir el Byte Green del pixel
            fwrite(&imgNew.pixelR[i][j],sizeof(char),1, archivo);  //Escribir el Byte Red del pixel
        }
    }
    qDebug("%u,%u,%u\n",imgNew.pixelB[0][0],imgNew.pixelG[0][0],imgNew.pixelR[0][0]);
    qDebug("%u,%u,%u\n",imagen->pixelB[0][0],imagen->pixelG[0][0],imagen->pixelR[0][0]);
    qDebug("%u,%u,%u\n",imgNew.pixelB[100][470],imgNew.pixelG[100][470],imgNew.pixelR[100][470]);
    qDebug("%u,%u,%u\n",imagen->pixelB[100][470],imagen->pixelG[100][470],imagen->pixelR[100][470]);
    //Cerrrar el archivo
    fclose(archivo);
}

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    // cargar la imagen em mapa
    //QPixmap picObj("/home/ewilderd/ImagesProcessor/imagen.bmp");
    QPixmap picObj("/home/ewilderd/ImagesProcessor/dog.jpg");

    //asignar la imagen al label
    ui->Lbl_pic-> setPixmap(picObj);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_btn_showPic_clicked()
{
    // Cargar la imagen en mapa para mostrar
    QPixmap newPic("/home/ewilderd/ImagesProcessor/imagen.bmp");



    // asignar img al label
    ui->Lbl_pic-> setPixmap(newPic);

     BMP img;
     int escala;

     abrir_imagen(&img, "/home/ewilderd/Documents/II_semestre/Imagenes/Algo1Parte/tarea1BMP/me.bmp");


//    crear_imagen(&img,IMAGEN_TRATADA,escala);


    string mystr="";
//    cout<<mystr<<endl;
    // we need to user Qstring object
    QString str ="DATOS DE LA IMAGEN \n";
    ui->textEdit->append(str);
    str = "Alto : "+QString::number(img.alto);
    ui->textEdit->append(str);
    str= "Ancho : "+QString::number(img.ancho);
    ui->textEdit->append(str);

    str= "Tamanio : "+QString::number(img.tamano);
    ui->textEdit->append(str);
    str= "Tipo Archivo : "+img.tipoCompresion;
    ui->textEdit->append(str);

//    ui->textEdit->setText(QString::fromStdString(mystr));

}

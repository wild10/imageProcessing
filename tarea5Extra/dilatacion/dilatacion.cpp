
#include <stdio.h>
#include <stdlib.h>
#include <string.h>	
#include <bits/stdc++.h>
#include <random>
#include <ctime>
#include <cmath>
using namespace std;
#define 	IMAGEN_TRATADA1	"imgDilatada.bmp"		//Ruta y nombre del archivo de la imagen de entrada BMP
#define 	IMAGEN_TRATADA2	"tratadaRestaurada.bmp"		//Ruta y nombre del archivo de la imagen de salida BMP

//g++ -std=c++11 dilatar.cpp -o dilatar
//./dilatar letraA.bmp

typedef struct BMP{
	char bm[2];					//(2 Bytes) BM (Tipo de archivo)
	int tamano;					//(4 Bytes) Tamaño del archivo en bytes
	int reservado;					//(4 Bytes) Reservado
	int offset;						//(4 Bytes) offset, distancia en bytes entre la img y los píxeles
	int tamanoMetadatos;			//(4 Bytes) Tamaño de Metadatos (tamaño de esta estructura = 40)
	int ancho;					//(4 Bytes) Alto (numero de pixeles verticales)
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

BMP imgNew;
void abrir_imagen(BMP *imagen, char ruta[]);	//Función para abrir la imagen BMP
void crear_imagenDilatada(BMP *imagen, char ruta[]);	//Función para crear una imagen BMP


int main (int argc, char* argv[]){	
	int i,j; 				//Variables auxiliares para loops
	//int n=IMAGEN_TRATADA.length();
	
	BMP img;				//Estructura de tipo imágen
	char IMAGEN[45];		//Almacenará la ruta de la imagen
	char imagen_tratada1[30];
	char imagen_tratada2[30];
		
	//Si no se introduce una ruta de imágen
	if (argc!=2) {
		printf("Indique el nombre del archivo a codificar - Ejemplo: [user@equipo]$ %s imagen.bmp\n",argv[0]);
		printf("Indique el valor de la escala - Ejemplo: -x2\n");
		exit(1);
	} 

	strcpy(imagen_tratada1,IMAGEN_TRATADA1);
	strcpy(imagen_tratada2,IMAGEN_TRATADA2);
	//Almacenar la ruta de la imágen
	strcpy(IMAGEN,argv[1]);
	//Almacenar del valor de escala

	//***************************************************************************************************************************
	//0 Abrir la imágen BMP de 24 bits, almacenar su cabecera en la estructura img y colocar sus pixeles en el arreglo img.pixel[][]
	//***************************************************************************************************************************	
	abrir_imagen(&img,IMAGEN);
	printf("\n*************************************************************************");
	printf("\nIMAGEN: %s",IMAGEN);
	printf("\n*************************************************************************");
	printf("\nDimensiones de la imágen:\tAlto=%d\tAncho=%d\n",img.alto,img.ancho);

	crear_imagenDilatada(&img,imagen_tratada1);
	printf("\nImágen BMP tratada en el archivo: %s\n",IMAGEN_TRATADA1);
	
	exit (0);	
}

//*************************************************************************************************************************************************
//Función para abrir la imagen, colocarla en escala de grisis en la estructura imagen imagen (Arreglo de bytes de alto*ancho  --- 1 Byte por pixel 0-255)
//Parametros de entrada: Referencia a un BMP (Estructura BMP), Referencia a la cadena ruta char ruta[]=char *ruta
//Parametro que devuelve: Ninguno
//*************************************************************************************************************************************************
void abrir_imagen(BMP *imagen, char* ruta){
	FILE *archivo;	//Puntero FILE para el archivo de imágen a abrir
	int i,j;
	
	//Abrir el archivo de imágen
	archivo = fopen( ruta, "rb+" );
	printf("ruta: \n%s",ruta);

	if(!archivo){ 
		//Si la imágen no se encuentra en la ruta dada
		printf( "La imágen %s no se encontro\n",ruta);
		exit(1);
	}

	//Leer la cabecera de la imagen y almacenarla en la estructura a la que apunta imagen
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
		printf ("La imagen debe ser un bitmap.\n"); 
		exit(1);
	}
	if (imagen->profundidadColor!= 24) {
		printf ("La imagen debe ser de 24 bits.\n"); 
		exit(1);
	}

	//Reservar memoria para el arreglo que tendra la imágen en escala de BLUE (Arreglo de tamaño "img.ancho X img.alto")
	imagen->pixelB=(unsigned char**)malloc (imagen->alto* sizeof(char *)); 
	for( i=0; i<imagen->alto; i++)
		imagen->pixelB[i]=(unsigned char*)malloc (imagen->ancho* sizeof(char));

	//Reservar memoria para el arreglo que tendra la imágen en escala de GREEN (Arreglo de tamaño "img.ancho X img.alto")
	imagen->pixelG=(unsigned char**)malloc (imagen->alto* sizeof(char *)); 
	for( i=0; i<imagen->alto; i++)
		imagen->pixelG[i]=(unsigned char*)malloc (imagen->ancho* sizeof(char));

	//Reservar memoria para el arreglo que tendra la imágen en escala de RED (Arreglo de tamaño "img.ancho X img.alto")
	imagen->pixelR=(unsigned char**)malloc (imagen->alto* sizeof(char *)); 
	for( i=0; i<imagen->alto; i++)
		imagen->pixelR[i]=(unsigned char*)malloc (imagen->ancho* sizeof(char));
	
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
	printf("imagen->alto: %d \n",imagen->alto);
	printf("imagen->ancho: %d \n",imagen->ancho);
	//Cerrrar el archivo
	fclose(archivo);	
}

//****************************************************************************************************************************************************
//Función para crear una imagen BMP, a partir de la estructura imagen (Arreglo de bytes de alto*ancho  --- 1 Byte por pixel 0-255)
//Parametros de entrada: Referencia a un BMP (Estructura BMP), Referencia a la cadena ruta char ruta[]=char *ruta
//Parametro que devuelve: Ninguno
//****************************************************************************************************************************************************
void crear_imagenDilatada(BMP *imagen, char ruta[]){
	FILE *archivo;	//Puntero FILE para el archivo de imágen a abrir

	int i,j;

	//Abrir el archivo de imágen
	archivo = fopen( ruta, "wb+" );
	if(!archivo)
	{ 
		//Si la imágen no se encuentra en la ruta dada
		printf( "La imágen %s no se pudo crear\n",ruta);
		exit(1);
	}
	//nueva img contenida en nueva estructura
	

	imgNew.bm[0]=imagen->bm[0];
	imgNew.bm[1]=imagen->bm[1];
	imgNew.tamano=imagen->tamano; //
	imgNew.reservado=imagen->reservado;
	imgNew.offset=imagen->offset;
	imgNew.tamanoMetadatos=imagen->tamanoMetadatos;
	imgNew.alto=imagen->alto; //
	imgNew.ancho=imagen->ancho; //
	imgNew.numeroPlanos=imagen->numeroPlanos;
	imgNew.profundidadColor=imagen->profundidadColor;
	imgNew.tipoCompresion=imagen->tipoCompresion;
	imgNew.tamanoEstructura=imagen->tamanoEstructura; //
	imgNew.pxmh=imagen->pxmh;
	imgNew.pxmv=imagen->pxmv;
	imgNew.coloresUsados=imagen->coloresUsados;
	imgNew.coloresImportantes=imagen->coloresImportantes;
	

	printf("tipo imagNew: %c%c\n",imgNew.bm[0],imgNew.bm[1]);
	printf("tamano imagNew: %d\n",imgNew.tamano);
	printf("tamano alto: %d\n",imgNew.alto);
	printf("tamano ancho: %d\n",imgNew.ancho);
	printf("tamano tamanoMetadatos: %d\n",imgNew.tamanoMetadatos);
	printf("tamano tamanoEstructura: %d\n",imgNew.tamanoEstructura);
	printf("pxmh: %d\n",imgNew.pxmh);
	printf("pxmv: %d\n",imgNew.pxmv);
	//printf("sizeof(int): %u\n",sizeof(int));
	//printf("sizeof(short int): %u\n",sizeof(short int));
	//printf("sizeof(char): %u\n",sizeof(char));

	////////////////////////////////////////////////////////////
	//Reservar memoria para el arreglo que tendra imgNew en escala de BLUE (Arreglo de tamaño "imgNew.ancho X imgNew.alto")
	imgNew.pixelB=(unsigned char**)malloc (imgNew.alto* sizeof(char *)); 
	for( i=0; i<imgNew.alto; i++)
		imgNew.pixelB[i]=(unsigned char*)malloc (imgNew.ancho* sizeof(char));

	//Reservar memoria para el arreglo que tendra imgNew en escala de GREEN (Arreglo de tamaño "imgNew.ancho X imgNew.alto")
	imgNew.pixelG=(unsigned char**)malloc (imgNew.alto* sizeof(char *));
	for( i=0; i<imgNew.alto; i++)
		imgNew.pixelG[i]=(unsigned char*)malloc (imgNew.ancho* sizeof(char));

	//Reservar memoria para el arreglo que tendra imgNew en escala de RED (Arreglo de tamaño "imgNew.ancho X imgNew.alto")
	imgNew.pixelR=(unsigned char**)malloc (imgNew.alto* sizeof(char *)); 
	for( i=0; i<imgNew.alto; i++)
		imgNew.pixelR[i]=(unsigned char*)malloc (imgNew.ancho* sizeof(char));

	//Reservar memoria para el "stencil"
	unsigned char** stencil;
	stencil=(unsigned char**)malloc (imgNew.alto* sizeof(char *)); 
	for( i=0; i<imgNew.alto; i++)
		stencil[i]=(unsigned char*)malloc (imgNew.ancho* sizeof(char));


	//asegurar que este en blanco y negro 0 ó 255
    for (i=0;i<imgNew.alto;i++){
		for (j=0;j<imgNew.ancho;j++){
			float B=imagen->pixelB[i][j];
			float G=imagen->pixelG[i][j];
			float R=imagen->pixelR[i][j];
			if(sqrt(B*B+G*G+R*R)<128){
				cout<<"sqrt(B*B+G*G+R*R): "<<sqrt(B*B+G*G+R*R)<<endl;
				imgNew.pixelB[i][j]=0;
				imgNew.pixelG[i][j]=0;
				imgNew.pixelR[i][j]=0;
			}
			else{
				imgNew.pixelB[i][j]=255;
				imgNew.pixelG[i][j]=255;
				imgNew.pixelR[i][j]=255;	
			}
		}
	}




	int it=4;
	for(int k=0;k<it;k++){
			// inicializacion del stencil
		for (i=0;i<imgNew.alto;i++){
			for (j=0;j<imgNew.ancho;j++){
				stencil[i][j]=0;
			}
		}

		//
		for (i=2;i<imgNew.alto-2;i++){
			for (j=2;j<imgNew.ancho-2;j++){
				if(imgNew.pixelB[i][j]==0){
					stencil[i][j-2]=stencil[i][j-2]+1;
					stencil[i][j-1]=stencil[i][j-1]+1;
					stencil[i][j+1]=stencil[i][j+1]+1;
					stencil[i][j+2]=stencil[i][j+2]+1;

					stencil[i-1][j-1]=stencil[i-1][j-1]+1;
					stencil[i-1][j]=stencil[i-1][j]+1;
					stencil[i-1][j+1]=stencil[i-1][j+1]+1;

					stencil[i+1][j-1]=stencil[i+1][j-1]+1;
					stencil[i+1][j]=stencil[i+1][j]+1;
					stencil[i+1][j+1]=stencil[i+1][j+1]+1;

					stencil[i-2][j]=stencil[i-2][j]+1;
					stencil[i+2][j]=stencil[i+2][j]+1;
				}
			}
		}
		/*for (i=1;i<imgNew.alto-1;i++){
			for (j=1;j<imgNew.ancho-1;j++){
				if(imgNew.pixelB[i][j]==0){
					stencil[i][j-1]=stencil[i][j-1]+1;
					stencil[i][j+1]=stencil[i][j+1]+1;
					stencil[i-1][j]=stencil[i-1][j]+1;
					stencil[i+1][j]=stencil[i+1][j]+1;
				}
			}
		}
		*/
		//
		for (i=1;i<imgNew.alto;i++){
			for (j=1;j<imgNew.ancho;j++){
				if(stencil[i][j]>0){
					imgNew.pixelB[i][j]=0;
					imgNew.pixelG[i][j]=0;
					imgNew.pixelR[i][j]=0;
				}
				else{
					imgNew.pixelB[i][j]=255;
					imgNew.pixelG[i][j]=255;
					imgNew.pixelR[i][j]=255;
				}
			}
		}
	}
    

	printf("xD1\n");
	//Escribir la cabecera de la imagen en el archivo
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
	
	printf("xD2\n");
	//Pasar la imágen del arreglo reservado en escala de BLUE,GREEN y RED a el archivo (Deben escribirse los valores BGR)
	for (i=0;i<imgNew.alto/*imagen->alto*/;i++){
		for (j=0;j<imgNew.ancho/*imagen->ancho*/;j++){ 
			//Ecribir los 3 bytes BGR al archivo BMP, en este caso todos se igualan al mismo valor (Valor del pixel en la matriz de la estructura imagen)
			fwrite(&imgNew.pixelB[i][j],sizeof(char),1, archivo);  //Escribir el Byte Blue del pixel 
			fwrite(&imgNew.pixelG[i][j],sizeof(char),1, archivo);  //Escribir el Byte Green del pixel
			fwrite(&imgNew.pixelR[i][j],sizeof(char),1, archivo);  //Escribir el Byte Red del pixel
		}
	}
	printf("%u,%u,%u\n",imgNew.pixelB[0][0],imgNew.pixelG[0][0],imgNew.pixelR[0][0]);
	printf("%u,%u,%u\n",imagen->pixelB[0][0],imagen->pixelG[0][0],imagen->pixelR[0][0]);
	printf("%u,%u,%u\n",imgNew.pixelB[100][470],imgNew.pixelG[100][470],imgNew.pixelR[100][470]);
	printf("%u,%u,%u\n",imagen->pixelB[100][470],imagen->pixelG[100][470],imagen->pixelR[100][470]);
	//Cerrrar el archivo
	fclose(archivo);
}
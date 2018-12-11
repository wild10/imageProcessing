
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define 	IMAGEN_TRATADA	"tratada.bmp"		//Ruta y nombre del archivo de la imagen de salida BMP

//gcc -o bmpV1 bmpV1.c
//./bmpV1 linux_detergente.bmp -x2
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

void abrir_imagen(BMP *imagen, char ruta[]);	//Función para abrir la imagen BMP
void crear_imagen(BMP *imagen, char ruta[],int escala);	//Función para crear una imagen BMP


int main (int argc, char* argv[]){	
	int i,j; 				//Variables auxiliares para loops
	BMP img;				//Estructura de tipo imágen
	char IMAGEN[45];		//Almacenará la ruta de la imagen
	char SCALE[3];
	int escala;
		
	//Si no se introduce una ruta de imágen
	if (argc!=3) 
	{
		printf("Indique el nombre del archivo a codificar - Ejemplo: [user@equipo]$ %s imagen.bmp\n",argv[0]);
		printf("Indique el valor de la escala - Ejemplo: -x2\n");
		exit(1);
	} 
	//Almacenar la ruta de la imágen
	strcpy(IMAGEN,argv[1]);
	//Almacenar del valor de escala
	strcpy(SCALE,argv[2]);
	printf("%s\n",SCALE);
	escala=SCALE[2]-48;
	printf("%d\n",escala);

	//***************************************************************************************************************************
	//0 Abrir la imágen BMP de 24 bits, almacenar su cabecera en la estructura img y colocar sus pixeles en el arreglo img.pixel[][]
	//***************************************************************************************************************************	
	abrir_imagen(&img,IMAGEN);
	printf("\n*************************************************************************");
	printf("\nIMAGEN: %s",IMAGEN);
	printf("\n*************************************************************************");
	printf("\nDimensiones de la imágen:\tAlto=%d\tAncho=%d\n",img.alto,img.ancho);
	

	crear_imagen(&img,IMAGEN_TRATADA,escala);
	printf("\nImágen BMP tratada en el archivo: %s\n",IMAGEN_TRATADA);
	
	exit (0);	
}

//*************************************************************************************************************************************************
//Función para abrir la imagen, colocarla en escala de grisis en la estructura imagen imagen (Arreglo de bytes de alto*ancho  --- 1 Byte por pixel 0-255)
//Parametros de entrada: Referencia a un BMP (Estructura BMP), Referencia a la cadena ruta char ruta[]=char *ruta
//Parametro que devuelve: Ninguno
//*************************************************************************************************************************************************
void abrir_imagen(BMP *imagen, char *ruta){
	FILE *archivo;	//Puntero FILE para el archivo de imágen a abrir
	int i,j;
	
	//Abrir el archivo de imágen
	archivo = fopen( ruta, "rb+" );
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
	imagen->pixelB=malloc (imagen->alto* sizeof(char *)); 
	for( i=0; i<imagen->alto; i++)
		imagen->pixelB[i]=malloc (imagen->ancho* sizeof(char));

	//Reservar memoria para el arreglo que tendra la imágen en escala de GREEN (Arreglo de tamaño "img.ancho X img.alto")
	imagen->pixelG=malloc (imagen->alto* sizeof(char *)); 
	for( i=0; i<imagen->alto; i++)
		imagen->pixelG[i]=malloc (imagen->ancho* sizeof(char));

	//Reservar memoria para el arreglo que tendra la imágen en escala de RED (Arreglo de tamaño "img.ancho X img.alto")
	imagen->pixelR=malloc (imagen->alto* sizeof(char *)); 
	for( i=0; i<imagen->alto; i++)
		imagen->pixelR[i]=malloc (imagen->ancho* sizeof(char));
	
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
//Función para crear una imagen BMP, a partir de la estructura imagen imagen (Arreglo de bytes de alto*ancho  --- 1 Byte por pixel 0-255)
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
		printf( "La imágen %s no se pudo crear\n",ruta);
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
	

	printf("tipo imagNew: %c%c\n",imgNew.bm[0],imgNew.bm[1]);
	printf("tamano imagNew: %d\n",imgNew.tamano);
	printf("tamano alto: %d\n",imgNew.alto);
	printf("tamano ancho: %d\n",imgNew.ancho);
	printf("tamano tamanoMetadatos: %d\n",imgNew.tamanoMetadatos);
	printf("tamano tamanoEstructura: %d\n",imgNew.tamanoEstructura);
	printf("escala: %d\n",escala);
	printf("pxmh: %d\n",imgNew.pxmh);
	printf("pxmv: %d\n",imgNew.pxmv);
	//printf("sizeof(int): %u\n",sizeof(int));
	//printf("sizeof(short int): %u\n",sizeof(short int));
	//printf("sizeof(char): %u\n",sizeof(char));

	////////////////////////////////////////////////////////////
	//Reservar memoria para el arreglo que tendra imgNew en escala de BLUE (Arreglo de tamaño "imgNew.ancho X imgNew.alto")
	imgNew.pixelB=malloc (imgNew.alto* sizeof(char *)); 
	for( i=0; i<imgNew.alto; i++)
		imgNew.pixelB[i]=malloc (imgNew.ancho* sizeof(char));

	//Reservar memoria para el arreglo que tendra imgNew en escala de GREEN (Arreglo de tamaño "imgNew.ancho X imgNew.alto")
	imgNew.pixelG=malloc (imgNew.alto* sizeof(char *));
	for( i=0; i<imgNew.alto; i++)
		imgNew.pixelG[i]=malloc (imgNew.ancho* sizeof(char));

	//Reservar memoria para el arreglo que tendra imgNew en escala de RED (Arreglo de tamaño "imgNew.ancho X imgNew.alto")
	imgNew.pixelR=malloc (imgNew.alto* sizeof(char *)); 
	for( i=0; i<imgNew.alto; i++)
		imgNew.pixelR[i]=malloc (imgNew.ancho* sizeof(char));
	
	//Pasar la imágen al arreglo reservado en escala de BLUE,GREEN y RED
	for (i=0;i<imgNew.alto;i++){
		for (j=0;j<imgNew.ancho;j++){ 
			imgNew.pixelB[i][j]=(0);
			imgNew.pixelG[i][j]=(0);
			imgNew.pixelR[i][j]=(0);
		}
	}
	
	printf("xD-1.0\n");
	//paso de pixeles imagen a posiciones pares de imgNew
	for (i=0;i<imgNew.alto;i++){
		for (j=0;j<imgNew.ancho;j++){
			if((i%escala==0) && (j%escala==0)){
				imgNew.pixelB[i][j]=(imagen->pixelB[i/escala][j/escala]);
				imgNew.pixelG[i][j]=(imagen->pixelG[i/escala][j/escala]);
				imgNew.pixelR[i][j]=(imagen->pixelR[i/escala][j/escala]);
				/*imgNew.pixelB[i][j]=imagen->pixelB[i][j];
				imgNew.pixelG[i][j]=imagen->pixelG[i][j];
				imgNew.pixelR[i][j]=imagen->pixelR[i][j];*/
			}
		}   
	}
	printf("imagen->ancho: %d\n",imagen->ancho);
	printf("imgNew.ancho%d\n",imgNew.ancho);
	printf("xD0.0\n");
	
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
	printf("%d\n",imgNew.ancho);
	printf("xD0.1\n");
	
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
	

	///////rotacion/////////////////////////////////////////////////////////////////
	unsigned char** pixelBrot;
	unsigned char** pixelGrot;
	unsigned char** pixelRrot;
	//Reservar memoria para el arreglo resultante de rotar imgNew en escala de BLUE
	pixelBrot=malloc (imgNew.alto* sizeof(char *)); 
	for( i=0; i<imgNew.alto; i++)
		pixelBrot[i]=malloc (imgNew.ancho* sizeof(char));

	pixelGrot=malloc (imgNew.alto* sizeof(char *)); 
	for( i=0; i<imgNew.alto; i++)
		pixelGrot[i]=malloc (imgNew.ancho* sizeof(char));

	pixelRrot=malloc (imgNew.alto* sizeof(char *)); 
	for( i=0; i<imgNew.alto; i++)
		pixelRrot[i]=malloc (imgNew.ancho* sizeof(char));

	float _PI=3.141592653;
	float alpha=_PI/4;

	float rotation[2][2]={{cos(3.141592653/16),-1*sin(3.141592653/16)},
						  {sin(3.141592653/16),cos(3.141592653/16)}
						 };

	for(int i=0;i<2;i++){
		for(int j=0;j<2;j++){
			printf("%f ",rotation[i][j]);
		}
		printf("\n");
	}

	for(int i=0;i<imgNew.alto;i++){
		for(int j=0;j<imgNew.ancho;j++){
			int ii=rotation[0][0]*(i-imgNew.alto/2)+rotation[0][1]*(j-imgNew.ancho/2) + imgNew.alto/2;
			int jj=rotation[1][0]*(i-imgNew.alto/2)+rotation[1][1]*(j-imgNew.ancho/2) + imgNew.ancho/2;
			if((0<=ii && ii<imgNew.alto) && (0<=jj && jj<imgNew.ancho)){
				pixelBrot[ii][jj]=imgNew.pixelB[i][j];
				pixelGrot[ii][jj]=imgNew.pixelG[i][j];
				pixelRrot[ii][jj]=imgNew.pixelR[i][j];
			}
		}
	}

	//correccion de puntos negros
	for(int i=0;i<imgNew.alto;i++){
		for(int j=1;j<imgNew.ancho-1;j++){
			if(pixelBrot[i][j]==0 && pixelGrot[i][j]==0 && pixelRrot[i][j]==0){
				pixelBrot[i][j]= (pixelBrot[i][j-1]+ pixelBrot[i][j+1])/2;
				pixelGrot[i][j]= (pixelGrot[i][j-1]+ pixelGrot[i][j+1])/2;
				pixelRrot[i][j]= (pixelRrot[i][j-1]+ pixelRrot[i][j+1])/2;
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
			/*imgNew.pixelB[i][j]= 255-imgNew.pixelB[i][j];
			imgNew.pixelG[i][j]= 255-imgNew.pixelG[i][j];
			imgNew.pixelR[i][j]= 255-imgNew.pixelR[i][j];*/
			/*
			fwrite(&imgNew.pixelB[i][j],sizeof(char),1, archivo);  //Escribir el Byte Blue del pixel 
			fwrite(&imgNew.pixelG[i][j],sizeof(char),1, archivo);  //Escribir el Byte Green del pixel
			fwrite(&imgNew.pixelR[i][j],sizeof(char),1, archivo);  //Escribir el Byte Red del pixel
			*/
			fwrite(&pixelBrot[i][j],sizeof(char),1, archivo);  //Escribir el Byte Blue del pixel 
			fwrite(&pixelGrot[i][j],sizeof(char),1, archivo);  //Escribir el Byte Green del pixel
			fwrite(&pixelRrot[i][j],sizeof(char),1, archivo);  //Escribir el Byte Red del pixel

			/*fwrite(&imagen->pixelB[i][j],sizeof(char),1, archivo);  //Escribir el Byte Blue del pixel 
			fwrite(&imagen->pixelG[i][j],sizeof(char),1, archivo);  //Escribir el Byte Green del pixel
			fwrite(&imagen->pixelR[i][j],sizeof(char),1, archivo);  //Escribir el Byte Red del pixel*/
		}
	}
	printf("%u,%u,%u\n",imgNew.pixelB[0][0],imgNew.pixelG[0][0],imgNew.pixelR[0][0]);
	printf("%u,%u,%u\n",imagen->pixelB[0][0],imagen->pixelG[0][0],imagen->pixelR[0][0]);
	printf("%u,%u,%u\n",imgNew.pixelB[100][470],imgNew.pixelG[100][470],imgNew.pixelR[100][470]);
	printf("%u,%u,%u\n",imagen->pixelB[100][470],imagen->pixelG[100][470],imagen->pixelR[100][470]);
	//Cerrrar el archivo
	fclose(archivo);
}	

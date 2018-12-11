
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
	if (argc!=3) {
		printf("Indique el nombre del archivo a codificar - Ejemplo: [user@equipo]$ %s imagen.bmp\n",argv[0]);
		printf("Indique el valor de la escala - Ejemplo: -x2\n");
		exit(1);
	} 

	//Almacenar la ruta de la imágen
	strcpy(IMAGEN,argv[1]);
	//Almacenar del valor de escala
	strcpy(SCALE,argv[2]);
	//printf("%s\n",SCALE);
	escala=SCALE[2]-48;
	//printf("%d\n",escala);

	//***************************************************************************************************************************
	//0 Abrir la imágen BMP de 24 bits, almacenar su cabecera en la estructura img y colocar sus pixeles en el arreglo img.pixel[][]
	//***************************************************************************************************************************	
	abrir_imagen(&img,"01.bmp");
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
	
	//histograma imagen//
	//creación de histogramas y reserva de memoria
	int np=imagen->ancho*imagen->alto; //numero total de pixeles
	int profColor=256;
	int* histogramB;
	histogramB=malloc (profColor* sizeof(int *)); 
	int* histogramG;
	histogramG=malloc (profColor* sizeof(int *)); 
	int* histogramR;
	histogramR=malloc (profColor* sizeof(int *)); 
	int* histogram;
	histogram=malloc (profColor* sizeof(int *)); 

	for (i=0;i<profColor;i++){
		histogram[i]=0; 
	}

	for (i=0;i<imagen->alto;i++){
		unsigned char griseq;
		for (j=0;j<imagen->ancho;j++){
			griseq=((imagen->pixelB[i][j])+(imagen->pixelG[i][j])+(imagen->pixelR[i][j]))/3;
			histogram[griseq]++;
		}
	}

	//función de ecualización
	float* fequalizer;
	float acumulado;
	fequalizer=malloc (profColor* sizeof(float *)); 
	fequalizer[0]=0;
	acumulado=histogram[0];
	for(i=1;i<profColor-1;i++){
		fequalizer[i]=(acumulado*255)/((float)np);
		//printf("histogram: %d \n",histogram[i-1]);
		//printf("fequalizer: %f \n",fequalizer[i]);
		acumulado=acumulado+(float)histogram[i];
		//printf("acumulado: %f \n",acumulado);
	}
	fequalizer[255]=255;


	//ver histogramas
/*	printf("histogramaB: \n");
	for (i=0;i<profColor;i++){
		printf("%d ",histogramB[i]);
	} printf("\n");

	printf("histogramaG: \n");
	for (i=0;i<profColor;i++){
		printf("%d ",histogramG[i]);
	} printf("\n");

	printf("histogramaG: \n");
	for (i=0;i<profColor;i++){
		printf("%d ",histogramR[i]);
	} printf("\n");
	*/
	printf("histograma: \n");
	for (i=0;i<profColor;i++){
		printf("%d ",histogram[i]);
	} printf("\n");
	printf("fequalizer: \n");
	for (i=0;i<profColor;i++){
		printf("%f ",fequalizer[i]);
	} printf("\n");


	for (i=0;i<imgNew.alto;i++){
		for (j=0;j<imgNew.ancho;j++){ 
			imgNew.pixelB[i][j]=(unsigned char)(fequalizer[imagen->pixelB[i][j]]);
			imgNew.pixelG[i][j]=(unsigned char)(fequalizer[imagen->pixelG[i][j]]);
			imgNew.pixelR[i][j]=(unsigned char)(fequalizer[imagen->pixelR[i][j]]);
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
			fwrite(&imgNew.pixelB[i][j],sizeof(char),1, archivo);  //Escribir el Byte Blue del pixel 
			fwrite(&imgNew.pixelG[i][j],sizeof(char),1, archivo);  //Escribir el Byte Green del pixel
			fwrite(&imgNew.pixelR[i][j],sizeof(char),1, archivo);  //Escribir el Byte Red del pixel
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
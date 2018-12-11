/*
*  program:  FFT using opencv
*  by : wild.10@gmail.com
*  class: image processing
*/

// COMPILACION
/*
*   g++ -Wall -o out fft.cpp `pkg-config --cflags --libs opencv`
*   ./out firulay.jpg
*/
// Primero vamos a importar las librerias de opencv
#include "opencv2/core/core.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"


#include<iostream>


using namespace std;
using namespace cv;

// ingresaremos desde la consola
// argcs: int
// argv : vector

static void help(char* progName){
  cout<<endl;
}

int main(int argc, char ** argv){


  const char* filename;

  help(argv[0]);

  if(argc >= 2)
    filename = argv[1];
  else
    filename = "firulay.jpg";

  // printf("%c", filename);
  cout<<"file name: "<<filename<<endl;
  //cargaremos la imagen en blanco y negro
  //Mat img = imread(filename, CV_LOAD_IMAGE_COLOR);
  Mat img = imread(filename, CV_LOAD_IMAGE_GRAYSCALE);
  if(img.empty())
    return -1;

  Mat imgAmpliada;
  // extraer bordes para hacer un copia de bordes
  int FilExt = getOptimalDFTSize(img.rows);
  int ColExt = getOptimalDFTSize(img.cols);

  printf("\n fila: %d \n cols: %d\n",FilExt,ColExt);


  // copyMakeBorder( src, dst, top, bottom, left, right, borderType, value );
  copyMakeBorder(img, imgAmpliada, 0, (FilExt - img.rows), 0, (ColExt-img.cols), BORDER_CONSTANT, Scalar::all(0));

  // cout<< imgAmpliada<<endl;
  // Mat planes[] = {Mat_<float>(imgAmpliada), Mat::zeros(imgAmpliada.size(), CV_32F)};
  Mat planos[] = { Mat_<float>(imgAmpliada), Mat::zeros(imgAmpliada.size(), CV_32F) };
   // Mat planes[] = {Mat_<float>(imgAmpliada), Mat::zeros(imgAmpliada.size(), CV_32F)};

  Mat imgComplejo;


  // using de arreglos (arrs,narrs, out)
  merge(planos, 2, imgComplejo);

  // aplicamos la dft a un array de img
  // dft open dft(inarr, outarr)
  dft(imgComplejo, imgComplejo);

  //volver de la dft a los planos
  split(imgComplejo, planos);

  // calculamos las magnitudes de los planos
  // mag...(arrin, arra , output)
  magnitude(planos[0], planos[1], planos[0]);
  Mat magImg = planos[0];

  //calcular el logartimo natural del array
  //log(arrin, outarr)
  magImg +=Scalar::all(1);
  log(magImg, magImg);

  magImg = magImg(Rect(0, 0, magImg.cols & -2, magImg.rows & -2) );

  int cx = magImg.cols/2;
  int cy = magImg.rows/2;

  Mat q0(magImg, Rect(0, 0, cx, cy) );
  Mat q1(magImg, Rect(cx, 0, cx, cy) );
  Mat q2(magImg, Rect(0, cy, cx, cy) );
  Mat q3(magImg, Rect(cx, cy, cx, cy) );

  Mat tmp;

  //intercambio q0 y q3
  q0.copyTo(tmp);
  q3.copyTo(q0);
  tmp.copyTo(q3);

  //intercambio q1 y q2
  q1.copyTo(tmp);
  q2.copyTo(q1);
  tmp.copyTo(q2);

  normalize(magImg, magImg, 0, 1, CV_MINMAX );
  // mostrar las imagenes con opencv2

  imshow("Imagen de Entrada", img);
  imshow("Imagen de Espectro", magImg);

  // teclado para esperar consola
  waitKey();

  return 0;
}

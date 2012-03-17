//
//  main.cpp
//  HelloWorld
//
//  Created by Ahmed BOUSSADIA on 1/10/12.
//  Copyright (c) 2012 Ecole Centrale Paris. All rights reserved.
//


#include <opencv2/opencv.hpp>
#include <dirent.h>
#include <string.h>
#include "imageProcessing.h"
void printFailMessage();
IplImage* computeSkinResult(IplImage* imageSkin, IplImage* imageNoSkin, IplImage* image);

using namespace std;

void printFailMessage(){
    cout << "Usage : /path/to/folder /path/to/image/to/analyse.jpg" << endl;
    cout << "\t\t- '/path/to/folder/' must contain  two folder : 'non-peau/' and 'peau/'" << endl;
    cout << "\t\t- '/path/to/image/to/analyse.jpg' image to analyse"<< endl << endl;
}

IplImage* computeSkinResult(IplImage* imageSkin, IplImage* imageNoSkin, IplImage* image){
    IplImage* final_img = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 3 );
    
    int sizes[CV_MAX_DIM];
    cvGetDims(imageSkin, sizes);
    
    for(int j=0; j< sizes[0]; j++){
        for(int k=0; k<sizes[1]; k++){
            double valueSkin = cvGetReal2D(imageSkin, j, k);
            double valueNoSkin = cvGetReal2D(imageNoSkin, j, k);
            
            CvScalar s;
            s = cvGet2D(image, j, k);
            
            if(valueSkin >= valueNoSkin){
                s.val[2] = 255;
                s.val[0] = 0;
                s.val[1] = 0;
            }
            cvSet2D(final_img, j, k, s);
            
            
        }
    }
    
    return final_img;

}


int main (int argc, const char * argv[])
{
    
    /* The program needs at least one argument.*/
    if (argc < 2)
    {
        printFailMessage();
        return EXIT_FAILURE;
    }
    
    DIR* directory = opendir(argv[1]);
    
    /*The argument must be a folder path*/
    if( directory == NULL){
        cout << argv[1] << " is not a directory !!!" << endl;
        printFailMessage();
        return EXIT_FAILURE;
    }
    
    /*We are looking for the doirectories "non-peau" and "peau" */
    string skinDIRNAME = argv[1];
    skinDIRNAME.append("peau");
    string noSkinDIRNAME = argv[1];
    noSkinDIRNAME.append("non-peau");
    ImageProcessing skinProcessor(skinDIRNAME);
    ImageProcessing noSkinProcessor(noSkinDIRNAME);
    
    cout << "LEARNING..." << endl <<endl;
    /*loading images*/
    skinProcessor.loadImages();
    noSkinProcessor.loadImages();
    
    cout << "Generating RGB histogram." << endl;
    skinProcessor.generateHistoRGB();
    noSkinProcessor.generateHistoRGB();
    
    cout << "Generating HSV histogram." << endl;
    skinProcessor.generateHistoHSV();
    noSkinProcessor.generateHistoHSV();
    
    cout << "Generating LAB histogram." << endl;
    skinProcessor.generateHistoLAB();
    noSkinProcessor.generateHistoLAB();
    
    cout << "Done Learning !" << endl;
    
    
        //Testing if second argument is an image
        if(argc < 3 ){
            printFailMessage();
            return EXIT_FAILURE;
        }
        
        string pathToImage = argv[2];
        IplImage* img = cvLoadImage(pathToImage.c_str(), CV_LOAD_IMAGE_UNCHANGED);
        
        if(!img){
            std::cout << pathToImage << " is not an image!" << endl << endl;
            printFailMessage();
            return EXIT_FAILURE;
        }
    
        cout << "Generating results..." << endl <<endl;
        skinProcessor.generateBackImages(img);
        noSkinProcessor.generateBackImages(img);
        cout << "Done Generating results..." << endl <<endl;
    
    
    
    {
        //Computing HSV result
        cout << "Computing HSV result..." << endl <<endl;
        String pathSkin = skinProcessor.directoryName+"/"+ImageProcessing::RESULTS_DIR_NAME+"/HSV.jpg";
        String pathNoSkin = noSkinProcessor.directoryName+"/"+ImageProcessing::RESULTS_DIR_NAME+"/HSV.jpg";
        IplImage* imgSkin = cvLoadImage(pathSkin.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
        IplImage* imgNoSkin = cvLoadImage(pathNoSkin.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
        IplImage* final_image_HSV = computeSkinResult(imgSkin, imgNoSkin, img );
        string path = argv[1];
        path.append("/HSV.jpg");
        cvSaveImage(path.c_str(),final_image_HSV);
        cout << "Done Computing HSV result..." << endl <<endl;
        
    }
    
    {
        //Computing RGB result
        cout << "Computing RGB result..." << endl <<endl;
        String pathSkin = skinProcessor.directoryName+"/"+ImageProcessing::RESULTS_DIR_NAME+"/RGB.jpg";
        String pathNoSkin = noSkinProcessor.directoryName+"/"+ImageProcessing::RESULTS_DIR_NAME+"/RGB.jpg";
        IplImage* imgSkin = cvLoadImage(pathSkin.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
        IplImage* imgNoSkin = cvLoadImage(pathNoSkin.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
        IplImage* final_image_RGB = computeSkinResult(imgSkin, imgNoSkin, img );
        string path = argv[1];
        path.append("/RGB.jpg");
        cvSaveImage(path.c_str(),final_image_RGB);
        cout << "Done Computing RGB result..." << endl <<endl;
        
    }
    
    {
        //Computing LAB result
        cout << "Computing LAB result..." << endl <<endl;
        String pathSkin = skinProcessor.directoryName+"/"+ImageProcessing::RESULTS_DIR_NAME+"/LAB.jpg";
        String pathNoSkin = noSkinProcessor.directoryName+"/"+ImageProcessing::RESULTS_DIR_NAME+"/LAB.jpg";
        IplImage* imgSkin = cvLoadImage(pathSkin.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
        IplImage* imgNoSkin = cvLoadImage(pathNoSkin.c_str(), CV_LOAD_IMAGE_GRAYSCALE);
        IplImage* final_image_LAB = computeSkinResult(imgSkin, imgNoSkin, img );
        string path = argv[1];
        path.append("/LAB.jpg");
        cvSaveImage(path.c_str(),final_image_LAB);
        cout << "Done Computing LAB result..." << endl <<endl;
        
    }
    
    closedir(directory);
    
    return EXIT_SUCCESS;
}
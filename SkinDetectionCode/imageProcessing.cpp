//
//  imageProcessing.cpp
//  HelloWorld
//
//  Created by Ahmed BOUSSADIA on 1/16/12.
//  Copyright (c) 2012 Ecole Centrale Paris. All rights reserved.
//

#include <iostream>
#include "imageProcessing.h"
#include <dirent.h>
#include <sys/stat.h>
#include <string>
#include <opencv/highgui.h>

string const ImageProcessing::HISTOGRAM_DIR_NAME = "Histograms";
string const ImageProcessing::RESULTS_DIR_NAME = "Results";
string const ImageProcessing::RGB_HISTOGRAM_FILE_NAME = "RGBHisto.jpg";
string const ImageProcessing::LAB_HISTOGRAM_FILE_NAME = "LABHisto.jpg";
string const ImageProcessing::HSV_HISTOGRAM_FILE_NAME = "HSVHisto.jpg";

ImageProcessing::ImageProcessing(string dirName): directoryName(dirName){
    directory = opendir(dirName.c_str());
    histoHSV = NULL;
    histoLAB = NULL;
    histoRGB = NULL;
    if( directory == NULL){
        cout << dirName << " does not exist!" << endl;
        return;
    }
    {
        
        string histoDIRNAME = dirName+"/"+HISTOGRAM_DIR_NAME+"/";
    
        DIR* histoDIR = opendir(histoDIRNAME.c_str());
        if(histoDIR == NULL){
            cout << "Creating directory "<< HISTOGRAM_DIR_NAME << " in " << directoryName << endl;
            mkdir(histoDIRNAME.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            
        }else{
            closedir(histoDIR);
        }
    }
    
    {
        
        string resultDIRNAME = dirName+"/"+RESULTS_DIR_NAME+"/";
        
        DIR* resultDIR = opendir(resultDIRNAME.c_str());
        if(resultDIR == NULL){
            cout << "Creating directory "<< RESULTS_DIR_NAME << " in " << directoryName << endl;
            mkdir(resultDIRNAME.c_str(),S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
            
        }else{
            closedir(resultDIR);
        }
    }
    
}

/****************************************************************************************************************************************
 *
 *                      LOADING IMAGES & HISTOGRAMS
 *
 ***************************************************************************************************************************************/

void ImageProcessing::loadImages(){
    vector<IplImage*> tempImages(0);

    while (true) {
        dirent* imageFile = readdir(this->directory);
        if(imageFile){
            IplImage* img = NULL;
            
            string imageName(imageFile->d_name);
            imageName = this->directoryName+"/"+imageName;
            img = cvLoadImage(imageName.c_str(), CV_LOAD_IMAGE_UNCHANGED);
            if(!img){
                std::cout << imageName << " is not an image!" << endl;
            }else{
                tempImages.push_back(img);
            }
        }else{
            break;
        }
    }
    
    images = tempImages;
    
    cout << images.size() << " images loaded!" << endl;
}

/****************************************************************************************************************************************
 *
 *                      GENERATING HISTOGRAM FOR A SINGLE IMAGE
 *
 ***************************************************************************************************************************************/

CvHistogram* ImageProcessing::generateHistoHSVForImage(IplImage* img){
    // Compute HSV image and separate into colors
    IplImage* hsv = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
    cvCvtColor( img, hsv, CV_BGR2HSV );
    
    IplImage* h_plane = cvCreateImage( cvGetSize( img ), 8, 1 );
	IplImage* s_plane =  cvCreateImage( cvGetSize( img ), 8, 1 );

	IplImage* planes[] = { h_plane, s_plane };
    cvSplit( hsv, h_plane, s_plane, NULL, NULL );
    

    // Build and fill the histogram
    int h_bins = 32, s_bins = 32;
	CvHistogram* hist;
	{
		int hist_size[] = { h_bins, s_bins };
		float h_ranges[] = { 0, 180 };
		float s_ranges[] = { 0, 255 };
		float* ranges[] = { h_ranges, s_ranges };
		hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	}
    
    cvCalcHist( planes, hist, 0, 0 ); // Compute histogram
    
	return hist;
}

CvHistogram* ImageProcessing::generateHistoLABForImage(IplImage* img){
    /*Does not seem to generate proper results*/
    // Compute LAB image and separate into colors
    IplImage* lab = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
    cvCvtColor( img, lab, CV_BGR2Lab );
    

	IplImage* a_plane = cvCreateImage( cvGetSize( img ), 8, 1 );
	IplImage* b_plane = cvCreateImage( cvGetSize( img ), 8, 1 );
	IplImage* planes[] = { a_plane, b_plane };
    cvSplit( lab, NULL, a_plane, b_plane, NULL );
    
    
    // Build and fill the histogram
    int a_bins = 32, b_bins = 32;
	CvHistogram* hist;
	{
		int hist_size[] = { a_bins, b_bins };
		float a_ranges[] = { -127, 127 };
		float b_ranges[] = { -127, 127 };
		float* ranges[] = { a_ranges, b_ranges };
		hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	}
    
    cvCalcHist( planes, hist, 0, 0 ); // Compute histogram
    return hist;
;}

CvHistogram* ImageProcessing::generateHistoRGBForImage(IplImage* img){
    // Compute RGB image and separate into colors
    IplImage* rgb = cvCreateImage( cvGetSize(img), IPL_DEPTH_8U, 3 );
    cvCvtColor( img, rgb, CV_BGR2RGB );
    
    IplImage* r_plane = cvCreateImage( cvGetSize( img ), img->depth, 1 );
	IplImage* g_plane = cvCreateImage( cvGetSize( img ), img->depth, 1 );

	IplImage* planes[] = { r_plane, g_plane };
    cvSplit( rgb,r_plane, g_plane, NULL, NULL );
    
    
    // Build and fill the histogram
    int r_bins = 32, g_bins = 32;
	CvHistogram* hist;
	{
		int hist_size[] = { r_bins, g_bins };
		float r_ranges[] = { 0, 255 };
		float g_ranges[] = { 0, 255 };
		float* ranges[] = { r_ranges, g_ranges };
		hist = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	}
     
    cvCalcHist( planes, hist, 0, 0 ); // Compute histogram
    
    return hist;
}


/****************************************************************************************************************************************
 *
 *                      GENERATING HISTOGRAMS
 *
 ***************************************************************************************************************************************/
void ImageProcessing::generateHistoRGB(){
    histoHSVImages = vector<CvHistogram*> (0);
    CvHistogram* histo;
    
    // Build the histogram
    int r_bins = 32, g_bins = 32;
	{
		int hist_size[] = { r_bins, g_bins };
		float r_ranges[] = { 0, 255 };
		float g_ranges[] = { 0, 255 };
		float* ranges[] = { r_ranges, g_ranges };
		histoRGB = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	}
    cvZero(histoRGB->bins);
    
    int sizes[CV_MAX_DIM];
    double value;
    
    for(int index=0; index< images.size(); index++){
        histo = this->generateHistoRGBForImage(images.at(index));
        histoRGBImages.push_back(histo);
        
        cvGetDims(histo->bins, sizes);
        
        double buffer [sizes[0]][sizes[1]];
        for(int j=0; j< sizes[0]; j++){
            for(int k=0; k<sizes[1]; k++){
                buffer[j][k] = 0;
            }
        }
        
        double count = 0;
        
        for(int j=0; j< sizes[0]; j++){
            for(int k=0; k<sizes[1]; k++){
                value = cvGetReal2D(histo->bins, j, k);
                buffer[j][k] = buffer[j][k] + value;
                count = count + value;
                
            }
        }
        
        for(int j=0; j< sizes[0]; j++){
            for(int k=0; k<sizes[1]; k++){
                value = buffer[j][k];
                cvSetReal2D(histoRGB->bins, j, k, value);
            }
        }

    }
    
    //histoRGB = histoRGBImages.at(0);
    // Create an image to visualize the histogram
    int scale = 10;
    IplImage* hist_img = cvCreateImage( cvSize( r_bins * scale, g_bins * scale ), 8, 3 );
    cvZero ( hist_img );
    
    // populate the visualization
    float max_value = 0;
    cvGetMinMaxHistValue( histoRGB, 0, &max_value, 0, 0 );
    
    
    for( int h = 0; h < r_bins; h++ ){
        for( int s = 0; s < g_bins; s++ ){
            float bin_val = cvGetReal2D(histoRGB->bins, h, s );
            int intensity = cvRound( bin_val * 255 / max_value );
            cvRectangle( hist_img, cvPoint( h*scale, s*scale ),
                        cvPoint( (h+1)*scale - 1, (s+1)*scale - 1 ),
                        CV_RGB( intensity, intensity, intensity ),
                        CV_FILLED );
        }
    }
    
    String path = directoryName+"/"+HISTOGRAM_DIR_NAME+"/"+RGB_HISTOGRAM_FILE_NAME;
    cout << "Saving Histogram RGB in " << path << endl;
    cvSaveImage(path.c_str(),hist_img);
    cvReleaseImage(&hist_img);
    cvReleaseHist(&histo);
    

}

void ImageProcessing::generateHistoLAB(){
    histoLABImages = vector<CvHistogram*> (0);
    CvHistogram* histo;
    
    // Build and fill the histogram
    int a_bins = 32, b_bins = 32;
	{
		int hist_size[] = { a_bins, b_bins };
		float a_ranges[] = { -127, 127 };
		float b_ranges[] = { -127, 127 };
		float* ranges[] = { a_ranges, b_ranges };
		histoLAB = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	}
    cvZero(histoLAB->bins);
    
    
    int sizes[CV_MAX_DIM];
    double value;
    
    for(int index=0; index< images.size(); index++){
        histo = this->generateHistoLABForImage(images.at(index));
        histoLABImages.push_back(histo);
        
        cvGetDims(histo->bins, sizes);
        
        double buffer [sizes[0]][sizes[1]];
        for(int j=0; j< sizes[0]; j++){
            for(int k=0; k<sizes[1]; k++){
                buffer[j][k] = 0;
            }
        }
        
        double count = 0;
        
        for(int j=0; j< sizes[0]; j++){
            for(int k=0; k<sizes[1]; k++){
                value = cvGetReal2D(histo->bins, j, k);
                buffer[j][k] = buffer[j][k] + value;
                count = count + value;
                
            }
        }
        
        for(int j=0; j< sizes[0]; j++){
            for(int k=0; k<sizes[1]; k++){
                value = buffer[j][k];
                cvSetReal2D(histoLAB->bins, j, k, value);
            }
        }

    }
    
    // Create an image to visualize the histogram
    int scale = 10;
    IplImage* hist_img = cvCreateImage( cvSize( a_bins * scale, b_bins * scale ), 8, 3 );
    cvZero ( hist_img );
    
    // populate the visualization
    float max_value = 0;
    cvGetMinMaxHistValue( histoLAB, 0, &max_value, 0, 0 );
    
    
    for( int h = 0; h < a_bins; h++ ){
        for( int s = 0; s < b_bins; s++ ){
            float bin_val = cvGetReal2D(histoLAB->bins, h, s );
            int intensity = cvRound( bin_val * 255 / max_value );
            cvRectangle( hist_img, cvPoint( h*scale, s*scale ),
                        cvPoint( (h+1)*scale - 1, (s+1)*scale - 1 ),
                        CV_RGB( intensity, intensity, intensity ),
                        CV_FILLED );
        }
    }
    
    String path = directoryName+"/"+HISTOGRAM_DIR_NAME+"/"+LAB_HISTOGRAM_FILE_NAME;
    cout << "Saving Histogram LAB in " << path << endl;
    cvSaveImage(path.c_str(),hist_img);
    cvReleaseImage(&hist_img);
    cvReleaseHist(&histo);

}

void ImageProcessing::generateHistoHSV(){
    histoHSVImages = vector<CvHistogram*> (0);
    CvHistogram* histo;
    
    // Build and fill the histogram
    int h_bins = 32, s_bins = 32;
	{
		int hist_size[] = { h_bins, s_bins };
		float h_ranges[] = { 0, 180 };
		float s_ranges[] = { 0, 255 };
		float* ranges[] = { h_ranges, s_ranges };
		histoHSV = cvCreateHist( 2, hist_size, CV_HIST_ARRAY, ranges, 1 );
	}
    cvZero(histoHSV->bins);
    
    
    int sizes[CV_MAX_DIM];
    double value;
    
    for(int index=0; index< images.size(); index++){
        histo = this->generateHistoHSVForImage(images.at(index));
        histoHSVImages.push_back(histo);
        
        cvGetDims(histo->bins, sizes);
        
        double buffer [sizes[0]][sizes[1]];
        for(int j=0; j< sizes[0]; j++){
            for(int k=0; k<sizes[1]; k++){
                buffer[j][k] = 0;
            }
        }
        
        double count = 0;
        
        for(int j=0; j< sizes[0]; j++){
            for(int k=0; k<sizes[1]; k++){
                value = cvGetReal2D(histo->bins, j, k);
                buffer[j][k] = buffer[j][k] + value;
                count = count + value;
                
            }
        }
        
        for(int j=0; j< sizes[0]; j++){
            for(int k=0; k<sizes[1]; k++){
                value = buffer[j][k];
                cvSetReal2D(histoHSV->bins, j, k, value);
            }
        }
        
    }
    
    
    // Create an image to visualize the histogram
    int scale = 10;
    IplImage* hist_img = cvCreateImage( cvSize( h_bins * scale, s_bins * scale ), 8, 3 );
    cvZero ( hist_img );
    
    // populate the visualization
    float max_value = 0;
    cvGetMinMaxHistValue( histoHSV, 0, &max_value, 0, 0 );
    
    
    for( int h = 0; h < h_bins; h++ ){
        for( int s = 0; s < s_bins; s++ ){
            float bin_val = cvGetReal2D(histoHSV->bins, h, s );
            int intensity = cvRound( bin_val * 255 / max_value );
            cvRectangle( hist_img, cvPoint( h*scale, s*scale ),
                        cvPoint( (h+1)*scale - 1, (s+1)*scale - 1 ),
                        CV_RGB( intensity, intensity, intensity ),
                        CV_FILLED );
        }
    }
    
    
    String path = directoryName+"/"+HISTOGRAM_DIR_NAME+"/"+HSV_HISTOGRAM_FILE_NAME;
    cout << "Saving Histogram HSV in " << path << endl;
    cvSaveImage(path.c_str(),hist_img);
    cvReleaseImage(&hist_img);
    cvReleaseHist(&histo);
}


/****************************************************************************************************************************************
 *
 *                      GENERATING BACK IMAGES
 *
 ***************************************************************************************************************************************/

void ImageProcessing::generateBackImages(IplImage* image){
    
    if(this->histoHSV != NULL){
        IplImage* back_img = cvCreateImage( cvGetSize( image ), IPL_DEPTH_8U, 1 );
        
        // Compute HSV image and separate into colors
        IplImage* hsv = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 3 );
        cvCvtColor( image, hsv, CV_BGR2HSV );
        
        IplImage* h_plane = cvCreateImage( cvGetSize( image ), 8, 1 );
        IplImage* s_plane =  cvCreateImage( cvGetSize( image ), 8, 1 );
        
        IplImage* planes[] = { h_plane, s_plane };
        cvSplit( hsv, h_plane, s_plane, NULL, NULL );
        
        
        cvCalcBackProject( planes, back_img, histoHSV );// Calculate back projection
        
        String path = directoryName+"/"+RESULTS_DIR_NAME+"/HSV.jpg";
        cout << "Saving Result HSV in " << path << endl << endl;
        cvSaveImage(path.c_str(),back_img);
        cvReleaseImage(&back_img);        
    }
    
    if(this->histoLAB != NULL){
        IplImage* back_proj = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
        
        IplImage* lab = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 3 );
        cvCvtColor( image, lab, CV_BGR2Lab );
        
        
        IplImage* a_plane = cvCreateImage( cvGetSize( image ), 8, 1 );
        IplImage* b_plane = cvCreateImage( cvGetSize( image ), 8, 1 );
        
        cvSplit( lab, NULL, a_plane, b_plane, NULL);
        
        IplImage* planes[] = { a_plane, b_plane };
        
        
        cvCalcBackProject( planes, back_proj, this->histoLAB );// Calculate back projection
        
        
        String path = directoryName+"/"+RESULTS_DIR_NAME+"/LAB.jpg";
        cout << "Saving Result LAB in " << path << endl << endl;
        cvSaveImage(path.c_str(),back_proj);
        cvReleaseImage(&back_proj);

    }
    
    if(this->histoRGB != NULL){
        IplImage* back_proj = cvCreateImage(cvGetSize(image), IPL_DEPTH_8U, 1);
        
        // Compute RGB image and separate into colors
        IplImage* rgb = cvCreateImage( cvGetSize(image), IPL_DEPTH_8U, 3 );
        cvCvtColor(image, rgb, CV_BGR2RGB);
        
        IplImage* r_plane = cvCreateImage( cvGetSize( image ), image->depth, 1 );
        IplImage* g_plane = cvCreateImage( cvGetSize( image ), image->depth, 1 );
        
        IplImage* planes[] = { r_plane, g_plane };
        cvSplit( rgb,r_plane, g_plane, NULL, NULL );
       
        cvCalcBackProject( planes, back_proj, histoRGB );// Calculate back projection
        
        String path = directoryName+"/"+RESULTS_DIR_NAME+"/RGB.jpg";
        cout << "Saving Result RGB in " << path << endl << endl;
        cvSaveImage(path.c_str(),back_proj);
        cvReleaseImage(&back_proj);
    }
}

/****************************************************************************************************************************************
 *
 *                      SHOW IMAGE METHODES
 *
 ***************************************************************************************************************************************/
void ImageProcessing::showImage(int i){
    
    if(i>=images.size() || i<0){
        cout << "Index out of range" << endl;
        return;
    }
    IplImage* img = images.at(i);
    
    const char* window_title = "Hello, OpenCV!";
    /* Création d'une fenêtre intitulée "Hello, OpenCV!" */
    cvNamedWindow (window_title, CV_WINDOW_AUTOSIZE);
    
    /* Affichage de l'image */
    cvShowImage (window_title, img);
    
    /* Pause le temps que l'utilisateur appuie sur une touche */
    cvWaitKey (0);
    
    /* Destruction de la fenêtre */
    cvDestroyAllWindows ();
}

void ImageProcessing::showImage(CvArr* img){
    const char* window_title = "Hello, OpenCV!";
    /* Création d'une fenêtre intitulée "Hello, OpenCV!" */
    cvNamedWindow (window_title, CV_WINDOW_AUTOSIZE);
    
    /* Affichage de l'image */
    cvShowImage (window_title, img);
    
    /* Pause le temps que l'utilisateur appuie sur une touche */
    cvWaitKey (0);
    
    /* Destruction de la fenêtre */
    cvDestroyAllWindows ();
}

ImageProcessing::~ImageProcessing(){
    closedir(directory);
    
    for(int i =0; i<this->images.size(); i++)
        cvReleaseImage (&images.at(i));
    
    try {
        cvReleaseHist(&histoHSV);
    } catch (Exception e) {
        cout << e.msg <<endl;
    }
    
    try {
        cvReleaseHist(&histoLAB);
    } catch (Exception e) {
        cout << e.msg <<endl;
    }
    
    try {
        cvReleaseHist(&histoRGB);
    } catch (Exception e) {
        cout << e.msg <<endl;
    }
    
    for (int i=0; i<histoLABImages.size(); i++){
        try {
            cvReleaseHist (&histoLABImages.at(i));
        } catch (Exception e) {
            cout << e.msg <<endl;
        }
    }
        
    
    for (int i=0; i<histoHSVImages.size(); i++){
        try {
            cvReleaseHist (&histoHSVImages.at(i));
        } catch (Exception e) {
            cout << e.msg <<endl;
        }
    }
        
    
    for (int i=0; i<histoRGBImages.size(); i++){
        try {
            cvReleaseHist (&histoRGBImages.at(i));
        } catch (Exception e) {
            cout << e.msg <<endl;
        }
    }
        
}
//
//  imageProcessing.h
//  HelloWorld
//
//  Created by Ahmed BOUSSADIA on 1/16/12.
//  Copyright (c) 2012 Ecole Centrale Paris. All rights reserved.
//

#ifndef HelloWorld_imageProcessing_h
#define HelloWorld_imageProcessing_h

#include <dirent.h>
#include <opencv2/opencv.hpp>
#include <vector>

using namespace std;
using namespace cv;


class ImageProcessing {
protected:
    vector<IplImage*> images;
    DIR* directory;
    
    CvHistogram* histoRGB;
    vector<CvHistogram*> histoRGBImages;
    CvHistogram* histoLAB;
    vector<CvHistogram*> histoLABImages;
    CvHistogram* histoHSV;
    vector<CvHistogram*> histoHSVImages;
    
    CvHistogram* generateHistoRGBForImage(IplImage* img);
    CvHistogram* generateHistoLABForImage(IplImage* img);
    CvHistogram* generateHistoHSVForImage(IplImage* img);
    
public:
    ImageProcessing( string dirName);
    void showImage(int i);
    void showImage(CvArr* img);
    ~ImageProcessing();
    void loadImages();
    void generateHistoRGB();
    void generateHistoLAB();
    void generateHistoHSV();
    
    void generateBackImages(IplImage* image);
    
    string directoryName;
    
    static string const HISTOGRAM_DIR_NAME ;
    static string const RESULTS_DIR_NAME;
    static string const RGB_HISTOGRAM_FILE_NAME;
    static string const LAB_HISTOGRAM_FILE_NAME;
    static string const HSV_HISTOGRAM_FILE_NAME;
};

#endif

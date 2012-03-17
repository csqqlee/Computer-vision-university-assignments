#include <stdio.h>
#include <stdlib.h>
#include <opencv/highgui.h>
#include <cv.h>

using namespace cv;

std::vector<int> globalResults(12);

Mat traiterImage(Mat src2)
{
	// Méthode qui floute et convertit en gris une image
	Mat img,retour;
	src2.copyTo(img);

	GaussianBlur( src2, retour, Size(3,3), 0, 0, BORDER_DEFAULT );
	cvtColor( retour, retour, CV_RGB2GRAY );
	return retour;
}

Mat seuilImage(int seuil, Mat img2)
{
	// Méthode qui permet de seuiller et d'inverser une image
	Mat img;
	img2.copyTo(img);

	for(int i=0;i<img.rows;i++)
	{
		for(int j=0;j<img.cols;j++)
		{
			uchar pix;
			Scalar_<uchar> bgrPixel;
			pix = img.at<uchar>(i,j);

			if(pix < seuil) 
				img.at<uchar>(i,j) = 255;


			else
				img.at<uchar>(i,j) = 0;

		}
	}

	return img;

}



Mat cannyImage(Mat img2)
{
	
	// Algorithme qui utilise le filtre de canny pour détecter les contours
	Mat img;
	img2.copyTo(img);
	cv::Canny(img,img,60,150);
	for(int i=0;i<img.rows;i++)
	{
		for(int j=0;j<img.cols;j++)
		{
			uchar pix;
			Scalar_<uchar> bgrPixel;
			pix = img.at<uchar>(i,j);
			if(pix == 255)
				img.at<uchar>(i,j) = 0;
			else
				img.at<uchar>(i,j) = 255;
		}
	}



	return img;
}


Mat laplacianImage(Mat img2)
{
	
	// Algorithme qui utilise le laplacien pour détecter les contours
	Mat img;
	img2.copyTo(img);

	Mat ret, retou;
	cv::Laplacian(img,ret,CV_8U);

	return seuilImage(1,ret);
}

Mat sobelImage(Mat img2)
{
	// Algorithme qui utilise le filtre de sobel pour détecter les contours
	Mat img;
	img2.copyTo(img);

	int scale = 1;
	int delta = 0;
	int ddepth = CV_16S;
	Mat grad;
	/// Generate grad_x and grad_y
	Mat grad_x, grad_y;
	Mat abs_grad_x, abs_grad_y;

	/// Gradient X

	Sobel( img, grad_x, ddepth, 1, 0, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( grad_x, abs_grad_x );

	/// Gradient Y

	Sobel( img, grad_y, ddepth, 0, 1, 3, scale, delta, BORDER_DEFAULT );
	convertScaleAbs( grad_y, abs_grad_y );

	/// Total Gradient (approximate)
	addWeighted( abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad );

	return seuilImage(100,grad);

}


int calcNbContour(Mat img)
{
	int nb = 0;
	for(int i=0;i<img.rows;i++)
	{
		for(int j=0;j<img.cols;j++)
		{
			uchar pix;

			pix = img.at<uchar>(i,j);
			if(pix == 255)
				nb +=1;

		}
	}
	return nb;

}

std::vector<int> calcResults(Mat img2, Mat ref2,int hgap, int vgap)
{
	// Cette méthode calcule les différents indicateurs pour une image
	// Hgap et Vgap permettent de gérer les cas de décalage entre l'image de contour de référence et l'image calculée
	Mat img;
	img2.copyTo(img);
	Mat ref;
	ref2.copyTo(ref);
	std::vector<int> results(3);
	int contOk=0;
	int fauxPos = 0;
	int fauxNeg = 0;
	for(int i=0;i<img.rows;i++)
	{
		for(int j=0;j<img.cols;j++)
		{
			uchar pixImg, pixRef;
			bool b = false;
			int c = 0;
			int d = 0;
			pixImg = img.at<uchar>(i,j);
			for(int l=-hgap;l<=hgap;l++)
			{
				for(int m=-vgap;m<=vgap;m++)
				{
					if(i+m<0 || j+l<0 || i+m >= img.rows || j+l >=img.cols)
						continue;

					pixRef=img.at<uchar>(i+m,j+l);
					if(pixImg== 255 && pixRef == 255)
						b =true;
					else if(pixImg == 255 && pixRef != 255)
						c +=1;
					else if(pixImg == 0 && pixRef == 255)
						d+=1;

				}
			}
			if(b)
				contOk+=1;
			if(c==hgap*vgap)
				fauxPos+=1;
			if(d==hgap*vgap)
				fauxNeg+=1;

		}
	}
	results[0] = contOk;
	results[1] = fauxPos;
	results[2] = fauxNeg;
	return results;
}

void printResults(Mat img, Mat ref, int hgap, int vgap, int methode)
{
	// Cette méthode permet d'afficher et de stocker les différents indicateurs pour une image.
	// Hgap et Vgap permettent de gérer les cas de décalage entre l'image de contour de référence et l'image calculée
	std::vector<int> results = calcResults(img, ref, hgap,vgap);
	float P, TFP, TFN;
	P = (float)results[0]/((float)results[0]+(float)results[1]+(float)results[2]);
	TFP =  (float)results[1]/((float)results[0]+(float)results[1]+(float)results[2]);
	TFN = (float)results[2]/((float)results[0]+(float)results[1]+(float)results[2]);

	for(int i=0;i<3;i++)
		globalResults[i+3*methode] += results[i];



	std::cout<<"--------------------"<<std::endl;
	std::cout<<"Contour Correc : "<<results[0]<<std::endl;
	std::cout<<"Faux positifs : " <<results[1]<<std::endl;
	std::cout<<"Faux négatifs : "<<results[2]<<std::endl;
	std::cout<<"P : "<< P <<std::endl;
	std::cout<<"TFP : "<< TFP <<std::endl;
	std::cout<<"TFN : "<< TFN <<std::endl;

}

void printGlobalResults(){
	// Cette méthode permet de calculer et d'afficher les différents indicateurs demandés calculés sur l'ensemble des images
	std::vector<std::string> methodes(4);
	methodes[0] = "Sobel";
	methodes[1] = "Laplace";
	methodes[2] = "Canny";
	methodes[3] = "Différence";
	for(int i=0;i<4;i++)
	{
		std::cout<<"--------------------------"<<std::endl;
		std::cout<<methodes[i]<<std::endl;
		float P, TFP, TFN;
		P = (float)globalResults[0+i*3]/((float)globalResults[0+i*3]+(float)globalResults[1+i*3]+(float)globalResults[2+i*3]);
		TFP =  (float)globalResults[1+i*3]/((float)globalResults[0+i*3]+(float)globalResults[1+i*3]+(float)globalResults[2+i*3]);
		TFN = (float)globalResults[2+i*3]/((float)globalResults[0+i*3]+(float)globalResults[1+i*3]+(float)globalResults[2+i*3]);
		std::cout<<"Contour Correc : "<<globalResults[0+i*3]<<std::endl;
		std::cout<<"Faux positifs : " <<globalResults[1+i*3]<<std::endl;
		std::cout<<"Faux négatifs : "<<globalResults[2+i*3]<<std::endl;
		std::cout<<"P : "<< P <<std::endl;
		std::cout<<"TFP : "<< TFP <<std::endl;
		std::cout<<"TFN : "<< TFN <<std::endl;
	}



}

Mat methodePerso(Mat img2, Mat ori2, int seuil)
{
	Mat img;
	img2.copyTo(img);
	Mat ori;
	ori2.copyTo(ori);

	for(int i=0;i<img.rows;i++)
	{
		for(int j=0;j<img.cols;j++)
		{
			uchar pixImg = img.at<uchar>(i,j);
			uchar pixOri = ori.at<uchar>(i,j);


			if(pixOri - pixImg < seuil)
				img.at<uchar>(i,j) = 255;
			else
				img.at<uchar>(i,j) = 0;

		}
	}
	return img;


}



/** @function main */
int main( int argc, char** argv )
{
	Mat src,ref,ori, sobel, canny, laplace, perso;


	// On boucle sur les images que l'on veut charger
	for(int i=1;i<=15;i++)
	{

		vector<int> results;
		
		// On concatène pour avoir les bons chemins vers les images
		char ch[80], refUrl[80];
		sprintf(ch,"images/%d.pgm",i);
		sprintf(refUrl,"images/gt/%d.pgm",i);
		//On charge les images
		ori = imread(ch);
		ref = imread(refUrl);

		// Si problème de chargement, le programme s'arrête
		if( !ori.data  || !ref.data)
		{ return -1; }

		// On floute et convertit en gris l'image initiale
		src = traiterImage(ori);

		// On convertit en gris l'image de contour de reference
		cvtColor( ref, ref, CV_RGB2GRAY );

		
		// On applique nos différentes méthodes de détection de contour
		perso = methodePerso(src,ori,3);
		printResults(perso,ref,1,1,3);
		
		sobel = sobelImage(src);  
		printResults(sobel, ref,1,1,0);
		
		laplace = laplacianImage(src);
		printResults(laplace, ref,1,1,1);
		
		canny = cannyImage(src);
		printResults(canny, ref,1,1,2);


		// Décommenter les lignes suivantes si on veut afficher les images résultantes

		/*
		namedWindow( "Origine", CV_WINDOW_AUTOSIZE );
		imshow("Origine",ori);
		namedWindow( "Reference", CV_WINDOW_AUTOSIZE );
		imshow("Reference",ref); 
		namedWindow( "canny", CV_WINDOW_AUTOSIZE );
		imshow("canny",canny); 
		namedWindow( "perso", CV_WINDOW_AUTOSIZE );
		imshow("perso",perso); 
		namedWindow( "laplace", CV_WINDOW_AUTOSIZE );
		imshow("laplace",laplace); 
		namedWindow( "sobel", CV_WINDOW_AUTOSIZE );
		imshow("sobel",sobel); 
		waitKey(0);
		*/

	}
	printGlobalResults();
	system("pause");




	return 0;
}



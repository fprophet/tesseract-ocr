// g++ ocr.cpp `pkg-config opencv4 --cflags --libs` -I/usr/local/include/opencv4 -L/usr/local/lib64 -llept -ltesseract -o ocr

#include <opencv4/opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <unistd.h>
#include "app.h"

using namespace std;



int main( int argc, char* argv[])
{
   application app;
    app.parse_arguments(argc,argv);

    if( app.image.empty() ){
        cout << "ERROR: File path is missing! Exiting!" << endl;
        return 0;
    }

    string image_path = IMAGE_DIR + "/" + app.image;
    cv::Mat image = cv::imread(image_path);
    if( image.empty()){
        cout << "ERROR: Image could not be read! Exiting!" << endl;
        return 0;
    }

    
    return 0;
}

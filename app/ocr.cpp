// g++ ocr.cpp `pkg-config opencv4 --cflags --libs` -I/usr/local/include/opencv4 -L/usr/local/lib64 -llept -ltesseract -o ocr -ljsoncpp

#include <opencv4/opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <unistd.h>
#include <fstream>
#include <tesseract/renderer.h>
// sudo apt-get install libjsoncpp-dev

#include "/usr/include/jsoncpp/json/json.h"

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
    
    if( app.testing ){
        IMAGE_PATH = APP_PATH + "/" + app.image;
    }else{
        IMAGE_PATH = IMAGE_DIR + "/" + app.image;
    }

    Mat image = imread(IMAGE_PATH);
    
    if( image.empty()){
        cout << "ERROR: Image could not be read!" << endl;
        cout << "Path to image: " << IMAGE_PATH << endl;
        cout << "Exiting!" << endl;
        
        return 0;
    }

    app.img_height = image.rows;
    app.img_width = image.cols;

    if( app.debugging ){
        cout << "Image width: " << app.img_height  << endl;
        cout << "Image height: " << app.img_width  << endl;
    }

    //scale image if necessary
    app.scale_image(image);
    Mat orig = image;

    Json::Value json;
    json["document"]["width"] = image.cols;
    json["document"]["height"] = image.rows;
    json["document"]["dpi"] = app.dpi;

    //convert to gray scale in order to threshold 
    app.gray_scale(image);

    //thershold the image for further pre processing
    app.threshold(image);

    //deskew image
    //find text orientation
    int orient = app.find_text_orientation(image);
    //create text image find the skew angle
    Mat text_image = app.create_text_image(image);
    //find the angle of the new Mat
    double angle = app.calculate_angle(text_image);
    app.deskew(image, angle);

    TessBaseAPI *api = app.init_api(image);

    app.remove_non_text_regions(api, image,json);
    app.ocr(api, image, json);
    // app.remove_straight_lines(image, app.avg_width, app.avg_height);

    api->End();

    if( app.debugging ){
        app.display_original(orig,orient,angle, json);
    }

    app.write_json(json);
   
    return 0;
}

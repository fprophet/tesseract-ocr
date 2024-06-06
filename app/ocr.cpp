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

    if( app.debugging ){
        // WRITE_IMAGES = true;
    }



    cv::Mat image = cv::imread(IMAGE_PATH);

    if( image.empty()){
        cout << "ERROR: Image could not be read!" << endl;
        cout << "Path to image: " << IMAGE_PATH << endl;
        cout << "Exiting!" << endl;
        
        return 0;
    }

    
    app.render_text_box();

    app.avg_char_size();

    app.scale_image(image);


    app.img_height = image.rows;
    app.img_width = image.cols;


    //convert to gray scale in order to threshold 
    cv::Mat gray = app.gray_scale(image);

    //thershold the image for further pre processing
    cv::Mat adaptive = app.threshold(gray);

    //remove noise like small dots using morphology close
    cv::Mat morphed = app.morph(adaptive);


    // cv::Mat removed_lines = app.remove_straight_lines3(adaptive);

    cv::Mat removed_lines = app.remove_straight_lines(morphed, app.avg_width, app.avg_height);
    cv::Mat deskewd = app.deskew_image(removed_lines);

    app.find_regions_extract_text();

    // Json::Value text_blocks = app.create_blocks_json(contours);

    // Json::Value text_blocks = app.ocr_image(contours);

    // Json::StyledWriter writer;

    // ofstream jsonResult;
    // jsonResult.open("results.json");

    // jsonResult << writer.write(text_blocks) << endl;
    
    // jsonResult.close();

    cout << "Script has finished!" << endl;    
    return 0;
}

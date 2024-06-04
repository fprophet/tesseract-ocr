// g++ ocr.cpp `pkg-config opencv4 --cflags --libs` -I/usr/local/include/opencv4 -L/usr/local/lib64 -llept -ltesseract -o ocr

#include <opencv4/opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <unistd.h>
#include<fstream>
#include <tesseract/renderer.h>

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



    app.original_height = image.rows;
    app.original_width = image.cols;

    //convert to gray scale in order to threshold 
    cv::Mat gray = app.gray_scale(image);

    //thershold the image for further pre processing
    cv::Mat adaptive = app.threshold(gray);

    //remove noise like small dots using morphology close
    cv::Mat morphed = app.morph(adaptive);


    // cv::Mat removed_lines = app.remove_straight_lines3(adaptive);

    cv::Mat removed_lines = app.remove_straight_lines(morphed, app.avg_width, app.avg_height);

    cv::Mat deskewd = app.deskew_image(removed_lines);


    // char *outText;


    // tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // // Initialize tesseract-ocr with English, without specifying tessdata path
    // if (api->Init(NULL, "eng")) {
    //     fprintf(stderr, "Could not initialize tesseract.\n");
    //     exit(1);
    // }

    // // Open input image with leptonica library
    // Pix *img = pixRead(image_path.c_str());
    // api->SetImage(img);
    // // Get OCR result
    // outText = api->GetUTF8Text();
    // printf("OCR output:\n%s", outText);

    // // Destroy used object and release memory
    // api->End();
    // delete api;
    // delete [] outText;
    // pixDestroy(&img);


    cout << "Script has finished!" << endl;    
    return 0;
}

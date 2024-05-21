// g++ ocr.cpp `pkg-config opencv4 --cflags --libs` -I/usr/local/include/opencv4 -L/usr/local/lib64 -llept -ltesseract -o ocr

#include <opencv4/opencv2/opencv.hpp>
#include <tesseract/baseapi.h>
#include <leptonica/allheaders.h>
#include <unistd.h>
#include <stdio.h>

using namespace std;
int main()
{
    // char *outText;

    // tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
    // // Initialize tesseract-ocr with English, without specifying tessdata path
    // if (api->Init(NULL, "eng")) {
    //     fprintf(stderr, "Could not initialize tesseract.\n");
    //     exit(1);
    // }

    // // Open input image with leptonica library
    // Pix *image = pixRead("testocr.png");
    // api->SetImage(image);
    // // Get OCR result
    // outText = api->GetUTF8Text();
    // printf("OCR output:\n%s", outText);

    // // Destroy used object and release memory
    // api->End();
    // delete api;
    // delete [] outText;
    // pixDestroy(&image);

    for( int i =0; i < 10; i++ ){
        cout<< "SUNT AICI: " << i << endl;
 sleep(1);
    }
    return 0;
}

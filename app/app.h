#include <stdio.h>
#include <iostream>
using namespace std;

const string ROOT = "/var/www/html/tesseract-ocr";
const string IMAGE_DIR = ROOT + "/uploads"; 
const string PROCESSED_DIR = ROOT + "/public/processed-images";

struct application{
    string image;
    void parse_arguments(int argc, char ** argv){
        for( int i = 1; i < argc; i ++ ){
            if ( this->argument_exists("--image", i, argc, argv) ){
                this->image = argv[i + 1];
            }
        }            

        cout << "Arguments parsed!" << endl;   
    }

    bool argument_exists(string arg, int i, int argc, char ** argv){

        string converted = argv[i];

        int comparison = converted.compare(arg);

        if(  comparison == 0 ){
            if( i + 1 < argc ){
                return true;
            }else{
                cout << "Error in arguments parsing! " << converted << " Trying to reach out of bounds array!" << endl;
                return false;
            }
        }

        return false;
    }
};
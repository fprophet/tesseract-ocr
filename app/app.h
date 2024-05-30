#include <stdio.h>
#include <iostream>
using namespace std;

const string ROOT = "/var/www/html/tesseract-ocr";
const string IMAGE_DIR = ROOT + "/uploads"; 
const string PROCESSED_DIR = ROOT + "/public/processed-images";
const string APP_PATH = ROOT + "/app";
const string TESS_DATA = "/usr/local/share/tessdata/";
string IMAGE_PATH;

const bool WRITE_IMAGES = true;
using namespace cv;
struct application{
    
    string image;

    bool debugging;

    int original_width;

    int original_height;

	float avg_height,avg_width;

    void parse_arguments(int argc, char ** argv){
        
        for( int i = 1; i < argc; i ++ ){
            if ( this->argument_exists("--image", i, argc, argv) ){
                this->image = argv[i + 1];
            }

            if ( this->argument_exists("--debugging", i, argc, argv) ){
                this->debugging = true;
                this->image = "test.jpg";
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

    
    vector<string> explode_name( string to_explode){
		string name,type;
		int pos = to_explode.find_last_of(".");
		type = to_explode.substr(pos+1);
		name = to_explode.substr(0,pos);
		vector<string> full_name;
		full_name.push_back(name);
		full_name.push_back(type);
		return full_name;
	}

    void write_image(string new_name, cv::Mat &img){
        vector<string> exploded = this->explode_name(this->image);

        string save_name = exploded[0] + "-" + new_name + "." + exploded[1];

        cv::imwrite(PROCESSED_DIR +  "/" + save_name, img);

        return;
    }

    cv::Mat gray_scale( cv::Mat &original){

        cv::Mat gray;
        cv::cvtColor(original, gray, cv::COLOR_BGR2GRAY);

        if( WRITE_IMAGES){
            this->write_image("gray", gray);
        }

        return gray;
    }

    cv::Mat threshold(cv::Mat &original){
        
        cv::Mat adaptive;
        cv::adaptiveThreshold(original,adaptive,255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 3,25);
        
        if( WRITE_IMAGES){
            this->write_image("adaptive", adaptive);
        }

        return adaptive;
    }

    cv::Mat morph(cv::Mat &original ){
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3,3));
        cv::Mat morphed;
        cv::morphologyEx(original, morphed, cv::MORPH_CLOSE,kernel);

         if( WRITE_IMAGES){
            this->write_image("morphed", morphed);
        }

        return morphed;
    }



    bool render_text_box(){
		string output = APP_PATH + "/box-file";
		int time_out = 0;
		string language = "eng";
		const char* retry_config = nullptr;
		bool text_only = false;
		tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
		if( api->Init(TESS_DATA.c_str(), language.c_str(), tesseract::OEM_DEFAULT)){
			fprintf(stderr, "Could not initialize Tesseract.\n");
			return false;
		}
		// Mat image_to_pdf = imread(image_path);
		Pix *image = pixRead(IMAGE_PATH.c_str());

		api->SetImage(image);
		api->SetPageSegMode(tesseract::PSM_SPARSE_TEXT);
		tesseract::TessBoxTextRenderer *renderer = new tesseract::TessBoxTextRenderer(output.c_str());
		bool succeed = api->ProcessPage(image, 1 ,IMAGE_PATH.c_str(), retry_config, time_out, renderer);
		if( !succeed){
			return false;
		}

		api->End();
		return true;
	}

	vector<Vec4i> read_box_file( string file_path){
		ifstream infile(file_path);
		string a;
		vector<Vec4i> result;
		if ( infile.peek() == std::ifstream::traits_type::eof() )
			{
			return result;
			}
		int b,c,d,e,f;
		while( infile >> a >> b >> c >> d >> e >>f ){
			Vec4i coords;
			coords[0] = b;
			coords[1] = c;
			coords[2] = d;
			coords[3] = e;
			result.push_back(coords);
		}
		return result;
	}

	bool avg_char_size(){
		string box_path = APP_PATH + "/box-file.box";
		vector<Vec4i> coords = this->read_box_file(box_path.c_str());
		if( coords.size() == 0){
			return false;
		}
		int coords_size = coords.size();
		float avg_w = 0, avg_h = 0;
		for ( int i = 0; i < coords_size; i++){
	// Rect rect = boundingRect(coords[i]);
			avg_w = avg_w + (coords[i][2] - coords[i][0]);
			avg_h = avg_h + (coords[i][3] - coords[i][1]);
			// cout << "AVG W: " << avg_w << endl;
			// cout << "AVG H: " << avg_h << endl;

		}
		avg_w = avg_w / coords_size;
		avg_h = avg_h / coords_size;
			// cout << "AVG W: " << avg_w << endl;
			// cout << "AVG H: " << avg_h << endl;

		this->avg_height = avg_h;
		this->avg_width = avg_w;
		cout << "Average width:" << avg_w << endl;
		cout << "Average height:" << avg_h << endl;
		return true;
	}

};
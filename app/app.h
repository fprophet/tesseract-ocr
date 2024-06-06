#include <stdio.h>
#include <iostream>
using namespace std;
using namespace tesseract;


const string ROOT = "/var/www/html/tesseract-ocr";
const string IMAGE_DIR = ROOT + "/uploads"; 
const string PROCESSED_DIR = ROOT + "/public/processed-images";
const string APP_PATH = ROOT + "/app";
const string TESS_DATA = "/usr/local/share/tessdata/";
string IMAGE_PATH;

bool WRITE_IMAGES = true;

using namespace cv;
struct application{
    
    string image;

    bool debugging = false;

	bool testing  = false;

	string dpi = "";

	string size;

    int img_width, img_height;

	float avg_height,avg_width;

    void parse_arguments(int argc, char ** argv){
        
		if( this->debugging ){
			cout << "Parsing arguments..." << endl;
		}

        for( int i = 1; i < argc; i ++ ){
            if ( this->argument_exists("--image", i, argc, argv) ){
                this->image = argv[i + 1];
            }

			if( this->argument_exists("--dpi",i,argc,argv) ){
				this->dpi = argv[i+1];
			}

			if( this->argument_exists("--size",i,argc,argv) ){
				this->size = argv[i+1];
			}

            if ( this->argument_exists("--debugging", i, argc, argv) ){
                this->debugging = true;
            }

			if ( this->argument_exists("--testing", i, argc, argv) ){
                this->testing = true;
                this->image = "test.jpg";
            }
        }            

        cout << "Arguments parsed!" << endl;   
		if( this->debugging ){
			cout << "Arguments:" << endl;
			cout << "DPI: " << this->dpi << endl;
			cout << "Size: " << this->size << " MB" << endl;

		}
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

    
    vector<string> explode( string to_explode, string separator){
		string name,type;
		int pos = to_explode.find_last_of(separator);
		type = to_explode.substr(pos+1);
		name = to_explode.substr(0,pos);
		vector<string> full_name;
		full_name.push_back(name);
		full_name.push_back(type);
		return full_name;
	}

    void write_image(string new_name, cv::Mat &img){
        vector<string> exploded = this->explode(this->image,"-");

        string save_name = exploded[0] + "-" + new_name + "." + exploded[1];

        cv::imwrite(PROCESSED_DIR +  "/" + save_name, img);

		cout << "--image: " <<  save_name << endl;

        return;
    }

	void scale_image( Mat &img){

		double dpi = atof(this->explode( this->dpi,"x")[0].c_str());
		double size = atof(this->size.c_str());

		if( dpi >= 200 ){
			return;
		}

		if( size > 1.5 ){
			this->dpi = "300";
			return;
		}

		if( this->debugging ){
			cout << "Resizing image..." << endl;
		}

		const double pix_to_inch = 0.0104166667;
		double inch_w, inch_h, resize_w, resize_h;

		//resize image
		inch_w = img.cols * pix_to_inch;
		inch_h = img.rows * pix_to_inch;

		resize_w = inch_w * 300;
		resize_h = inch_h * 300;

		if( this->debugging ){
			cout << "New dimensions:" << endl;
			cout << "Width:" << resize_w << endl;
			cout << "Height:" << resize_h << endl;
		}

		resize(img,img,Size(resize_w ,resize_h));

		if( WRITE_IMAGES){
            this->write_image("resized", img);
        }

        vector<string>  name_exploded = this->explode(this->image,"-");

		if( this->testing ){
			IMAGE_PATH = PROCESSED_DIR + "/" + name_exploded[0] + "-resized." + name_exploded[1] ;
		}else{
			IMAGE_PATH = PROCESSED_DIR + "/" + name_exploded[0]  + "-resized." + name_exploded[1] ;
		}

		//adjust avg values
		this->avg_height = this->avg_height * pix_to_inch * 300;
		this->avg_width = this->avg_width * pix_to_inch * 300;

		if( this->debugging ){
			cout << "New avarages:" << endl;
			cout << "Width:" << this->avg_height  << endl;
			cout << "Height:" << this->avg_width  << endl;
		}


	}

    cv::Mat gray_scale( cv::Mat &original){

		if( this->debugging ){
			cout << "Grayscaling image..." << endl;
		}

        cv::Mat gray;
        cv::cvtColor(original, gray, cv::COLOR_BGR2GRAY);
        if( WRITE_IMAGES){
            this->write_image("gray", gray);
        }

        return gray;
    }

    cv::Mat threshold(cv::Mat &original){
        
		if( this->debugging ){
			cout << "Thresholding image..." << endl;
		}

        cv::Mat adaptive;

		float block_size;

		int truncated;

		if( !this->avg_width ){
			block_size = 23;

		}else{
			truncated = trunc((int)this->avg_width );
			if ( truncated % 2 != 1){
				block_size = truncated- 1;
			}else{
				block_size = truncated;
			}
		}

		// cout << "BLOCK SIZE: " << this->avg_width << endl;
		
		// cout << "BLOCK SIZE: " << cv::THRESH_BINARY_INV << endl;

        // cv::threshold(original,adaptive,0,255,THRESH_BINARY_INV+THRESH_OTSU);

        cv::adaptiveThreshold(original,adaptive,255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, block_size,20);
        
        if( WRITE_IMAGES){
            this->write_image("adaptive", adaptive);

        }

        return adaptive;
    }

    cv::Mat morph(cv::Mat &original ){

		if( this->debugging ){
			cout << "Morphing image..." << endl;
		}


        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(1,1));
        cv::Mat morphed;
        cv::morphologyEx(original, morphed,MORPH_RECT, kernel);
        // cv::erode(original, morphed, kernel);

         if( WRITE_IMAGES){
            this->write_image("morphed", morphed);

        }

        return morphed;
    }

    cv::Mat remove_straight_lines( cv::Mat img, float  avg_width, float avg_height){

		if( this->debugging ){
			cout << "Removing straight lines from image..." << endl;
		}

		cv::Mat horizontal = img.clone();
		cv::Mat vertical = img.clone();
		int horizontal_size = horizontal.cols / (15) ;
		cv::Mat horizontalStructure = getStructuringElement(cv::MORPH_RECT, cv::Size(horizontal_size, 1));
		erode(horizontal, horizontal, horizontalStructure, Point(-1, -1));
		dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));
		bitwise_not(horizontal, horizontal);
		cv::Mat edges2;
		adaptiveThreshold(horizontal, edges2, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, -2);

		cv::Mat kernel2 = cv::Mat::ones(2, 2, CV_8UC1);
		dilate(edges2, edges2, kernel2);
		cv::Mat smooth2;
		horizontal.copyTo(smooth2);

		blur(smooth2, smooth2, Size(2, 2));
		smooth2.copyTo(horizontal, edges2);
		if( WRITE_IMAGES){
            this->write_image("horizontal_lines", edges2);
        }


		int vertical_size = vertical.rows / (15) ;
		cv::Mat verticalStructure = getStructuringElement(cv::MORPH_RECT, cv::Size(1, vertical_size));

		erode(vertical, vertical, verticalStructure, Point(-1, -1));
		dilate(vertical, vertical, verticalStructure, Point(-1, -1));

		bitwise_not(vertical, vertical);
		cv::Mat edges;
		adaptiveThreshold(vertical, edges, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, -2);

		cv::Mat kernel = cv::Mat::ones(2, 2, CV_8UC1);
		dilate(edges, edges, kernel);
		cv::Mat smooth;
		vertical.copyTo(smooth);

		blur(smooth, smooth, Size(2, 2));
		smooth.copyTo(vertical, edges);
			if( WRITE_IMAGES){
            this->write_image("vertical_lines", edges);

        }
		// imwrite(processed_image_path + "vertical.png",edges);
		// processed_paths.push_back(processed_image_path + "vertical.png");

		vector<vector<Point>> cnts_h, cnts_v;
		vector<Vec4i> hier_h, hier_v;
		findContours(edges2, cnts_h, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0,0));
		findContours(edges, cnts_v, RETR_CCOMP, CHAIN_APPROX_SIMPLE, Point(0,0));
		drawContours( img, cnts_v, -1, (0,255,0), avg_height);

		drawContours( img, cnts_h, -1, (0,255,0), avg_width);
		 if( WRITE_IMAGES){
            this->write_image("removed_lines", img);

        }

		return img;
	}

    bool render_text_box(){

		if( this->debugging ){
			cout << "Rendering text box..." << endl;
		}

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

		if( this->debugging ){
			cout << "Calcuating avg width and height for found characters..." << endl;
		}

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

		if( this->debugging ){
			cout << "Average width:" << avg_w << endl;
			cout << "Average height:" << avg_h << endl;
		}

		
		return true;
	}

    cv::Mat remove_straight_lines2( cv::Mat img){
		Mat horizontal_kernel = getStructuringElement(MORPH_RECT, Size(25,1));
		Mat remove_horizontal;
 		morphologyEx(img,remove_horizontal,MORPH_CLOSE, horizontal_kernel,Point(-1,-1), 3);
		if( WRITE_IMAGES){
            this->write_image("removed_2", remove_horizontal);
        }
		return img;
	}

	  cv::Mat remove_straight_lines3( cv::Mat img){
		Mat color_dst;
		vector<Vec4i> lines;
		 cvtColor( img, color_dst, COLOR_GRAY2BGR );
		HoughLinesP( img, lines, 1, CV_PI/180, 80, 50,0 );
		for( size_t i = 0; i < lines.size(); i++ )
		{
		line( color_dst, Point(lines[i][0], lines[i][1]),
		Point( lines[i][2], lines[i][3]), Scalar(0,0,255), 3, 8 );
		}

		if( WRITE_IMAGES){
            this->write_image("removed_3", color_dst);
        }
		return img;
	}

	Mat deskew_image( Mat img ){

		if( this->debugging ){
			cout << "Deskewing image..." << endl;
		}

		vector<Point> locations;
		
		int angle;

		RotatedRect rect;

		Mat matrix,rotated;

		Point2f center( img.cols/2, img.rows/2);
		
		findNonZero(img, locations);
		rect = minAreaRect(locations);

		if( rect.angle == 90 ){
			return img;
		}

		// if (rect.angle < 90){

		// 	angle = 90 - rect.angle;
			
		// }else{
		// 	angle = rect.angle;

		// }

		matrix = getRotationMatrix2D(center,rect.angle  ,1);

		warpAffine(img, rotated,matrix,Size(img.cols, img.rows));

		if( WRITE_IMAGES){
            this->write_image("deskewd", rotated);
        }

		if( this->debugging ){
			cout << "Skew angel:" << rect.angle << endl;
		}

		return img;
	}

	void find_regions_extract_text(){

        vector<string>  name_exploded = this->explode(this->image,"-");
		string name = PROCESSED_DIR + "/" + name_exploded[0] + "-removed_lines." + name_exploded[1];
		

		Pix *image = pixRead(name.c_str());
		int  left, right, top, bottom;
		bool found = false;
		Pixa * pixaImg = pixaCreate(0);
		pixaAddPix(pixaImg, image, L_CLONE);
		
		tesseract::TessBaseAPI *ocrAPI;
		ocrAPI = new tesseract::TessBaseAPI;
		ocrAPI->SetVariable("user_defined_dpi", this->dpi.c_str());
		ocrAPI->SetVariable("preserve_interword_spaces", "true");

		PageIteratorLevel line_level = RIL_BLOCK;
		
		// analyse->InitForAnalysePage();
		ocrAPI->Init(TESS_DATA.c_str(),"ron") ;
		ocrAPI->SetImage(image);
		ocrAPI->SetPageSegMode(tesseract::PSM_AUTO);

		PageIterator* pi = ocrAPI->AnalyseLayout(true);

  		this->ocr(ocrAPI, pixaImg);

		ocrAPI->End();

	}

	void ocr(TessBaseAPI *&api, Pixa *&pixaImg ){
		Mat regions_filled = Mat::zeros(this->img_height,this->img_width,0);

		cout << "OCR-ing" << endl;

		ofstream myfile("res.txt");
		if (!myfile.is_open()){
			cout << "Could not open file!" << endl;
		}

		tesseract::PageIteratorLevel level = tesseract::RIL_TEXTLINE;


		Boxa * boxes = api->GetRegions(&pixaImg);
		for( int i = 0; i < boxes->n; i++){
			BOX *box = boxaGetBox(boxes,i, L_CLONE);
			cv::Rect roi(box->x, box->y, box->w, box->h);
				api->SetRectangle(box->x, box->y, box->w, box->h);
				api->Recognize(0);

				tesseract::ResultIterator* ri = api->GetIterator();

				do{
					const char* line_of_text = ri->GetUTF8Text(level);
					if( line_of_text != NULL && line_of_text != ""){
						myfile << line_of_text <<  endl;
					}

				}while(ri->Next(level));

			cv::rectangle(regions_filled, roi, cv::Scalar(255, 255, 255), -1);
			// if( WRITE_IMAGES){
            // 	this->write_image("analyse_regions_filled", regions_filled);
        	// }
		}

		if( WRITE_IMAGES){
            this->write_image("analyse_regions_filled", regions_filled);
        }

		myfile.close();
		regions_filled.release();

	}

	Json::Value ocr_image( vector<vector<Point>> contours){

		vector<string>  name_exploded = this->explode(this->image,"-");
		string pix_path = PROCESSED_DIR + "/" + name_exploded[0] + "-adaptive." + name_exploded[1];
		Pix *pixImg = pixRead(pix_path.c_str());
		Json::Value text_blocks;

		tesseract::TessBaseAPI *ocrApi;
		ocrApi = new tesseract::TessBaseAPI;
				if ( ocrApi->Init(TESS_DATA.c_str(),"ron")  ) {
		fprintf(stderr, "Could not init ocr api");
		}

	 	ocrApi->SetImage(pixImg);
		ocrApi->SetVariable("preserve_interword_spaces", "true");
		ocrApi->SetVariable("user_defined_dpi","300");
		ocrApi->SetPageSegMode(tesseract::PSM_SINGLE_BLOCK);


  		tesseract::PageIteratorLevel line_level = tesseract::RIL_TEXTLINE;
		tesseract::PageIteratorLevel symbol_level = tesseract::RIL_SYMBOL;
  		// tesseract::PageIteratorLevel line_level = tesseract::RIL_SYMBOL;

		
ofstream myfile("res.txt");
		   if (!myfile.is_open())
			{
				cout << "Could not open file!" << endl;
			}
		for(int idx = 0; idx < contours.size(); idx++){
			Rect rect = boundingRect(contours[idx]);

			//create json item
			text_blocks[idx]["x"] 		= rect.x;
			text_blocks[idx]["y"] 		= rect.y;
			text_blocks[idx]["width"] 	= rect.width;
			text_blocks[idx]["height"] = rect.height;
			text_blocks[idx]["id"] = idx;


			ocrApi->SetRectangle(rect.x, rect.y, rect.width, rect.height);		
			ocrApi->Recognize(0);

			tesseract::ResultIterator* ri = ocrApi->GetIterator();
			tesseract::ResultIterator* riSym = ocrApi->GetIterator();

			int line_c = 0;
			do {


				const char* line_of_text = ri->GetUTF8Text(line_level);
				float conf = ri->Confidence(line_level);
				if( line_of_text != NULL && line_of_text != ""){

					myfile << line_of_text <<  endl;
						
			
					text_blocks[idx]["lines"]["line_" + to_string(line_c)]["text"] = line_of_text;
						//paragraph Information
					tesseract::ParagraphJustification just;
					bool is_list, is_crown;
					int first_line_indent;
					ri->ParagraphInfo(&just, &is_list, &is_crown, &first_line_indent);	        

					// text_blocks[idx]["lines"]["text"]["text_lines"]["line_" + to_string(line_c)] = line_of_text;
					// int x, y, width_line, height_line, symbol_c = 0;
					// ri->BoundingBox(line_level, &x, &y, &width_line, &height_line);
			
					line_c++;
				}

			}while(ri->Next(line_level));	
		
		}
	myfile.close();
		ocrApi->End();
		cout << "Done OCR!" << endl;
		return  text_blocks;
	}

	  

};
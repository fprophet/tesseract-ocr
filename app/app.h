#include <stdio.h>
#include <iostream>
using namespace std;
using namespace tesseract;
using namespace cv;

const string ROOT = "/var/www/html/tesseract-ocr";
const string IMAGE_DIR = ROOT + "/uploads"; 
const string PROCESSED_DIR = ROOT + "/public/processed-images";
const string APP_PATH = ROOT + "/app";
const string TESS_DATA = "/usr/local/share/tessdata/";
string IMAGE_PATH;

bool WRITE_IMAGES = false;

struct application{
    
    string image;

    bool debugging = false;

	bool testing  = false;	

	string dpi = "", size, language = "eng";

    int img_width, img_height;


    void parse_arguments(int argc, char ** argv){
        
		cout << "Parsing arguments..." << endl;

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

			if ( this->argument_exists("--language", i, argc, argv) ){
                this->language = argv[i+1];
            }

			if ( this->argument_exists("--write_images", i, argc, argv) ){
                WRITE_IMAGES = true;
            }
        }            

		if( this->debugging ){
        	cout << "Arguments parsed!" << endl;   
			cout << "Arguments:" << endl;
			cout << "DPI: " << this->dpi << endl;
			cout << "Size: " << this->size << " MB" << endl;
			cout << "Language: " << this->language << endl;

		}
    }

    bool argument_exists(string arg, int i, int argc, char ** argv){

        string converted = argv[i];

        int comparison = converted.compare(arg);

        if(  comparison == 0 ){
            if( i + 1 < argc ){
                return true;
            }else{
                cout << "Error in arguments parsing! " 
				<< converted 
				<< " Trying to reach out of bounds array!" << endl;
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
        // vector<string> exploded = this->explode(this->image,"-");

        // string save_name = exploded[0] + "-" + new_name + "." + exploded[1];

        // cv::imwrite(PROCESSED_DIR +  "/" + save_name, img);

        cv::imwrite(PROCESSED_DIR +  "/" + new_name + ".png", img);


		cout << "--image: " <<  new_name << endl;

        return;
    }


	void scale_image( Mat &img){

		double dpi = atof(this->explode( this->dpi,"x")[0].c_str());
		double size = atof(this->size.c_str());
		bool non_scale_condition = (dpi > 200) 
			|| ( this->img_width > 1000 && this->img_height > 1000 ) 
		 	|| (size > 1.5);

		if( non_scale_condition ){
			return;
		}

		if( this->debugging ){
			cout << "Resizing image..." << endl;
		}

		this->dpi = "300";

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

		this->img_height = resize_h;
		this->img_width = resize_w;
		resize(img,img,Size(resize_w ,resize_h));

		if( WRITE_IMAGES){
            this->write_image("resized", img);
        }

	}

    void gray_scale(Mat &image){

		if( this->debugging ){
			cout << "Grayscaling image..." << endl;
		}

        cvtColor(image, image, cv::COLOR_BGR2GRAY);
        if( WRITE_IMAGES){
            this->write_image("gray", image);
        }

    }

    void threshold(Mat &image){
        
		if( this->debugging ){
			cout << "Thresholding image..." << endl;
		}

        adaptiveThreshold(image,image,255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 21,20);
        if( WRITE_IMAGES){
            this->write_image("adaptive", image);

        }
    }

   	void morph(cv::Mat &original ){

		if( this->debugging ){
			cout << "Morphing image..." << endl;
		}


        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5));
        cv::Mat morphed;
		
        cv::morphologyEx(original, original,MORPH_OPEN, kernel);
        // cv::erode(original, morphed, kernel);

         if( WRITE_IMAGES){
            this->write_image("morphed", original);

        }

    }

	void deskew(Mat &img, double angle){
		if( angle == 0 ){
			return;
		}
		Mat matrix,rotated;
		Point2f center( img.cols/2, img.rows/2);

		matrix = getRotationMatrix2D(center,angle   ,1);

		warpAffine(img, img,matrix,Size(img.cols, img.rows));

		if( WRITE_IMAGES){
            this->write_image("deskewed", img);
        }
	}

	double calculate_angle( Mat pre_desk){

		if( this->debugging ){
			cout << "Calculating skew angle.." << endl;
		}

		vector<Point> locations;
		int angle;
		RotatedRect rect;
		
		findNonZero(pre_desk, locations);
		rect = minAreaRect(locations);

		if( rect.angle == 90 ){
			return 0;
		}

		if (rect.angle < -45){
			angle = -(90 + rect.angle);
			
		}else{
			angle = -rect.angle;
		}

		if ( this->debugging){
			cout << "Skew angel:" << angle << endl;
		}

		return angle;

	}

	int find_text_orientation(Mat &img){
		if( this->debugging ){
			cout << "Finding orentation..." << endl;
		}

		PIX *image = this->matToPix(img);
		
		tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
		if( api->Init(NULL, "osd")){
			fprintf(stderr, "Could not initialize tesseract.\n");
        	exit(1);
		}

		api->SetPageSegMode(tesseract::PSM_SPARSE_TEXT_OSD);
		api->SetImage(image);
		int orient_deg;
		float orient_conf,script_conf;
		const char* script_name;
		api->DetectOrientationScript(&orient_deg, &orient_conf, &script_name, &script_conf);
		printf("Orientation in degrees: %d\n Orientation confidence: %.2f\n",
		orient_deg, orient_conf);
		api->End();
		delete api;
		pixDestroy(&image);

		if( orient_deg == 90 ){
			rotate(img,img,ROTATE_90_COUNTERCLOCKWISE);
		}
		if( orient_deg == 180){
			rotate(img,img,ROTATE_90_CLOCKWISE);
			rotate(img,img,ROTATE_90_CLOCKWISE);

		}
		if(orient_deg == 270 ){
			rotate(img,img,ROTATE_90_CLOCKWISE);
		}

		this->img_height = img.rows;
		this->img_width = img.cols;
		
		if(WRITE_IMAGES){
			this->write_image("reoriented", img);
		}

		return orient_deg;
    
	}

	Mat create_text_image(Mat &img){
		if( this->debugging ){
			cout << "Creating text image..." << endl;
		}

		Mat text_img = Mat::zeros(img.rows,img.cols,0);

		//convert to pix
		PIX *image = this->matToPix(img);

		//create api instance
		tesseract::TessBaseAPI *api = new tesseract::TessBaseAPI();
		api->Init(TESS_DATA.c_str(), this->language.c_str());
		api->SetImage(image);
		//seg mode set to sparse_text
		api->SetPageSegMode(tesseract::PSM_SPARSE_TEXT);

		//run a analysis layout
		api->AnalyseLayout(true);
		
		//get symbol components  
		Boxa* boxes = api->GetComponentImages(RIL_SYMBOL, true, NULL, NULL);

		//using components create a new Mat with the symbols found in order calulcate the skew with a better accuracy
		for (int i = 0; i < boxes->n; i++) {
			BOX* box = boxaGetBox(boxes, i, L_CLONE);
			Rect bounding_box(box->x, box->y, box->w, box->h);
			if( box->w == img.cols || box->h == img.rows){
				continue;
			}
			Mat part = img(bounding_box);
			part.copyTo(text_img(bounding_box));
			boxDestroy(&box);
		}
		delete api;
	
		if( WRITE_IMAGES){
			this->write_image("text_image", text_img);
		}
		
		return text_img;
	}

	TessBaseAPI * init_api(Mat &img){

		Pix *image = this->matToPix(img);

		tesseract::TessBaseAPI *ocrAPI;
		ocrAPI = new tesseract::TessBaseAPI;
		
		if( ocrAPI->Init(TESS_DATA.c_str(),this->language.c_str()) ){
			fprintf(stderr, "Could not initialize tesseract.\n");
        	exit(1);
		}

		ocrAPI->SetVariable("user_defined_dpi", this->dpi.c_str());
		ocrAPI->SetVariable("preserve_interword_spaces", "true");

		ocrAPI->SetPageSegMode(tesseract::PSM_AUTO_ONLY );
		ocrAPI->SetImage(image);

		return ocrAPI;

	}

	void remove_non_text_regions(TessBaseAPI *&api, Mat &img, Json::Value & json ){
		if( this->debugging ){
			cout << "Finding text regions..." << endl;
		}

		Mat text_regions = Mat::zeros(this->img_height,this->img_width,0);

		Pix* pixImg = this->matToPix(img);
		Pixa * pixaImg = pixaCreate(0);
		pixaAddPix(pixaImg, pixImg, L_CLONE);

		PageIterator* pi = api->AnalyseLayout(true);

		Boxa * reg_boxes = api->GetRegions(&pixaImg);

		for( int j = 0; j < reg_boxes->n; j++){
			
			PolyBlockType blk = pi->BlockType();
			if(PTIsTextType(blk)){
				BOX *box = boxaGetBox(reg_boxes,j, L_CLONE);
				Rect bounding_box(box->x, box->y, box->w, box->h);

				Mat part = img(bounding_box);
				part.copyTo(text_regions(bounding_box));

				json["blocks"][j]["coordinates"]["x"] 		= bounding_box.x;
				json["blocks"][j]["coordinates"]["y"] 		= bounding_box.y;
				json["blocks"][j]["coordinates"]["width"] 	= bounding_box.width;
				json["blocks"][j]["coordinates"]["height"] = bounding_box.height;
				json["blocks"][j]["block-id"] = j;
			}

			pi->Next(RIL_BLOCK);
		};

		img = text_regions;
		if( WRITE_IMAGES){
			this->write_image("text_regions", text_regions);
		}
		return;
		pixDestroy(&pixImg);
		pixaDestroy(&pixaImg);

	}


	void get_text_regions(TessBaseAPI *& api, PageIterator *& pi, Pixa * pixaImg, Json::Value & json ){
		cout << "Getting text regions..." << endl;
		Mat text_regions = Mat::zeros(this->img_height,this->img_width,0);

		Boxa * reg_boxes = api->GetRegions(&pixaImg);
		for( int j = 0; j < reg_boxes->n; j++){
			
			bool found = false;
			int imgs_c = 0;
			tesseract::ResultIterator* ri = api->GetIterator();
			Pix * found_block = pi->GetBinaryImage(RIL_BLOCK);

			PolyBlockType blk = pi->BlockType();
		
			if( PTIsTextType(blk)){
				BOX *box = boxaGetBox(reg_boxes,j, L_CLONE);
				cv::Rect roi(box->x, box->y, box->w, box->h);
				json["blocks"][j]["x"] 		= roi.x;
				json["blocks"][j]["y"] 		= roi.y;
				json["blocks"][j]["width"] 	= roi.width;
				json["blocks"][j]["height"] = roi.height;
				json["blocks"][j]["id"] = j;
				cv::rectangle(text_regions, roi, cv::Scalar(255, 255, 255), -1);

				pi->Next(RIL_BLOCK);
			}else{
				cout << "Block Type: " << blk << endl;
				// string nm = "pix-" + to_string(j) + ".png";
					// cout << "PIXL: " << j  << "TYPE: " << blk <<endl;
					// pixWrite( nm.c_str(), found_block, IFF_PNG);
			}
		};

		if( WRITE_IMAGES ){
			this->write_image("text_block_regions", text_regions);
		}
	}

	void extract_text(TessBaseAPI *& api, PageIterator *& pi,  Pixa * pixaImg, Json::Value & json){
		if( this->debugging){
			cout << "Extracting text..." << endl;
		}

		Mat regions_filled = Mat::zeros(this->img_height,this->img_width,0);

		Boxa * boxes = api->GetRegions(&pixaImg);
		Boxa * line_boxes = api->GetTextlines(true, 0, &pixaImg, nullptr, nullptr);

		int  left, right, top, bottom;
		for( int i = 0; i < boxes->n; i++){

			BOX *box = boxaGetBox(boxes,i, L_CLONE);
			cv::Rect roi(box->x, box->y, box->w, box->h);

			api->SetRectangle(box->x, box->y, box->w, box->h);
			api->Recognize(0);

			tesseract::ResultIterator* ri = api->GetIterator();

			// this->process_output(ri, json,line_boxes,i);

			rectangle(regions_filled, roi, cv::Scalar(255, 255, 255), -1);
		}

		if( WRITE_IMAGES){
            this->write_image("analyse_regions_filled", regions_filled);
        }

		regions_filled.release();

	}

	void ocr(TessBaseAPI *&api, Mat img, Json::Value &json ){
		if( this->debugging){
			cout << "OCR-ing..." << endl;
		}

		Mat regions_filled = Mat::zeros(this->img_height,this->img_width,0);

		Pix* pixImg = this->matToPix(img);
		api->SetImage(pixImg);

		PageIterator* pi = api->AnalyseLayout(true);

		for( auto &block:json["blocks"]){
			Rect roi(block["coordinates"]["x"].asInt(), block["coordinates"]["y"].asInt(), block["coordinates"]["width"].asInt(), block["coordinates"]["height"].asInt());

			api->SetRectangle(roi.x, roi.y, roi.width, roi.height);
			api->Recognize(0);

			tesseract::ResultIterator* ri = api->GetIterator();

			this->process_output(ri, block);

			rectangle(regions_filled, roi, Scalar(255, 255, 255), -1);
		} 

		if( WRITE_IMAGES){
            this->write_image("analyse_regions_filled", regions_filled);
        }

		regions_filled.release();

		pixDestroy(&pixImg);
	}


	void process_output(ResultIterator *& ri, Json::Value &block){
		int line_count = 0, word_count = 0, left,right,top,bottom;
		const char *font_name;
					bool bold, italic, underlined, monospace, serif, smallcaps;
					int pointsize, font_id;
		do{
			ri->BoundingBox(RIL_TEXTLINE, &left, &top, &right, &bottom);
			block["lines"]["line_" + to_string(line_count)]["coordinates"]["left"] = left;
			block["lines"]["line_" + to_string(line_count)]["coordinates"]["top"] = top;
			block["lines"]["line_" + to_string(line_count)]["coordinates"]["right"] = right;
			block["lines"]["line_" + to_string(line_count)]["coordinates"]["bottom"] = bottom;

			word_count = 0;
		
			do{
				const char* word = ri->GetUTF8Text(RIL_WORD);

				font_name = ri->WordFontAttributes(&bold, &italic, &underlined,
									&monospace, &serif,
									&smallcaps, &pointsize,
									&font_id);
				if( word != NULL && word != ""){
					block["lines"]["line_" + to_string(line_count)]["words"]["word_" + to_string(word_count)]["text"] = word;
					block["lines"]["line_" + to_string(line_count)]["words"]["word_" + to_string(word_count)]["italic"] = italic;
					block["lines"]["line_" + to_string(line_count)]["words"]["word_" + to_string(word_count)]["bold"] = bold;
					block["lines"]["line_" + to_string(line_count)]["words"]["word_" + to_string(word_count)]["pointsize"] = pointsize;
					block["lines"]["line_" + to_string(line_count)]["words"]["word_" + to_string(word_count)]["font_id"] = font_id;

					word_count++;
					delete[] word;
				}
			}while(!ri->IsAtFinalElement(RIL_TEXTLINE, RIL_WORD) && ri->Next(RIL_WORD) );

			line_count++;
		}while(ri->Next(RIL_TEXTLINE));

	}

	int find_block_idx(Json::Value json, Rect roi){
		int idx = 0;
		for( const  auto& obj:json["blocks"]){
			if( roi.x >= obj["x"].asInt() && roi.x + roi.width <= obj["x"].asInt() + obj["width"].asInt() ){
				if( roi.y  >= obj["y"].asInt() && roi.y + roi.height <= obj["y"].asInt() + obj["height"].asInt() ){

					return idx;
				}
			}
			idx++;
		}	

		return -1;
	}

	void clean_json(Json::Value &json ){
		vector<int> to_rem;
		for( int i = 0; i < json["blocks"].size(); i++){
			if( !json["blocks"][i]["lines"] || json["blocks"][i]["lines"].size() == 0
			|| json["blocks"][i]["lines"].isNull()){
				json["blocks"][i].clear();
			}
		}

		for (const auto &obj:json){
			if( obj.empty()){
				// json.removeMember(obj);
			}
			//  json["blocks"].removeMember("block_" + to_string(to_rem[i]));
		}

	}

	Pix *matToPix(cv::Mat mat){
		Pix *pixd = pixCreate(mat.size().width, mat.size().height, 8);
		for(int y=0; y<mat.rows; y++) {
			for(int x=0; x<mat.cols; x++) {
				pixSetPixel(pixd, x, y, (l_uint32) mat.at<uchar>(y,x));
			}
		}
		return pixd;
	}	  

	bool is_child_of( Rect R1, Rect R2){
		{
    if (   (R2.x+R2.width) < (R1.x+R1.width)
        && (R2.x) > (R1.x)
        && (R2.y) > (R1.y)
        && (R2.y+R2.height) < (R1.y+R1.height)
        )
    {
        return true;
    }
    else
    {
        return false;
    }
}
	}

	void write_json(Json::Value json){
		Json::StyledWriter writer;

		ofstream jsonResult;
		jsonResult.open("results.json");

		jsonResult << writer.write(json) << endl;
		
		jsonResult.close();

		cout << "Finished!" << endl; 
	}

	void display_original (Mat img, int orient, double angle, Json::Value json ){
		
		if( orient == 90 ){
			rotate(img,img,ROTATE_90_COUNTERCLOCKWISE);
		}
		if( orient == 180){
			// rotate(img,img,ROTATE_90_CLOCKWISE);
			// rotate(img,img,ROTATE_90_CLOCKWISE);
		}
		if(orient == 270 ){
			rotate(img,img,ROTATE_90_COUNTERCLOCKWISE );
			rotate(img,img,ROTATE_180 );
		}

		this->deskew(img, angle);
		for( auto &block:json["blocks"]){
			for( auto &line:block["lines"]){
				rectangle(img, Point(line["coordinates"]["left"].asInt(), line["coordinates"]["top"].asInt()),
					Point(line["coordinates"]["right"].asInt(), line["coordinates"]["bottom"].asInt()), Scalar(0,255,0) );
			}
		}

		this->write_image("original", img);
	}

};
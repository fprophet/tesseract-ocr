<?php
require_once("xml-section.php");
class XMLApi {
    public $xml_header = '<?xml version="1.0" encoding="UTF-8" standalone="yes"?>
    <w:document xmlns:ve="http://schemas.openxmlformats.org/markup-compatibility/2006" xmlns:o="urn:schemas-microsoft-com:office:office" xmlns:r="http://schemas.openxmlformats.org/officeDocument/2006/relationships" xmlns:m="http://schemas.openxmlformats.org/officeDocument/2006/math" xmlns:v="urn:schemas-microsoft-com:vml" xmlns:wp="http://schemas.openxmlformats.org/drawingml/2006/wordprocessingDrawing" xmlns:w10="urn:schemas-microsoft-com:office:word" xmlns:w="http://schemas.openxmlformats.org/wordprocessingml/2006/main" xmlns:wne="http://schemas.microsoft.com/office/word/2006/wordml"> 
    <w:body>';
    public 	$xml_end = '
    </w:body>
    </w:document>';
    public $file = '';
    public $xml_content = '';
    public $sections = [];
    public $json_data;
    public $zip_handle;

    public $page_width;
    public $page_height;
    public $dpi;

    // public function addPara($text){
        

    //     $this->xml_content .= '
    //         <w:p>
    //             <w:r><w:t>' . $text . ' </w:t></w:r>
    //         </w:p>
    //     ';
    //     // echo var_dump($this->xml_content );
    //     // fopen($this->file, 'a');
    //     // file_put_contents($this->file, $xml_string);
    //     // fclose($this->file);
        
    // }

    public function addSection($style){
        $section = new Section($style);
        // $section_xml = $section->create();
        // echo var_dump($section_xml);

        array_push($this->sections,$section); 
        return $section;
        // echo var_dump($this->sections);
    }

    // public function addParaStyle($arr){
    //     if( !is_array($arr) || empty($arr)){
    //         return false;
    //     }

    // }

    

  
    public function assemble_sections(){
        $last = end($this->sections);
        foreach($this->sections as $section){
            if($section == $last){
                $section->section_xml = $section->section_text . $section->section_xml;
            }else{
                $section->section_xml = $section->section_text . '<w:p><w:pPr>' . $section->section_xml . '</w:pPr></w:p>';
            }
            $this->xml_content .= $section->section_xml;
        }
    }


    
    public function write_file(){
        $this->assemble_sections();
        $handle = fopen($this->file, 'w');
        // echo "<pre>";
        // print_r($this->xml_content);
        fwrite($handle, $this->xml_header);
        fwrite($handle, $this->xml_content);
        fwrite($handle, $this->xml_end);
        fclose($handle);
    }

    public function get_json(){
        $contents = file_get_contents(APP_PATH . "/results.json");
        if( $contents ){
            $this->json_data = json_decode($contents,true);
            return true;
        }
        return false;
    }

    public function copy_and_unzip_original(){
        $save_path = ROOT . "/public/results.docx";
        if (!copy(APP_PATH . "/word.docx", $save_path)){
            return false;
        }

        $this->zip_handle = new ZipArchive();
        if( ! $this->zip_handle->open($save_path)){
            return false;
        }
       
        $this->zip_handle->deleteName("word/document.xml");
        $this->file = ROOT . '/public/document.xml';
        // var_dump($this->json_data);
        return true;
    }

    public function get_converted_dims(){
        $dpi = explode("x", $this->dpi);

        $dims["width"] = ($this->page_width / (int)$dpi[0]) * 1440;
         $dims["height"] =  ($this->page_height / (int)$dpi[0]) * 1440;
        // var_dump($this->page_width);
        // var_dump($this->page_height);    
        return $dims;
    }

    public function find_point_size($words){
        $sizes = [];
        foreach( $words as $word){
            $sizes[] = $word["pointsize"];
        }
        return array_sum($sizes)/count($sizes);
        $freqArray = array_count_values($sizes);
        $maxFreq = max($freqArray);
        $mostFrequent = array_search($maxFreq, $freqArray);
        return $mostFrequent;
    }

    public function escape_xml_special($text) {
        if (stripos($text, "<") !== false || stripos($text, ">") !== false || stripos($text, "&") !== false) {
            $text = str_replace("\n","<w:br/>", $text);
            $text = str_replace("<", '  ', $text);
            $text = str_replace(">", '  ', $text);
            $text = str_replace('&', '  ', $text);
            $text = str_replace('—', '-', $text);
            // echo var_dump($text);
            return $text;
        }
        return $text;
    }

    public function create_xml(){
        $dims = $this->get_converted_dims();
        $orientation = "landscape";
        $dpi = explode("x", $this->dpi);

        $style = array(
            'orient' => $orientation,
            // 'pgSzW' => $dims["width"],
            // 'pgSzH' => $dims["height"],
        );

        $paperWidth = $this->_pixel_to_inch($this->page_width); 
        $paperHeight = $this->_pixel_to_inch($this->page_height);

        $width_percent = ($dims["width"] / $paperWidth);
        $height_percent = ($dims["height"] / $paperHeight);

        $section = $this->addSection($style);
        foreach( $this->json_data["blocks"] as $block ){
            if( !$block || !$block["lines"]){
                continue;
            }
            
            $x = $this->_pixel_to_twip(($block["x"] / 0.0104166667) / (int)$dpi[0]);
            $y = $this->_pixel_to_twip(($block["y"] / 0.0104166667) / (int)$dpi[0]);
            $width = $this->_pixel_to_twip(($block["width"] / 0.0104166667)/ (int)$dpi[0]);
            $height = $this->_pixel_to_twip(($block["height"] / 0.0104166667) / (int)$dpi[0]);
            $text = "";    
            $section->addPara();
            for( $i = 0; $i < count($block["lines"]); $i++){
                // $point_size = $this->find_point_size($line["lines"]["line_" . $i]["words"]);
                $point_size = 18;
                
                // var_dump($point_size);
                if(!$block["lines"]["line_" . $i]["words"] ){
                    continue;
                }
                $section->addLine();
                for( $j = 0; $j < count($block["lines"]["line_" . $i]["words"] ); $j++ ){
                    $text = $block["lines"]["line_" . $i]["words"]["word_" . $j]["text"] . " ";
                    $section->addText($this->_escape_xml_special($text), $point_size, $x,$y,$width,$height,$block["lines"]["line_" . $i]["words"]["word_" . $j]["bold"],$block["lines"]["line_" . $i]["words"]["word_" . $j]["italic"]  );
                }
                // $text .= "\n";

                $section->endLine();
               
                
            }
            $section->endPara();
        }

        $this->write_file();
        //                echo var_dump($this->xml_content);

        $this->zip_handle->addFile($this->file, "word/document.xml");
        $this->zip_handle->close();
    }

    private function _inch_to_twip($val) {
        return (float) $val * 1440;
    }

    private function _pixel_to_inch($val) {
        return ($val * 0.0104166667);
    }
    
    private function _pixel_to_twip($val) {

        $pixel_to_inch = ((float) ($val / 96));
        $inch_to_twip = $this->_inch_to_twip($pixel_to_inch);

        return $inch_to_twip;
    }

    private function _escape_xml_special($text) {
        if (stripos($text, "<") !== false || stripos($text, ">") !== false || stripos($text, "&") !== false) {
            // $text = str_replace("\n","<w:br/>", $text);
            $text = str_replace("<", '  ', $text);
            $text = str_replace(">", '  ', $text);
            $text = str_replace('&', '  ', $text);
            $text = str_replace('—', '-', $text);
            // echo var_dump($text);
            return $text;
        }
        return $text;
    }

    
}
?>
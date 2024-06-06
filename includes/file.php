<?php
require_once("config.php");

class File{
    public $name = "";
    public $tmp ="";
    public $save_path = UPLOADS;

    function __construct($options){
        foreach( $options as $key => $val ){
            if( !property_exists($this, $key)){
                die("Wrong class Parameters!");
            }
            $this->$key = $val;
        }

    }

    public function move_file(){
        if( !move_uploaded_file( $this->tmp, $this->save_path . "/" . $this->name ) ){
            return false;
        }
        return true;
    }

    public static function get_dpi($file){
        $im = new Imagick();
        $im->readImage($file);
        // $im->setImageUnits(2);
        
        $dpi_arr = $im->getImageResolution();
        if( !is_array($dpi_arr) ){
            return "300x300";
        }

        if( $dpi_arr["x"] == "" || $dpi_arr["x"] == 0 ){
            return "300x300";
        }

        return $dpi_arr["x"] . "x" . $dpi_arr["y"];
    }

    public static function get_size($file){
        return round(filesize($file) * 0.000001, 2);
    }

}

?>
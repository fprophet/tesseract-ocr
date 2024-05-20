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

}

?>
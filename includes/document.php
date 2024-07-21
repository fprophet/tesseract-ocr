<?php
require_once("config.php");
class Document{

    public $type = "";

    public $json_data;

    public static function copy_original(){
        if (copy(APP_PATH . "/word.docx", ROOT . "/public/results.docx")){
            return true;
        }

        return false;
    }



}

?>
<?php
session_start();

if( isset($_GET["progress"])){
    $context = $_GET["progress"];
    // var_dump($_SESSION);
    if( isset( $_SESSION["progress"][$context])){
        echo json_encode(["status"=>"success","data" => $_SESSION["progress"][$context] ]);
        unset($_SESSION["progress"][$context] );
        exit();
    }else{
        echo json_encode(["status" =>'empty' ]);
        exit();
    }

    // $contents = file_get_contents("/var/www/html/tesseract-ocr/logs/process.txt");
    //  echo json_encode(["status"=>"success","data" => $contents]);
    //     exit();

}


?>
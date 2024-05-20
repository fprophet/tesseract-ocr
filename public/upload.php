<?php
require_once("../includes/file.php");
if( $_FILES && isset($_FILES["file"])){
    $upload = $_FILES["file"];
    $file = new File(["name" => $upload["name"], "tmp" =>$upload["tmp_name"]]);

    if( !$file->move_file()){
        echo json_encode(["status"=> "fail","message"=> "Problems in uploading file!"]);
    }else{
        echo json_encode(["status"=> "success","message"=> "File Uploaded!"]);
    }
}
?>
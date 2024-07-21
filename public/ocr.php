<?php
require_once("../includes/config.php");
require_once("../includes/file.php");
require_once("../includes/xml-api.php");

require_once("../includes/process.php");
$data = json_decode(file_get_contents('php://input'), true);
$path = ROOT . "/uploads/" . $data["file"];


$dpi = File::get_dpi($path);
$size = File::get_size($path);

$strt = time();
if( !$data ){
    echo json_encode(["status"=> "fail", "message" => "No data provided!"]);
    exit();
}
session_start();
unset($_SESSION["progress"]);

delete_files_from(ROOT . "/public/processed-images/");

$process = new Process("process.txt");
$process->output_file->clear_contents();
$process->command = "./ocr --image " . escapeshellarg($data["file"]) . " --dpi " . $dpi . " --size " . $size . " --language " . $data["language"];
if( $data["debugging"] ){
    $process->command .= " --debugging true ";
}
if( $data["write_images"] ){
    $process->command .= " --write_images true ";
}
// echo "<pre>";
// var_dump($process->command);
$process->cwd = APP_PATH;
$process->run_process();
// var_dump($_SESSION);
$size = getimagesize($path);
switch( $data["type"] ){
    case "docx":
        $api = new XMLApi();
        if( !$api->get_json() ){
            echo "errorrrr";
            die();
        }
        $api->page_width = $api->json_data["document"]["width"];
        $api->page_height = $api->json_data["document"]["height"];
        $api->dpi = $api->json_data["document"]["dpi"];

        $api->copy_and_unzip_original();
        $api->create_xml();
        break;
    case "txt":


}


echo json_encode(["status"=> "sucecss", "message" => "Process done! Time to completion: "  . (time() - $strt), "link" => "http://www.tesseract-ocr.test/results.docx"]);
    
?>
<?php
require_once("../includes/config.php");
require_once("../includes/process.php");

$data = json_decode(file_get_contents('php://input'), true);

if( !$data ){
    echo json_encode(["status"=> "fail", "message" => "No data provided!"]);
    exit();
}
session_start();
unset($_SESSION["progress"]);
$process = new Process("process.txt");
$process->output_file->clear_contents();
$process->command = "./ocr" ;
$process->cwd = APP_PATH;
$process->run_process();
// var_dump($_SESSION);

echo json_encode(["status"=> "sucecss", "message" => "Process done!"]);

?>
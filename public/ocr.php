<?php
require_once("../includes/config.php");
require_once("../includes/file.php");

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
$process->command = "./ocr --image " . escapeshellarg($data["file"]) . " --dpi " . $dpi . " --size " . $size . " --debugging true";
$process->cwd = APP_PATH;
$process->run_process();
// var_dump($_SESSION);

echo json_encode(["status"=> "sucecss", "message" => "Process done! Time to completion: "  . (time() - $strt)]);

?>
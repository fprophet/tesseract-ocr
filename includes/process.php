<?php
require_once("config.php");
require_once("log.php");

class Process{
    public $command = "";
    public $descriptors_array = [];
    public $pipes = [];
    public $output_file = "";
    public $cwd = "";
    public $process = null;
    public $return_value;
    public $return_message; 
    
    public $output = '';
    
    public function __construct($output_file){
        if( $output_file !== "" && $output_file !== null){
            $this->_initialize_process_err($output_file);
        }
        $this->_initialize_descriptors();
        $this->_initialize_cwd();
    }
    
    
    private function _initialize_process_err($output_file){
        
        $this->output_file = new Log($output_file);
    }

    private function _initialize_descriptors(){
        global $command_log;
        $this->descriptors_array = [ 0 => ["pipe", "r"], 
                                     1 => ["pipe", "w"], 
                                     2 => ["file", ROOT . "/logs/" . $this->output_file->file_name , "a"]];
    }

    private function _initialize_cwd(){
        $this->cwd = ROOT . "uploads/";
    }

    public function run_process(){
        $this->process = proc_open($this->command, $this->descriptors_array, $this->pipes, $this->cwd, null);
        if (is_resource($this->process)){
            $this->return_message = fgets($this->pipes[1], 1024);
            fclose($this->pipes[0]);
            $this->output = stream_get_contents($this->pipes[1]);
            $message = "\nCommand: " . $this->command . " \n\nResponse\n";
            $this->output_file->addEntry($message, $this->output);
            $this->output_file->add_separator("#");
                
            fclose($this->pipes[1]);
            $this->return_value = proc_close($this->process);

        }
      
       
    }
    
    public static function run_c($command){
        $desc_arr = [ 0 => ["pipe", "r"], 
                                    1 => ["pipe", "w"], 
                                    2 => ["file", ROOT . "logs/quick_process_log.txt", "a"]];
        $pipes = [];
        $command = str_replace("(", "\(",$command);
        $command = str_replace(")", "\)",$command);
        $process = proc_open($command,  $desc_arr, $pipes, ROOT . "uploads/", null);
        
         if (is_resource($process)){
            $msg = fgets($pipes[1], 1024);
            fclose($pipes[0]);
            $output = stream_get_contents($pipes[1]);
            if( $output !== ""){
//                $this->output_file->addEntry("cv response", $output);
//                $this->output_file->add_separator("#");
            }
            fclose($pipes[1]);
            $val = proc_close($process);
            return  $msg;

        }

    }

 
}
?>
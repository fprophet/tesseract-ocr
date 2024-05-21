<?php

require_once("config.php");

class Log {

    public $action;
    static $log_dir = ROOT . "/logs/";
    public $file_name = "";
    public $contents = "";
    public $page_contents = "";
    public $path = "";

    function __construct($file_name) {
        $this->set_log_name($file_name);
        $this->path = static::$log_dir . $this->file_name;
        if (!self::check_file($this->file_name)) {
            self::create_file($this->file_name);
        } else {
            $this->get_contents();
            $this->page_entries();
        }
    }

    public static function create_file($file_name) {
        $dir = static::$log_dir . $file_name;
        if (fopen($dir, "w")) {
            return true;
        } else {
            return false;
        };
    }

    public function set_log_name($file_name) {
        $this->file_name = $file_name;
    }

    public function add_separator($separator = "") {
        $dir = static::$log_dir . $this->file_name;
        $content = str_repeat($separator, 150) . "\n";
        $handle = fopen($dir, 'a');
        // echo var_dump($dir);
        // die();
        fwrite($handle, $content);
        fclose($handle);
    }

    public function get_contents() {
        $this->contents = file_get_contents($this->path);
    }

    public static function check_dir($dir) {
        if (is_dir($dir)) {
            return true;
        } else {
            return false;
        }
    }

    // public static function check_permisions() {

    //     if ($handle = fopen(self::$log_dir . $this->file_name, "w")) {
    //         return true;
    //     } else {
    //         return false;
    //     }
    // }

    public function check_file($file_name) {
        if (file_exists($this->path)) {
            return true;
        } else {
            return false;
        }
    }

    public function addEntry($action, $message) {
        // global $file;
        $dir = static::$log_dir . $this->file_name;
        $content = date("F j, Y, g:i a") . "|" . $action . ": " . $message . "\r\n";
        $handle = fopen($dir, 'a');
        // echo var_dump($dir);
        // die();
        fwrite($handle, $content);
        fclose($handle);
    }

    public function clear_contents(){
        $handle = fopen(static::$log_dir . $this->file_name, 'w');
        // echo var_dump($dir);
        // die();
        fwrite($handle, "");
        fclose($handle);
    }
    
    public function page_entries() {
        $to_show = 1000;
        $to_cut = count(file($this->path)) - $to_show;
        $lines = file($this->path);
        if (count($lines) > $to_show) {
            $lines = array_splice($lines, $to_cut, $to_show);
        }
        $this->page_contents = implode("", $lines);

        // echo var_dump(count(file($this->path)));
    }

    public function remove_empty_lines() {
        $this->contents = preg_replace("/(^[\r\n]*|[\r\n]+)[\s\t]*[\r\n]+/", "\r\n", $this->contents);
        file_put_contents($this->path, $this->contents);
    }
}

$logs = new Log("logs");
// echo var_dump($logs);
?>
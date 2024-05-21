<?php
function update_session($context,$data){
   if (session_status() !== PHP_SESSION_ACTIVE) {
       session_start();
    }
 if( isset($_SESSION["progress"]) ){
    $_SESSION["progress"][$context] .= $_SESSION["progress"][$context] . "\n" . $data;
 }else{
    $_SESSION["progress"][$context] = $data;
 }
 session_write_close();
}

?>
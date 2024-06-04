<?php
function update_session($context,$data){
   if (session_status() !== PHP_SESSION_ACTIVE) {
       session_start();
    }
 if( isset($_SESSION["progress"]) ){
   if( !is_array($_SESSION["progress"][$context]) ){
      $_SESSION["progress"][$context] = [];
   }
   if( !in_array($data, $_SESSION["progress"][$context]) ){
     
      $_SESSION["progress"][$context][] = $data;
   }
 }else{
    $_SESSION["progress"][$context][] = $data;
 }
 session_write_close();
}

?>
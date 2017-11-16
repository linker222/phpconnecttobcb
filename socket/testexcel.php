<?php
    require_once("excel.php");

    $xls = new Excel('Report');

    $colors = array(
        array("red", "blue", "green", "yellow", "orange", "purple"),
        array('過以上','以','c','過','目錄','f')
    );

    foreach ($colors as $key => $value) {
        $xls->home();
        foreach ($value as  $row) {
          // $xls->home();
          $xls->label($row);
          $xls->right();
           // $xls->label($row);
        }
        $xls->down();
    }    
    ob_start();
    $xls->send();
    $data = ob_get_clean();
    file_put_contents(__DIR__ .'/report.xls', $data);
?>
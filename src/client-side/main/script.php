<?php
    // read from the body
    $inhanging = true;

    if($inhanging){
        while(1){
            echo "Hello World";
        }
        return;
    }
    $body = file_get_contents('php://input');
    
    // decode the body
    $body = json_decode($body, true);

    $number1 = $body['number1'];
    $number2 = $body['number2'];
    $result = $number1 + $number2;
    echo "Result is: $result";
?>
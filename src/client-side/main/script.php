<?php
    // set custom status code
    http_response_code(404);
    // tell the response that we're sending JSON
    header('Content-Type: application/json');
    // create a json
    $abahsine = json_encode(array('message' => 'Hello World!'), JSON_PRETTY_PRINT);
    // output the json
    echo $abahsine;
?>
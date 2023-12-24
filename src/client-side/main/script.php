<?php
    // set custom status code
    http_response_code(301);

    // tell the response that we're sending JSON
    header('Content-Type: application/json');

    // create a json
    $abahsine = "The beloved abahsine.";
    if (isset($_GET['key'])) {
        $abahsine = "The beloved abahsine says: " . $_GET['key'];
    }

    // output the json
    echo $abahsine;
?>
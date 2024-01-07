<?php

    if($_SERVER['REQUEST_METHOD'] == 'POST') {
        $body = file_get_contents('php://input');
        $body = json_decode($body, true);

        if(!isset($body['name'])) {
            http_response_code(400);
            echo "Can you specify your cool name in the body (as JSON) so I can welcome you?";
            return;
        }
        $name = $body['name'];
        echo "Welcome $name to our webserver!";
    } else if ($_SERVER['REQUEST_METHOD'] == 'GET') {
        echo "Welcome to our webserver!";
    }
    
?>
<?php
    header('Content-Type: application/json');
    $data = json_decode(file_get_contents('php://input'), true);
    if (!isset($data['username']) || !isset($data['password'])) {
        http_response_code(400);
        printfJSON(array('status' => false, 'message' => 'No arguments provided'));
        return;
    }
    $username = $data['username'];
    $password = $data['password'];

    if ($username != '1337' || $password != 'admin') {
        http_response_code(401);
        printfJSON(array('status' => false, 'message' => 'Invalid username or password'));
        return;
    }

    if(isset($_GET['sessionId'])) {
        header('SessionID: ' . $_GET['sessionId']);
    }
    
    http_response_code(200);
    printfJSON(array('status' => true, 'message' => 'Login success'));


    function printfJSON($array) {
        echo json_encode($array, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES);
    }
?>
<?php
    header('Content-Type: application/json');
    $data = json_decode(file_get_contents('php://input'), true);
    if (!isset($data['username']) || !isset($data['password'])) {
        http_response_code(400);
        printfJSON(array('status' => 'error', 'message' => 'No arguments provided'));
        return;
    }
    $username = $data['username'];
    $password = $data['password'];

    if ($username != 'admin' && $password != 'admin') {
        http_response_code(401);
        printfJSON(array('status' => 'error', 'message' => 'Invalid username or password'));
        return;
    }

    http_response_code(200);
    printfJSON(array('status' => 'success', 'message' => 'Login success'));
    function printfJSON($array) {
        echo json_encode($array, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES);
    }

?>
<?php
    header('Content-Type: application/json');
    $data = json_decode(file_get_contents('php://input'), true);
    if (!isset($_GET['sessionId'])) {
        http_response_code(400);
        printfJSON(array('status' => false, 'message' => 'No arguments provided'));
        return;
    }
    $sessionId = "sessionId=" . $_GET['sessionId'];
    $headerCookie = $_SERVER['HTTP_COOKIE'];

    if ($sessionId != $headerCookie) {
        http_response_code(401);
        $sessionsIDs = array('sessionId' => $sessionId, 'cookie' => $headerCookie);
        
        printfJSON(array('status' => false, 'message' => 'Invalid session id', 'sessionsIDs' => $sessionsIDs));
        return;
    }

    http_response_code(200);
    printfJSON(array('status' => true, 'message' => 'Logged in'));

    function printfJSON($array) {
        echo json_encode($array, JSON_PRETTY_PRINT | JSON_UNESCAPED_UNICODE | JSON_UNESCAPED_SLASHES);
    }
?>
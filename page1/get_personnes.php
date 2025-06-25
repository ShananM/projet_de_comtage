<?php
header('Content-Type: application/json');

$host = 'localhost';
$user = 'blaise';
$pass = 'Blaise1234';
$db = 'passage_personnes';

$conn = new mysqli($host, $user, $pass, $db);

if ($conn->connect_error) {
    http_response_code(500);
    echo json_encode(["error" => "Erreur de connexion"]);
    exit();
}

// Requête pour compter les passages (type_passage = 0)
$sql = "SELECT COUNT(*) AS total FROM passage WHERE type_passage = 0";
$result = $conn->query($sql);

if ($row = $result->fetch_assoc()) {
    echo json_encode(["total" => $row['total']]);
} else {
    echo json_encode(["total" => 0]);
}

$conn->close();
?>

<?php
header('Content-Type: application/json');

try {
    $pdo = new PDO('mysql:host=localhost;dbname=passage_personnes;charset=utf8', 'blaise', 'Blaise1234');
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    // On récupère tous les noms et niveaux de batterie
    $stmt = $pdo->query("SELECT nom, tension FROM tension");

    $data = [];

    while ($row = $stmt->fetch(PDO::FETCH_ASSOC)) {
        $data[$row['nom']] = $row['tension'];
    }

    echo json_encode($data);

} catch (PDOException $e) {
    echo json_encode(['error' => 'Erreur DB : ' . $e->getMessage()]);
}

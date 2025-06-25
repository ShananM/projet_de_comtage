<?php
try {
    $pdo = new PDO('mysql:host=localhost;dbname=passage_personnes;charset=utf8', 'blaise', 'Blaise1234', [
            PDO::ATTR_ERRMODE => PDO::ERRMODE_EXCEPTION,
            PDO::ATTR_DEFAULT_FETCH_MODE => PDO::FETCH_ASSOC
    ]);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);
    echo "✅ Connexion BDD réussie.\n";
} catch (PDOException $e) {
    die("❌ Erreur de connexion :" . $e->getMessage() . "\n");
}

?>

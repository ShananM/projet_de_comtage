<?php
session_start();
$identifiant_correct = "blaise"; // 🔒 À changer
$mot_de_passe_correct = "blaise1234"; // 🔒 À changer

// Si formulaire envoyé
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    // On sécurise un minimum l'entrée
    $username = isset($_POST['username']) ? trim($_POST['username']) : '';
    $mot_de_passe = isset($_POST['password']) ? trim($_POST['password']) : '';

    if ($mot_de_passe === $mot_de_passe_correct && $username === $identifiant_correct) {
        // Authentification réussie
        $_SESSION['auth'] = true;
        $_SESSION['username'] = $username; // Stocke le nom d'utilisateur dans la session

        echo "Authentification en cours...";

        // Redirection vers la page sécurisée
        header('Location:/page_de_reglage/page_reglages.html');
        echo "Authentification réussie !";
        exit;
    }
    } else {
        // Mot de passe incorrecto

        $erreur = "Mot de passe incorrect.";
        echo "erreur mdp ";
    }

?>
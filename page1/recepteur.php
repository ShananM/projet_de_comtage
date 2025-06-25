<?php
// --- Affichage des erreurs pour débogage ---
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

// --- Informations de connexion à la base de données ---
$host = 'localhost';          // Adresse du serveur MySQL
$user = 'blaise';             // Nom d'utilisateur MySQL
$pass = 'Blaise1234';         // Mot de passe MySQL
$db   = 'passage_personnes';  // Nom de la base de données

// --- Récupération du choix principal envoyé par le formulaire ---
$choix = $_POST['choix'] ?? '';  // Si rien n'est envoyé, on prend une chaîne vide

try {
    // --- Connexion à la base de données avec encodage UTF-8 ---
    $pdo = new PDO("mysql:host=$host;dbname=$db;charset=utf8", $user, $pass);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION); // Affiche les erreurs SQL

    // --- Sélection des enregistrements selon le type_passage choisi ---
    $stmt = $pdo->prepare("SELECT * FROM recepteur WHERE type_passage = :type");
    $stmt->execute(['type' => $choix]);
    $resultats = $stmt->fetchAll(PDO::FETCH_ASSOC); // On récupère tous les résultats dans un tableau associatif

    // --- Capteurs récupérés depuis le formulaire (via POST) ---
    $capteurs = [
        'capteurA_droite' => $_POST['capteurA_droite'] ?? '',
        'capteurA_gauche' => $_POST['capteurA_gauche'] ?? '',
        'capteurB_droite' => $_POST['capteurB_droite'] ?? '',
        'capteurB_gauche' => $_POST['capteurB_gauche'] ?? '',
        'capteurC_droite' => $_POST['capteurC_droite'] ?? '',
        'capteurC_gauche' => $_POST['capteurC_gauche'] ?? ''
    ];

    // --- Pour chaque capteur, si une valeur est présente, on met à jour la BDD ---
   foreach ($capteurs as $nomChamp => $valeur) {
    if ($valeur !== '') {
        $correspondance = [
            'Entrees' => 0,
            'Sorties' => 1,
        ];
        $type_passage = $correspondance[$valeur] ?? 0; // Valeur par défaut 0 si non trouvée
        // Vérifie que le nom du champ correspond bien à "capteurX_direction"
        if (preg_match('/capteur([ABC])_(droite|gauche)/', $nomChamp, $matches)) {
            $recepteur = $matches[1];
            $capteur   = $matches[2];

            // Mise à jour SQL
            $update = $pdo->prepare("UPDATE recepteur SET type_passage = :type_passage WHERE recepteur = :recepteur AND capteur = :capteur");
            $update->execute([
                'type_passage' => $type_passage,
                'recepteur' => $recepteur,
                'capteur' => $capteur
            ]);
        } else {
            // Optionnel : afficher une erreur si le nom ne correspond pas au format attendu
            echo "<p style='color:orange;'>Nom de champ inattendu : $nomChamp</p>";
        }
    }
}

} catch (PDOException $e) {
    // --- En cas d'erreur de connexion ou SQL ---
    echo "<p style='color:red;'>Erreur de connexion à la base de données : " . $e->getMessage() . "</p>";
    exit; // On arrête le script
}

// --- Affichage simple des données reçues ---
echo "<h2>Configuration des capteurs :</h2><ul>";
foreach ($capteurs as $nom => $valeur) {
    echo "<li><strong>" . htmlspecialchars($nom) . "</strong> : " . htmlspecialchars($valeur) . "</li>";
}
echo "</ul>";
?>


<!DOCTYPE html>
<html lang="fr">
<head>
  <meta charset="UTF-8">
  <title>Configuration des capteurs</title>
  <style>
    body {
      font-family: 'Segoe UI', Tahoma, sans-serif;
      background-color: #f0f4f8;
      margin: 0;
      padding: 40px;
    }

    h2 {
      text-align: center;
      color: #2c3e50;
      margin-bottom: 30px;
    }

    table {
      width: 80%;
      margin: auto;
      border-collapse: collapse;
      background-color: #ffffff;
      box-shadow: 0 4px 10px rgba(0, 0, 0, 0.1);
      border-radius: 10px;
      overflow: hidden;
    }

    th {
      background-color: #3498db;
      color: white;
      padding: 15px;
      font-size: 16px;
    }

    td {
      padding: 12px;
      border-top: 1px solid #eee;
      text-align: center;
    }

    tr:nth-child(even) {
      background-color: #f9f9f9;
    }

    tr:hover {
      background-color: #f1f1f1;
    }

    button {
      display: block;
      margin: 40px auto 0 auto;
      padding: 12px 30px;
      font-size: 16px;
      color: white;
      background-color: #3498db;
      border: none;
      border-radius: 5px;
      cursor: pointer;
      transition: background-color 0.3s ease;
    }

    button:hover {
      background-color: #2c80b4;
    }
  </style>
</head>
<body>

<?php
// --- Affichage des erreurs ---
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

// --- Connexion ---
$host = 'localhost';
$user = 'blaise';
$pass = 'Blaise1234';
$db   = 'passage_personnes';

$choix = $_POST['choix'] ?? '';

try {
    $pdo = new PDO("mysql:host=$host;dbname=$db;charset=utf8", $user, $pass);
    $pdo->setAttribute(PDO::ATTR_ERRMODE, PDO::ERRMODE_EXCEPTION);

    $stmt = $pdo->prepare("SELECT * FROM recepteur WHERE type_passage = :type");
    $stmt->execute(['type' => $choix]);
    $resultats = $stmt->fetchAll(PDO::FETCH_ASSOC);

    $capteurs = [
        'capteurA_droite' => $_POST['capteurA_droite'] ?? '',
        'capteurA_gauche' => $_POST['capteurA_gauche'] ?? '',
        'capteurB_droite' => $_POST['capteurB_droite'] ?? '',
        'capteurB_gauche' => $_POST['capteurB_gauche'] ?? '',
        'capteurC_droite' => $_POST['capteurC_droite'] ?? '',
        'capteurC_gauche' => $_POST['capteurC_gauche'] ?? ''
    ];

    foreach ($capteurs as $nomChamp => $valeur) {
        if ($valeur !== '') {
            $correspondance = [
                'Entrees' => 0,
                'Sorties' => 1,
            ];
            $type_passage = $correspondance[$valeur] ?? 0;

            if (preg_match('/capteur([ABC])_(droite|gauche)/', $nomChamp, $matches)) {
                $recepteur = $matches[1];
                $capteur   = $matches[2];

                $update = $pdo->prepare("UPDATE recepteur SET type_passage = :type_passage WHERE recepteur = :recepteur AND capteur = :capteur");
                $update->execute([
                    'type_passage' => $type_passage,
                    'recepteur' => $recepteur,
                    'capteur' => $capteur
                ]);
            }
        }
    }

} catch (PDOException $e) {
    echo "<p style='color:red;'>Erreur : " . $e->getMessage() . "</p>";
    exit;
}
?>

<h2>Configuration des capteurs</h2>
<table>
  <thead>
    <tr>
      <th>Capteur</th>
      <th>Gauche</th>
      <th>Droite</th>
    </tr>
  </thead>
  <tbody>
    <tr>
      <td>Capteur A</td>
      <td><?= htmlspecialchars($capteurs['capteurA_gauche']) ?></td>
      <td><?= htmlspecialchars($capteurs['capteurA_droite']) ?></td>
    </tr>
    <tr>
      <td>Capteur B</td>
      <td><?= htmlspecialchars($capteurs['capteurB_gauche']) ?></td>
      <td><?= htmlspecialchars($capteurs['capteurB_droite']) ?></td>
    </tr>
    <tr>
      <td>Capteur C</td>
      <td><?= htmlspecialchars($capteurs['capteurC_gauche']) ?></td>
      <td><?= htmlspecialchars($capteurs['capteurC_droite']) ?></td>
    </tr>
  </tbody>
</table>

<button onclick="window.history.back()">Retour</button>

</body>
</html>

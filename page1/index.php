<?php

include("connexionBDD.php");

// Commande d'abonnement MQTT
//$command = 'mosquitto_sub -h 192.168.1.21 -t "test"';

// Ouvre un flux vers la commande
$handle = popen($command, 'r');

echo "En attente de messages MQTT...\n";

while (!feof($handle)) {
    $line = fgets($handle);

    if ($line !== false) {
        $data = json_decode(trim($line), true);

        echo "Message brut : $line\n";
        if (json_last_error() === JSON_ERROR_NONE) {
            echo "Contenu JSON décodé :\n";
            print_r($data);
            $temps = $data['Temps'];
            $dates = $data['Date'];
            $jour = $data['Jour'];
            $passages = $data['passages'];
            $type_passages = $data['type_passage'];
            $cote = $data['cote'];
            $recepteur = $data['recepteur'];
          
            // Création du timestamp complet
            $timestamp = $dates . ' ' . $temps; // => "2025-06-23 14:30:00"

            // Vérification avec DateTime (facultatif mais conseillé)
            $dt = DateTime::createFromFormat('Y-m-d H:i:s', $timestamp);
            $mysqlTimestamp = $dt->format('Y-m-d H:i:s');   
            $test = "test";
        

          try {
            // Préparation
            $stmt = $pdo->prepare("
                INSERT INTO passage (id_cote, numero_recepteur, temps, type_passage)
                VALUES (:id_cote, :numero_recepteur, :temps, :type_passage)
            ");

            // Liaison des paramètres
            $stmt->bindParam(':id_cote', $test, PDO::PARAM_STR);
            $stmt->bindParam(':numero_recepteur', $recepteur, PDO::PARAM_STR);
            $stmt->bindParam(':temps', $timestamp, PDO::PARAM_STR);
            $stmt->bindParam(':type_passage', $type_passages, PDO::PARAM_STR);
            $stmt->execute();

              $lastid = $pdo->lastInsertId();

    // Étape 3 : génération de id_cote personnalisé
    $id_cote = $lastid . $cote; // attention, ici c’est une concaténation de chaînes

    // Étape 4 : mise à jour du champ id_cote
    $update = $pdo->prepare("
        UPDATE passage SET id_cote = :cote WHERE id = :vraiid
    ");

    $update->execute([
        ':cote' => $id_cote,
        ':vraiid' => $lastid
    ]);
             
            echo "✅ Insertion réussie\n";

        } catch (PDOException $e) {
            echo "❌ Erreur PDO : " . $e->getMessage() . "\n";
        }

        } else {
            echo "⚠️ Erreur JSON : " . json_last_error_msg() . "\n";
        }

    echo "-------------------------------\n";
}
}

pclose($handle);
?>

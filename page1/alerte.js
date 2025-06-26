const noms = ['R-A', 'R-B', 'R-C',]

  function verifierAlertesBatterie() {
    fetch('../alerte.php') // ⚠️ Adapte le chemin si besoin
      .then(response => {
        if (!response.ok) {
          throw new Error('Réponse réseau incorrecte');
        }
        return response.json();
      })
      .then(data => {
        const alertesRouge = [];
        const alertesOrange = [];

        noms.forEach(nom => {
          const niveauStr = data[nom];
          const niveau = parseFloat(niveauStr);

          if (!isNaN(niveau)) {
            if (niveau < 4.2) {
              alertesRouge.push(nom);
            } else if (niveau < 8.4) {
              alertesOrange.push(nom);
            }
          }
        });

        // Supprimer les anciennes alertes
        document.querySelectorAll('.alerte-batterie').forEach(el => el.remove());

        // Construire le message
        let message = '';
        if (alertesRouge.length > 0) {
          message += `🔴 Batterie CRITIQUE : ${alertesRouge.join(', ')}\n`;
        }
        if (alertesOrange.length > 0) {
          message += `🟠 Batterie FAIBLE : ${alertesOrange.join(', ')}`;
        }

        // Créer une nouvelle alerte s’il y a un message
        if (message) {
          const div = document.createElement('div');
          div.className = 'alerte-batterie';
          div.textContent = message;
          div.style.position = 'fixed';
          div.style.top = '0';
          div.style.left = '0';
          div.style.right = '0';
          div.style.padding = '15px';
          div.style.fontSize = '18px';
          div.style.fontWeight = 'bold';
          div.style.color = 'white';
          div.style.textAlign = 'center';
          div.style.zIndex = '9999';
          div.style.whiteSpace = 'pre-line';
          div.style.backgroundColor = alertesRouge.length > 0 ? '#f44336' : '#ff9800';
          document.body.appendChild(div);
        }
      })
      .catch(error => {
        console.error('Erreur lors de la récupération des données batterie :', error);
      })
  };

  // Appel immédiat + mise à jour régulière
  verifierAlertesBatterie();
  setInterval(verifierAlertesBatterie, 10000);


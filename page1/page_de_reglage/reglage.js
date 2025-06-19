function goToIP(inputId) {
    const ip = document.getElementById(inputId).value.trim();
    if (ip && ip !== "#") {
        if (!ip.startsWith("http://") && !ip.startsWith("https://")) {
            window.location.href = "http://" + ip;
        } else {
            window.location.href = ip;
        }
    } else {
        alert("Veuillez entrer une adresse IP valide.");
    }
}

function submitConfig() {
    const ssid = document.getElementById("user_login").value;
    const pwd = document.getElementById("user_mdp").value;
    const mode = document.getElementById("ip-mode").value;
    const emissionMode = document.querySelector('input[name="emission"]:checked')?.value || "non sélectionné";

    let message = `SSID : ${ssid}\nMot de passe : ${pwd}\nMode IP : ${mode}\nMode d’émission : ${emissionMode}`;

    if (mode === "static") {
        const ip = document.getElementById("static-ip").value;
        const mask = document.getElementById("subnet-mask").value;
        const gateway = document.getElementById("gateway").value;
        message += `\nAdresse IP : ${ip}\nMasque : ${mask}\nPasserelle : ${gateway}`;
    }

    alert(message);
}


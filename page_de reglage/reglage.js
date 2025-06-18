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

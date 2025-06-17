const ctx = document.getElementById('presenceChart').getContext('2d');

const chart = new Chart(ctx, {
    type: 'bar',
    data: {
        labels: ['Entrées', 'Sorties', 'Présents'],
        datasets: [{
            label: 'Nombre de personnes',
            data: [0, 0, 0],
            backgroundColor: ['green', 'red', 'blue']
        }]
    },
    options: {
        responsive: true,
        plugins: {
            legend: { display: false },
            title: {
                display: true,
                text: 'Statistiques de présence'
            }
        },
        scales: {
            y: {
                beginAtZero: true,
                ticks: {
                    stepSize: 100
                }
            }
        }
    }
});
document.getElementById('updateButton').addEventListener('click', () => {
    const newEntries = parseInt(document.getElementById('newEntries').value) || 0;
    const newExits = parseInt(document.getElementById('newExits').value) || 0;

    // Update the chart data
    chart.data.datasets[0].data[0] += newEntries; // Entrées
    chart.data.datasets[0].data[1] += newExits; // Sorties
    chart.data.datasets[0].data[2] = chart.data.datasets[0].data[0] - chart.data.datasets[0].data[1]; // Présents

    // Update the chart
    chart.update();
});
document.getElementById('resetButton').addEventListener('click', () => {
    // Reset the chart data
    chart.data.datasets[0].data = [830, 0, 830];
    
    // Update the chart
    chart.update();
});
// Add event listener for the reset button
document.getElementById('resetButton').addEventListener('click', () => {
    // Reset the chart data
    chart.data.datasets[0].data = [830, 0, 830];

    // Update the chart
    chart.update();
});
// Add event listener for the update button
document.getElementById('updateButton').addEventListener('click', () => {
    const newEntries = parseInt(document.getElementById('newEntries').value) || 0;
    const newExits = parseInt(document.getElementById('newExits').value) || 0;

    // Update the chart data
    chart.data.datasets[0].data[0] += newEntries; // Entrées
    chart.data.datasets[0].data[1] += newExits; // Sorties
    chart.data.datasets[0].data[2] = chart.data.datasets[0].data[0] - chart.data.datasets[0].data[1]; // Présents

    // Update the chart
    chart.update();
});

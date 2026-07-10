
const API_URL = "http://localhost:3000";

let chartInstance = null;

// ===========================
// Enviar comandos
// ===========================
function enviarComando(rota) {

    fetch(`${API_URL}/${rota}`, {
        method: 'POST'
    })
    .then(res => res.json())
    .then(data => {

        console.log("Comando enviado:", data);

        buscarDados();

    })
    .catch(err => {

        console.error("Erro ao enviar comando:", err);

    });
}

// ===========================
// Buscar dados
// ===========================
function buscarDados() {

    fetch(`${API_URL}/dados`)
    .then(res => res.json())
    .then(dados => {

        if (!dados || dados.length === 0) {
            return;
        }

        const atual = dados[0];

        console.log(atual);

        // ===========================
        // Temperatura
        // ===========================
        document.getElementById("txtTemp").innerText =
            `${atual.temperatura.toFixed(1)} °C`;

        // ===========================
        // Luminosidade
        // ===========================
        document.getElementById("txtLuz").innerText =
            `${atual.luminosidade}%`;

        // ===========================
        // Porta
        // ===========================
        if (atual.porta) {

            document.getElementById("txtPorta").innerText = "ABERTA";

            document.getElementById("cardPorta").className =
                "card card-status bg-danger-subtle text-danger p-3 d-flex flex-row align-items-center justify-content-between";

            document.getElementById("iconPorta").className =
                "fa-solid fa-door-open text-danger icon-size";

        } else {

            document.getElementById("txtPorta").innerText = "FECHADA";

            document.getElementById("cardPorta").className =
                "card card-status bg-white p-3 d-flex flex-row align-items-center justify-content-between";

            document.getElementById("iconPorta").className =
                "fa-solid fa-door-closed text-secondary icon-size";
        }

        // ===========================
        // Status AC
        // ===========================
        document.getElementById("statusAC").innerText =
            atual.ac ? "ON" : "OFF";

        document.getElementById("statusAC").className =
            atual.ac
                ? "badge bg-success float-end"
                : "badge bg-secondary float-end";

        // ===========================
        // Status Luz
        // ===========================
        document.getElementById("statusLuz").innerText =
            atual.luz ? "ON" : "OFF";

        document.getElementById("statusLuz").className =
            atual.luz
                ? "badge bg-success float-end"
                : "badge bg-secondary float-end";

        // ===========================
        // Modo
        // ===========================
        if (atual.modoManual) {

            document.getElementById("badgeModo").innerText =
                "MODO MANUAL ATIVO";

            document.getElementById("badgeModo").className =
                "badge bg-warning text-dark p-2 fs-6 fw-bold";

        } else {

            document.getElementById("badgeModo").innerText =
                "MODO AUTOMÁTICO";

            document.getElementById("badgeModo").className =
                "badge bg-success p-2 fs-6 fw-bold";
        }

        // ===========================
        // Dados gráfico
        // ===========================
        const dadosGrafico = [...dados].reverse();

        const labels = dadosGrafico.map(d =>
            new Date(d.data).toLocaleTimeString('pt-BR', {
                hour: '2-digit',
                minute: '2-digit',
                second: '2-digit'
            })
        );

        const temperaturas = dadosGrafico.map(d => d.temperatura);

        renderizarGrafico(labels, temperaturas);

    })
    .catch(err => {

        console.error("Erro ao buscar dados:", err);

    });
}

// ===========================
// Renderizar gráfico
// ===========================
function renderizarGrafico(labels, dados) {

    const ctx = document
        .getElementById('graficoTemperatura')
        .getContext('2d');

    if (chartInstance) {

        chartInstance.data.labels = labels;

        chartInstance.data.datasets[0].data = dados;

        chartInstance.update('none');

    } else {

        chartInstance = new Chart(ctx, {

            type: 'line',

            data: {
                labels: labels,

                datasets: [{
                    label: 'Temperatura (°C)',
                    data: dados,
                    borderColor: '#dc3545',
                    backgroundColor: 'rgba(220, 53, 69, 0.1)',
                    borderWidth: 3,
                    tension: 0.3,
                    fill: true
                }]
            },

            options: {
                responsive: true,
                maintainAspectRatio: false,

                scales: {

                    y: {
                        min: 15,
                        max: 40,

                        ticks: {
                            stepSize: 5
                        },

                        grid: {
                            color: '#eaeaea'
                        }
                    },

                    x: {
                        grid: {
                            display: false
                        }
                    }
                }
            }
        });
    }
}

// ===========================
// Inicialização
// ===========================

// Primeira busca ao carregar a página
buscarDados();

// ===========================
// WebSocket Socket.IO
// ===========================
const socket = io("http://localhost:3000");

// Atualização instantânea
socket.on("atualizacaoSensores", (dadoNovo) => {

    console.log("Chegou dado instantâneo!", dadoNovo);

    buscarDados();
});

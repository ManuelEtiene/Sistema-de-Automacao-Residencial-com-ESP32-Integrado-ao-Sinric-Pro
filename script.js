/* Script preparado para futura atualização */

document.getElementById("updateButton")
.addEventListener("click", atualizarDados);



 /*Função que gera dados fictícios*/

function atualizarDados() {
    

    /*Temperatura fictícia (20 a 30 °C) */
    let temp = (20 + Math.random() * 10).toFixed(1);

     /*Umidade fictícia (40 a 80 %) */  
    let hum = (40 + Math.random() * 40).toFixed(0);

     /*Estado fictício da lâmpada*/
    let lampOn = Math.random() > 0.5;

     /*Estado fictício da porta*/
    let doorOpen = Math.random() > 0.5;


     /*Atualizando temperatura*/    

    document.getElementById("temperature")
        .innerText = temp + " °C";


     /*Atualizando umidade */ 

    document.getElementById("humidity")
        .innerText = hum + " %";


     /*Atualizando lâmpada */ 

    let lamp = document.getElementById("lampStatus");

    if (lampOn) {
        lamp.innerText = "Ligada";
        lamp.className = "status-on";
    }
    else {
        lamp.innerText = "Desligada";
        lamp.className = "status-off";
    }


     /*Atualizando porta*/

    let door = document.getElementById("doorStatus");

    if (doorOpen) {
        door.innerText = "Aberta";
        door.className = "status-on";
    }
    else {
        door.innerText = "Fechada";
        door.className = "status-off";
    }

}


/* Atualiza automaticamente ao abrir */


atualizarDados();
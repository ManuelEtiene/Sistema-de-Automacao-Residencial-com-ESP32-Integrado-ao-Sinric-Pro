require("dotenv").config();

const express = require("express");
const mongoose = require("mongoose");
const mqtt = require("mqtt");
const cors = require("cors");
const http = require("http"); 
const { Server } = require("socket.io");
const SensorData = require("./sensorData");

const app = express();
const server = http.createServer(app);
const io = new Server(server, { cors: { origin: "*" } });
app.use(cors());
app.use(express.json());


// ======================
// MONGODB
// ======================

mongoose.connect(process.env.MONGO_URI)
.then(() => {
  console.log("MongoDB conectado");
})
.catch((err) => {
  console.log(err);
});


// ======================
// MQTT
// ======================

const client = mqtt.connect("mqtt://broker.hivemq.com:1883");

client.on("connect", () => {

  console.log("MQTT conectado");

  client.subscribe("mamadjam123/iot/sensores");   

});


// ======================
// RECEBER DADOS ESP32
// ======================

client.on("message", async (topic, message) => {

  try {

    const dados = JSON.parse(message.toString());

    console.log(dados);

    await SensorData.create({

      temperatura: dados.temperatura,
      luminosidade: dados.luminosidade,
      porta: dados.porta,
      ac: dados.ac,
      modoManual: dados.modoManual,
      luz: dados.luz

    });
    io.emit("atualizacaoSensores", dados);

  } catch (err) {

    console.log(err);

  }

});


// ======================
// API
// ======================

// últimos dados
app.get("/dados", async (req, res) => {

  try {

    const dados = await SensorData
      .find()
      .sort({ data: -1 })
      .limit(20);

    res.json(dados);

  } catch (err) {

    res.status(500).json(err);

  }

});


// ligar AC
app.post("/ac/on", (req, res) => {

  client.publish("mamadjam123/iot/comandos", "AC_ON");

  res.json({
    mensagem: "AC ligado"
  });

});


// desligar AC
app.post("/ac/off", (req, res) => {

  client.publish("mamadjam123/iot/comandos", "AC_OFF");

  res.json({
    mensagem: "AC desligado"
  });

});


// ligar luz
app.post("/luz/on", (req, res) => {

  client.publish("mamadjam123/iot/comandos", "LUZ_ON");

  res.json({
    mensagem: "Luz ligada"
  });

});


// desligar luz
app.post("/luz/off", (req, res) => {

  client.publish("mamadjam123/iot/comandos", "LUZ_OFF");

  res.json({
    mensagem: "Luz desligada"
  });

});

// ======================
// API - ATIVAR MODO AUTOMÁTICO
// ======================

app.post("/auto/on", (req, res) => {

  // Envia o comando que o ESP32 está à espera para desligar o modo manual
  client.publish("mamadjam123/iot/comandos", "AUTO_ON");

  res.json({
    mensagem: "Modo automático ativado com sucesso"
  });

});

server.listen(process.env.PORT, () => {
  console.log(`Servidor rodando na porta ${process.env.PORT} com WebSockets ativos!`);
});
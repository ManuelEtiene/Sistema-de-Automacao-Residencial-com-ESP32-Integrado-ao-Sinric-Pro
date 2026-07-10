  const mongoose = require("mongoose");

  const SensorSchema = new mongoose.Schema({

    temperatura: Number,

    luminosidade: Number,

    porta: Boolean,

    ac: Boolean,
    
    modoManual: Boolean,

    luz: Boolean,

    data: {
      type: Date,
      default: Date.now
    }

  });

  module.exports = mongoose.model("SensorData", SensorSchema);
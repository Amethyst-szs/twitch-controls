module.exports = {
  Ampli: function (Amplitude, canBeNegative) {
    if (canBeNegative) {
      return Math.random() * (Amplitude * 2) - Amplitude;
    } else {
      return Math.random() * Amplitude;
    }
  },
};

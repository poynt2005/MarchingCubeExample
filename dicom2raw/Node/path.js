var path = require("path");

var DicomRawConvAPILIB = path
  .resolve("../DicomRawConverterAPI.lib")
  .replace(/\\/gm, "\\\\");

module.exports = {
  DicomRawConvAPILIB: DicomRawConvAPILIB,
};

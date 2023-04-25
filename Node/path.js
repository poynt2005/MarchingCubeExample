var path = require("path");

var MarchingCubeAPILIB = path
  .resolve("../MarchingCubeAPI.lib")
  .replace(/\\/gm, "\\\\");

var MarchingCubeDrawlerAPILIB = path
  .resolve("../DrawlerAPI.lib")
  .replace(/\\/gm, "\\\\");

module.exports = {
  MarchingCubeAPILIB: MarchingCubeAPILIB,
  MarchingCubeDrawlerAPILIB: MarchingCubeDrawlerAPILIB,
};

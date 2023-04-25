var MarchingCube = require("./MarchingCube");

var rawFilename = "../ABC_64_64_64.raw";
var rawDim = MarchingCube.ParseFileName(rawFilename);
var isoValue = 10;

console.log(
  "Filename: %s, width: %d, height: %d, depth: %d",
  rawFilename,
  rawDim.width,
  rawDim.height,
  rawDim.depth
);

var mc = new MarchingCube(rawFilename, rawDim);

console.log("Start marching with isovalue: %d", isoValue);
mc.March(isoValue);

// console.log("Current triangle with normalized coordinates:");
var ndcMesh = mc.GetCurrentMesh(true);
// // console.log(normMesh);

// console.log("Current mesh normal");
// var normMesh = MarchingCube.GetMeshNormal(ndcMesh);
// console.log(normMesh);

// console.log(ndcMesh.length, normMesh.length);

console.log("Write mesh to Wavefront obj");
mc.WriteCurrentMeshToObj("output.obj");

console.log("Constructing canvas...");
mc.CreateDrawler();

console.log("Calculate normal vector");
mc.CalculateNorm();

var rotateAngle = 0;
mc.SetColor(0.1, 0.2, 0.7);

mc.SetLightPosition(0.5, 0.5, 0.5);
mc.SetLightAmbiet(0.1, 0.6, 0.3);
mc.SetLightDiffuse(0.5, 0.2, 0.3);

mc.SetMaterialAmbiet(0.1, 0.1, 0.8);
mc.SetMaterialDiffuse(0.5, 0.5, 0.5);

var interval = setInterval(function () {
  if (mc.CheckRenderClose()) {
    console.log("Render closing...");
    clearInterval(interval);

    mc.ReleaseDrawler();
    mc.ReleaseMarchingCubeInstance();

    return;
  }

  if (rotateAngle >= 360) {
    rotateAngle = 0;
  }

  mc.SetRotate(rotateAngle, "x");

  rotateAngle += 1;
  mc.RenderFrame();
}, 10);

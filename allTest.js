var DicomRawConverter = require("./DicomRawConverter");
var MarchingCube = require("./MarchingCube");

console.log("Create Dicom Raw Converter");
var drc = new DicomRawConverter("sample_dicom", ".+.dcm");

console.log("Dicom counts: %d", drc.GetDicomCounts());

var numberPattern = "[0-9]+";
console.log("Sorting Dicom files by pattern: %s...", numberPattern);
drc.SortDicomFile(numberPattern, 1);

console.log("Start building raw file...");
var isBuildingSuccess = drc.Build();
console.log("Is building success: %s", isBuildingSuccess);

if (!isBuildingSuccess) {
  console.log("Build raw failed, exiting...");
  process.exit();
}

var order = 5;

console.log("Get dicom buffer sequence by order: %d", order);
var buffer = drc.GetDicomBufferSequential(order);
console.log(buffer);

console.log("Get dicom filename by order: %d", order);
var filename = drc.GetDicomNameSequential(order);
console.log(filename);

console.log("Show dicom filename by order: %d", order);
drc.ShowDicomBufferSequential(order);

var rawBuffer = drc.GetRawData();
console.log("Get raw data buffer with size: %d", rawBuffer.length);
console.log(rawBuffer);

var dimension = drc.GetRawDimension();
console.log(
  "Get raw data dimension: %dx%dx%d",
  dimension.width,
  dimension.height,
  dimension.depth
);

var outFilename =
  "ABC_" +
  dimension.width +
  "_" +
  dimension.height +
  "_" +
  dimension.depth +
  ".raw";
console.log("Write raw file to: %s", outFilename);
drc.WriteToRawFile(outFilename);

var bokenLayer = drc.GetBrokenLayer();
console.log("Get broken layer with count: %d", bokenLayer.length);
console.log(bokenLayer);

console.log("Release converter instance");
drc.ReleaseDicomRawConverter();

var isoValue = 100;

var rawDim = dimension;
var mc = new MarchingCube(rawBuffer, rawDim);

console.log("Start marching with isovalue: %d", isoValue);
mc.March(isoValue);

var ndcMesh = mc.GetCurrentMesh(true);

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

  mc.SetRotate(rotateAngle, "y");

  rotateAngle += 1;
  mc.RenderFrame();
}, 10);

console.log("end");

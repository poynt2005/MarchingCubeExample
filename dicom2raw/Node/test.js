var DicomRawConverter = require("./DicomRawConverter");

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

console.log("end");

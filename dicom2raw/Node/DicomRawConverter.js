var path = require("path");
var fs = require("fs");

/** Custom Error: RuntimeError */
function RuntimeError(message) {
  this.name = "RuntimeError";
  this.message = message || "Error running the current script";
  this.stack = new Error().stack;
}

var _f = function () {};
_f.prototype = Error.prototype;
RuntimeError.prototype = new _f();
RuntimeError.prototype.constructor = RuntimeError;
/** */

var findRuntimeDll = function () {
  var dlls = [
    "DicomRawConverterAPI.dll",
    "gdcmcharls.dll",
    "gdcmCommon.dll",
    "gdcmDICT.dll",
    "gdcmDSED.dll",
    "gdcmexpat.dll",
    "gdcmgetopt.dll",
    "gdcmIOD.dll",
    "gdcmjpeg12.dll",
    "gdcmjpeg16.dll",
    "gdcmjpeg8.dll",
    "gdcmmd5.dll",
    "gdcmMEXD.dll",
    "gdcmMSFF.dll",
    "gdcmopenjp2.dll",
    "gdcmzlib.dll",
    "msvcp120.dll",
    "msvcr120.dll",
    "opencv_world470.dll",
    "socketxx.dll",
  ];
  dlls.forEach(function (dll) {
    if (!fs.existsSync(dll)) {
      throw new RuntimeError("dependice DLL: " + dll + " not found");
    }
  });
};

findRuntimeDll();

var nativeBinding = require("./DicomRawConverter.node");
var privateMap = new WeakMap();

/**
 * Describe the dimension of a raw file
 * @typedef {Object} Dimension
 * @property {number} width -  width of raw file
 * @property {number} height -  height of raw file
 * @property {number} depth -  depth of raw file
 */

/**
 * Creates a Dicom sequence files to Raw file Converter
 * @class
 * @param {string} parentDirectory - The parent directory path which contains dicom sequence files
 * @param {string} dicomSearchPattern - The Regular expression string you want to identify the dicom filename
 */
function DicomRawConverter(parentDirectory, dicomSearchPattern) {
  /**
   * The privateVariable stores the private variable of DicomRawConverter instance
   * @memberof MarchingCube
   */
  var privateVariable = {
    /**
     * parentDirectory is a directory path contains dicom sequence files
     * @memberof DicomRawConverter
     * @type {string}
     * @private
     */
    parentDirectory: parentDirectory,

    /**
     * dicomSearchPattern is a regex string you want to identify the dicom filename
     * @memberof DicomRawConverter
     * @type {string}
     * @private
     */
    dicomSearchPattern: dicomSearchPattern,

    /**
     * converterHandle is the handle point to DicomRawConverter instance
     * @memberof DicomRawConverter
     * @type {string}
     * @private
     */
    converterHandle: "0",

    /**
     * isConverterRelease indicates whether the DicomRawConverter instance is released
     * @memberof DicomRawConverter
     * @type {boolean}
     * @private
     */
    isConverterRelease: false,
  };

  privateVariable.converterHandle = nativeBinding.CreateDicomRawConverter(
    parentDirectory,
    dicomSearchPattern
  );

  privateMap.set(this, privateVariable);
}

/**
 * This method releases the DicomRawConverter instance
 * it cannot be called after the instance is released,
 * NOTE: You must check every of dicom sequence have same width and height
 * @memberof DicomRawConverter
 */
DicomRawConverter.prototype.ReleaseDicomRawConverter = function () {
  var privateVariable = privateMap.get(this);

  if (
    !nativeBinding.CheckDicomRawConverterExists(privateVariable.converterHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isConverterRelease) {
    throw new Error("Handle of current instance has been released");
  }

  nativeBinding.ReleaseDicomRawConverter(privateVariable.converterHandle);

  privateVariable.converterHandle = "0";
  privateVariable.isConverterRelease = true;
};

/**
 * This method set the sorting rule of the dicom sequence,
 * Usually the dicom file will have a number to help identify the order of the sequence,
 * you have to select this number and sort it,
 * otherwise the Z-axis of the volume data will be misaligned.
 * @memberof DicomRawConverter
 * @param {string} numberPattern - The Regular expression of dicom serial number, this pattern MUST BE selected number correctly
 * @param {number} patternGroupOrder - The matched order of the regex result
 */
DicomRawConverter.prototype.SortDicomFile = function (
  numberPattern,
  patternGroupOrder
) {
  var privateVariable = privateMap.get(this);
  if (
    !nativeBinding.CheckDicomRawConverterExists(privateVariable.converterHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isConverterRelease) {
    throw new Error("Handle of current instance has been released");
  }

  var testStr = "12345";

  if (!new RegExp(numberPattern).test(testStr)) {
    throw new Error("Search pattern invalid, it cannot select any number");
  }

  nativeBinding.SortDicomFile(
    privateVariable.converterHandle,
    numberPattern,
    patternGroupOrder
  );
};

/**
 * Build the raw volume data by current dicom sequence,
 * you have to do the sorting method before, while the operation may still succeed,
 * but the output raw data might be incorrect,
 * if any of the images has a different dimension, the operation will fail.
 * @memberof DicomRawConverter
 * @param {boolean} [isOpenCV=false] - (Optional, default false) Use OpenCV to do the image processing(erode, sharpen, blur...)
 * @returns {boolean} - Check whether the building operation is failed or not, if success, returns true
 */
DicomRawConverter.prototype.Build = function (isOpenCV) {
  var privateVariable = privateMap.get(this);
  if (
    !nativeBinding.CheckDicomRawConverterExists(privateVariable.converterHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isConverterRelease) {
    throw new Error("Handle of current instance has been released");
  }

  isOpenCV = isOpenCV || false;
  return nativeBinding.Build(privateVariable.converterHandle, isOpenCV);
};

/**
 * Get the specific Dicom image buffer by given order
 * You MUST call this method AFTER you do the build operation, or the return buffer will be empty
 * @memberof DicomRawConverter
 * @param {number} order - The specific dicom sequence order
 * @returns {Buffer} - Dicom image buffer
 */
DicomRawConverter.prototype.GetDicomBufferSequential = function (order) {
  var privateVariable = privateMap.get(this);
  if (
    !nativeBinding.CheckDicomRawConverterExists(privateVariable.converterHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isConverterRelease) {
    throw new Error("Handle of current instance has been released");
  }

  return nativeBinding.GetDicomBufferSequential(
    privateVariable.converterHandle,
    order
  );
};

/**
 * Get the specific Dicom image filename by given order
 * @memberof DicomRawConverter
 * @param {number} order - The specific dicom sequence order
 * @returns {string} - Dicom image filename
 */
DicomRawConverter.prototype.GetDicomNameSequential = function (order) {
  var privateVariable = privateMap.get(this);
  if (
    !nativeBinding.CheckDicomRawConverterExists(privateVariable.converterHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isConverterRelease) {
    throw new Error("Handle of current instance has been released");
  }

  return nativeBinding.GetDicomNameSequential(
    privateVariable.converterHandle,
    order
  );
};

/**
 * Show the specific Dicom image buffer by given order by OpenCV builtin imshow function
 * You MUST call this method AFTER you do the build operation, or this operation won't do anything
 * @memberof DicomRawConverter
 * @param {number} order - The specific dicom sequence order
 */
DicomRawConverter.prototype.ShowDicomBufferSequential = function (order) {
  var privateVariable = privateMap.get(this);
  if (
    !nativeBinding.CheckDicomRawConverterExists(privateVariable.converterHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isConverterRelease) {
    throw new Error("Handle of current instance has been released");
  }

  return nativeBinding.ShowDicomBufferSequential(
    privateVariable.converterHandle,
    order
  );
};

/**
 * This method gives the raw file dimension
 * You MUST call this method AFTER you do the build operation, or the return result might incorrect
 * @memberof DicomRawConverter
 * @returns {Dimension} - The raw file dimension
 */
DicomRawConverter.prototype.GetRawDimension = function () {
  var privateVariable = privateMap.get(this);
  if (
    !nativeBinding.CheckDicomRawConverterExists(privateVariable.converterHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isConverterRelease) {
    throw new Error("Handle of current instance has been released");
  }
  return nativeBinding.GetRawDimension(privateVariable.converterHandle);
};

/**
 * This method return how many dicom files in this sequence selected by the given search pattern,
 * you can also check this value from Dimension.depth by calling method "GetRawDimension"
 * @memberof DicomRawConverter
 * @returns {number} - the counts of Dicom files
 */
DicomRawConverter.prototype.GetDicomCounts = function () {
  var privateVariable = privateMap.get(this);
  if (
    !nativeBinding.CheckDicomRawConverterExists(privateVariable.converterHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isConverterRelease) {
    throw new Error("Handle of current instance has been released");
  }

  return nativeBinding.GetDicomCounts(privateVariable.converterHandle);
};

/**
 * This method writes the raw file to disk
 * You MUST call this method AFTER you do the build operation, or it won't do anything
 * @memberof DicomRawConverter
 * @param {string} outRawFilename - The output raw filename you want to write to disk
 */
DicomRawConverter.prototype.WriteToRawFile = function (outRawFilename) {
  var privateVariable = privateMap.get(this);
  if (
    !nativeBinding.CheckDicomRawConverterExists(privateVariable.converterHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isConverterRelease) {
    throw new Error("Handle of current instance has been released");
  }

  nativeBinding.WriteToRawFile(privateVariable.converterHandle, outRawFilename);
};

/**
 * Get the raw file buffer
 * You MUST call this method AFTER you do the build operation, or the return buffer will be empty
 * @memberof DicomRawConverter
 * @returns {Buffer} - The output raw data buffer
 */
DicomRawConverter.prototype.GetRawData = function () {
  var privateVariable = privateMap.get(this);
  if (
    !nativeBinding.CheckDicomRawConverterExists(privateVariable.converterHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isConverterRelease) {
    throw new Error("Handle of current instance has been released");
  }

  return nativeBinding.GetRawData(privateVariable.converterHandle);
};

/**
 * When any of the dicom layer builds are wrong,
 * which wrong layers can be obtained using this method
 * @memberof DicomRawConverter
 * @returns {Array<string>} - layers which are incorrect
 */
DicomRawConverter.prototype.GetBrokenLayer = function () {
  var privateVariable = privateMap.get(this);
  if (
    !nativeBinding.CheckDicomRawConverterExists(privateVariable.converterHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isConverterRelease) {
    throw new Error("Handle of current instance has been released");
  }

  return nativeBinding.GetBrokenLayer(privateVariable.converterHandle);
};

module.exports = DicomRawConverter;

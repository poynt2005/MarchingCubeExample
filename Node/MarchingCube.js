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
    "MarchingCube.node",
    "MarchingCubeAPI.dll",
    "DrawlerAPI.dll",
    "freeglut.dll",
    "glfw3.dll",
  ];
  dlls.forEach(function (dll) {
    if (!fs.existsSync(dll)) {
      throw new RuntimeError("dependice DLL: " + dll + " not found");
    }
  });
};

findRuntimeDll();

var nativeBinding = require("./MarchingCube.node");

var privateMap = new WeakMap();

/**
 * Describe a coordinate floating point of a vertex set
 * @typedef {Object} fPoint
 * @property {number} x - x coordinate
 * @property {number} y - y coordinate
 * @property {number} z - z coordinate
 */

/**
 * Describe a vertices set of a Triangle
 * @typedef {Object} Triangle
 * @property {fPoint} v0 - Point of vertex 0
 * @property {fPoint} v1 - Point of vertex 1
 * @property {fPoint} v2 - Point of vertex 2
 */

/**
 * Describe the dimension of a raw file
 * @typedef {Object} Dimension
 * @property {number} width -  width of raw file
 * @property {number} height -  height of raw file
 * @property {number} depth -  depth of raw file
 */

/**
 * Creates a Marching cubes algorithm instance
 * @class
 * @param {string|Buffer} sourceRAWFile - The source RAW file, if given parameter is string, it will read the given string as filename and read the raw file from disk, otherwise, if the given parameter is a buffer, it will read the raw file from given buffer
 * @param {Dimension} dimension - The dimension structure of RAW file
 */
function MarchingCube(sourceRAWFile, dimension) {
  /**
   * The privateVariable stores the private variable of marching cubes instance
   * @memberof MarchingCube
   */
  var privateVariable = {
    /**
     * dimension indicates the dimension structure of RAW file
     * @memberof MarchingCube
     * @type {string}
     * @private
     */
    dimension: dimension,

    /**
     * marchingCubeHandle is the handle point to Marching cube instance
     * @memberof MarchingCube
     * @type {string}
     * @private
     */
    marchingCubeHandle: "0",

    /**
     * isMCRelease indicates that whether the Marching Cubes instance has been released
     * @memberof MarchingCube
     * @type {boolean}
     * @private
     */
    isMCRelease: false,

    /**
     * isMarchCalled indicates that the "March" method has been called
     * so that we can get the mesh triangles to render
     * @memberof MarchingCube
     * @type {boolean}
     * @private
     */
    isMarchCalled: false,

    /**
     * drawlerHandle holds the handle point to Drawler instance
     * @memberof MarchingCube
     * @type {string}
     * @private
     */
    drawlerHandle: "0",

    /**
     * isDRRelease indicates that the handle point to Draler instance is released or not
     * @memberof MarchingCube
     * @type {string}
     * @private
     */
    isDRRelease: false,
  };

  privateVariable.marchingCubeHandle = nativeBinding.CreateMarchingCubeInstance(
    sourceRAWFile,
    dimension
  );

  privateMap.set(this, privateVariable);
}

/**
 * This method releases the marching cubes instance
 * it cannot be called after the instance is released
 * @memberof MarchingCube
 */
MarchingCube.prototype.ReleaseMarchingCubeInstance = function () {
  var privateVariable = privateMap.get(this);

  if (
    !nativeBinding.CheckIsMCInstanceExists(privateVariable.marchingCubeHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isMCRelease) {
    throw new Error("Handle of current instance has been released");
  }

  nativeBinding.ReleaseMarchingCubeInstance(privateVariable.marchingCubeHandle);
  privateVariable.isMCRelease = true;
  privateVariable.marchingCubeHandle = "0";
};

/**
 * March the volume to construct the 3d surfaces
 * @memberof MarchingCube
 * @param {number} isoValue - The isovalue use to march
 */
MarchingCube.prototype.March = function (isoValue) {
  var privateVariable = privateMap.get(this);
  if (
    !nativeBinding.CheckIsMCInstanceExists(privateVariable.marchingCubeHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isMCRelease) {
    throw new Error("Handle of current instance has been released");
  }

  if (isoValue < 0 || isoValue > 255) {
    throw new TypeError("Isovalue cannot be greater than 255 or negative");
  }

  nativeBinding.March(privateVariable.marchingCubeHandle, isoValue);
  privateVariable.isMarchCalled = true;
};

/**
 * Get the current mesh by marching the raw volumn by given isovalue
 * @memberof MarchingCube
 * @param {boolean} isNormalized - To get the normalized coordinates of the mesh or not
 * @returns {Triangle} - Mesh triangles generated by marching cubes algorithm
 */
MarchingCube.prototype.GetCurrentMesh = function (isNormalized) {
  var privateVariable = privateMap.get(this);
  if (
    !nativeBinding.CheckIsMCInstanceExists(privateVariable.marchingCubeHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isMCRelease) {
    throw new Error("Handle of current instance has been released");
  }

  return nativeBinding.GetCurrentMesh(
    privateVariable.marchingCubeHandle,
    isNormalized
  );
};

/**
 * Write current mesh to Wavefront .obj file format
 * @memberof MarchingCube
 * @param {string} outFilename - The output obj filename
 */
MarchingCube.prototype.WriteCurrentMeshToObj = function (outFilename) {
  var privateVariable = privateMap.get(this);

  if (
    !nativeBinding.CheckIsMCInstanceExists(privateVariable.marchingCubeHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.isMCRelease) {
    throw new Error("Handle of current instance has been released");
  }

  nativeBinding.WriteCurrentMeshToObj(
    privateVariable.marchingCubeHandle,
    outFilename
  );
};

/**
 * Parse filename into Dimension struct, the filename must contains substring like '<...>width_height_depth<...>' format,
 * or it will raise a error
 * @memberof MarchingCube
 * @static
 * @param {string} filename - Filename you wanna to parse
 * @returns {Dimension} - The parsed dimension
 */
MarchingCube.ParseFileName = function (filename) {
  return nativeBinding.ParseFileName(filename);
};

/**
 * Calculate and get the normal vector of the given mesh triangles
 * @memberof MarchingCube
 * @static
 * @param {Triangle} inputMesh - Mesh you wanna calculate
 * @returns {fPoint} - The normal of mesh triangles
 */
MarchingCube.GetMeshNormal = function (inputMesh) {
  return nativeBinding.GetMeshNormal(inputMesh);
};

/**
 * This method create a OpenGL drawler instance to preview the 3D model,
 * you CANNOT create the drawker instance before you do the march method,
 * you CANNOT create twice once the drawler instance is released
 * @memberof MarchingCube
 */
MarchingCube.prototype.CreateDrawler = function () {
  var privateVariable = privateMap.get(this);

  if (
    !nativeBinding.CheckIsMCInstanceExists(privateVariable.marchingCubeHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (privateVariable.drawlerHandle !== "0") {
    throw new Error(
      "You cannot create drawler instance when the previous one is not released"
    );
  }

  if (!privateVariable.isMarchCalled) {
    throw new Error("You cannot create drawler instance without any mesh data");
  }

  privateVariable.drawlerHandle = nativeBinding.CreateDrawlerInstance(
    this.GetCurrentMesh(true)
  );
};

/**
 * Release the current Drawler instance
 * @memberof MarchingCube
 */
MarchingCube.prototype.ReleaseDrawler = function () {
  var privateVariable = privateMap.get(this);

  if (privateVariable.isDRRelease || privateVariable.drawlerHandle === "0") {
    throw new Error("You cannot release instance twice");
  }

  if (
    !nativeBinding.CheckIsDrawlerInstanceExists(privateVariable.drawlerHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  nativeBinding.ReleaseDrawlerInstance(privateVariable.drawlerHandle);

  privateVariable.drawlerHandle = "0";
  privateVariable.isDRRelease = true;
};

/**
 * Set the model mesh you want to draw
 * @memberof MarchingCube
 * @param {Array<Triangle>} mesh - The mesh triangles to draw, coordinates of the mesh MUST be normalized, or you cannot see any scene in OpenGL canvas
 */
MarchingCube.prototype.SetMesh = function (mesh) {
  var privateVariable = privateMap.get(this);

  if (privateVariable.isDRRelease) {
    throw new Error("Drawler instance has been released");
  }

  if (
    !nativeBinding.CheckIsDrawlerInstanceExists(privateVariable.drawlerHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  nativeBinding.SetMesh(privateVariable.drawlerHandle, mesh);
};

/**
 * Set the rotate angle with xyz axis,
 * you can ONLY set axis to "x" "y" or "z"
 * @memberof MarchingCube
 * @param {number} angle - The rotate angle
 * @param {string} axis - rotate axis, must be "x" "y" or "z"
 */
MarchingCube.prototype.SetRotate = function (angle, axis) {
  var privateVariable = privateMap.get(this);

  if (privateVariable.isDRRelease) {
    throw new Error("Drawler instance has been released");
  }

  if (
    !nativeBinding.CheckIsDrawlerInstanceExists(privateVariable.drawlerHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  if (["x", "y", "z"].indexOf(axis) == -1) {
    throw new TypeError('Rotate axis must be "x", "y" or "z"');
  }

  nativeBinding.SetRotate(privateVariable.drawlerHandle, angle, axis);
};

/**
 * Set the rgb color of vertices,
 * @memberof MarchingCube
 * @param {number} r - Red weight of Red color
 * @param {number} g - Green weight of Green color
 * @param {number} b - Blue weight of Blue color
 */
MarchingCube.prototype.SetColor = function (r, g, b) {
  var privateVariable = privateMap.get(this);

  if (privateVariable.isDRRelease) {
    throw new Error("Drawler instance has been released");
  }

  if (
    !nativeBinding.CheckIsDrawlerInstanceExists(privateVariable.drawlerHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  nativeBinding.SetColor(privateVariable.drawlerHandle, r, g, b);
};

/**
 * Set the rgb color of Ambient light,
 * @memberof MarchingCube
 * @param {number} r - Red weight of red color
 * @param {number} g - Green weight of green color
 * @param {number} b - Blue weight of blue color
 */
MarchingCube.prototype.SetLightAmbiet = function (r, g, b) {
  var privateVariable = privateMap.get(this);

  if (privateVariable.isDRRelease) {
    throw new Error("Drawler instance has been released");
  }

  if (
    !nativeBinding.CheckIsDrawlerInstanceExists(privateVariable.drawlerHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  nativeBinding.SetLightAmbiet(privateVariable.drawlerHandle, r, g, b);
};

/**
 * Set the rgb color of Diffusion light,
 * @memberof MarchingCube
 * @param {number} r - Red weight of red color
 * @param {number} g - Green weight of green color
 * @param {number} b - Blue weight of blue color
 */
MarchingCube.prototype.SetLightDiffuse = function (r, g, b) {
  var privateVariable = privateMap.get(this);

  if (privateVariable.isDRRelease) {
    throw new Error("Drawler instance has been released");
  }

  if (
    !nativeBinding.CheckIsDrawlerInstanceExists(privateVariable.drawlerHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  nativeBinding.SetLightDiffuse(privateVariable.drawlerHandle, r, g, b);
};

/**
 * Set the Position of the light,
 * x, y, z must be NDC coordinate, or it will not rendered in canvas
 * @memberof MarchingCube
 * @param {number} x - x coordinate
 * @param {number} y - y coordinate
 * @param {number} z - z coordinate
 */
MarchingCube.prototype.SetLightPosition = function (x, y, z) {
  var privateVariable = privateMap.get(this);

  if (privateVariable.isDRRelease) {
    throw new Error("Drawler instance has been released");
  }

  if (
    !nativeBinding.CheckIsDrawlerInstanceExists(privateVariable.drawlerHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  nativeBinding.SetLightPosition(privateVariable.drawlerHandle, x, y, z);
};

/**
 * Set the rgb color of material embiet,
 * @memberof MarchingCube
 * @param {number} r - Red weight of red color
 * @param {number} g - Green weight of green color
 * @param {number} b - Blue weight of blue color
 */
MarchingCube.prototype.SetMaterialAmbiet = function (r, g, b) {
  var privateVariable = privateMap.get(this);

  if (privateVariable.isDRRelease) {
    throw new Error("Drawler instance has been released");
  }

  if (
    !nativeBinding.CheckIsDrawlerInstanceExists(privateVariable.drawlerHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  nativeBinding.SetMaterialAmbiet(privateVariable.drawlerHandle, r, g, b);
};

/**
 * Set the rgb color of material diffuse,
 * @memberof MarchingCube
 * @param {number} r - Red weight of red color
 * @param {number} g - Green weight of green color
 * @param {number} b - Blue weight of blue color
 */
MarchingCube.prototype.SetMaterialDiffuse = function (r, g, b) {
  var privateVariable = privateMap.get(this);

  if (privateVariable.isDRRelease) {
    throw new Error("Drawler instance has been released");
  }

  if (
    !nativeBinding.CheckIsDrawlerInstanceExists(privateVariable.drawlerHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  nativeBinding.SetMaterialDiffuse(privateVariable.drawlerHandle, r, g, b);
};

/**
 * Render the current frame,
 * you can use setInterval to do the render
 * @memberof MarchingCube
 */
MarchingCube.prototype.RenderFrame = function () {
  var privateVariable = privateMap.get(this);

  if (privateVariable.isDRRelease) {
    throw new Error("Drawler instance has been released");
  }

  if (
    !nativeBinding.CheckIsDrawlerInstanceExists(privateVariable.drawlerHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  nativeBinding.RenderFrame(privateVariable.drawlerHandle);
};

/**
 * Calculate the current normal vector of the mesh
 * @memberof MarchingCube
 */
MarchingCube.prototype.CalculateNorm = function () {
  var privateVariable = privateMap.get(this);

  if (privateVariable.isDRRelease) {
    throw new Error("Drawler instance has been released");
  }

  if (
    !nativeBinding.CheckIsDrawlerInstanceExists(privateVariable.drawlerHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  nativeBinding.CalculateNorm(privateVariable.drawlerHandle);
};

/**
 * Check whether use close the OpenGL window
 * @memberof MarchingCube
 * @returns {boolean} - the window is close or not
 */
MarchingCube.prototype.CheckRenderClose = function () {
  var privateVariable = privateMap.get(this);

  if (privateVariable.isDRRelease) {
    throw new Error("Drawler instance has been released");
  }

  if (
    !nativeBinding.CheckIsDrawlerInstanceExists(privateVariable.drawlerHandle)
  ) {
    throw new Error("Handle of current instance is not exists");
  }

  return nativeBinding.CheckRenderClose(privateVariable.drawlerHandle);
};

module.exports = MarchingCube;

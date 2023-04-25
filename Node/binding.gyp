{
    "targets": [
        {
            "target_name": "MarchingCube",
            "cflags!": ["-fno-exceptions"],
            "cflags_cc!": ["-fno-exceptions"],
            "sources": ["NodeBinding.cc"],
            "include_dirs": [
                "<!@(node -p \"require('node-addon-api').include\")"
            ],
            'defines': ['NAPI_DISABLE_CPP_EXCEPTIONS'],
            'libraries': [
                "<!@(node -p \"require('./path.js').MarchingCubeAPILIB\")",
                "<!@(node -p \"require('./path.js').MarchingCubeDrawlerAPILIB\")"
            ],
        }
    ]
}

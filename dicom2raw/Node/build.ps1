node-gyp configure; node-gyp build; echo F | xcopy /Y .\build\Release\DicomRawConverter.node .\DicomRawConverter.node; rm .\build -r -force
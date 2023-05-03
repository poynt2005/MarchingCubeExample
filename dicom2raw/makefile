cc = "C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\bin\Hostx64\x64\cl.exe"
cflags = /I./GDCM/include/gdcm-3.0  /I./opencv/include /I"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\include" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\shared" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\ucrt" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\um" /I"C:\Program Files (x86)\Windows Kits\10\Include\10.0.18362.0\winrt"  
ldflags = /LIBPATH:./GDCM/lib /LIBPATH:./opencv/x64/vc16/lib /LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\lib\x64"  /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.18362.0\ucrt\x64" /LIBPATH:"C:\Program Files (x86)\Windows Kits\10\Lib\10.0.18362.0\um\x64" /LIBPATH:"C:\Program Files (x86)\Microsoft Visual Studio\2019\Professional\VC\Tools\MSVC\14.29.30133\lib\x64"
libs = opencv_world470.lib gdcmcharls.lib gdcmCommon.lib gdcmDICT.lib gdcmDSED.lib gdcmexpat.lib gdcmgetopt.lib gdcmIOD.lib gdcmjpeg12.lib gdcmjpeg16.lib gdcmjpeg8.lib gdcmmd5.lib gdcmMEXD.lib gdcmMSFF.lib gdcmopenjp2.lib gdcmzlib.lib socketxx.lib



dll:
	$(cc) $(cflags) /std:c++17 /c DicomRawConverter.cc /Fo:DicomRawConverter.o
	$(cc) $(cflags) /DBUILDD2RAPI /c DicomRawConverterAPI.cc /Fo:DicomRawConverterAPI.o
	$(cc) /LD DicomRawConverterAPI.o DicomRawConverter.o /link $(ldflags) $(libs) /OUT:DicomRawConverterAPI.dll /IMPLIB:DicomRawConverterAPI.lib

test:
	gcc -c test.c -o test.o
	gcc -L./ test.o -o test.exe -lDicomRawConverterAPI
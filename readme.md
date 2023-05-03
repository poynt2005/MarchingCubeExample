# Marching Cube Example

## 說明

這個是使用 C++ 製作等值立方法立體渲染的練習，  
目的是要將研究所期間所不懂得這個演算法做一次重新的理解  
本項目包含兩個部分，一個是將 dicom 轉換成 raw，一個是將 raw 進行立體渲染

## 依賴庫

1. FreeGLUT
2. GLFW
3. libuv (測試用的，選用)
4. GDCM (dicom2raw 用)
5. opencv (dicom2raw 用)

## 編譯

Compiler:
MinGWx64 -> core, lib  
MSVC -> node extension  
不想編譯依賴庫的話，直接去下載 release 裡面的 Dependencies.zip，
放在根目錄後，在根目錄打開 shell，  
運行 python set.py 編譯

## 使用

立體渲染(根目錄):  
測試用 exe -> testDrawler.exe  
測試用 js -> cd Node && node test.js

DICOM RAW 轉換(dicom2raw 目錄):  
測試用 exe -> test.exe  
測試用 js -> cd Node && node test.js

完整流程 [DICOM -> RAW -> 立體渲染] (根目錄):  
測試用 js -> node allTest.js

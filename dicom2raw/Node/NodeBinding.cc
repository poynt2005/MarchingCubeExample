
#include "../../Types.h"
#include "../DicomRawConverterAPI.h"

#include <Napi.h>

#include <string>
#include <string.h>
#include <vector>

Napi::Value Node_CreateDicomRawConverter(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong Arguments, excepted 2 arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString() && !info[1].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 and 1 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto dicomDirectory = info[0].As<Napi::String>().Utf8Value();
    auto dicomSearchPattern = info[1].As<Napi::String>().Utf8Value();

    const ConvHandle handle = CreateDicomRawConverter(dicomDirectory.data(), dicomSearchPattern.data());

    return Napi::String::New(env, std::to_string(handle));
}

Napi::Value Node_ReleaseDicomRawConverter(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, excepted one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const ConvHandle handle = std::stoull(info[0].As<Napi::String>().Utf8Value());
    ReleaseDicomRawConverter(handle);
    return env.Null();
}

Napi::Value Node_CheckDicomRawConverterExists(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, excepted one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const ConvHandle handle = std::stoull(info[0].As<Napi::String>().Utf8Value());

    return Napi::Boolean::New(env, CheckDicomRawConverterExists(handle));
}

Napi::Value Node_SortDicomFile(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 3)
    {
        Napi::TypeError::New(env, "Wrong Argument, excepted one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString() && !info[1].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 and 1 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[2].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a number").ThrowAsJavaScriptException();
        return env.Null();
    }

    const ConvHandle handle = std::stoull(info[0].As<Napi::String>().Utf8Value());
    const auto numberPattern = info[1].As<Napi::String>().Utf8Value();
    const unsigned int patternGroupOrder = info[2].As<Napi::Number>().Uint32Value();
    SortDicomFile(handle, numberPattern.data(), patternGroupOrder);
    return env.Null();
}

Napi::Value Node_Build(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong Argument, excepted one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsBoolean())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a boolean").ThrowAsJavaScriptException();
        return env.Null();
    }

    const ConvHandle handle = std::stoull(info[0].As<Napi::String>().Utf8Value());
    const bool isCV = info[1].As<Napi::Boolean>().Value();

    return Napi::Boolean::New(env, Build(handle, isCV));
}

Napi::Value Node_GetDicomBufferSequential(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong Argument, excepted one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a number").ThrowAsJavaScriptException();
        return env.Null();
    }

    const ConvHandle handle = std::stoull(info[0].As<Napi::String>().Utf8Value());
    const unsigned int sequenceOrder = info[1].As<Napi::Number>().Uint32Value();

    char *imgBuf = nullptr;
    unsigned int bufSize = 0;
    GetDicomBufferSequential(
        handle,
        sequenceOrder,
        &imgBuf,
        &bufSize);

    auto jsImgBuffer = Napi::Buffer<uint8_t>::New(
        env,
        reinterpret_cast<uint8_t *>(imgBuf),
        bufSize);

    ReleaseDicomBuffer(&imgBuf);

    return jsImgBuffer;
}

Napi::Value Node_ShowDicomBufferSequential(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong Argument, excepted one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a number").ThrowAsJavaScriptException();
        return env.Null();
    }

    const ConvHandle handle = std::stoull(info[0].As<Napi::String>().Utf8Value());
    const unsigned int sequenceOrder = info[1].As<Napi::Number>().Uint32Value();

    ShowDicomSequential(handle, sequenceOrder);
    return env.Null();
}

Napi::Value Node_GetDicomNameSequential(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong Argument, excepted one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a number").ThrowAsJavaScriptException();
        return env.Null();
    }

    const ConvHandle handle = std::stoull(info[0].As<Napi::String>().Utf8Value());
    const unsigned int sequenceOrder = info[1].As<Napi::Number>().Uint32Value();

    char *dicomFilename = nullptr;

    GetDicomNameSequential(
        handle,
        sequenceOrder,
        &dicomFilename);

    auto jsDicomName = Napi::String::New(env, dicomFilename);

    ReleaseDicomBuffer(&dicomFilename);

    return jsDicomName;
}

Napi::Value Node_GetRawDimension(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, excepted one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const ConvHandle handle = std::stoull(info[0].As<Napi::String>().Utf8Value());

    Dimension dimension;
    GetRawDimension(handle, &dimension);

    auto jsDimensionStru = Napi::Object::New(env);

    jsDimensionStru.Set("width", dimension.width);
    jsDimensionStru.Set("height", dimension.height);
    jsDimensionStru.Set("depth", dimension.depth);

    return jsDimensionStru;
}

Napi::Value Node_GetDicomCounts(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, excepted one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const ConvHandle handle = std::stoull(info[0].As<Napi::String>().Utf8Value());

    const unsigned int dicomCounts = GetDicomCounts(handle);

    return Napi::Number::New(env, dicomCounts);
}

Napi::Value Node_WriteToRawFile(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong Argument, excepted one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString() && !info[1].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 and 1 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const ConvHandle handle = std::stoull(info[0].As<Napi::String>().Utf8Value());
    const auto outputFilename = info[1].As<Napi::String>().Utf8Value();

    WriteToRawFile(
        handle,
        outputFilename.data());

    return env.Null();
}

Napi::Value Node_GetRawData(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, excepted one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const ConvHandle handle = std::stoull(info[0].As<Napi::String>().Utf8Value());

    char *rawBuf = nullptr;
    unsigned int rawSize = 0;

    GetRawData(
        handle,
        &rawBuf,
        &rawSize);

    const auto jsRawBuf = Napi::Buffer<uint8_t>::New(
        env,
        reinterpret_cast<uint8_t *>(rawBuf),
        rawSize);

    return jsRawBuf;
}

Napi::Value Node_GetBrokenLayer(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, excepted one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted a string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const ConvHandle handle = std::stoull(info[0].As<Napi::String>().Utf8Value());

    unsigned int *brokenLayer = nullptr;
    unsigned int layerCount = 0;

    GetBrokenLayer(
        handle,
        &brokenLayer,
        &layerCount);

    auto jsDicomLayer = Napi::Array::New(env, layerCount);

    for (unsigned int i = 0; i < layerCount; i++)
    {
        jsDicomLayer.Set(i, Napi::Number::New(env, brokenLayer[i]));
    }

    ReleaseDicomLayer(&brokenLayer);

    return jsDicomLayer;
}

Napi::Object Initialize(Napi::Env env, Napi::Object exports)
{
    exports.Set(Napi::String::New(env, "CreateDicomRawConverter"), Napi::Function::New(env, Node_CreateDicomRawConverter));
    exports.Set(Napi::String::New(env, "ReleaseDicomRawConverter"), Napi::Function::New(env, Node_ReleaseDicomRawConverter));
    exports.Set(Napi::String::New(env, "CheckDicomRawConverterExists"), Napi::Function::New(env, Node_CheckDicomRawConverterExists));
    exports.Set(Napi::String::New(env, "SortDicomFile"), Napi::Function::New(env, Node_SortDicomFile));
    exports.Set(Napi::String::New(env, "Build"), Napi::Function::New(env, Node_Build));
    exports.Set(Napi::String::New(env, "GetDicomBufferSequential"), Napi::Function::New(env, Node_GetDicomBufferSequential));
    exports.Set(Napi::String::New(env, "GetDicomNameSequential"), Napi::Function::New(env, Node_GetDicomNameSequential));
    exports.Set(Napi::String::New(env, "ShowDicomBufferSequential"), Napi::Function::New(env, Node_ShowDicomBufferSequential));
    exports.Set(Napi::String::New(env, "GetRawDimension"), Napi::Function::New(env, Node_GetRawDimension));
    exports.Set(Napi::String::New(env, "GetDicomCounts"), Napi::Function::New(env, Node_GetDicomCounts));
    exports.Set(Napi::String::New(env, "WriteToRawFile"), Napi::Function::New(env, Node_WriteToRawFile));
    exports.Set(Napi::String::New(env, "GetRawData"), Napi::Function::New(env, Node_GetRawData));
    exports.Set(Napi::String::New(env, "GetBrokenLayer"), Napi::Function::New(env, Node_GetBrokenLayer));
    return exports;
}

NODE_API_MODULE(DicomRawConverter, Initialize)
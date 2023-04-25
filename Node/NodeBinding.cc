#include "../MarchingCubeAPI.h"
#include "../DrawlerAPI.h"
#include "../Table.h"

#include <Napi.h>
#include <string>
#include <string.h>
#include <memory>

/** utilities */

bool CheckAndSetMesh(const Napi::Env &env, const Napi::Array &jsTriangleArr, const unsigned int triangleLength, Triangle *outTri)
{
    for (unsigned int i = 0; i < triangleLength; ++i)
    {
        if (!jsTriangleArr.Get(i).IsObject())
        {
            Napi::TypeError::New(env, "Wrong Argument, triangle struct expected a object").ThrowAsJavaScriptException();
            return false;
        }

        auto jsTriangleObj = jsTriangleArr.Get(i).As<Napi::Object>();

        if (!jsTriangleObj.Get("v0").IsObject())
        {
            Napi::TypeError::New(env, "Wrong Argument, vertex 0 of triangle struct expected a object").ThrowAsJavaScriptException();
            return false;
        }

        if (!jsTriangleObj.Get("v1").IsObject())
        {
            Napi::TypeError::New(env, "Wrong Argument, vertex 1 of triangle struct expected a object").ThrowAsJavaScriptException();
            return false;
        }

        if (!jsTriangleObj.Get("v2").IsObject())
        {
            Napi::TypeError::New(env, "Wrong Argument, vertex 2 of triangle struct expected a object").ThrowAsJavaScriptException();
            return false;
        }

        auto jsPointV0Obj = jsTriangleObj.Get("v0").As<Napi::Object>();
        auto jsPointV1Obj = jsTriangleObj.Get("v1").As<Napi::Object>();
        auto jsPointV2Obj = jsTriangleObj.Get("v2").As<Napi::Object>();

        /** Check V0 coord */
        if (!jsPointV0Obj.Get("x").IsNumber())
        {
            Napi::TypeError::New(env, "Wrong Argument, x coordinate of triangle point v0 expected a number").ThrowAsJavaScriptException();
            return false;
        }

        if (!jsPointV0Obj.Get("y").IsNumber())
        {
            Napi::TypeError::New(env, "Wrong Argument, y coordinate of triangle point v0 expected a number").ThrowAsJavaScriptException();
            return false;
        }

        if (!jsPointV0Obj.Get("z").IsNumber())
        {
            Napi::TypeError::New(env, "Wrong Argument, z coordinate of triangle point v0 expected a number").ThrowAsJavaScriptException();
            return false;
        }

        /** Check V1 coord */
        if (!jsPointV1Obj.Get("x").IsNumber())
        {
            Napi::TypeError::New(env, "Wrong Argument, x coordinate of triangle point v1 expected a number").ThrowAsJavaScriptException();
            return false;
        }

        if (!jsPointV1Obj.Get("y").IsNumber())
        {
            Napi::TypeError::New(env, "Wrong Argument, y coordinate of triangle point v1 expected a number").ThrowAsJavaScriptException();
            return false;
        }

        if (!jsPointV1Obj.Get("z").IsNumber())
        {
            Napi::TypeError::New(env, "Wrong Argument, z coordinate of triangle point v1 expected a number").ThrowAsJavaScriptException();
            return false;
        }

        /** Check V2 coord */
        if (!jsPointV2Obj.Get("x").IsNumber())
        {
            Napi::TypeError::New(env, "Wrong Argument, x coordinate of triangle point v2 expected a number").ThrowAsJavaScriptException();
            return false;
        }

        if (!jsPointV2Obj.Get("y").IsNumber())
        {
            Napi::TypeError::New(env, "Wrong Argument, y coordinate of triangle point v2 expected a number").ThrowAsJavaScriptException();
            return false;
        }

        if (!jsPointV2Obj.Get("z").IsNumber())
        {
            Napi::TypeError::New(env, "Wrong Argument, z coordinate of triangle point v2 expected a number").ThrowAsJavaScriptException();
            return false;
        }

        Triangle tri{
            fPoint{
                jsPointV0Obj.Get("x").As<Napi::Number>().FloatValue(),
                jsPointV0Obj.Get("y").As<Napi::Number>().FloatValue(),
                jsPointV0Obj.Get("z").As<Napi::Number>().FloatValue(),
            },
            fPoint{
                jsPointV1Obj.Get("x").As<Napi::Number>().FloatValue(),
                jsPointV1Obj.Get("y").As<Napi::Number>().FloatValue(),
                jsPointV1Obj.Get("z").As<Napi::Number>().FloatValue(),
            },
            fPoint{
                jsPointV2Obj.Get("x").As<Napi::Number>().FloatValue(),
                jsPointV2Obj.Get("y").As<Napi::Number>().FloatValue(),
                jsPointV2Obj.Get("z").As<Napi::Number>().FloatValue(),
            }};
        memcpy(&(outTri[i]), &tri, sizeof(Triangle));
    }

    return true;
}

/** Marching Cube Core*/

Napi::Value Node_CreateMarchingCubeInstance(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong Arguments, expected 2 arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsObject())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 1 excepted one Object").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto rawFilename = info[0].As<Napi::String>().Utf8Value();
    auto dimObj = info[1].As<Napi::Object>();

    auto dimObjw = dimObj.Get("width");
    if (!dimObjw.IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, \"width\" of Dimension object excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto dimObjh = dimObj.Get("height");
    if (!dimObjh.IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, \"height\" of Dimension object excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto dimObjd = dimObj.Get("depth");
    if (!dimObjd.IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, \"depth\" of Dimension object excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    Dimension dimension{
        dimObjw.As<Napi::Number>().Uint32Value(),
        dimObjh.As<Napi::Number>().Uint32Value(),
        dimObjd.As<Napi::Number>().Uint32Value(),
    };

    const auto handle = CreateMarchingCubeInstance(rawFilename.data(), &dimension);

    if (!handle)
    {
        Napi::Error::New(env, "Create Marching Cubes instances failed").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, std::to_string(handle));
}

Napi::Value Node_ReleaseMarchingCubeInstance(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, expected  argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<MCHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));
    ReleaseMarchingCubeInstance(handle);

    return env.Null();
}

Napi::Value Node_CheckIsMCInstanceExists(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, expected  argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<MCHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));

    return Napi::Boolean::New(env, CheckIsMCInstanceExists(handle));
}

Napi::Value Node_March(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong Arguments, expected 2 arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 1 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<MCHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));
    auto isoValue = info[1].As<Napi::Number>().Uint32Value();

    March(handle, isoValue);

    return env.Null();
}

Napi::Value Node_GetCurrentMesh(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong Arguments, expected 2 arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsBoolean())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 1 excepted one boolean").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<MCHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));
    Triangle *tri = nullptr;
    unsigned int faces = 0;

    if (info[1].As<Napi::Boolean>().Value())
    {
        GetCurrentMeshNormalized(handle, &tri, &faces);
    }
    else
    {
        GetCurrentMesh(handle, &tri, &faces);
    }

    auto jsTriangleArr = Napi::Array::New(env, faces);

    for (unsigned int i = 0; i < faces; ++i)
    {
        auto jsTriangleObj = Napi::Object::New(env);

        auto jsV0Obj = Napi::Object::New(env);
        jsV0Obj.Set("x", Napi::Number::New(env, tri[i].v0.x));
        jsV0Obj.Set("y", Napi::Number::New(env, tri[i].v0.y));
        jsV0Obj.Set("z", Napi::Number::New(env, tri[i].v0.z));

        auto jsV1Obj = Napi::Object::New(env);
        jsV1Obj.Set("x", Napi::Number::New(env, tri[i].v1.x));
        jsV1Obj.Set("y", Napi::Number::New(env, tri[i].v1.y));
        jsV1Obj.Set("z", Napi::Number::New(env, tri[i].v1.z));

        auto jsV2Obj = Napi::Object::New(env);
        jsV2Obj.Set("x", Napi::Number::New(env, tri[i].v2.x));
        jsV2Obj.Set("y", Napi::Number::New(env, tri[i].v2.y));
        jsV2Obj.Set("z", Napi::Number::New(env, tri[i].v2.z));

        jsTriangleObj.Set("v0", std::move(jsV0Obj));
        jsTriangleObj.Set("v1", std::move(jsV1Obj));
        jsTriangleObj.Set("v2", std::move(jsV2Obj));

        jsTriangleArr.Set(i, std::move(jsTriangleObj));
    }

    ReleaseCurrentMesh(&tri);

    return jsTriangleArr;
}

Napi::Value Node_WriteCurrentMeshToObj(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong Arguments, expected 2 arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 1 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<MCHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));
    auto filename = info[1].As<Napi::String>().Utf8Value();

    WriteCurrentMeshToObj(handle, filename.data());

    return env.Null();
}

Napi::Value Node_ParseFileName(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, expected  argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto filename = info[0].As<Napi::String>().Utf8Value();
    Dimension dimension;
    auto isParsed = ParseFileName(filename.data(), &dimension);

    if (isParsed)
    {
        auto jsDimensionStruct = Napi::Object::New(env);
        jsDimensionStruct.Set("width", dimension.width);
        jsDimensionStruct.Set("height", dimension.height);
        jsDimensionStruct.Set("depth", dimension.depth);
        return jsDimensionStruct;
    }
    else
    {
        Napi::Error::New(env, "Filename is not suitable for format \"width_height_depth\"").ThrowAsJavaScriptException();
        return env.Null();
    }
}

Napi::Value Node_GetMeshNormal(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, expected one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsArray())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 0 excepted one array").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto jsTriangleArr = info[0].As<Napi::Array>();
    auto triangleLength = static_cast<unsigned int>(jsTriangleArr.Length());
    auto triangleArr = std::make_unique<Triangle[]>(triangleLength);

    if (!CheckAndSetMesh(env, jsTriangleArr, triangleLength, triangleArr.get()))
    {
        return env.Null();
    }

    fPoint *normArr = nullptr;
    unsigned int normCounts = 0;
    GetMeshNormal(triangleArr.get(), triangleLength, &normArr, &normCounts);

    auto jsNormArr = Napi::Array::New(env, normCounts);
    for (unsigned int i = 0; i < normCounts; i++)
    {
        auto jsNormObj = Napi::Object::New(env);
        jsNormObj.Set("x", Napi::Number::New(env, normArr[i].x));
        jsNormObj.Set("y", Napi::Number::New(env, normArr[i].y));
        jsNormObj.Set("z", Napi::Number::New(env, normArr[i].z));
        jsNormArr.Set(i, std::move(jsNormObj));
    }

    ReleaseCurrentPoint(&normArr);

    return jsNormArr;
}

/** Marching Cube OpenGL Drawler*/

Napi::Value Node_CreateDrawlerInstance(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, expected 1 argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsArray())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted 1 array").ThrowAsJavaScriptException();
        return env.Null();
    }

    auto jsTriangleArr = info[0].As<Napi::Array>();
    auto triangleLength = static_cast<unsigned int>(jsTriangleArr.Length());
    auto triangleArr = std::make_unique<Triangle[]>(triangleLength);

    if (!CheckAndSetMesh(env, jsTriangleArr, triangleLength, triangleArr.get()))
    {
        return env.Null();
    }

    const DRHandle handle = CreateDrawlerInstance(triangleArr.get(), triangleLength);

    if (!handle)
    {
        Napi::Error::New(env, "Create drawler instances failed").ThrowAsJavaScriptException();
        return env.Null();
    }

    return Napi::String::New(env, std::to_string(handle));
}

Napi::Value Node_ReleaseDrawlerInstance(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, expected  argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<DRHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));
    ReleaseDrawlerInstance(handle);

    return env.Null();
}

Napi::Value Node_CheckIsDrawlerInstanceExists(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, expected  argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<DRHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));

    return Napi::Boolean::New(env, CheckIsDrawlerInstanceExists(handle));
}

Napi::Value Node_SetMesh(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 2)
    {
        Napi::TypeError::New(env, "Wrong Arguments, expected 2 arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsArray())
    {
        Napi::TypeError::New(env, "Wrong Argument, position 1 excepted 1 array").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<DRHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));

    auto jsTriangleArr = info[1].As<Napi::Array>();
    auto triangleLength = static_cast<unsigned int>(jsTriangleArr.Length());
    auto triangleArr = std::make_unique<Triangle[]>(triangleLength);

    if (!CheckAndSetMesh(env, jsTriangleArr, triangleLength, triangleArr.get()))
    {
        return env.Null();
    }

    SetMesh(handle, triangleArr.get(), triangleLength);

    return env.Null();
}

Napi::Value Node_SetRotate(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 3)
    {
        Napi::TypeError::New(env, "Wrong Arguments, expected 3 arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 1 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[2].IsString())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 2 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<DRHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));
    auto angle = info[1].As<Napi::Number>().FloatValue();
    auto axis = info[2].As<Napi::String>().Utf8Value().at(0);

    SetRotate(handle, angle, axis);

    return env.Null();
}

Napi::Value Node_SetColor(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 4)
    {
        Napi::TypeError::New(env, "Wrong Arguments, expected 4 arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 1 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[2].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 2 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[3].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 3 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<DRHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));

    auto rColor = info[1].As<Napi::Number>().FloatValue();
    auto gColor = info[2].As<Napi::Number>().FloatValue();
    auto bColor = info[3].As<Napi::Number>().FloatValue();

    SetColor(handle, rColor, gColor, bColor);

    return env.Null();
}

Napi::Value Node_SetLightAmbiet(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 4)
    {
        Napi::TypeError::New(env, "Wrong Arguments, expected 4 arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 1 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[2].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 2 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[3].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 3 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<DRHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));

    auto rColor = info[1].As<Napi::Number>().FloatValue();
    auto gColor = info[2].As<Napi::Number>().FloatValue();
    auto bColor = info[3].As<Napi::Number>().FloatValue();

    Color3 colorStru{rColor, gColor, bColor};
    SetLightAmbiet(handle, &colorStru);
    return env.Null();
}

Napi::Value Node_SetLightDiffuse(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 4)
    {
        Napi::TypeError::New(env, "Wrong Arguments, expected 4 arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 1 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[2].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 2 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[3].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 3 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<DRHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));

    auto rColor = info[1].As<Napi::Number>().FloatValue();
    auto gColor = info[2].As<Napi::Number>().FloatValue();
    auto bColor = info[3].As<Napi::Number>().FloatValue();
    Color3 colorStru{rColor, gColor, bColor};
    SetLightDiffuse(handle, &colorStru);
    return env.Null();
}

Napi::Value Node_SetMaterialAmbiet(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 4)
    {
        Napi::TypeError::New(env, "Wrong Arguments, expected 4 arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 1 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[2].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 2 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[3].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 3 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<DRHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));

    auto rColor = info[1].As<Napi::Number>().FloatValue();
    auto gColor = info[2].As<Napi::Number>().FloatValue();
    auto bColor = info[3].As<Napi::Number>().FloatValue();
    Color3 colorStru{rColor, gColor, bColor};
    SetMaterialAmbiet(handle, &colorStru);
    return env.Null();
}

Napi::Value Node_SetMaterialDiffuse(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 4)
    {
        Napi::TypeError::New(env, "Wrong Arguments, expected 4 arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 1 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[2].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 2 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[3].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 3 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<DRHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));

    auto rColor = info[1].As<Napi::Number>().FloatValue();
    auto gColor = info[2].As<Napi::Number>().FloatValue();
    auto bColor = info[3].As<Napi::Number>().FloatValue();
    Color3 colorStru{rColor, gColor, bColor};
    SetMaterialDiffuse(handle, &colorStru);
    return env.Null();
}

Napi::Value Node_SetLightPosition(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 4)
    {
        Napi::TypeError::New(env, "Wrong Arguments, expected 4 arguments").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 1 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[2].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 2 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[3].IsNumber())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 3 excepted one number").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<DRHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));

    auto x = info[1].As<Napi::Number>().FloatValue();
    auto y = info[2].As<Napi::Number>().FloatValue();
    auto z = info[3].As<Napi::Number>().FloatValue();
    fPoint pos{x, y, z};
    SetLightPosition(handle, &pos);
    return env.Null();
}

Napi::Value Node_RenderFrame(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, expected one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<DRHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));

    RenderFrame(handle);

    return env.Null();
}

Napi::Value Node_CalculateNorm(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, expected one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<DRHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));

    CalculateNorm(handle);

    return env.Null();
}

Napi::Value Node_CheckRenderClose(const Napi::CallbackInfo &info)
{
    auto env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong Argument, expected one argument").ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "Wrong Arguments, position 0 excepted one string").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto handle = static_cast<DRHandle>(std::stoull(info[0].As<Napi::String>().Utf8Value()));

    return Napi::Boolean::New(env, CheckIsClose(handle));
}

Napi::Object Initialize(Napi::Env env, Napi::Object exports)
{

    exports.Set(
        Napi::String::New(env, "CreateMarchingCubeInstance"),
        Napi::Function::New(env, Node_CreateMarchingCubeInstance));

    exports.Set(
        Napi::String::New(env, "ReleaseMarchingCubeInstance"),
        Napi::Function::New(env, Node_ReleaseMarchingCubeInstance));

    exports.Set(
        Napi::String::New(env, "CheckIsMCInstanceExists"),
        Napi::Function::New(env, Node_CheckIsMCInstanceExists));

    exports.Set(
        Napi::String::New(env, "March"),
        Napi::Function::New(env, Node_March));

    exports.Set(
        Napi::String::New(env, "GetCurrentMesh"),
        Napi::Function::New(env, Node_GetCurrentMesh));

    exports.Set(
        Napi::String::New(env, "WriteCurrentMeshToObj"),
        Napi::Function::New(env, Node_WriteCurrentMeshToObj));

    exports.Set(
        Napi::String::New(env, "ParseFileName"),
        Napi::Function::New(env, Node_ParseFileName));

    exports.Set(
        Napi::String::New(env, "GetMeshNormal"),
        Napi::Function::New(env, Node_GetMeshNormal));

    exports.Set(
        Napi::String::New(env, "CreateDrawlerInstance"),
        Napi::Function::New(env, Node_CreateDrawlerInstance));

    exports.Set(
        Napi::String::New(env, "ReleaseDrawlerInstance"),
        Napi::Function::New(env, Node_ReleaseDrawlerInstance));

    exports.Set(
        Napi::String::New(env, "CheckIsDrawlerInstanceExists"),
        Napi::Function::New(env, Node_CheckIsDrawlerInstanceExists));

    exports.Set(
        Napi::String::New(env, "SetRotate"),
        Napi::Function::New(env, Node_SetRotate));

    exports.Set(
        Napi::String::New(env, "SetColor"),
        Napi::Function::New(env, Node_SetColor));

    exports.Set(
        Napi::String::New(env, "SetLightAmbiet"),
        Napi::Function::New(env, Node_SetLightAmbiet));

    exports.Set(
        Napi::String::New(env, "SetLightDiffuse"),
        Napi::Function::New(env, Node_SetLightDiffuse));

    exports.Set(
        Napi::String::New(env, "SetLightPosition"),
        Napi::Function::New(env, Node_SetLightPosition));

    exports.Set(
        Napi::String::New(env, "SetMaterialAmbiet"),
        Napi::Function::New(env, Node_SetMaterialAmbiet));

    exports.Set(
        Napi::String::New(env, "SetMaterialDiffuse"),
        Napi::Function::New(env, Node_SetMaterialDiffuse));

    exports.Set(
        Napi::String::New(env, "RenderFrame"),
        Napi::Function::New(env, Node_RenderFrame));

    exports.Set(
        Napi::String::New(env, "CheckRenderClose"),
        Napi::Function::New(env, Node_CheckRenderClose));

    exports.Set(
        Napi::String::New(env, "CalculateNorm"),
        Napi::Function::New(env, Node_CalculateNorm));

    return exports;
}

NODE_API_MODULE(MarchingCube, Initialize)
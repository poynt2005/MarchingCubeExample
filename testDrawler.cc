#include "MarchingCubeAPI.h"
#include "DrawlerAPI.h"

#include <iostream>
#include <string.h>
#include <uv.h>
#include <vector>

DRHandle drHandle;
int angle = 0;

int main()
{
  const char *rawFileName = "ABC_512_512_51.raw";
  Dimension fileDimension;
  ParseFileName(rawFileName, &fileDimension);
  MCHandle handle = CreateMarchingCubeInstance(rawFileName, &fileDimension);

  MCHandle handle2 = CreateMarchingCubeInstance(rawFileName, &fileDimension);

  March(handle, 100);

  std::vector<Triangle> triVec;
  std::vector<float> triColor;

  Triangle *tri = nullptr;
  unsigned int faces = 0;
  GetCurrentMeshNormalized(handle, &tri, &faces);

  std::cout << "Faces: " << faces << '\n';

  drHandle = CreateDrawlerInstance(tri, faces);

  ReleaseCurrentMesh(&tri);
  ReleaseMarchingCubeInstance(handle);

  auto loop = uv_default_loop();

  CalculateNorm(drHandle);

  uv_idle_t idler;
  uv_idle_init(loop, &idler);

  uv_idle_start(&idler, [](uv_idle_t *uvHandle) -> void
                {
                    if(angle >= 360){
                        angle = 0;
                    }
                    SetRotate(drHandle, angle, 'x');
                    angle += 1;
                    std::cout << angle << '\n';
                      RenderFrame(drHandle);

                        int isClose = CheckIsClose(drHandle);
                      if (isClose)
                      {
                          uv_idle_stop(uvHandle);
                      } });

  uv_run(loop, UV_RUN_DEFAULT);
  uv_loop_close(uv_default_loop());

  ReleaseDrawlerInstance(drHandle);

  std::cout << "End\n"
            << '\n';

  return 0;
}
#include <napi.h>
#include "zal-web.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  return ZalWeb::Init(env, exports);
}

NODE_API_MODULE(addon, InitAll)

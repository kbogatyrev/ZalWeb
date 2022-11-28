#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <map>
#include <functional>
#include <napi.h>
#include "Dictionary.h"

using fnHandler = std::function<Napi::Value(const Napi::CallbackInfo& info, Hlib::ILexeme *)>;

static std::map<Hlib::ET_AccentType, std::string> MapAccentTypeToString {
  { Hlib::ET_AccentType::AT_A, "a" },
  { Hlib::ET_AccentType::AT_A1, "a'" },
  { Hlib::ET_AccentType::AT_B, "b" },
  { Hlib::ET_AccentType::AT_B1, "b'" },
  { Hlib::ET_AccentType::AT_C, "c" },
  { Hlib::ET_AccentType::AT_C1, "c'" },
  { Hlib::ET_AccentType::AT_C2, "c''" },
  { Hlib::ET_AccentType::AT_D, "d" },
  { Hlib::ET_AccentType::AT_D1, "d'" },
  { Hlib::ET_AccentType::AT_E, "e" },
  { Hlib::ET_AccentType::AT_F, "f" },
  { Hlib::ET_AccentType::AT_F1, "f'" },
  { Hlib::ET_AccentType::AT_F2, "f''" }
};


class ZalWeb : public Napi::ObjectWrap<ZalWeb> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  ZalWeb(const Napi::CallbackInfo& info);

 private:
  void SetDbPath(const Napi::CallbackInfo& info);
  void InitPropertyHandlers();
  Napi::Value GetLexemesByInitialForm(const Napi::CallbackInfo& info);
  Napi::Value LoadFirstLexeme(const Napi::CallbackInfo& info);
  Napi::Value LoadNextLexeme(const Napi::CallbackInfo& info);
  Napi::Value GetProperty(const Napi::CallbackInfo& info);
  Napi::Value SetProperty(const Napi::CallbackInfo& info);

  Hlib::IDictionary * m_pDictionary { nullptr };
  Hlib::ILexemeEnumerator * m_pLexemeEnumerator { nullptr };
  Hlib::ILexeme * m_pCurrentLexeme { nullptr };

  std::map<string, fnHandler> m_mapKeyToPropHandler;

};

#endif

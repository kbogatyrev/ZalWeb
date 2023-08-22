#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <map>
#include <functional>
#include <memory>
#include <napi.h>
#include "Dictionary.h"
#include "Lexeme.h"
#include "Inflection.h"
#include "Singleton.h"

using fnLexemeHandler = std::function<Napi::Value(const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme>)>;
using fnInflectionHandler = std::function<Napi::Value(const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection>)>;

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
  Napi::Value CreateLexemeEnumerator(const Napi::CallbackInfo& info);
  Napi::Value GetFirstLexeme(const Napi::CallbackInfo& info);
  Napi::Value GetNextLexeme(const Napi::CallbackInfo& info);
  Napi::Value CreateInflectionEnumerator(const Napi::CallbackInfo& info);
  Napi::Value GetFirstInflection(const Napi::CallbackInfo& info);
  Napi::Value GetNextInflection(const Napi::CallbackInfo& info);
  Napi::Value GetLexemeProperty(const Napi::CallbackInfo& info);
  Napi::Value SetLexemeProperty(const Napi::CallbackInfo& info);
  Napi::Value GetInflectionProperty(const Napi::CallbackInfo& info);
  Napi::Value SetInflectionProperty(const Napi::CallbackInfo& info);

  shared_ptr <Hlib::CDictionary> m_spDictionary;
  shared_ptr <Hlib::CLexemeEnumerator> m_spLexemeEnumerator;
  shared_ptr <Hlib::CLexeme> m_spCurrentLexeme;
  shared_ptr <Hlib::CInflectionEnumerator> m_spInflectionEnumerator;
  shared_ptr <Hlib::CInflection> m_spCurrentInflection;

  std::multimap<std::shared_ptr<Hlib::CLexeme>, std::shared_ptr<Hlib::CInflection>> m_mmapLexemeToInflections;

  std::map<string, fnLexemeHandler> m_mapKeyToLexemePropHandler;
  std::map<string, fnInflectionHandler> m_mapKeyToInflectionPropHandler;

};

#endif

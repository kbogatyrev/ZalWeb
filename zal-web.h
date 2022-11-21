#ifndef MYOBJECT_H
#define MYOBJECT_H

#include "Dictionary.h"
#include <napi.h>

class ZalWeb : public Napi::ObjectWrap<ZalWeb> {
 public:
  static Napi::Object Init(Napi::Env env, Napi::Object exports);
  ZalWeb(const Napi::CallbackInfo& info);

 private:
  void SetDbPath(const Napi::CallbackInfo& info);
  Napi::Value GetLexemesByInitialForm(const Napi::CallbackInfo& info);
  Napi::Value LoadFirstLexeme(const Napi::CallbackInfo& info);
  Napi::Value LoadNextLexeme(const Napi::CallbackInfo& info);
  Napi::Value GetProperty(const Napi::CallbackInfo& info);
  Napi::Value SetProperty(const Napi::CallbackInfo& info);

  Hlib::IDictionary * m_pDictionary { nullptr };
  Hlib::ILexemeEnumerator * m_pLexemeEnumerator { nullptr };
  Hlib::ILexeme * m_pCurrentLexeme { nullptr };

};

#endif

#include <iostream>
#include "EString.h"
#include "zal-web.h"

extern "C"
{
  Hlib::ET_ReturnCode GetDictionary(Hlib::IDictionary*&);
}

Napi::Object ZalWeb::Init(Napi::Env env, Napi::Object exports) 
{
  Napi::Function func =
      DefineClass(env, "ZalWeb", { InstanceMethod("setDbPath", &ZalWeb::SetDbPath),
                                   InstanceMethod("getLexemesByInitialForm", &ZalWeb::GetLexemesByInitialForm),
                                   InstanceMethod("setProperty", &ZalWeb::SetProperty),
                                   InstanceMethod("getProperty", &ZalWeb::GetProperty) });
  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor);
  exports.Set("ZalWeb", func);
  return exports;
}

ZalWeb::ZalWeb(const Napi::CallbackInfo& info) : Napi::ObjectWrap<ZalWeb>(info) 
{
//  Napi::Env env = info.Env();
//  int length = info.Length();
//  if (length <= 0 || !info[0].IsNumber()) {
//    Napi::TypeError::New(env, "Number expected").ThrowAsJavaScriptException();
//    return;
//  }

  //Napi::Number value = info[0].As<Napi::Number>();
  //this->value_ = value.DoubleValue();
}

void ZalWeb::SetDbPath(const Napi::CallbackInfo& info) 
{
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Did not receive path to the database.")
          .ThrowAsJavaScriptException();
        return;
    }

    auto rc = GetDictionary(m_pDictionary);
    if (rc != Hlib::H_NO_ERROR)
    {
        Napi::TypeError::New(env, "Error getting dictionary pointer.")
          .ThrowAsJavaScriptException();
    }

    auto sDbPath = Hlib::CEString::sFromUtf8(info[0].As<Napi::String>().Utf8Value());
    rc = m_pDictionary->eSetDbPath(sDbPath);
    if (rc != Hlib::H_NO_ERROR)
    {
        Napi::TypeError::New(env, "Error setting database path.")
          .ThrowAsJavaScriptException();
    }

/*    
    rc = m_pDictionary->eGetLexemesByInitialForm(L"мама");
    Hlib::ILexemeEnumerator * pLexEnum = nullptr;
    Hlib::ILexeme * pLexeme = nullptr;
    rc = pDict->eCreateLexemeEnumerator(pLexEnum);
    rc = pLexEnum->eGetFirstLexeme(pLexeme);
    while (rc == Hlib::H_NO_ERROR) {
        std::cout << Hlib::CEString::stl_sToUtf8(pLexeme->sGraphicStem()) << std::endl;
        rc = pLexEnum->eGetNextLexeme(pLexeme);
    }

    return Napi::String::New(env, "Generation ended.");
*/    
}

Napi::Value ZalWeb::GetLexemesByInitialForm(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
      Napi::TypeError::New(env, "Wrong number of arguments")
          .ThrowAsJavaScriptException();
      return Napi::Boolean::New(info.Env(), false);
    }

    if (!info[0].IsString()) {
       Napi::TypeError::New(env, "Did not receive search word.")
          .ThrowAsJavaScriptException();
      return Napi::Boolean::New(info.Env(), false);
    }

    auto sSearchWord = Hlib::CEString::sFromUtf8(info[0].As<Napi::String>().Utf8Value());
    auto rc = m_pDictionary->eGetLexemesByInitialForm(sSearchWord);
    if (rc != Hlib::H_NO_MORE && rc != Hlib::H_FALSE) {
      std::cout << "------------- Lexeme not found.\n";
      return Napi::Boolean::New(info.Env(), false);
    }

    rc = m_pDictionary->eCreateLexemeEnumerator(m_pLexemeEnumerator);
    if (rc != Hlib::H_NO_ERROR) {
       Napi::TypeError::New(env, "Unable to create lexeme enumerator.")
          .ThrowAsJavaScriptException();
      return Napi::Boolean::New(info.Env(), false);
    }
    rc = m_pLexemeEnumerator->eGetFirstLexeme(m_pCurrentLexeme);
    if (rc != Hlib::H_NO_ERROR) {
       Napi::TypeError::New(env, "Unable to load first lexeme.")
          .ThrowAsJavaScriptException();
      return Napi::Boolean::New(info.Env(), false);
    }

    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value ZalWeb::LoadNextLexeme(const Napi::CallbackInfo& info)
{
  auto rc = m_pLexemeEnumerator->eGetNextLexeme(m_pCurrentLexeme);
  auto bRet = (Hlib::H_NO_ERROR == rc) ? true : false;
  return Napi::Boolean::New(info.Env(), bRet);
}

Napi::Value ZalWeb::SetProperty(const Napi::CallbackInfo& info) 
{
  if (info.Length() <= 1) {
    Napi::TypeError::New(info.Env(), "Expected two arguments.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }
  else {
    auto kiki1 = info[0].As<Napi::String>();
    auto kiki2 = info[1].As<Napi::String>();

    std::cout << "------------- " << kiki1.Utf8Value() << std::endl;
    std::cout << "              " << kiki2.Utf8Value() << std::endl;

    return Napi::Boolean::New(info.Env(), true);
  }
}

Napi::Value ZalWeb::GetProperty(const Napi::CallbackInfo& info) 
{
  if (info.Length() < 1) {
    Napi::TypeError::New(info.Env(), "No argument.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }

  if (nullptr == m_pCurrentLexeme) {
    Napi::TypeError::New(info.Env(), "Lexeme pointer is NULL.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  auto stLexemeProperties = m_pCurrentLexeme->stGetProperties();

  auto sKey = Hlib::CEString::sFromUtf8(info[0].As<Napi::String>().Utf8Value());

  // homonyms
  if (L"homonyms" == sKey) {
    if (stLexemeProperties.vecHomonyms.size() > 0) {
      Napi::Int32Array arrHomonyms = Napi::Int32Array::New(info.Env(), stLexemeProperties.vecHomonyms.size());
      for (auto itHomonym = stLexemeProperties.vecHomonyms.begin(); 
           itHomonym != stLexemeProperties.vecHomonyms.end();
           ++itHomonym) {
          arrHomonyms[(size_t)(itHomonym-stLexemeProperties.vecHomonyms.begin())] = *itHomonym;
      }
      return arrHomonyms;
    } else {
      return Napi::Boolean::New(info.Env(), false);
    }
  }

  // contexts
  if (L"contexts" == sKey) {
    if (stLexemeProperties.sContexts.uiLength() > 0) {
      Napi::String sContexts = Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stLexemeProperties.sContexts));
      return sContexts;
    } else {
      return Napi::Boolean::New(info.Env(), false);
    }
  }

  // headword variant
  if (L"headwordVariant" == sKey) {
    if (stLexemeProperties.sHeadwordVariant.uiLength() > 0) {
      Napi::String sHeadwordVariant = Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stLexemeProperties.sHeadwordVariant));
      return sHeadwordVariant;
    } else {
      return Napi::Boolean::New(info.Env(), false);
    }
  }

  // spryazh. sm.
  if (L"spryazhSm" == sKey) {
    return Napi::Boolean::New(info.Env(), stLexemeProperties.bSpryazhSm);
  }

  return Napi::Boolean::New(info.Env(), true);
}

//Napi::Value ZalWeb::PlusOne(const Napi::CallbackInfo& info) 
//{
//  this->value_ = this->value_ + 1;
//  return ZalWeb::GetValue(info);
//}

/*
Napi::Value ZalWeb::Multiply(const Napi::CallbackInfo& info) 
{
  Napi::Number multiple;
  if (info.Length() <= 0 || !info[0].IsNumber()) {
    multiple = Napi::Number::New(info.Env(), 1);
  } else {
    multiple = info[0].As<Napi::Number>();
  }

  Napi::Object obj = info.Env().GetInstanceData<Napi::FunctionReference>()->New(
      {Napi::Number::New(info.Env(), this->value_ * multiple.DoubleValue())});

  return obj;
}
*/

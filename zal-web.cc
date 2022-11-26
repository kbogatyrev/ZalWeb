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
                                   InstanceMethod("loadNextLexeme", &ZalWeb::LoadNextLexeme),
                                   InstanceMethod("setProperty", &ZalWeb::SetProperty),
                                   InstanceMethod("getProperty", &ZalWeb::GetProperty) });
  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor);
  exports.Set("ZalWeb", func);
  return exports;
}

//
//  Property handlers
//
fnHandler fHomonyms = [](const Napi::CallbackInfo& info, Hlib::StLexemeProperties& stProps) -> Napi::Value 
{ 
  if (stProps.vecHomonyms.size() > 0) {
    Napi::Int32Array arrHomonyms = Napi::Int32Array::New(info.Env(), stProps.vecHomonyms.size());
    for (auto itHomonym = stProps.vecHomonyms.begin(); 
         itHomonym != stProps.vecHomonyms.end();
         ++itHomonym) 
    {
      arrHomonyms[(size_t)(itHomonym-stProps.vecHomonyms.begin())] = *itHomonym;
    }
    return arrHomonyms;
  } else {
    return Napi::Boolean::New(info.Env(), false);
  }
};

fnHandler fContexts = [](const Napi::CallbackInfo& info, Hlib::StLexemeProperties& stProps) -> Napi::Value 
{
  if (stProps.sContexts.uiLength() > 0) {
    Napi::String sContexts = Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sContexts));
    return sContexts;
  } else {
    return Napi::Boolean::New(info.Env(), false);
  }
};

fnHandler fHeadwordVariant = [](const Napi::CallbackInfo& info, Hlib::StLexemeProperties& stProps) -> Napi::Value 
{
  if (stProps.sHeadwordVariant.uiLength() > 0) {
    Napi::String sHeadwordVariant = Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sHeadwordVariant));
    return sHeadwordVariant;
  } else {
    return Napi::Boolean::New(info.Env(), false);
  }
};

fnHandler fSpryazhSm = [](const Napi::CallbackInfo& info, Hlib::StLexemeProperties& stProps) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), stProps.bSpryazhSm);
};

fnHandler fMainSymbol = [](const Napi::CallbackInfo& info, Hlib::StLexemeProperties& stProps) -> Napi::Value 
{
    return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sMainSymbol));
};


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

  m_mapKeyToPropHandler["homonyms"] = fHomonyms;
  m_mapKeyToPropHandler["contexts"] = fContexts;
  m_mapKeyToPropHandler["headwordVariant"] = fHeadwordVariant;
  m_mapKeyToPropHandler["spryazhSm"] = fSpryazhSm;
  m_mapKeyToPropHandler["mainSymbol"] = fMainSymbol;

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
       Napi::TypeError::New(env, "Unable to create lexeme enumerator.")
          .ThrowAsJavaScriptException();
    }

    if (Hlib::H_FALSE == rc) {
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
  auto sKey = info[0].As<Napi::String>().Utf8Value();

//  auto itProperty = m_mapStringToProperty.find(sKey);
//  if (m_mapStringToProperty.end() == itProperty) {
//    Napi::TypeError::New(info.Env(), "Property not found.").ThrowAsJavaScriptException();
//    return Napi::Boolean::New(info.Env(), false);   
//  }

  auto itProperty = m_mapKeyToPropHandler.find(sKey);
  if (m_mapKeyToPropHandler.end() == itProperty) {
    std::cout << "ERROR: property " << sKey << " not found.\n";
    return Napi::Boolean::New(info.Env(), false);
  }

  return itProperty->second(info, stLexemeProperties);

}

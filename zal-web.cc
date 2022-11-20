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
        return;
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Did not receive search word.")
          .ThrowAsJavaScriptException();
        return;
    }

    auto sSearchWord = Hlib::CEString::sFromUtf8(info[0].As<Napi::String>().Utf8Value());
    auto rc = m_pDictionary->eGetLexemesByInitialForm(sSearchWord);

//    Hlib::ILexeme * pLexeme = nullptr;
    rc = m_pDictionary->eCreateLexemeEnumerator(m_pLexemeEnumerator);
    rc = pLexEnum->eGetFirstLexeme(pLexeme);
//    while (rc == Hlib::H_NO_ERROR) {
//        std::cout << Hlib::CEString::stl_sToUtf8(pLexeme->sGraphicStem()) << std::endl;
//        rc = pLexEnum->eGetNextLexeme(pLexeme);
    }

    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value LoadNextLexeme(const Napi::CallbackInfo& info)
{

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

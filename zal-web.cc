#include <iostream>
#include "Enums.h"
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
fnHandler fnHomonyms = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{ 
  auto& stProps = pLexeme->stGetProperties();
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

fnHandler fnContexts = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  auto& stProps = pLexeme->stGetProperties();
  if (stProps.sContexts.uiLength() > 0) {
    Napi::String sContexts = Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sContexts));
    return sContexts;
  } else {
    return Napi::Boolean::New(info.Env(), false);
  }
};

fnHandler fnHeadwordVariant = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  auto& stProps = pLexeme->stGetProperties();
  if (stProps.sHeadwordVariant.uiLength() > 0) {
    Napi::String sHeadwordVariant = Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sHeadwordVariant));
    return sHeadwordVariant;
  } else {
    return Napi::Boolean::New(info.Env(), false);
  }
};

fnHandler fnSpryazhSm = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bSpryazhSm);
};

fnHandler fnMainSymbol = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  auto& stProps = pLexeme->stGetProperties();
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sMainSymbol));
};

fnHandler fnInflectionSymbol = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  auto& stProps = pLexeme->stGetProperties();
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sInflectionType));
};

fnHandler fnInflectionType = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Number::New(info.Env(), pLexeme->stGetProperties().iType);
};

fnHandler fnAccentType1 = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  auto eAt1 = pLexeme->stGetProperties().eAccentType1;
  auto itAt1String = MapAccentTypeToString.find(eAt1);
  if (itAt1String == MapAccentTypeToString.end()) {
    return Napi::Boolean::New(info.Env(), false);
  }
  std::string strAt1 = itAt1String->second;  
  return Napi::String::New(info.Env(), strAt1);
};

fnHandler fnAccentType2 = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  auto eAt2 = pLexeme->stGetProperties().eAccentType2;
  auto itAt2String = MapAccentTypeToString.find(eAt2);
  if (itAt2String == MapAccentTypeToString.end()) {
    return Napi::Boolean::New(info.Env(), false);
  }
  std::string strAt2 = itAt2String->second;  
  return Napi::String::New(info.Env(), strAt2);
};

fnHandler fnComment = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(pLexeme->stGetProperties().sComment));
};

fnHandler fnAspectPair = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  Hlib::CEString sAspectPair;
  int iStressPos = -1;
  auto rc = pLexeme->eGetAspectPair(sAspectPair, iStressPos);
  if (Hlib::H_NO_ERROR == rc) {
    return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(sAspectPair));
  } else {
    return Napi::Boolean::New(info.Env(), false);    
  }
};

fnHandler fnAltAspectPair = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  Hlib::CEString sAltAspectPair;
  int iStressPos = -1;
  auto rc = pLexeme->eGetAltAspectPair(sAltAspectPair, iStressPos);
  if (Hlib::H_NO_ERROR == rc) {
    return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(sAltAspectPair));
  } else {
    return Napi::Boolean::New(info.Env(), false);    
  }
};

fnHandler fnHeadwordComment = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  auto& stProps = pLexeme->stGetProperties();
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sHeadwordComment));
};

fnHandler fnIsPluralOf = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bIsPluralOf);
};

fnHandler fnPluralOf = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(pLexeme->stGetProperties().sPluralOf));
};

fnHandler fnUsage = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(pLexeme->stGetProperties().sUsage));
};

fnHandler fnSeeRef = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(pLexeme->stGetProperties().sSeeRef));
};

fnHandler fnStemAugment = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  return Napi::Number::New(info.Env(), pLexeme->stGetProperties().iStemAugment);
};

fnHandler fnTrailingComment = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(pLexeme->stGetProperties().sTrailingComment));
};

fnHandler fnRestrictedContexts = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(pLexeme->stGetProperties().sRestrictedContexts));
};

fnHandler fnCommonDeviations = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
  auto& stProps = pLexeme->stGetProperties();
  Napi::Int32Array arrCommonDeviations = Napi::Int32Array::New(info.Env(), 2*stProps.mapCommonDeviations.size());
  int iSize {0};
  for (auto& pairDeviation : stProps.mapCommonDeviations) {
    arrCommonDeviations[iSize++] = pairDeviation.first;
    arrCommonDeviations[iSize++] = pairDeviation.second;
  }
  if (iSize > 0) {
    return arrCommonDeviations;    
  } else {
    return Napi::Boolean::New(info.Env(), false);
  }
};

fnHandler fnSection = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Number::New(info.Env(), pLexeme->stGetProperties().iSection);
};

fnHandler fnFleetingVowel = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bFleetingVowel);
};

fnHandler fnHasYoAlternation = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bYoAlternation);
};

fnHandler fnNoComparative = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bNoComparative);
};

fnHandler fnAssumedForms = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bAssumedForms);
};

fnHandler fnHasIrregularForms = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bHasIrregularForms);
};

fnHandler fnHasIrregularVariants = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bHasIrregularVariants);
};

fnHandler fnShortFormsRestricted = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bShortFormsRestricted);
};

fnHandler fnShortFormsIncomplete = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bShortFormsIncomplete);
};

fnHandler fnNoLongForms = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bNoLongForms);
};

fnHandler fnPastParticipleRestricted = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bPastParticipleRestricted);
};

fnHandler fnNoPassivePastParticiple = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bNoPassivePastParticiple);
};

fnHandler fnDifficultForms = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->stGetProperties().bHasDifficultForms);
};

fnHandler fnSecondPart = [](const Napi::CallbackInfo& info, Hlib::ILexeme * pLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), pLexeme->bIsSecondPart());
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

  m_mapKeyToPropHandler["homonyms"] = fnHomonyms;
  m_mapKeyToPropHandler["contexts"] = fnContexts;
  m_mapKeyToPropHandler["headwordVariant"] = fnHeadwordVariant;
  m_mapKeyToPropHandler["spryazhSm"] = fnSpryazhSm;
  m_mapKeyToPropHandler["mainSymbol"] = fnMainSymbol;
  m_mapKeyToPropHandler["inflectionSymbol"] = fnInflectionSymbol;
  m_mapKeyToPropHandler["inflectionType"] = fnInflectionType;
  m_mapKeyToPropHandler["accentType1"] = fnAccentType1;
  m_mapKeyToPropHandler["accentType2"] = fnAccentType2;
  m_mapKeyToPropHandler["comment"] = fnComment;
  m_mapKeyToPropHandler["aspectPair"] = fnAspectPair;
  m_mapKeyToPropHandler["altAspectPair"] = fnAltAspectPair;
  m_mapKeyToPropHandler["isPluralOf"] = fnIsPluralOf;
  m_mapKeyToPropHandler["pluralOf"] = fnIsPluralOf;
  m_mapKeyToPropHandler["usage"] = fnUsage;
  m_mapKeyToPropHandler["seeRef"] = fnUsage;
  m_mapKeyToPropHandler["stemAugment"] = fnStemAugment;
  m_mapKeyToPropHandler["trailingComment"] = fnTrailingComment;
  m_mapKeyToPropHandler["restrictedContexts"] = fnRestrictedContexts;
  m_mapKeyToPropHandler["section"] = fnSection;
  m_mapKeyToPropHandler["fleetingVowel"] = fnFleetingVowel;
  m_mapKeyToPropHandler["hasYoAlternation"] = fnHasYoAlternation;
  m_mapKeyToPropHandler["noComparative"] = fnNoComparative;
  m_mapKeyToPropHandler["assumedForms"] = fnAssumedForms;
  m_mapKeyToPropHandler["hasIrregularForms"] = fnHasIrregularForms;
  m_mapKeyToPropHandler["hasIrregularVariants"] = fnHasIrregularVariants;
  m_mapKeyToPropHandler["shortFormsRestricted"] = fnShortFormsRestricted;
  m_mapKeyToPropHandler["shortFormsIncomplete"] = fnShortFormsIncomplete;
  m_mapKeyToPropHandler["noLongForms"] = fnNoLongForms;
  m_mapKeyToPropHandler["pastParticipleRestricted"] = fnPastParticipleRestricted;
  m_mapKeyToPropHandler["noPassivePastParticiple"] = fnNoPassivePastParticiple;
  m_mapKeyToPropHandler["difficultForms"] = fnDifficultForms;
  m_mapKeyToPropHandler["secondPart"] = fnSecondPart;
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

  return itProperty->second(info, m_pCurrentLexeme);

}

#include <iostream>
#include "Enums.h"
#include "EString.h"
#include "zal-web.h"

//extern "C"
//{
//  Hlib::ET_ReturnCode GetDictionary(shared_ptr<Hlib::CDictionary>&);
//}

Napi::Object ZalWeb::Init(Napi::Env env, Napi::Object exports) 
{
  Napi::Function func =
      DefineClass(env, "ZalWeb", { InstanceMethod("setDbPath", &ZalWeb::SetDbPath),
                                   InstanceMethod("getLexemesByInitialForm", &ZalWeb::GetLexemesByInitialForm),
                                   InstanceMethod("createLexemeEnumerator", &ZalWeb::CreateLexemeEnumerator),
                                   InstanceMethod("getFirstLexeme", &ZalWeb::GetFirstLexeme),
                                   InstanceMethod("getNextLexeme", &ZalWeb::GetNextLexeme),
                                   InstanceMethod("createInflectionEnumerator", &ZalWeb::CreateInflectionEnumerator),
                                   InstanceMethod("getFirstInflection", &ZalWeb::GetFirstInflection),
                                   InstanceMethod("getNextInflection", &ZalWeb::GetNextInflection),
                                   InstanceMethod("setLexemeProperty", &ZalWeb::SetLexemeProperty),
                                   InstanceMethod("getLexemeProperty", &ZalWeb::GetLexemeProperty),
                                   InstanceMethod("setInflectionProperty", &ZalWeb::SetInflectionProperty),
                                   InstanceMethod("getInflectionProperty", &ZalWeb::GetInflectionProperty) });
  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor);
  exports.Set("ZalWeb", func);
  return exports;
}

//
//  Property handlers
//
fnLexemeHandler fnSourceForm = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{ 
  Hlib::CEString sSource;
  bool bIsVariant = false;
  auto rc = spLexeme->eGetSourceFormWithDiacritics(sSource, bIsVariant);
  if (rc != Hlib::H_NO_ERROR) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve source form.")
      .ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(sSource));
};

fnLexemeHandler fnHomonyms = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{ 
  auto& stProps = spLexeme->stGetProperties();
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

fnLexemeHandler fnContexts = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  auto& stProps = spLexeme->stGetProperties();
  if (stProps.sContexts.uiLength() > 0) {
    Napi::String sContexts = Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sContexts));
    return sContexts;
  } else {
    return Napi::Boolean::New(info.Env(), false);
  }
};

fnLexemeHandler fnHeadwordVariant = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  auto& stProps = spLexeme->stGetProperties();
  if (stProps.sHeadwordVariant.uiLength() > 0) {
    Napi::String sHeadwordVariant = Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sHeadwordVariant));
    return sHeadwordVariant;
  } else {
    return Napi::Boolean::New(info.Env(), false);
  }
};

fnLexemeHandler fnSpryazhSm = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bSpryazhSm);
};

fnLexemeHandler fnMainSymbol = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  auto& stProps = spLexeme->stGetProperties();
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sMainSymbol));
};

fnLexemeHandler fnInflectionSymbol = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  auto& stProps = spLexeme->stGetProperties();
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sInflectionType));
};

fnLexemeHandler fnComment = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spLexeme->stGetProperties().sComment));
};

fnLexemeHandler fnAspectPair = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  Hlib::CEString sAspectPair;
  int iStressPos = -1;
  auto rc = spLexeme->eGetAspectPair(sAspectPair, iStressPos);
  if (Hlib::H_NO_ERROR == rc) {
    return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(sAspectPair));
  } else {
    return Napi::Boolean::New(info.Env(), false);    
  }
};

fnLexemeHandler fnAltAspectPair = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  Hlib::CEString sAltAspectPair;
  int iStressPos = -1;
  auto rc = spLexeme->eGetAltAspectPair(sAltAspectPair, iStressPos);
  if (Hlib::H_NO_ERROR == rc) {
    return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(sAltAspectPair));
  } else {
    return Napi::Boolean::New(info.Env(), false);    
  }
};

fnLexemeHandler fnHeadwordComment = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  auto& stProps = spLexeme->stGetProperties();
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sHeadwordComment));
};

fnLexemeHandler fnIsPluralOf = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bIsPluralOf);
};

fnLexemeHandler fnPluralOf = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spLexeme->stGetProperties().sPluralOf));
};

fnLexemeHandler fnUsage = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spLexeme->stGetProperties().sUsage));
};

fnLexemeHandler fnSeeRef = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spLexeme->stGetProperties().sSeeRef));
};

fnLexemeHandler fnTrailingComment = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spLexeme->stGetProperties().sTrailingComment));
};

fnLexemeHandler fnRestrictedContexts = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spLexeme->stGetProperties().sRestrictedContexts));
};

fnLexemeHandler fnSection = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Number::New(info.Env(), spLexeme->stGetProperties().iSection);
};

fnLexemeHandler fnHasYoAlternation = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bYoAlternation);
};

fnLexemeHandler fnNoComparative = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bNoComparative);
};

fnLexemeHandler fnAssumedForms = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bAssumedForms);
};

fnLexemeHandler fnHasIrregularForms = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bHasIrregularForms);
};

fnLexemeHandler fnHasIrregularVariants = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bHasIrregularVariants);
};

fnLexemeHandler fnNoLongForms = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bNoLongForms);
};

fnLexemeHandler fnDifficultForms = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bHasDifficultForms);
};

fnLexemeHandler fnSecondPart = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->bIsSecondPart());
};

//-------------------------------------------------------------------------------------------------------------------------------

fnInflectionHandler fnPrimaryInflectionGroup = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
  return Napi::Number::New(info.Env(), spInflection->stGetProperties().bPrimaryInflectionGroup);
};

fnInflectionHandler fnInflectionType = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
    return Napi::Number::New(info.Env(), spInflection->stGetProperties().iType);
};

fnInflectionHandler fnAccentType1 = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
  auto eAt1 = spInflection->stGetProperties().eAccentType1;
  auto itAt1String = MapAccentTypeToString.find(eAt1);
  if (itAt1String == MapAccentTypeToString.end()) {
    return Napi::Boolean::New(info.Env(), false);
  }
  std::string strAt1 = itAt1String->second;  
  return Napi::String::New(info.Env(), strAt1);
};

fnInflectionHandler fnAccentType2 = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
  auto eAt2 = spInflection->stGetProperties().eAccentType2;
  auto itAt2String = MapAccentTypeToString.find(eAt2);
  if (itAt2String == MapAccentTypeToString.end()) {
    return Napi::Boolean::New(info.Env(), false);
  }
  std::string strAt2 = itAt2String->second;  
  return Napi::String::New(info.Env(), strAt2);
};

fnInflectionHandler fnCommonDeviations = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
  auto& stProps = spInflection->stGetProperties();
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

fnInflectionHandler fnStemAugment = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
  return Napi::Number::New(info.Env(), spInflection->stGetProperties().iStemAugment);
};

fnInflectionHandler fnFleetingVowel = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spInflection->stGetProperties().bFleetingVowel);
};

fnInflectionHandler fnShortFormsRestricted = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spInflection->stGetProperties().bShortFormsRestricted);
};

fnInflectionHandler fnShortFormsIncomplete = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spInflection->stGetProperties().bShortFormsIncomplete);
};

fnInflectionHandler fnPastParticipleRestricted = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spInflection->stGetProperties().bPastParticipleRestricted);
};

fnInflectionHandler fnNoPassivePastParticiple = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spInflection->stGetProperties().bNoPassivePastParticiple);
};

// -------------------------------------------------------------------------------------------------------------------------

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

  m_mapKeyToLexemePropHandler["sourceForm"] = fnSourceForm;
  m_mapKeyToLexemePropHandler["homonyms"] = fnHomonyms;
  m_mapKeyToLexemePropHandler["contexts"] = fnContexts;
  m_mapKeyToLexemePropHandler["headwordVariant"] = fnHeadwordVariant;
  m_mapKeyToLexemePropHandler["spryazhSm"] = fnSpryazhSm;
  m_mapKeyToLexemePropHandler["mainSymbol"] = fnMainSymbol;
  m_mapKeyToLexemePropHandler["inflectionSymbol"] = fnInflectionSymbol;
  m_mapKeyToLexemePropHandler["comment"] = fnComment;
  m_mapKeyToLexemePropHandler["aspectPair"] = fnAspectPair;
  m_mapKeyToLexemePropHandler["altAspectPair"] = fnAltAspectPair;
  m_mapKeyToLexemePropHandler["headwordComment"] = fnHeadwordComment;
  m_mapKeyToLexemePropHandler["isPluralOf"] = fnIsPluralOf;
  m_mapKeyToLexemePropHandler["pluralOf"] = fnIsPluralOf;
  m_mapKeyToLexemePropHandler["usage"] = fnUsage;
  m_mapKeyToLexemePropHandler["seeRef"] = fnUsage;
  m_mapKeyToLexemePropHandler["trailingComment"] = fnTrailingComment;
  m_mapKeyToLexemePropHandler["restrictedContexts"] = fnRestrictedContexts;
  m_mapKeyToLexemePropHandler["section"] = fnSection;
  m_mapKeyToLexemePropHandler["hasYoAlternation"] = fnHasYoAlternation;
  m_mapKeyToLexemePropHandler["noComparative"] = fnNoComparative;
  m_mapKeyToLexemePropHandler["assumedForms"] = fnAssumedForms;
  m_mapKeyToLexemePropHandler["hasIrregularForms"] = fnHasIrregularForms;
  m_mapKeyToLexemePropHandler["hasIrregularVariants"] = fnHasIrregularVariants;
  m_mapKeyToLexemePropHandler["noLongForms"] = fnNoLongForms;
  m_mapKeyToLexemePropHandler["difficultForms"] = fnDifficultForms;
  m_mapKeyToLexemePropHandler["secondPart"] = fnSecondPart;

  m_mapKeyToInflectionPropHandler["primaryInflectionGroup"] = fnPrimaryInflectionGroup;
  m_mapKeyToInflectionPropHandler["commonDeviations"] = fnCommonDeviations;
  m_mapKeyToInflectionPropHandler["inflectionType"] = fnInflectionType;
  m_mapKeyToInflectionPropHandler["accentType1"] = fnAccentType1;
  m_mapKeyToInflectionPropHandler["accentType2"] = fnAccentType2;
  m_mapKeyToInflectionPropHandler["stemAugment"] = fnStemAugment;
  m_mapKeyToInflectionPropHandler["fleetingVowel"] = fnFleetingVowel;
  m_mapKeyToInflectionPropHandler["shortFormsRestricted"] = fnShortFormsRestricted;
  m_mapKeyToInflectionPropHandler["shortFormsIncomplete"] = fnShortFormsIncomplete;
  m_mapKeyToInflectionPropHandler["pastParticipleRestricted"] = fnPastParticipleRestricted;
  m_mapKeyToInflectionPropHandler["noPassivePastParticiple"] = fnNoPassivePastParticiple;

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

//    auto rc = GetDictionary(m_spDictionary);
//    if (rc != Hlib::H_NO_ERROR)
//    {
//        Napi::TypeError::New(env, "Error getting dictionary pointer.")
//          .ThrowAsJavaScriptException();
//    }

    auto rc = Hlib::Singleton::pGetInstance()->eGetDictionary(m_spDictionary);
    if (rc != Hlib::H_NO_ERROR || nullptr == m_spDictionary)
    {
        Napi::TypeError::New(env, "Error getting dictionary pointer.")
          .ThrowAsJavaScriptException();
    }


    auto sDbPath = Hlib::CEString::sFromUtf8(info[0].As<Napi::String>().Utf8Value());
    rc = m_spDictionary->eSetDbPath(sDbPath);
    if (rc != Hlib::H_NO_ERROR)
    {
        Napi::TypeError::New(env, "Error setting database path.")
          .ThrowAsJavaScriptException();
    }

/*    
    rc = m_spDictionary->eGetLexemesByInitialForm(L"мама");
    Hlib::ILexemeEnumerator * pLexEnum = nullptr;
    shared_ptr<Hlib::CLexeme> spLexeme = nullptr;
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
    auto rc = m_spDictionary->eGetLexemesByInitialForm(sSearchWord);
    if (rc != Hlib::H_NO_MORE && rc != Hlib::H_FALSE) {
       Napi::TypeError::New(env, "Unable to retrieve lexeme.")
          .ThrowAsJavaScriptException();
    }

    if (Hlib::H_FALSE == rc) {
      return Napi::Boolean::New(info.Env(), false);
    }

    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value ZalWeb::CreateLexemeEnumerator(const Napi::CallbackInfo& info)
{
    auto rc = m_spDictionary->eCreateLexemeEnumerator(m_spLexemeEnumerator);
    if (rc != Hlib::H_NO_ERROR) {
       Napi::TypeError::New(info.Env(), "Unable to create lexeme enumerator.")
          .ThrowAsJavaScriptException();
      return Napi::Boolean::New(info.Env(), false);
    }
    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value ZalWeb::GetFirstLexeme(const Napi::CallbackInfo& info)
{
    auto rc = m_spLexemeEnumerator->eGetFirstLexeme(m_spCurrentLexeme);
    if (rc != Hlib::H_NO_ERROR) {
       Napi::TypeError::New(info.Env(), "Unable to load first lexeme.")
          .ThrowAsJavaScriptException();
      return Napi::Boolean::New(info.Env(), false);
    }
    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value ZalWeb::GetNextLexeme(const Napi::CallbackInfo& info)
{
  auto rc = m_spLexemeEnumerator->eGetNextLexeme(m_spCurrentLexeme);
  auto bRet = (Hlib::H_NO_ERROR == rc) ? true : false;
  return Napi::Boolean::New(info.Env(), bRet);
}

Napi::Value ZalWeb::CreateInflectionEnumerator(const Napi::CallbackInfo& info)
{
    auto rc = m_spCurrentLexeme->eCreateInflectionEnumerator(m_spInflectionEnumerator);
    if (rc != Hlib::H_NO_ERROR) {
       Napi::TypeError::New(info.Env(), "Unable to create inflection enumerator.")
          .ThrowAsJavaScriptException();
      return Napi::Boolean::New(info.Env(), false);
    }
    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value ZalWeb::GetFirstInflection(const Napi::CallbackInfo& info)
{
    auto rc = m_spInflectionEnumerator->eGetFirstInflection(m_spCurrentInflection);
    if (rc != Hlib::H_NO_ERROR) {
       Napi::TypeError::New(info.Env(), "Unable to load first inflection.")
          .ThrowAsJavaScriptException();
      return Napi::Boolean::New(info.Env(), false);
    }

    m_mmapLexemeToInflections.insert(std::pair{m_spCurrentLexeme, m_spCurrentInflection});

    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value ZalWeb::GetNextInflection(const Napi::CallbackInfo& info)
{
  auto rc = m_spInflectionEnumerator->eGetNextInflection(m_spCurrentInflection);
  auto bRet = (Hlib::H_NO_ERROR == rc) ? true : false;
  if (bRet) {
    m_mmapLexemeToInflections.insert(std::pair{m_spCurrentLexeme, m_spCurrentInflection});
  }
  return Napi::Boolean::New(info.Env(), bRet);
}

Napi::Value ZalWeb::SetLexemeProperty(const Napi::CallbackInfo& info) 
{
  if (info.Length() <= 1) {
    Napi::TypeError::New(info.Env(), "Expected two arguments.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }
  else {
    return Napi::Boolean::New(info.Env(), true);
  }
}

Napi::Value ZalWeb::GetLexemeProperty(const Napi::CallbackInfo& info) 
{
  if (info.Length() < 1) {
    Napi::TypeError::New(info.Env(), "No argument.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }

  if (nullptr == m_spCurrentLexeme) {
    Napi::TypeError::New(info.Env(), "Lexeme pointer is NULL.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  auto stLexemeProperties = m_spCurrentLexeme->stGetProperties();
  auto sKey = info[0].As<Napi::String>().Utf8Value();

//  auto itProperty = m_mapStringToProperty.find(sKey);
//  if (m_mapStringToProperty.end() == itProperty) {
//    Napi::TypeError::New(info.Env(), "Property not found.").ThrowAsJavaScriptException();
//    return Napi::Boolean::New(info.Env(), false);   
//  }

  auto itProperty = m_mapKeyToLexemePropHandler.find(sKey);
  if (m_mapKeyToLexemePropHandler.end() == itProperty) {
    std::cout << "ERROR: property " << sKey << " not found.\n";
    return Napi::Boolean::New(info.Env(), false);
  }

  return itProperty->second(info, m_spCurrentLexeme);

}

Napi::Value ZalWeb::SetInflectionProperty(const Napi::CallbackInfo& info) 
{
  if (info.Length() <= 1) {
    Napi::TypeError::New(info.Env(), "Expected two arguments.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }
  else {
    return Napi::Boolean::New(info.Env(), true);
  }
}

Napi::Value ZalWeb::GetInflectionProperty(const Napi::CallbackInfo& info) 
{
  if (info.Length() < 1) {
    Napi::TypeError::New(info.Env(), "No argument.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }

  if (nullptr == m_spCurrentInflection) {
    Napi::TypeError::New(info.Env(), "Inflection pointer is NULL.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  auto stInflectionProperties = m_spCurrentInflection->stGetProperties();
  auto sKey = info[0].As<Napi::String>().Utf8Value();

//  auto itProperty = m_mapStringToProperty.find(sKey);
//  if (m_mapStringToProperty.end() == itProperty) {
//    Napi::TypeError::New(info.Env(), "Property not found.").ThrowAsJavaScriptException();
//    return Napi::Boolean::New(info.Env(), false);   
//  }

  auto itProperty = m_mapKeyToInflectionPropHandler.find(sKey);
  if (m_mapKeyToInflectionPropHandler.end() == itProperty) {
    std::cout << "ERROR: inflection property " << sKey << " not found.\n";
    return Napi::Boolean::New(info.Env(), false);
  }

  return itProperty->second(info, m_spCurrentInflection);

}

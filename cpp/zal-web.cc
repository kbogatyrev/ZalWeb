#include <iostream>
#include <sstream>
//#include <napi.h>
#include "Logging.h"
#include "Enums.h"
#include "EString.h"
#include "Singleton.h"
#include "Lexeme.h"
#include "Inflection.h"
#include "WordForm.h"
#include "Analytics.h"
#include "zal-web.h"

Napi::Object InitAll(Napi::Env env, Napi::Object exports) {
  return ZalWeb::Init(env, exports);
}

Napi::Object ZalWeb::Init(Napi::Env env, Napi::Object exports) 
{
  Napi::Function func =
      DefineClass(env, "ZalWeb", { InstanceMethod("setDbPath", &ZalWeb::SetDbPath),
                                   InstanceMethod("clear", &ZalWeb::Clear),
                                   InstanceMethod("wordQuery", &ZalWeb::WordQuery),
//                                   InstanceMethod("getLexemesByInitialForm", &ZalWeb::GetLexemesByInitialForm),
//                                   InstanceMethod("loadFirstLexeme", &ZalWeb::LoadFirstLexeme),
//                                   InstanceMethod("loadNextLexeme", &ZalWeb::LoadNextLexeme),
                                   InstanceMethod("getLexemeId", &ZalWeb::GetLexemeId),
//                                   InstanceMethod("loadFirstInflection", &ZalWeb::LoadFirstInflection),
//                                   InstanceMethod("loadNextInflection", &ZalWeb::LoadNextInflection),
//                                   InstanceMethod("getFirstWordForm", &ZalWeb::GetFirstWordForm),
//                                   InstanceMethod("getNextWordForm", &ZalWeb::GetNextWordForm),
//                                   InstanceMethod("setLexemeProperty", &ZalWeb::SetLexemeProperty),
                                   InstanceMethod("getLexemeProperty", &ZalWeb::GetLexemeProperty),
//                                   InstanceMethod("setInflectionProperty", &ZalWeb::SetInflectionProperty),
                                   InstanceMethod("getInflectionId", &ZalWeb::GetInflectionId),
                                   InstanceMethod("getInflectionProperty", &ZalWeb::GetInflectionProperty),
                                   InstanceMethod("generateParadigm", &ZalWeb::GenerateParadigm),
//                                   InstanceMethod("setWordFormProperty", &ZalWeb::SetWordFormProperty),
//                                   InstanceMethod("getWordFormProperty", &ZalWeb::GetWordFormProperty),
                                   InstanceMethod("loadFirstSegment", &ZalWeb::LoadFirstSegment),
                                   InstanceMethod("loadNextSegment", &ZalWeb::LoadNextSegment),
                                   InstanceMethod("segmentSize", &ZalWeb::SegmentSize),
                                   InstanceMethod("getWordInTextProperty", &ZalWeb::GetWordInTextProperty) });
  Napi::FunctionReference* constructor = new Napi::FunctionReference();
  *constructor = Napi::Persistent(func);
  env.SetInstanceData(constructor);
  exports.Set("ZalWeb", func);
  return exports;
}

//
//  Property handlers
//
fnHandlerLexeme fnLexemeId = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{ 
  auto llId = spLexeme->llLexemeId();
  return Napi::String::New(info.Env(), std::to_string(llId));
};

fnHandlerLexeme fnSourceForm = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{ 
  Hlib::CEString sSource;
  bool bIsVariant = false;
  auto rc = spLexeme->eGetSourceFormWithDiacritics(sSource, bIsVariant);
  if (rc != Hlib::H_NO_ERROR) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve source form.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(sSource));
};

// sHeadwordComment

fnHandlerLexeme fnHeadwordVariant = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  auto& stProps = spLexeme->stGetProperties();
  if (stProps.sHeadwordVariant.uiLength() > 0) {
    Napi::String sHeadwordVariant = Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sHeadwordVariant));
    return sHeadwordVariant;
  } else {
    return Napi::Boolean::New(info.Env(), false);
  }
};

// sHeadwordVariantComment

fnHandlerLexeme fnHomonyms = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
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

fnHandlerLexeme fnMainSymbol = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  auto& stProps = spLexeme->stGetProperties();
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sMainSymbol));
};

fnHandlerLexeme fnPartOfSpeechLexeme = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value
{
  auto ePos = spLexeme-> ePartOfSpeech();
  auto itPosString = MapPosToString.find(ePos);
  if (itPosString == MapPosToString.end()) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve source part of speech value.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::String::New(info.Env(), itPosString->second);
};

fnHandlerLexeme fnIsTransitive = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  auto& stProps = spLexeme->stGetProperties();
  return Napi::Boolean::New(info.Env(), stProps.bTransitive);
};

fnHandlerLexeme fnInflectionSymbol = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  auto& stProps = spLexeme->stGetProperties();
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sInflectionType));
};

fnHandlerLexeme fnIsPluralOf = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bIsPluralOf);
};

fnHandlerLexeme fnPluralOf = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spLexeme->stGetProperties().sPluralOf));
};

fnHandlerLexeme fnSection = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Number::New(info.Env(), spLexeme->stGetProperties().iSection);
};
fnHandlerLexeme fnSpryazhSm = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bSpryazhSm);
};

fnHandlerLexeme fnSpryazhSmRef = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    auto& stProps = spLexeme->stGetProperties();
    return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sSpryazhSmRefSource));
};

fnHandlerLexeme fnContexts = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  auto& stProps = spLexeme->stGetProperties();
  if (stProps.sContexts.uiLength() > 0) {
    Napi::String sContexts = Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sContexts));
    return sContexts;
  } else {
    return Napi::Boolean::New(info.Env(), false);
  }
};

fnHandlerLexeme fnComment = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spLexeme->stGetProperties().sComment));
};

fnHandlerLexeme fnHeadwordComment = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  auto& stProps = spLexeme->stGetProperties();
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(stProps.sHeadwordComment));
};

fnHandlerLexeme fnUsage = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spLexeme->stGetProperties().sUsage));
};

fnHandlerLexeme fnSeeRef = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spLexeme->stGetProperties().sSeeRef));
};

fnHandlerLexeme fnTrailingCommentLexeme = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spLexeme->stGetProperties().sTrailingComment));
};

fnHandlerLexeme fnRestrictedContexts = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spLexeme->stGetProperties().sRestrictedContexts));
};

fnHandlerLexeme fnHasYoAlternation = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme>  spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bYoAlternation);
};

fnHandlerLexeme fnNoComparative = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bNoComparative);
};

fnHandlerLexeme fnAssumedForms = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bAssumedForms);
};

fnHandlerLexeme fnHasIrregularForms = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bHasIrregularForms);
};

fnHandlerLexeme fnHasIrregularVariants = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bHasIrregularVariants);
};

fnHandlerLexeme fnNoLongForms = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bNoLongForms);
};

fnHandlerLexeme fnDifficultForms = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->stGetProperties().bHasDifficultForms);
};

fnHandlerLexeme fnSecondPart = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme> spLexeme) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spLexeme->bIsSecondPart());
};

// --------------------------------------------------------------------------------------------------------------------------------------------

fnHandlerInflection fnInflectionId = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
  auto llId = spInflection->llInflectionId();
  return Napi::String::New(info.Env(), std::to_string(llId));
};

fnHandlerInflection fnShortFormsRestricted = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spInflection->stGetProperties().bShortFormsRestricted);
};

fnHandlerInflection fnShortFormsIncomplete = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spInflection->stGetProperties().bShortFormsIncomplete);
};

fnHandlerInflection fnCommonDeviations = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
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

fnHandlerInflection fnAspectPair = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
  Hlib::CEString sAspectPair;
  int iStressPos = -1;
  auto rc = spInflection->eGetAspectPair(sAspectPair, iStressPos);
  if (Hlib::H_NO_ERROR == rc && sAspectPair.uiLength() > 0) {
    sAspectPair.sInsert(iStressPos, Hlib::CEString::g_chrCombiningAcuteAccent);
    return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(sAspectPair));
  } else {
    return Napi::Boolean::New(info.Env(), false);    
  }
};

fnHandlerInflection fnAltAspectPair = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
  Hlib::CEString sAltAspectPair;
  int iStressPos = -1;
  auto rc = spInflection->eGetAltAspectPair(sAltAspectPair, iStressPos);
  if (Hlib::H_NO_ERROR == rc) {
    return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(sAltAspectPair));
  } else {
    return Napi::Boolean::New(info.Env(), false);    
  }
};

fnHandlerInflection fnInflectionType = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
    return Napi::Number::New(info.Env(), spInflection->stGetProperties().iType);
};

fnHandlerInflection fnAccentType1 = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
  auto eAt1 = spInflection->stGetProperties().eAccentType1;
  auto itAt1String = MapAccentTypeToString.find(eAt1);
  if (itAt1String == MapAccentTypeToString.end()) {
    return Napi::Boolean::New(info.Env(), false);
  }
  std::string strAt1 = itAt1String->second;  
  return Napi::String::New(info.Env(), strAt1);
};

fnHandlerInflection fnAccentType2 = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
  auto eAt2 = spInflection->stGetProperties().eAccentType2;
  auto itAt2String = MapAccentTypeToString.find(eAt2);
  if (itAt2String == MapAccentTypeToString.end()) {
    return Napi::Boolean::New(info.Env(), false);
  }
  std::string strAt2 = itAt2String->second;  
  return Napi::String::New(info.Env(), strAt2);
};

fnHandlerInflection fnStemAugment = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
  return Napi::Number::New(info.Env(), spInflection->stGetProperties().iStemAugment);
};

fnHandlerInflection fnHasFleetingVowel = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spInflection->stGetProperties().bFleetingVowel);
};

fnHandlerInflection fnPastParticipleRestricted = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spInflection->stGetProperties().bPastParticipleRestricted);
};

fnHandlerInflection fnNoPassivePastParticiple = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection> spInflection) -> Napi::Value 
{
    return Napi::Boolean::New(info.Env(), spInflection->stGetProperties().bNoPassivePastParticiple);
};

// -------------------------------------------------------------------------------------------------------------------------------------------------

fnHandlerWordForm fnWordForm = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  Hlib::CEString sWordForm;
  auto rc = spWordForm->eGetFormWithDiacritics(sWordForm);
  if (rc != Hlib::H_NO_ERROR) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve word form.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(sWordForm));
};

/*
fnHandlerWordForm fnStem = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
    return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spWordForm->sStem()));
};

fnHandlerWordForm fnEnding = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
    return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spWordForm->sEnding()));
};
*/

fnHandlerWordForm fnPartOfSpeech = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  auto ePos = spWordForm->ePos();
  auto itPosString = MapPosToString.find(ePos);
  if (itPosString == MapPosToString.end()) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve source part of speech value.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::String::New(info.Env(), itPosString->second);
};

fnHandlerWordForm fnCase = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  auto eCase = spWordForm->eCase();
  auto itCaseString = MapCaseToString.find(eCase);
  if (itCaseString == MapCaseToString.end()) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve case value.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::String::New(info.Env(), itCaseString->second);
};

fnHandlerWordForm fnSubParadigm = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  auto eSubparadigm = spWordForm->eSubparadigm();
  auto itSubparadigmString = MapSubparadigmToString.find(eSubparadigm);
  if (itSubparadigmString == MapSubparadigmToString.end()) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve subparadigm value.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::String::New(info.Env(), itSubparadigmString->second);
};

fnHandlerWordForm fnNumber = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  auto eNumber = spWordForm->eNumber();
  auto itNumberString = MapNumberToString.find(eNumber);
  if (itNumberString == MapNumberToString.end()) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve number value.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::String::New(info.Env(), itNumberString->second);
};

fnHandlerWordForm fnGender = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  auto eGender = spWordForm->eGender();
  auto itGenderString = MapGenderToString.find(eGender);
  if (itGenderString == MapGenderToString.end()) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve gender value.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::String::New(info.Env(), itGenderString->second);
};

fnHandlerWordForm fnPerson = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  auto ePerson = spWordForm->ePerson();
  auto itPersonString = MapPersonToString.find(ePerson);
  if (itPersonString == MapPersonToString.end()) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve person value.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::String::New(info.Env(), itPersonString->second);
};

fnHandlerWordForm fnAnimacy = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  auto eAnimacy = spWordForm->eAnimacy();
  auto itAnimacyString = MapAnimacyToString.find(eAnimacy);
  if (itAnimacyString == MapAnimacyToString.end()) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve animacy value.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::String::New(info.Env(), itAnimacyString->second);
};

/*
fnHandlerWordForm fnReflexivity = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  auto eReflexivity = spWordForm->eReflexive();
  auto itReflexivityString = MapReflexivityToString.find(eReflexivity);
  if (itReflexivityString == MapReflexivityToString.end()) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve reflexivity value.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::String::New(info.Env(), itReflexivityString->second);
};

fnHandlerWordForm fnAspect = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  auto eAspect = spWordForm->eAspect();
  auto itAspectString = MapAspectToString.find(eAspect);
  if (itAspectString == MapAspectToString.end()) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve aspect value.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::String::New(info.Env(), itAspectString->second);
};
*/

fnHandlerWordForm fnStatus = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  auto eStatus = spWordForm->eStatus();
  auto itStatusString = MapStatusToString.find(eStatus);
  if (itStatusString == MapStatusToString.end()) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve status value.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::String::New(info.Env(), itStatusString->second);
};

fnHandlerWordForm fnIsIrregular = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  return Napi::Boolean::New(info.Env(), spWordForm->bIrregular());
};

fnHandlerWordForm fnIsVariant = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  return Napi::Boolean::New(info.Env(), spWordForm->bIsVariant());
};

fnHandlerWordForm fnIsDifficult = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  auto sGramHash = spWordForm->sGramHash();
//  &&&&
  return Napi::Boolean::New(info.Env(), false);
};

/*
fnHandlerWordForm fnLeadCommentWordForm = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spWordForm->sLeadComment()));
};

fnHandlerWordForm fnTrailingCommentWordForm = [](const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm> spWordForm) -> Napi::Value
{
  return Napi::String::New(info.Env(), Hlib::CEString::stl_sToUtf8(spWordForm->sTrailingComment()));
};
*/

// -------------------------------------------------------------------------------------------

fnHandlerWordInText fnIncompleteParse = [](const Napi::CallbackInfo& info, Hlib::StWordContext stWord) -> Napi::Value
{
  return Napi::Boolean::New(info.Env(), stWord.bIncomplete);
};

fnHandlerWordInText fnLineBreak = [](const Napi::CallbackInfo& info, Hlib::StWordContext stWord) -> Napi::Value
{
  return Napi::Boolean::New(info.Env(), stWord.bBreak);
};

fnHandlerWordInText fnSegmentId = [](const Napi::CallbackInfo& info, Hlib::StWordContext stWord) -> Napi::Value
{
  return Napi::Number::New(info.Env(), stWord.llSegmentId);
};

fnHandlerWordInText fnSeqNum = [](const Napi::CallbackInfo& info, Hlib::StWordContext stWord) -> Napi::Value
{
  return Napi::Number::New(info.Env(), stWord.iSeqNum);
};

fnHandlerWordInText fnWordSource = [](const Napi::CallbackInfo& info, Hlib::StWordContext stWord) -> Napi::Value
{
  return Napi::String::New(info.Env(), stWord.sWord.stl_sToUtf8());
};

fnHandlerWordInText fnGramHash = [](const Napi::CallbackInfo& info, Hlib::StWordContext stWord) -> Napi::Value
{
  return Napi::String::New(info.Env(), stWord.sGramHash.stl_sToUtf8());
};

// -------------------------------------------------------------------------------------------

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

  auto pSingleton = Hlib::Singleton::pGetInstance();
  auto rc = pSingleton->eGetDictionary(m_spDictionary);
  if (rc != Hlib::H_NO_ERROR)
  {
      Napi::TypeError::New(info.Env(), "Error getting dictionary pointer.").ThrowAsJavaScriptException();
  }

//  Hlib::CEString sDbPath = L"/home/konstantin/Zal-Web/data/ZalData_Master.db3";
//  auto ret = m_spDictionary->eSetDbPath(sDbPath);

//  rc = m_spDictionary->eGetAnalytics(m_spAnalytics);
//  if (rc != Hlib::H_NO_ERROR || !m_spAnalytics)
//  {
//      Napi::TypeError::New(info.Env(), "Error accessing analytics module.").ThrowAsJavaScriptException();
//  }

  m_mapKeyToLexemePropHandler["lexemeId"] = fnLexemeId;
  m_mapKeyToLexemePropHandler["sourceForm"] = fnSourceForm;
  m_mapKeyToLexemePropHandler["homonyms"] = fnHomonyms;
  m_mapKeyToLexemePropHandler["contexts"] = fnContexts;
  m_mapKeyToLexemePropHandler["headwordVariant"] = fnHeadwordVariant;
  m_mapKeyToLexemePropHandler["spryazhSm"] = fnSpryazhSm;
  m_mapKeyToLexemePropHandler["spryazhSmRef"] = fnSpryazhSmRef;
  m_mapKeyToLexemePropHandler["mainSymbol"] = fnMainSymbol;
  m_mapKeyToLexemePropHandler["partOfSpeech"] = fnPartOfSpeechLexeme;
  m_mapKeyToLexemePropHandler["isTransitive"] = fnIsTransitive;
  m_mapKeyToLexemePropHandler["inflectionSymbol"] = fnInflectionSymbol;
  m_mapKeyToLexemePropHandler["comment"] = fnComment;
  m_mapKeyToLexemePropHandler["headwordComment"] = fnHeadwordComment;
  m_mapKeyToLexemePropHandler["isPluralOf"] = fnIsPluralOf;
  m_mapKeyToLexemePropHandler["pluralOf"] = fnIsPluralOf;
  m_mapKeyToLexemePropHandler["usage"] = fnUsage;
  m_mapKeyToLexemePropHandler["seeRef"] = fnUsage;
  m_mapKeyToLexemePropHandler["trailingComment"] = fnTrailingCommentLexeme;
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

  m_mapKeyToInflectionPropHandler["inflectionId"] = fnInflectionId;
  m_mapKeyToInflectionPropHandler["inflectionType"] = fnInflectionType;
  m_mapKeyToInflectionPropHandler["accentType1"] = fnAccentType1;
  m_mapKeyToInflectionPropHandler["accentType2"] = fnAccentType2;
  m_mapKeyToInflectionPropHandler["aspectPair"] = fnAspectPair;
  m_mapKeyToInflectionPropHandler["altAspectPair"] = fnAltAspectPair;
  m_mapKeyToInflectionPropHandler["stemAugment"] = fnStemAugment;
  m_mapKeyToInflectionPropHandler["commonDeviations"] = fnCommonDeviations;
  m_mapKeyToInflectionPropHandler["hasFleetingVowel"] = fnHasFleetingVowel;
  m_mapKeyToInflectionPropHandler["shortFormsRestricted"] = fnShortFormsRestricted;
  m_mapKeyToInflectionPropHandler["shortFormsIncomplete"] = fnShortFormsIncomplete;
  m_mapKeyToInflectionPropHandler["pastParticipleRestricted"] = fnPastParticipleRestricted;
  m_mapKeyToInflectionPropHandler["noPassivePastParticiple"] = fnNoPassivePastParticiple;

  m_mapKeyToWordFormPropHandler["wordForm"] = fnWordForm;
//  m_mapKeyToWordFormPropHandler["stem"] = fnStem;
//  m_mapKeyToWordFormPropHandler["ending"] = fnEnding;
  m_mapKeyToWordFormPropHandler["partOfSpeech"] = fnPartOfSpeech;
  m_mapKeyToWordFormPropHandler["case"] = fnCase;
  m_mapKeyToWordFormPropHandler["subParadigm"] = fnSubParadigm;
  m_mapKeyToWordFormPropHandler["number"] = fnNumber;
  m_mapKeyToWordFormPropHandler["gender"] = fnGender;
  m_mapKeyToWordFormPropHandler["person"] = fnPerson;
  m_mapKeyToWordFormPropHandler["animacy"] = fnAnimacy;
//  m_mapKeyToWordFormPropHandler["reflexivity"] = fnReflexivity;
//  m_mapKeyToWordFormPropHandler["aspect"] = fnAspect;
  m_mapKeyToWordFormPropHandler["status"] = fnStatus;
  m_mapKeyToWordFormPropHandler["isIrregular"] = fnIsIrregular;
  m_mapKeyToWordFormPropHandler["isVariant"] = fnIsVariant;
  m_mapKeyToWordFormPropHandler["isDifficult"] = fnIsDifficult;
//  m_mapKeyToWordFormPropHandler["leadComment"] = fnLeadCommentWordForm;
//  m_mapKeyToWordFormPropHandler["trailingComment"] = fnTrailingCommentWordForm;

  m_mapKeyToWordInTextHandler["incompleteParse"] = fnIncompleteParse;
  m_mapKeyToWordInTextHandler["lineBreak"] = fnLineBreak;
  m_mapKeyToWordInTextHandler["segmentId"] = fnSegmentId;
  m_mapKeyToWordInTextHandler["seqNum"] = fnSeqNum;
  m_mapKeyToWordInTextHandler["wordSource"] = fnWordSource;
  m_mapKeyToWordInTextHandler["gramHash"] = fnGramHash;
}

void ZalWeb::SetDbPath(const Napi::CallbackInfo& info) 
{
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
        Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString()) {
        Napi::TypeError::New(env, "Did not receive path to the database.").ThrowAsJavaScriptException();
        return;
    }

    auto sDbPath = Hlib::CEString::sFromUtf8(info[0].As<Napi::String>().Utf8Value());
    auto rc = m_spDictionary->eSetDbPath(sDbPath);
    if (rc != Hlib::H_NO_ERROR)
    {
        Napi::TypeError::New(env, "Error setting database path.").ThrowAsJavaScriptException();
    }
}

void ZalWeb::Clear(const Napi::CallbackInfo& info)
{
    if (info.Length() != 0) {
        Napi::TypeError::New(info.Env(), "Wrong number of arguments.").ThrowAsJavaScriptException();
        return;
    }
    m_spDictionary->Clear();
}

bool ZalWeb::bLoadInflections(const Napi::CallbackInfo& info, std::shared_ptr<Hlib::CLexeme> spLexeme) {
  auto rc = spLexeme->eCreateInflectionEnumerator(m_spInflectionEnumerator);
  if (rc != Hlib::H_NO_ERROR)
  {
    Napi::TypeError::New(info.Env(), "Unable to create inflection enumerator.").ThrowAsJavaScriptException();
    return false;
  }

  std::shared_ptr<Hlib::CInflection> spInflection;
  rc = m_spInflectionEnumerator->eGetFirstInflection(spInflection);
  if (!spInflection || rc != Hlib::H_NO_ERROR)
  {
    Napi::TypeError::New(info.Env(), "Unable to enumerate inflections.").ThrowAsJavaScriptException();
    return false;
  }

  while (Hlib::H_NO_ERROR == rc && spLexeme != nullptr) {
    m_mapInflectionIdToInflectionObj[spInflection->llInflectionId()] = spInflection;
    m_mmapLexemeIdToInflectionIds.insert(make_pair(spLexeme->llLexemeId(), spInflection->llInflectionId()));
    rc = m_spInflectionEnumerator->eGetNextInflection(spInflection);
    if (rc != Hlib::H_NO_ERROR && rc != Hlib::H_FALSE && rc != Hlib::H_NO_MORE) {
      Napi::TypeError::New(info.Env(), "Error retrieving next inflection.").ThrowAsJavaScriptException();
      return false;
    }
  }
  return true;

}   //  bLoadInflections()

Napi::Value ZalWeb::WordQuery(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() != 1) {
      Napi::TypeError::New(env, "Expected one argument.").ThrowAsJavaScriptException();
      return Napi::Boolean::New(info.Env(), false);
    }

    if (!info[0].IsString()) {
       Napi::TypeError::New(env, "Did not receive search word.").ThrowAsJavaScriptException();
      return Napi::Boolean::New(info.Env(), false);
    }

    auto sSearchWord = Hlib::CEString::sFromUtf8(info[0].As<Napi::String>().Utf8Value());

    auto pairIdRange = m_mmapWordToLexemeIds.equal_range(sSearchWord);
    if (pairIdRange.first != pairIdRange.second) {
      for (auto it = pairIdRange.first; it != pairIdRange.second; ++it) {
        m_mapLexemeStatus[(*it).second] = ET_LexemeStatus::LexemeStatusNew;
      }
      return Napi::Boolean::New(info.Env(), true);  
    }

    auto rc = m_spDictionary->eGetLexemesByInitialForm(sSearchWord);
    if (rc != Hlib::H_NO_MORE && rc != Hlib::H_FALSE) {
       Napi::TypeError::New(env, "Lexeme lookup failed.").ThrowAsJavaScriptException();
    }

    if (Hlib::H_FALSE == rc) {
      return Napi::Boolean::New(info.Env(), false);
    }

    rc = m_spDictionary->eCreateLexemeEnumerator(m_spLexemeEnumerator);
    if (rc != Hlib::H_NO_ERROR) {
        Napi::TypeError::New(info.Env(), "Unable to create lexeme enumerator.")
            .ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false);
    }

    std::shared_ptr<Hlib::CLexeme> spLexeme;
    rc = m_spLexemeEnumerator->eGetFirstLexeme(spLexeme);
    if (rc != Hlib::H_NO_ERROR || nullptr == spLexeme) {
        Napi::TypeError::New(info.Env(), "Unable to load first lexeme.")
            .ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false);
    }
    while (Hlib::H_NO_ERROR == rc && spLexeme != nullptr) {
      m_mapLexemeIdToLexemeObj[spLexeme->llLexemeId()] = spLexeme;
      if (!bLoadInflections(info, spLexeme)) {
        return Napi::Boolean::New(info.Env(), false);
      }
      m_mmapWordToLexemeIds.insert(make_pair(sSearchWord, spLexeme->llLexemeId()));
      m_mapLexemeStatus.insert(make_pair(spLexeme->llLexemeId(), ET_LexemeStatus::LexemeStatusNew));
      rc = m_spLexemeEnumerator->eGetNextLexeme(spLexeme);
      if (rc != Hlib::H_NO_ERROR && rc != Hlib::H_NO_MORE) {
        Napi::TypeError::New(info.Env(), "Unable to acquire lexeme.")
            .ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false);
      }
    }
    return Napi::Boolean::New(info.Env(), true);
}   //  WordQuery()

Napi::Value ZalWeb::GetLexemeId(const Napi::CallbackInfo& info)
{
  if (info.Length() != 1) {
    Napi::TypeError::New(info.Env(), "Wrong number of arguments").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  if (!info[0].IsNumber()) {
    Napi::TypeError::New(info.Env(), "Lexeme index must be numeric.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  int iIdx = info[0].As<Napi::Number>().ToNumber();
  int iCount = 0;
  auto it = m_mapLexemeStatus.begin();
  for (; it != m_mapLexemeStatus.end(); ++it) {
    if (ET_LexemeStatus::LexemeStatusNew == (*it).second) {
      if (iCount++ == iIdx) {
        return Napi::Number::New(info.Env(), (*it).first);
      }      
    }
  }

  // End of enumeration
  if (m_mapLexemeStatus.end() == it) {
    for (it = m_mapLexemeStatus.begin(); it != m_mapLexemeStatus.end(); ++it) {
      (*it).second = ET_LexemeStatus::LexemeStatusDescriptor;
    }
  }
  return Napi::Boolean::New(info.Env(), false); 
}   // GetLexemeId()


/*
Napi::Value ZalWeb::GetLexemesByInitialForm(const Napi::CallbackInfo& info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1) {
      Napi::TypeError::New(env, "Wrong number of arguments").ThrowAsJavaScriptException();
      return Napi::Boolean::New(info.Env(), false);
    }

    if (!info[0].IsString()) {
       Napi::TypeError::New(env, "Did not receive search word.").ThrowAsJavaScriptException();
      return Napi::Boolean::New(info.Env(), false);
    }

    auto sSearchWord = Hlib::CEString::sFromUtf8(info[0].As<Napi::String>().Utf8Value());
    auto rc = m_spDictionary->eGetLexemesByInitialForm(sSearchWord);
    if (rc != Hlib::H_NO_MORE && rc != Hlib::H_FALSE) {
       Napi::TypeError::New(env, "Unable to create lexeme enumerator.").ThrowAsJavaScriptException();
    }

    if (Hlib::H_FALSE == rc) {
      return Napi::Boolean::New(info.Env(), false);
    }

    auto rc = m_spDictionary->eCreateLexemeEnumerator(m_spLexemeEnumerator);
    if (rc != Hlib::H_NO_ERROR) {
        Napi::TypeError::New(info.Env(), "Unable to create lexeme enumerator.")
            .ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false);
    }

    std::shared_ptr<Hlib::CLexeme> spLexeme;
    rc = m_spLexemeEnumerator->eGetFirstLexeme(spLexeme);
    if (rc != Hlib::H_NO_ERROR || nullptr == spLexeme) {
        Napi::TypeError::New(info.Env(), "Unable to load first lexeme.")
            .ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false);
    }

    spLexeme->&&&&&

    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value ZalWeb::LoadFirstLexeme(const Napi::CallbackInfo& info)
{
    auto rc = m_spDictionary->eCreateLexemeEnumerator(m_spLexemeEnumerator);
    if (rc != Hlib::H_NO_ERROR) {
        Napi::TypeError::New(info.Env(), "Unable to create lexeme enumerator.")
            .ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false);
    }

    rc = m_spLexemeEnumerator->eGetFirstLexeme(m_spCurrentLexeme);
    if (rc != Hlib::H_NO_ERROR) {
        Napi::TypeError::New(info.Env(), "Unable to load first lexeme.")
            .ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false);
    }
    return Napi::Boolean::New(info.Env(), true);
}

Napi::Value ZalWeb::LoadNextLexeme(const Napi::CallbackInfo& info)
{
  auto rc = m_spLexemeEnumerator->eGetNextLexeme(m_spCurrentLexeme);
  auto bRet = (Hlib::H_NO_ERROR == rc) ? true : false;
  return Napi::Boolean::New(info.Env(), bRet);
}

Napi::Value ZalWeb::LoadFirstInflection(const Napi::CallbackInfo& info)
{  
  auto rc = m_spCurrentLexeme->eCreateInflectionEnumerator(m_spInflectionEnumerator);
  if (rc != Hlib::H_NO_ERROR)
  {
    ERROR_LOG(L"UNable to retrieve inflection data.");
    return Napi::Boolean::New(info.Env(), false);
  }

  rc = m_spInflectionEnumerator->eGetFirstInflection(m_spCurrentInflection);
  if (!m_spCurrentInflection)
  {
    ERROR_LOG(L"Inflection instance is NULL.");
    return Napi::Boolean::New(info.Env(), false);
  }

  auto llId = m_spCurrentInflection->llInflectionId();
  m_mapInflectionIdToInflectionObj[llId] = m_spCurrentInflection;
  m_mapInflectionIdToWordFormObj.emplace(llId, GramHashToWordForm{});

  return Napi::Boolean::New(info.Env(), true);
}

Napi::Value ZalWeb::LoadNextInflection(const Napi::CallbackInfo& info)
{
  auto rc = m_spInflectionEnumerator->eGetNextInflection(m_spCurrentInflection);
  auto bRet = (Hlib::H_NO_ERROR == rc) ? true : false;
  if (bRet) {
    auto llId = m_spCurrentInflection->llInflectionId();
    m_mapInflectionIdToInflectionObj[llId] = m_spCurrentInflection;
  }
  return Napi::Boolean::New(info.Env(), bRet);
}
*/
//Napi::Value ZalWeb::SetLexemeProperty(const Napi::CallbackInfo& info) 
//{
//  if (info.Length() != 2) {
//    Napi::TypeError::New(info.Env(), "Expected two arguments.").ThrowAsJavaScriptException();
//    return Napi::Boolean::New(info.Env(), false);   
//  }
//  else {
//    return Napi::Boolean::New(info.Env(), true);
//  }
//}

Napi::Value ZalWeb::GetLexemeProperty(const Napi::CallbackInfo& info) 
{
  if (info.Length() != 2) {
    Napi::TypeError::New(info.Env(), "Expected two arguments.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }

  if (!info[0].IsNumber()) {
        Napi::TypeError::New(info.Env(), "Lexeme ID is not numeric.").ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false); 
  }

  int64_t llLexemeId = info[0].As<Napi::Number>().ToNumber();
  auto sPropName = info[1].As<Napi::String>().Utf8Value();

  auto itLexemeObj = m_mapLexemeIdToLexemeObj.find(llLexemeId);
  if (m_mapLexemeIdToLexemeObj.end() ==  itLexemeObj) {
    Hlib::CEString sMsg(L"Could not find lexeme with ID = ");
    sMsg += Hlib::CEString::sToString(llLexemeId);
    ERROR_LOG(sMsg);
  }

  auto itProperty = m_mapKeyToLexemePropHandler.find(sPropName);
  if (m_mapKeyToLexemePropHandler.end() == itProperty) {
    Hlib::CEString sMsg(L"Lexeme property ");
    sMsg += Hlib::CEString::sFromUtf8(sPropName);
    sMsg += L" not found.";
    ERROR_LOG(sMsg);
    return Napi::Boolean::New(info.Env(), false);
  }

  auto fnHandler = itProperty->second;
  return fnHandler(info, itLexemeObj->second);

}   // GetLexemeProperty()

//Napi::Value ZalWeb::SetInflectionProperty(const Napi::CallbackInfo& info) 
//{
//  if (info.Length() <= 1) {
//    Napi::TypeError::New(info.Env(), "Expected two arguments.").ThrowAsJavaScriptException();
//    return Napi::Boolean::New(info.Env(), false);   
//  }
//  else {
//    return Napi::Boolean::New(info.Env(), true);
//  }
//}

Napi::Value ZalWeb::GetInflectionId(const Napi::CallbackInfo& info)
{
  if (info.Length() != 2) {
    Napi::TypeError::New(info.Env(), "Expected one argument.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }

  if (!info[0].IsNumber()) {
    Napi::TypeError::New(info.Env(), "Lexeme ID is not numeric.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false); 
  }

  int64_t llLexemeId = info[0].As<Napi::Number>().ToNumber();

  if (!info[1].IsNumber()) {
    Napi::TypeError::New(info.Env(), "Inflection index is not numeric.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false); 
  }

  int iIdx = info[1].As<Napi::Number>().ToNumber();
  auto pairIdRange = m_mmapLexemeIdToInflectionIds.equal_range(llLexemeId);
  auto nInflections = std::distance(pairIdRange.first, pairIdRange.second);
  if (iIdx < 0 || iIdx > nInflections) {    // Unexpected index values
    Napi::TypeError::New(info.Env(), "Inflection index out of range.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false); 
  }

  // End of enumeration
  if (iIdx == nInflections) {
    return Napi::Boolean::New(info.Env(), false); 
  }

  auto itPos = pairIdRange.first;
  std::advance(itPos, iIdx);
  return Napi::Number::New(info.Env(), (*itPos).second);

}   //  GetInflectionId

Napi::Value ZalWeb::GetInflectionProperty(const Napi::CallbackInfo& info) 
{
  if (info.Length() != 2) {
    Napi::TypeError::New(info.Env(), "Expected two arguments.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }

  if (!info[0].IsNumber()) {
    Napi::TypeError::New(info.Env(), "Inflection ID is not numeric.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false); 
  }

  int64_t llInflectionId = info[0].As<Napi::Number>().ToNumber();
  auto sPropName = info[1].As<Napi::String>().Utf8Value();

  auto itInflectionObj = m_mapInflectionIdToInflectionObj.find(llInflectionId);
  if (m_mapInflectionIdToInflectionObj.end() ==  itInflectionObj) {
    Hlib::CEString sMsg(L"Could not find inflection with ID = ");
    sMsg += Hlib::CEString::sToString(llInflectionId);
    ERROR_LOG(sMsg);
  }

  auto itProperty = m_mapKeyToInflectionPropHandler.find(sPropName);
  if (m_mapKeyToInflectionPropHandler.end() == itProperty) {
    Hlib::CEString sMsg(L"Inflection property ");
    sMsg += Hlib::CEString::sFromUtf8(sPropName);
    sMsg += L" not found.";
    ERROR_LOG(sMsg);
    return Napi::Boolean::New(info.Env(), false);
  }

  auto fnHandler = itProperty->second;
  return fnHandler(info, itInflectionObj->second);
}

Napi::Value ZalWeb::GenerateParadigm(const Napi::CallbackInfo& info)
{
  if (info.Length() != 1) {
    Napi::TypeError::New(info.Env(), "Expected two arguments.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }

  if (!info[0].IsNumber()) {
        Napi::TypeError::New(info.Env(), "Inflection ID is not numeric.").ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false); 
  }

  int64_t llInflectionId = info[0].As<Napi::Number>().ToNumber();
  auto itInflection = m_mapInflectionIdToInflectionObj.find(llInflectionId);
  if (m_mapInflectionIdToInflectionObj.end() == itInflection) {
    Napi::TypeError::New(info.Env(), "Unable to find inflection object for this key.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  auto spInflection = itInflection->second;
  if (nullptr == spInflection) {
    Napi::TypeError::New(info.Env(), "Inflection pointer is NULL.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  auto rc = spInflection->eGenerateParadigm();
  if (rc != Hlib::H_NO_ERROR) {
    Napi::TypeError::New(info.Env(), "Unable to generate paradigm.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  shared_ptr<Hlib::CWordForm> spWf;
  rc = spInflection->eGetFirstWordForm(spWf);
  if (rc != Hlib::H_NO_ERROR || !spWf) {
    Napi::TypeError::New(info.Env(), "Error retrieving word forms.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  while (Hlib::H_NO_ERROR == rc) {
    m_mmapInflectionIdToGramHash.emplace(make_pair(spInflection->llInflectionId(), spWf->sGramHash()));
    m_mmapGramHashToWordFormObj.emplace(make_pair(spWf->sGramHash(), spWf));
    rc = spInflection->eGetNextWordForm(spWf);
    if (rc != Hlib::H_NO_ERROR && rc != Hlib::H_FALSE && rc != Hlib::H_NO_MORE) {
      Napi::TypeError::New(info.Env(), "Error retrieving next word form.").ThrowAsJavaScriptException();
      return Napi::Boolean::New(info.Env(), false);
    }
  }
  return Napi::Boolean::New(info.Env(), true);

}   // GenerateParadigm()

/*
Napi::Value ZalWeb::GetFirstWordForm(const Napi::CallbackInfo& info)
{
  if (info.Length() < 1) {
    Napi::TypeError::New(info.Env(), "No argument.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }

  if (!info[0].IsNumber()) {
        Napi::TypeError::New(info.Env(), "Inflection ID is not numeric.").ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false); 
  }

  auto llInflectionId = info[0].As<Napi::Number>().ToNumber();
  auto itInflection = m_mmapInflectionIdToWordFormObj.find(llInflectionId);
  if (m_mmapInflectionIdToWordFormObj.end() == itInflection) {
    Napi::TypeError::New(info.Env(), "Unable to find inflection object for this key.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::Boolean::New(info.Env(), true);
}

Napi::Value ZalWeb::GetNextWordForm(const Napi::CallbackInfo& info)
{
  if (info.Length() < 1) {
    Napi::TypeError::New(info.Env(), "No argument.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }

  if (!info[0].IsNumber()) {
        Napi::TypeError::New(info.Env(), "Inflection ID is not numeric.").ThrowAsJavaScriptException();
        return Napi::Boolean::New(info.Env(), false); 
  }

  auto llInflectionId = info[0].As<Napi::Number>().ToNumber();
  auto itInflection = m_mapInflectionIdToInflectionObj.find(llInflectionId);
  if (m_mapInflectionIdToInflectionObj.end() == itInflection) {
    Napi::TypeError::New(info.Env(), "Unable to find inflection object for this key.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  auto rc = itInflection->second->eGetNextWordForm(m_spCurrentWordForm);
  if (rc != Hlib::H_NO_ERROR) {
    return Napi::Boolean::New(info.Env(), false);
  }
  return Napi::Boolean::New(info.Env(), true);
}

Napi::Value ZalWeb::SetWordFormProperty(const Napi::CallbackInfo& info) 
{
  if (info.Length() <= 1) {
    Napi::TypeError::New(info.Env(), "Expected two arguments.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }
  else {
    return Napi::Boolean::New(info.Env(), true);
  }
}

Napi::Value ZalWeb::GetWordFormProperty(const Napi::CallbackInfo& info) 
{
  if (info.Length() < 1) {
    Napi::TypeError::New(info.Env(), "No argument.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }

  if (nullptr == m_spCurrentWordForm) {
    Napi::TypeError::New(info.Env(), "WordForm pointer is NULL.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  auto sKey = info[0].As<Napi::String>().Utf8Value();
  auto itProperty = m_mapKeyToWordFormPropHandler.find(sKey);
  if (m_mapKeyToWordFormPropHandler.end() == itProperty) {
//    std::cout << "ERROR: property " << sKey << " not found.\n";
    Hlib::CEString sMsg(L"Property ");
    sMsg += Hlib::CEString::sFromUtf8(sKey);
    sMsg += L" not found.";
    ERROR_LOG(sMsg);
    return Napi::Boolean::New(info.Env(), false);
  }
  return itProperty->second(info, m_spCurrentWordForm);
}
*/
//-----------------------------------------------------------------------------------------------------

Napi::Value ZalWeb::GetWordInTextProperty(const Napi::CallbackInfo& info) 
{
  // Args: Index of the segment (line or paragraph), index of the word in segment, property name

  if (info.Length() < 3) {
    Napi::TypeError::New(info.Env(), "No argument.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }

  // Segment idx
  if (!info[0].IsNumber()) {
    ERROR_LOG(L"Argument must be numeric.");
    return Napi::Boolean::New(info.Env(), false);
  }

  auto uiSegment = info[0].As<Napi::Number>().Uint32Value();
  if (uiSegment > m_mapSegNumToDbId.size())
  {
    ERROR_LOG(L"Segment index out of bounds.")
    return Napi::Boolean::New(info.Env(), false);
  }
  
  auto uiDbId = m_mapSegNumToDbId[uiSegment];
  auto& vecWordsInSegment = m_mapWordsInSegment[uiDbId];

  // Word pos
  if (!info[1].IsNumber()) {
    ERROR_LOG(L"Argument must be numeric.");
    return Napi::Boolean::New(info.Env(), false);
  }

  auto uiWordIdx = info[1].As<Napi::Number>().Uint32Value();
  if (uiWordIdx > vecWordsInSegment.size())
  {
    ERROR_LOG(L"Word index out of bounds.")
    return Napi::Boolean::New(info.Env(), false);
  }

  // Property to return
  auto sKey = info[2].As<Napi::String>().Utf8Value();
  auto itProperty = m_mapKeyToWordInTextHandler.find(sKey);
  if (m_mapKeyToWordInTextHandler.end() == itProperty) {
    Hlib::CEString sMsg(L"Property ");
    sMsg += Hlib::CEString::sFromUtf8(sKey);
    sMsg += L" not found.";
    ERROR_LOG(sMsg);
    return Napi::Boolean::New(info.Env(), false);
  }

  return itProperty->second(info, vecWordsInSegment[uiWordIdx]);
}

Napi::Value ZalWeb::LoadFirstSegment(const Napi::CallbackInfo& info)
{
  m_mapWordsInSegment.clear();
  m_mapSegments.clear();
  m_mapSegNumToDbId.clear();
  m_iSegmentCount = 0;

  if (!m_spDictionary) {
    Napi::TypeError::New(info.Env(), "Dictionary is not initialized.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  if (!m_spAnalytics)
  {
      Napi::TypeError::New(info.Env(), "Error accessing analytics module.").ThrowAsJavaScriptException();
  }

  if (info.Length() < 1) {
    Napi::TypeError::New(Env(), "Expecting one argument").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  if (!info[0].IsNumber()) {
    Napi::TypeError::New(Env(), "Argument must be numeric.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  auto iStartAt = info[0].As<Napi::Number>();
//  auto iStartAt = info[0].ToNumber().Int32Value();
  Hlib::CEString sText;
  std::vector<Hlib::StWordContext> vecWordsInSegment;
  
  auto rc = m_spAnalytics->eGetFirstSegment(vecWordsInSegment, iStartAt);
  if (rc != Hlib::H_NO_ERROR) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve first line or paragraph.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  if (vecWordsInSegment.empty()) {
    Napi::TypeError::New(info.Env(), "Parse vector is empty.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  auto llSegmentId = vecWordsInSegment[0].llSegmentId;
  m_mapWordsInSegment[llSegmentId] = vecWordsInSegment;
  m_mapSegments[llSegmentId] = sText;
  m_iSegmentCount = 0;
  m_mapSegNumToDbId[m_iSegmentCount++] = llSegmentId;

  return Napi::Boolean::New(info.Env(), true);
}

Napi::Value ZalWeb::LoadNextSegment(const Napi::CallbackInfo& info)
{
  if (!m_spAnalytics) {
    Napi::TypeError::New(info.Env(), "Analytics module is not initialized.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  Hlib::CEString sText;
  std::vector<Hlib::StWordContext> vecWordsInSegment;
  auto rc = m_spAnalytics->eGetNextSegment(vecWordsInSegment);
  if (rc != Hlib::H_NO_ERROR) {
    Napi::TypeError::New(info.Env(), "Failed to retrieve line or paragraph.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  if (vecWordsInSegment.empty()) {
    Napi::TypeError::New(info.Env(), "Parse vector is empty.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);
  }

  auto llSegmentId = vecWordsInSegment[0].llSegmentId;
  m_mapWordsInSegment[llSegmentId] = vecWordsInSegment;
  m_mapSegments[llSegmentId] = sText;
  m_mapSegNumToDbId[m_iSegmentCount++] = llSegmentId;

  return Napi::Boolean::New(info.Env(), true);
}

Napi::Value ZalWeb::SegmentSize(const Napi::CallbackInfo& info)
{
  // Args: Index of the segment (line of paragraph)

  if (info.Length() < 1) {
    Napi::TypeError::New(info.Env(), "No argument.").ThrowAsJavaScriptException();
    return Napi::Boolean::New(info.Env(), false);   
  }

  // Segment idx
  if (!info[0].IsNumber()) {
    ERROR_LOG(L"Argument must be numeric.");
    return Napi::Boolean::New(info.Env(), false);
  }

  auto uiSegment = info[0].As<Napi::Number>().Uint32Value();
  if (uiSegment >= m_mapSegNumToDbId.size() )
  {
    ERROR_LOG(L"Segment index out of bounds.")
    return Napi::Boolean::New(info.Env(), false);
  }

  auto iSegId = m_mapSegNumToDbId[uiSegment];

  return Napi::Number::New(info.Env(), m_mapWordsInSegment[iSegId].size());
}


NODE_API_MODULE(addon, InitAll)

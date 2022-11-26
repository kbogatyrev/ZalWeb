#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <map>
#include <functional>

#include <napi.h>

#include "Dictionary.h"

enum class LexemeProperties {
  sourceForm,
  homonyms,
  contexts,
  spryazhSm,
  mainSymbol,
  inflectionSymbol,
  inflectionType,
  accentType1,
  accentType2,
  comment,
  aspectPair,
  altAspectPair,
  headwordComment,
  pluralOf,
  usage,
  seeRef,
  stemAugment,
  trailingComment,
  restrictedContexts,
  commonDeviations,
  section,
  hasFleetingVowel,
  hasYoAlternation,
  noComparative,
  assumedPlForms,
  assumedShortPl,
  hasIrregularForms,
  hasIrregularVariants,
  shortFormsRestricted,
  shortFormsIncomplete,
  noLongForms,
  pastParticipleRestricted,
  noPassivePastParticiple,
  difficultForms,
  secondPart
};

using fnHandler = std::function<Napi::Value(const Napi::CallbackInfo& info, Hlib::StLexemeProperties&)>;


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

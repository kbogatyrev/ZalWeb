#ifndef MYOBJECT_H
#define MYOBJECT_H

#include <map>
#include <functional>
#include <memory>
#include <napi.h>
#include "Dictionary.h"
#include "Lexeme.h"

using fnHandlerLexeme = std::function<Napi::Value(const Napi::CallbackInfo& info, shared_ptr<Hlib::CLexeme>)>;
using fnHandlerInflection = std::function<Napi::Value(const Napi::CallbackInfo& info, shared_ptr<Hlib::CInflection>)>;
using fnHandlerWordForm = std::function<Napi::Value(const Napi::CallbackInfo& info, shared_ptr<Hlib::CWordForm>)>;
using fnHandlerWordInText = std::function<Napi::Value(const Napi::CallbackInfo& info, Hlib::StWordContext)>;

static std::map<Hlib::ET_AccentType, std::string> MapAccentTypeToString {
  { Hlib::ET_AccentType::AT_UNDEFINED, "" },
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
  { Hlib::ET_AccentType::AT_F2, "f''" },
  { Hlib::ET_AccentType::AT_COUNT, "" }
};

static std::map<Hlib::ET_PartOfSpeech, std::string> MapPosToString {
  { Hlib::ET_PartOfSpeech::POS_UNDEFINED, "" },
  { Hlib::ET_PartOfSpeech::POS_NOUN, "Noun" },
  { Hlib::ET_PartOfSpeech::POS_VERB, "Verb" },
  { Hlib::ET_PartOfSpeech::POS_ADJ, "Adj" },
  { Hlib::ET_PartOfSpeech::POS_PRONOUN, "Pronoun" },
  { Hlib::ET_PartOfSpeech::POS_PRONOUN_ADJ, "PronounAdj" },
  { Hlib::ET_PartOfSpeech::POS_PRONOUN_PREDIC, "PronounPredic" },
  { Hlib::ET_PartOfSpeech::POS_NUM, "Numeral" },
  { Hlib::ET_PartOfSpeech::POS_NUM_ADJ, "NumeralAdj" },
  { Hlib::ET_PartOfSpeech::POS_ADV, "Adv" },
  { Hlib::ET_PartOfSpeech::POS_COMPAR, "Comparative" },
  { Hlib::ET_PartOfSpeech::POS_PREDIC, "Predicative" },
  { Hlib::ET_PartOfSpeech::POS_PREPOSITION, "Preposition" },
  { Hlib::ET_PartOfSpeech::POS_CONJUNCTION, "Conjunction" },
  { Hlib::ET_PartOfSpeech::POS_PARTICLE, "Particle" },
  { Hlib::ET_PartOfSpeech::POS_INTERJ, "Interjection" },
  { Hlib::ET_PartOfSpeech::POS_PARENTH, "Parenthetical" },
  { Hlib::ET_PartOfSpeech::POS_COUNT, "" }
};

static std::map<Hlib::ET_Case, std::string> MapCaseToString {
  { Hlib::ET_Case::CASE_UNDEFINED, "" },
  { Hlib::ET_Case::CASE_NOM, "Nominative" },
  { Hlib::ET_Case::CASE_ACC, "Accusative" },
  { Hlib::ET_Case::CASE_GEN, "Genitive" },
  { Hlib::ET_Case::CASE_PART, "Partitive" },
  { Hlib::ET_Case::CASE_DAT, "Dative" },
  { Hlib::ET_Case::CASE_INST, "Instrumental" },
  { Hlib::ET_Case::CASE_PREP, "Prepositional" },
  { Hlib::ET_Case::CASE_LOC, "Locative" },
  { Hlib::ET_Case::CASE_NUM, "Numeral" },
  { Hlib::ET_Case::CASE_COUNT, "" }
};

static std::map<Hlib::ET_Subparadigm, std::string> MapSubparadigmToString {
  { Hlib::ET_Subparadigm::SUBPARADIGM_UNDEFINED, "" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_NOUN, "Noun" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_LONG_ADJ, "Verb" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_SHORT_ADJ, "Adj" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_COMPARATIVE, "Pronoun" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_PRONOUN_ADJ, "PronounAdj" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_INFINITIVE, "Infinitive" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_PRESENT_TENSE, "PresentTense" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_PAST_TENSE, "PastTense" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_IMPERATIVE, "Imperative" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_ADVERBIAL_PRESENT, "AdverbialPresent" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_ADVERBIAL_PAST, "AdverbialPast" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_PART_PRES_ACT, "PartPresentAct" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_PART_PRES_PASS_LONG, "PartPresentActLong" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_PART_PRES_PASS_SHORT, "PartPresentActShort" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_PART_PAST_ACT, "PartPastAct" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_PART_PAST_PASS_LONG, "PasrtPastPassLong" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_PART_PAST_PASS_SHORT, "PartPastPassShort" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_NUM_2TO4, "Numeral2To4" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_NUM, "Numeral" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_NUM_ADJ, "NumeralAdj" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_ASPECT_PAIR, "AspectPair" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_PARTICLE, "Particle" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_PREPOSITION, "Preposition" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_ADVERB, "Adverb" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_CONJUNCTION, "Conjunction" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_INTERJECTION, "Interjection" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_PARENTHESIS, "Parenthetical" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_PREDICATIVE, "Predicative" },
  { Hlib::ET_Subparadigm::SUBPARADIGM_COUNT, "" }
};

static std::map<Hlib::ET_Number, std::string> MapNumberToString {
  { Hlib::ET_Number::NUM_UNDEFINED, "" },
  { Hlib::ET_Number::NUM_SG, "Singular" },
  { Hlib::ET_Number::NUM_PL, "Plural" },
  { Hlib::ET_Number::NUM_COUNT, "" }
};

static std::map<Hlib::ET_Gender, std::string> MapGenderToString {
  { Hlib::ET_Gender::GENDER_UNDEFINED, "" },
  { Hlib::ET_Gender::GENDER_M, "Masculine" },
  { Hlib::ET_Gender::GENDER_F, "Feminine" },
  { Hlib::ET_Gender::GENDER_N, "Neuter" },
  { Hlib::ET_Gender::GENDER_COUNT, "" }
};

static std::map<Hlib::ET_Person, std::string> MapPersonToString {
  { Hlib::ET_Person::PERSON_UNDEFINED, "" },
  { Hlib::ET_Person::PERSON_1, "1stPerson" },
  { Hlib::ET_Person::PERSON_2, "2ndPerson" },
  { Hlib::ET_Person::PERSON_3, "3rdPerson" },
  { Hlib::ET_Person::PERSON_COUNT, "" }
};

static std::map<Hlib::ET_Animacy, std::string> MapAnimacyToString {
  { Hlib::ET_Animacy::ANIM_UNDEFINED, "" },
  { Hlib::ET_Animacy::ANIM_YES, "Animate" },
  { Hlib::ET_Animacy::ANIM_NO, "Inanimate" },
  { Hlib::ET_Animacy::ANIM_COUNT, "" }
};

static std::map<Hlib::ET_Reflexivity, std::string> MapReflexivityToString {
  { Hlib::ET_Reflexivity::REFL_UNDEFINED, "" },
  { Hlib::ET_Reflexivity::REFL_YES, "Reflexive" },
  { Hlib::ET_Reflexivity::REFL_NO, "NonReflexive" },
  { Hlib::ET_Reflexivity::REFL_COUNT, "" }
};

static std::map<Hlib::ET_Aspect, std::string> MapAspectToString {
  { Hlib::ET_Aspect::ASPECT_UNDEFINED, "" },
  { Hlib::ET_Aspect::ASPECT_IMPERFECTIVE, "Imperfective" },
  { Hlib::ET_Aspect::ASPECT_PERFECTIVE, "Perfective" },
  { Hlib::ET_Aspect::ASPECT_COUNT, "" }
};

static std::map<Hlib::ET_Status, std::string> MapStatusToString {
  { Hlib::ET_Status::STATUS_UNDEFINED, "" },
  { Hlib::ET_Status::STATUS_COMMON, "Common" },
  { Hlib::ET_Status::STATUS_OBSOLETE, "Obsolete" },
  { Hlib::ET_Status::STATUS_RARE, "Rare" },
  { Hlib::ET_Status::STATUS_QUESTIONABLE, "Questionable" },
  { Hlib::ET_Status::STATUS_ASSUMED, "Assumed" },
  { Hlib::ET_Status::STATUS_INCORRECT, "Incorrect" },
  { Hlib::ET_Status::STATUS_COUNT, "" }
};

class ZalWeb : public Napi::ObjectWrap<ZalWeb> {
  public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    ZalWeb(const Napi::CallbackInfo& info);

// private:
    void SetDbPath(const Napi::CallbackInfo& info);
    void Clear(const Napi::CallbackInfo& info);
//  void InitPropertyHandlers();

    //
    //  Dictionary
    //
    Napi::Value GetLexemesByInitialForm(const Napi::CallbackInfo& info);
    Napi::Value LoadFirstLexeme(const Napi::CallbackInfo& info);
    Napi::Value LoadNextLexeme(const Napi::CallbackInfo& info);
    Napi::Value GetLexemeProperty(const Napi::CallbackInfo& info);
    Napi::Value SetLexemeProperty(const Napi::CallbackInfo& info);
    Napi::Value LoadFirstInflection(const Napi::CallbackInfo& info);
    Napi::Value LoadNextInflection(const Napi::CallbackInfo& info);
    Napi::Value GetInflectionProperty(const Napi::CallbackInfo& info);
    Napi::Value SetInflectionProperty(const Napi::CallbackInfo& info);
    Napi::Value GenerateParadigm(const Napi::CallbackInfo& info);
    Napi::Value LoadFirstWordForm(const Napi::CallbackInfo& info);
    Napi::Value LoadNextWordForm(const Napi::CallbackInfo& info);
    Napi::Value GetWordFormProperty(const Napi::CallbackInfo& info);
    Napi::Value SetWordFormProperty(const Napi::CallbackInfo& info);

    //
    //  Manual text editing
    //
    Napi::Value LoadFirstParagraph(const Napi::CallbackInfo& info);
    Napi::Value LoadNextParagraph(const Napi::CallbackInfo& info);
    Napi::Value ParagraphSize(const Napi::CallbackInfo& info);
    Napi::Value GetWordInTextProperty(const Napi::CallbackInfo& info);

  private:
    shared_ptr<Hlib::CDictionary> m_spDictionary;
    shared_ptr<Hlib::CLexemeEnumerator> m_spLexemeEnumerator;
    shared_ptr<Hlib::CLexeme> m_spCurrentLexeme;
    shared_ptr<Hlib::CInflectionEnumerator> m_spInflectionEnumerator;  
    shared_ptr<Hlib::CInflection> m_spCurrentInflection;
    shared_ptr<Hlib::CWordForm> m_spCurrentWordForm;
    shared_ptr<Hlib::CAnalytics> m_spAnalytics;

    std::vector<Hlib::StWordContext> m_vecWordsInParagraph;

    std::map<string, fnHandlerLexeme> m_mapKeyToLexemePropHandler;
    std::map<string, fnHandlerInflection> m_mapKeyToInflectionPropHandler;
    std::map<string, fnHandlerWordForm> m_mapKeyToWordFormPropHandler;
    std::map<string, fnHandlerWordInText> m_mapKeyToWordInTextHandler;
};

#endif

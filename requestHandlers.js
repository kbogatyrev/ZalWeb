const Declinables = [
  "Noun",
  "LongAdj",
  "Pronoun",
  "PronounAdj",
  "Numeral",
  "NumeralAdj",
  "PartPresAct",
  "PartPresPassLong",
  "PartPastAct",
  "PartPastPassLong",
  "PartPastPassShort",
];

const SingularPlural = [
  "Noun",
  "LongAdj",
  "ShortAdj",
  "Pronoun",
  "PronounAdj",
  "PresentTense",
  "Imperative",
  "PastPresAct",
  "PartPresPassLong",
  "PartPresPassShort",
  "PartPastAct",
  "PartPastPassLong",
  "PartPastPassShort",
  "NumeralAdj",
];

const ThreeGenders = [
  "LongAdj",
  "ShortAdj",
  "PronounAdj",
  "PastTense",
  "PartPresAct",
  "PartPresPassLong",
  "PartPresPassShort",
  "PartPastAct",
  "PartPastPassLong",
  "PartPastPassShort",
  "NumeralAdj",
];

const AnimRelevant = [
  "LongAdj",
  "PronounAdj",
  "PartPresAct",
  "PartPresPassLong",
  "PartPastAct",
  "PartPastPassLong",
  "NumeralAdj",
];

function setDbPath(path) {
  obj.setDbPath(path);
}

function wordQuery(searchString, response) {
  obj.clear();

  var lexData = {
    lexemes: [],
  };

  class Lexeme {}
  class Inflection {}

  response.writeHead(200, { "Content-Type": "text/json; charset=utf-8" });

  try {
    var bLexemeFound = obj.getLexemesByInitialForm(searchString);
    if (!bLexemeFound) {
      console.log("Lexeme %s not found.", searchString);
      response.write("Lexeme not found");
      response.end();
      return;
    }

    var bLexemeLoaded = obj.loadFirstLexeme();
    if (!bLexemeLoaded) {
      console.error("loadFirstLexeme() failed");
      response.write("Internal error");
      response.end();
      return;
    }

    do {
      lexeme = new Lexeme();
      //            lexeme.lexemeId = obj.getLexemeProperty("lexemeId");
      lexeme.sourceForm = obj.getLexemeProperty("sourceForm");
      lexeme.mainSymbol = obj.getLexemeProperty("mainSymbol");
      lexeme.partOfSpeech = obj.getLexemeProperty("partOfSpeech");
      if ("Verb" == lexeme.partOfSpeech) {
        lexeme.isTransitive = obj.getLexemeProperty("isTransitive");
      }
      spryazhSm = obj.getLexemeProperty("spryazhSm");
      if (spryazhSm) {
        lexeme.spryazhSmRef = obj.getLexemeProperty("spryazhSmRef");
      }

      section = obj.getLexemeProperty("section");
      if (section >= 1) {
        lexeme.section = obj.getLexemeProperty("section");
      }

      var bInflectionLoaded = obj.loadFirstInflection();
      if (!bInflectionLoaded) {
        console.error("loadFirstInflection() failed");
        response.write("Internal error");
        response.end();
        return;
      }

      //      var inflectionId = obj.getInflectionProperty("inflectionId");
      //      var bGenerated = obj.generateParadigm(inflectionId);
      //      if (!bGenerated) {
      //        console.error(
      //          "Failed to generate paradigm for inflection ID %s.",
      //          inflectionId
      //        );
      //        response.write("Failed to generate paradigm.");
      //        response.end();
      //        return;
      //      }

      lexeme.inflections = [];
      do {
        inflection = new Inflection();
        inflection.inflectionId = obj.getInflectionProperty("inflectionId");
        inflection.inflectionType = obj.getInflectionProperty("inflectionType");
        const ap1 = obj.getInflectionProperty("accentType1");
        if (ap1 != "") {
          inflection.accentType1 = ap1;
          const ap2 = obj.getInflectionProperty("accentType2");
          if (ap2 != "") {
            inflection.accentType2 = ap2;
          }
        }
        if ("Verb" == lexeme.partOfSpeech) {
          aspectPair = obj.getInflectionProperty("aspectPair");
          if (aspectPair.length > 0) {
            inflection.aspectPair = aspectPair;
          }
        }
        lexeme.inflections.push(inflection);
      } while (obj.loadNextInflection());

      lexData.lexemes.push(lexeme);

      //            import ("uuid");
    } while (obj.loadNextLexeme());
  } catch (e) {
    console.error("NodeJS exception: %s", e.message);
  }

  var json = JSON.stringify(lexData.lexemes);
  console.log(JSON.parse(json));
  response.write(json);
  response.end();
} // wordQuery()

function paradigmQuery(inflectionId, response) {
  var paradigm = {
    wordForms: [],
  };

  response.writeHead(200, { "Content-Type": "text/json; charset=utf-8" });

  try {
    console.log("typeof = " + typeof inflectionId);
    var bGenerated = obj.generateParadigm(inflectionId);
    if (!bGenerated) {
      console.error(
        "Failed to generate paradigm for inflection ID %s.",
        inflectionId
      );
      response.write("Failed to generate paradigm.");
      response.end();
      return;
    }
    var bWordFormLoaded = obj.loadFirstWordForm(inflectionId);
    if (!bWordFormLoaded) {
      console.error("loadFirstWordForm() failed.");
      response.write("Internal error.");
      response.end();
      return;
    }

    do {
      let subParadigm = obj.getWordFormProperty("subParadigm");

      var wordForm = {
        wordForm: "",
        //        stem : '',
        //        ending : '',
        //        partOfSpeech : '',
        //        case : '',
        //        subParadigm : '',
        //        number : '',
        //        gender : '',
        //        person : '',
        //        animacy : '',
        //        reflexivity : '',
        //        aspect : '',
        //        status : '',
        //        isIrregular : false,
        //        isVariant : false,
        //        isDifficult : false
        //        leadComent : '',
        //        trailingComment : ''
      };

      wordForm.wordForm = obj.getWordFormProperty("wordForm");

      if (Declinables.includes(subParadigm)) {
        wordForm.case = obj.getWordFormProperty("case");
      }

      if (SingularPlural.includes(subParadigm)) {
        wordForm.number = obj.getWordFormProperty("number");
      }

      if (ThreeGenders.includes(subParadigm)) {
        wordForm.gender = obj.getWordFormProperty("gender");
      }

      if ("PresentTense" === subParadigm) {
        wordForm.person = obj.getWordFormProperty("person");
      }

      if (AnimRelevant.includes(subParadigm)) {
        if (Declinables.includes(subParadigm)) {
          if ("Accusative" == wordForm.case && wordForm.gender != "Neuter") {
            wordForm.animacy = obj.getWordFormProperty("animacy");
          }
        } else {
          wordForm.animacy = obj.getWordFormProperty("animacy"); // when will this happen??
        }
      }

      //            wordForm.reflexivity = obj.getWordFormProperty("reflexivity");
      //            wordForm.aspect = obj.getWordFormProperty("aspect");
      let status = obj.getWordFormProperty("status");
      if (status && status !== "Common") {
        wordForm.status = status;
      }
      let isIrregular = obj.getWordFormProperty("isIrregular");
      if (isIrregular && isIrregular !== "") {
        wordForm.isIrregular = isIrregular;
      }
      //            let isVariant = obj.getWordFormProperty('isVariant');
      //            if (isVariant && isVariant != '') {
      //                wordForm.isVariant = isVariant;
      //            }
      let isDifficult = obj.getWordFormProperty("isDifficult");
      if (isDifficult && isDifficult !== "") {
        wordForm.isDifficult = isDifficult;
      }
      //            wordForm.leadComent = obj.getWordFormProperty("leadComment");
      //            wordForm.trailingComment = obj.getWordFormProperty("trailingComment");

      paradigm.wordForms.push(JSON.parse(JSON.stringify(wordForm)));
    } while (obj.loadNextWordForm(inflectionId));

    var json = JSON.stringify(paradigm);
    console.log(JSON.parse(json));
    response.write(json);
    response.end();
  } catch (e) {
    console.error("NodeJS exception: %s", e.message);
  }
} // paradigmQuery()

function wordParse(response) {
  console.log("Request handler 'wordParse' was called.");
  response.writeHead(200, { "Content-Type": "text/plain" });
  response.write("============================== wordParse");
  response.end();
}

function Word(
  incompleteParse,
  lineBreak,
  segmentId,
  seqNum,
  wordSource,
  gramHash
) {
  this.incompleteParse = incompleteParse;
  this.lineBreak = lineBreak;
  this.segmentId = segmentId;
  this.seqNum = seqNum;
  this.wordSource = wordSource;
  this.gramHashes = gramHash;
}

function formatSegment(segNum, segObj, text) {
  try {
    for (let at = 0; at < segObj.segmentSize(segNum); ++at) {
      const word = new Word(
        segObj.getWordInTextProperty(segNum, at, "incompleteParse"),
        segObj.getWordInTextProperty(segNum, at, "lineBreak"),
        segObj.getWordInTextProperty(segNum, at, "segmentId"),
        segObj.getWordInTextProperty(segNum, at, "seqNum"),
        segObj.getWordInTextProperty(segNum, at, "wordSource"),
        segObj.getWordInTextProperty(segNum, at, "gramHash")
      );
      text.push(word);
    }
  } catch (e) {
    console.log("NodeJS exception : " + e.message);
  }
}

// arguments: (1) 1st segment id, (2) num of segments to show
// e.g., HTTP GET /text&startIdx=0&size=2
function textQuery(start_id, size, response) {
  let text = [];
  try {
    var rc = obj.loadFirstSegment(start_id);
    formatSegment(0, obj, text);
    //    console.log(text);

    for (let segNum = 1; segNum < size; ++segNum) {
      bRet = obj.loadNextSegment();
      if (!bRet) {
        break;
      }
      formatSegment(segNum, obj, text);
    }
  } catch (e) {
    console.log("NodeJS exception: " + e.message);
  }

  response.writeHead(200, { "Content-Type": "text/json; charset=utf-8" });
  var json = JSON.stringify(text);
  //    console.log(JSON.parse(json));
  response.write(json);
  response.end();
}

const addon = require("bindings")("zal-web.node");
const obj = new addon.ZalWeb();

//const sDbPath = "/home/konstantin/Zal-Web/data/ZalData_Master.db3";
exports.setDbPath = setDbPath;
exports.wordQuery = wordQuery;
exports.paradigmQuery = paradigmQuery;
//exports.wordParse = wordParse;
exports.textQuery = textQuery;

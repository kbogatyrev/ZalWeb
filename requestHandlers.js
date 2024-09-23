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

const listLexemes = [];
const objInflectionToWordForms = {};

//class Inflection {}

// -----------------------------------------------------

function setDbPath(path) {
  zalWebObj.setDbPath(path);
}

function* lexemeIdGenerator() {
  let idx = 0;
  while (true) {
    let rc = zalWebObj.getLexemeId(idx);
    if (rc === false) {
      return;
    }
    yield rc;
    ++idx;
  }
}

function* inflectionGenerator(lexemeId) {
  let idx = 0;
  while (true) {
    let rc = zalWebObj.getInflectionId(lexemeId, idx);
    if (rc === false) {
      return;
    }
    yield rc;
    ++idx;
  }
}

function* gramHashGenerator(inflectionId) {
  let idx = 0;
  let lastGramHash = "";
  while (true) {
    let rc = zalWebObj.getGramHash(inflectionId, idx);
    if (rc === false) {
      return;
    }
    ++idx;
    // dedupe
    if (rc === lastGramHash) {
      continue;
    }
    yield rc;
    lastGramHash = rc;
  }
}

function* wordFormGenerator(inflectionId, gramHash) {
  let idx = 0;
  while (true) {
    let rc = zalWebObj.getWordForm(inflectionId, gramHash, idx);
    if (rc === false) {
      return;
    }
    yield rc;
    ++idx;
  }
}

//
//  Lexeme + inflections
//
function wordQuery(searchString, response) {
  try {
    zalWebObj.clear();

    // Load lexeme ID's and inflection ID's
    const rc = zalWebObj.wordQuery(searchString);
    const lexGen = lexemeIdGenerator();
    let itLexemeId = lexGen.next();
    while (!itLexemeId.done) {
      console.log(itLexemeId.value);
      let lexeme = {};
      lexeme["lexemeId"] = itLexemeId.value;
      lexeme["sourceForm"] = zalWebObj.getLexemeProperty(
        itLexemeId.value,
        "sourceForm"
      );
      homonyms = zalWebObj.getLexemeProperty(itLexemeId.value, "homonyms");
      if (homonyms) {
        lexeme["homonyms"] = homonyms;
      }

      lexeme["mainSymbol"] = zalWebObj.getLexemeProperty(
        itLexemeId.value,
        "mainSymbol"
      );
      lexeme["partOfSpeech"] = zalWebObj.getLexemeProperty(
        itLexemeId.value,
        "partOfSpeech"
      );
      if ("Verb" == lexeme[(itLexemeId, "partOfSpeech")]) {
        lexeme.isTransitive = zalWebObj.getLexemeProperty(
          itLexemeId.value,
          "isTransitive"
        );
      }
      let spryazhSm = zalWebObj.getLexemeProperty(
        itLexemeId.value,
        "spryazhSm"
      );
      if (spryazhSm) {
        lexeme["spryazhSmRef"] = zalWebObj.getLexemeProperty(
          itLexemeId.value,
          "spryazhSmRef"
        );
      }
      section = zalWebObj.getLexemeProperty(itLexemeId.value, "section");
      if (section >= 1) {
        lexeme["section"] = zalWebObj.getLexemeProperty(
          itLexemeId.value,
          "section"
        );
      }

      lexeme["inflections"] = [];
      const iGen = inflectionGenerator(itLexemeId.value);
      let itInflectionId = iGen.next();
      while (!itInflectionId.done) {
        let inflection = {};
        inflection["inflectionId"] = itInflectionId.value;
        inflection["inflectionType"] = zalWebObj.getInflectionProperty(
          itInflectionId.value,
          "inflectionType"
        );
        inflection["accentType1"] = zalWebObj.getInflectionProperty(
          itInflectionId.value,
          "accentType1"
        );
        let at2 = zalWebObj.getInflectionProperty(
          itInflectionId.value,
          "accentType2"
        );
        if (at2) {
          inflection["accentType2"] = at2;
        }
        if ("Verb" === lexeme["partOfSpeech"]) {
          inflection["aspectPair"] = zalWebObj.getInflectionProperty(
            itInflectionId.value,
            "aspectPair"
          );
        }

        lexeme["inflections"].push(inflection);
        itInflectionId = iGen.next();
      }
      listLexemes.push(lexeme);
      itLexemeId = lexGen.next();
    }
  } catch (err) {
    console.log(err);
    json = "{}";
  }
  response.writeHead(200, { "Content-Type": "text/json; charset=utf-8" });
  var json = JSON.stringify(listLexemes);
  listLexemes.length = 0;
  console.log(JSON.parse(json));
  response.write(json);
  response.end();

  // Get lexemes' properties
  //  console.log(JSON.stringify(listLexemes));
} // wordQuery()

//
//  Paradigm
//
function collectWordFormProperties(inflectionId, wordFormKey, objWordForm) {
  objWordForm.wordForm = zalWebObj.getWordFormProperty(wordFormKey, "wordForm");

  let subParadigm = zalWebObj.getWordFormProperty(wordFormKey, "subParadigm");
  objWordForm.subParadigm = subParadigm;

  if (Declinables.includes(subParadigm)) {
    objWordForm.case = zalWebObj.getWordFormProperty(wordFormKey, "case");
    if (objWordForm.case == "Locative") {
      p2Preposition = zalWebObj.getInflectionProperty(
        inflectionId,
        "p2Preposition"
      );
      if (p2Preposition) {
        objWordForm.p2Preposition = p2Preposition;
      }
    }
  }

  if (SingularPlural.includes(subParadigm)) {
    objWordForm.number = zalWebObj.getWordFormProperty(wordFormKey, "number");
  }

  if (ThreeGenders.includes(subParadigm)) {
    if ("ShortAdj" == subParadigm) {
      if ("Singular" == objWordForm.number) {
        objWordForm.gender = zalWebObj.getWordFormProperty(
          wordFormKey,
          "gender"
        );
      }
    } else {
      objWordForm.gender = zalWebObj.getWordFormProperty(wordFormKey, "gender");
    }
  }

  if ("PresentTense" === subParadigm) {
    objWordForm.person = zalWebObj.getWordFormProperty(wordFormKey, "person");
  }

  if (AnimRelevant.includes(subParadigm)) {
    if (Declinables.includes(subParadigm)) {
      if (
        "Accusative" == objWordForm.case &&
        "Masculine" == objWordForm.gender
      ) {
        objWordForm.animacy = zalWebObj.getWordFormProperty(
          wordFormKey,
          "animacy"
        );
      }
    } else {
      objWordForm.animacy = zalWebObj.getWordFormProperty(
        wordFormKey,
        "animacy"
      ); // when will this happen??
    }
  }

  //            wordForm.reflexivity = zalWebObj.getWordFormProperty("reflexivity");
  //            wordForm.aspect = zalWebObj.getWordFormProperty("aspect");
  let status = zalWebObj.getWordFormProperty(wordFormKey, "status");
  console.log(wordFormKey, "\t ", status);
  if (status && status !== "Common") {
    objWordForm.status = status;
  }
  let isIrregular = zalWebObj.getWordFormProperty(wordFormKey, "isIrregular");
  if (isIrregular && isIrregular !== "") {
    objWordForm.isIrregular = isIrregular;
  }
  //            let isVariant = zalWebObj.getWordFormProperty('isVariant');
  //            if (isVariant && isVariant != '') {
  //                wordForm.isVariant = isVariant;
  //            }
  let isDifficult = zalWebObj.getWordFormProperty(wordFormKey, "isDifficult");
  if (isDifficult) {
    objWordForm.isDifficult = isDifficult;
  }
  //            wordForm.leadComent = zalWebObj.getWordFormProperty("leadComment");
  //            wordForm.trailingComment = zalWebObj.getWordFormProperty("trailingComment");
}

function generateParadigm(inflectionId, objParadigm) {
  try {
    var bGenerated = zalWebObj.generateParadigm(inflectionId);
    if (!bGenerated) {
      console.error(
        "Failed to generate paradigm for inflection ID %s.",
        inflectionId
      );
      response.write("Failed to generate paradigm.");
      response.end();
      return;
    }

    listWordForms = [];

    //    const regex = /^S+-S+-(d)/;
    const hashGen = gramHashGenerator(inflectionId);
    let itGramHash = hashGen.next();
    while (!itGramHash.done) {
      const wordFormGen = wordFormGenerator(inflectionId, itGramHash.value);
      itWordFormKey = wordFormGen.next();
      let objWordForm = {};
      while (!itWordFormKey.done) {
        captures = /^\S+-\S+-(\d)/.exec(itWordFormKey.value);
        if (captures.length < 2) {
          console.error(
            "Failed to parse word form key: %s",
            itWordFormKey.value
          );
        }
        objWordForm = {};
        collectWordFormProperties(
          inflectionId,
          itWordFormKey.value,
          objWordForm
        );
        if (captures[1] == 0) {
          listWordForms.push(objWordForm);
        } else {
          let last = listWordForms.pop();
          last.wordForm += "/" + objWordForm.wordForm;
          listWordForms.push(last);
        }
        itWordFormKey = wordFormGen.next();
      }
      itGramHash = hashGen.next();
    }

    objParadigm["wordForms"] = listWordForms;
    objInflectionToWordForms[inflectionId] = objParadigm;

    //    do {
    // wordForm: "",
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
    //      };
  } catch (e) {
    console.error("NodeJS exception: %s", e.message);
  }
}

function paradigmQuery(inflectionId, response) {
  response.writeHead(200, { "Content-Type": "text/json; charset=utf-8" });

  let objParadigm = {};

  if (inflectionId in objInflectionToWordForms) {
    objParadigm = objInflectionToWordForms[inflectionId];
  } else {
    generateParadigm(inflectionId, objParadigm);
  }

  var json = JSON.stringify(objParadigm);
  console.log(JSON.parse(json));
  response.write(json);
  response.end();
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
    var rc = zalWebObj.loadFirstSegment(start_id);
    formatSegment(0, zalWebObj, text);
    //    console.log(text);

    for (let segNum = 1; segNum < size; ++segNum) {
      bRet = zalWebObj.loadNextSegment();
      if (!bRet) {
        break;
      }
      formatSegment(segNum, zalWebObj, text);
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
const zalWebObj = new addon.ZalWeb();

//const sDbPath = "/home/konstantin/Zal-Web/data/ZalData_Master.db3";
exports.setDbPath = setDbPath;
exports.wordQuery = wordQuery;
exports.paradigmQuery = paradigmQuery;
//exports.wordParse = wordParse;
exports.textQuery = textQuery;

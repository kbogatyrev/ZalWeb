function wordQuery(searchString, response) {

    obj.clear();

    var lexData = {
        lexemes:[]
    };

    class Lexeme {};
    class Inflection {};

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
            lexeme.lexemeId = obj.getLexemeProperty("lexemeId");
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

            var inflectionId = obj.getInflectionProperty("inflectionId");
            var bGenerated = obj.generateParadigm(inflectionId);
            if (!bGenerated) {
                console.error("Failed to generate paradigm for inflection ID %s.", inflectionId);
                response.write("Failed to generate paradigm.");
                response.end();
                return;
            }
    
            lexeme.inflections = [];
            do {
                inflection = new Inflection();
                inflection.inflectionId = obj.getInflectionProperty("inflectionId");
                inflection.inflectionType = obj.getInflectionProperty("inflectionType");
                inflection.accentType1 = obj.getInflectionProperty("accentType1");
                inflection.accentType2 = obj.getInflectionProperty("accentType2");
                if ("Verb" == lexeme.partOfSpeech) {
                    aspectPair = obj.getInflectionProperty("aspectPair");
                    if (aspectPair.length > 0) {
                        inflection.aspectPair = aspectPair;
                    }
                }
                lexeme.inflections.push(inflection);

            } while(obj.loadNextInflection());

            lexData.lexemes.push(lexeme);

//            import ("uuid");

        } while (obj.loadNextLexeme());
    }
    catch (e) {
        console.error("NodeJS exception: %s", e.message);
    }

    var json = JSON.stringify(lexData.lexemes);
    console.log(JSON.parse(json));
    response.write(json);
   
    response.end();

}   // wordQuery()

function paradigmQuery(inflectionId, response) {
    var wordForm = {
        wordForm : '',
        stem : '',
        ending : '',
        partOfSpeech : '',
        case : '',
        subParadigm : '',
        number : '',
        gender : '',
        person : '',
        animacy : '',
        reflexivity : '',
        aspect : '',
        status : '',
        isIrregular : false,
        isVariant : false,
        isDifficult : false,
        leadComent : '',
        trailingComment : ''
    };

    var paradigm = {
        wordForms : []
    };

    response.writeHead(200, { "Content-Type": "text/json; charset=utf-8" });

    try {
        var bGenerated = obj.generateParadigm(inflectionId);
        if (!bGenerated) {
            console.error("Failed to generate paradigm for inflection ID %s.", inflectionId);
            response.write("Failed to generate paradigm.");
            response.end();
            return;
        }

        var bWordFormLoaded = obj.loadFirstWordForm();
        if (!bWordFormLoaded) {
            console.error("loadFirstWordForm() failed.");
            response.write("Internal error.");
            response.end();
            return;
        }

        do {
            wordForm.wordForm = obj.getWordFormProperty("wordForm");
            wordForm.stem = obj.getWordFormProperty("stem");
            wordForm.ending = obj.getWordFormProperty("ending");
            wordForm.partOfSpeech = obj.getWordFormProperty("partOfSpeech");
            wordForm.case = obj.getWordFormProperty("case");
            wordForm.subParadigm = obj.getWordFormProperty("subParadigm");
            wordForm.number = obj.getWordFormProperty("number");
            wordForm.gender = obj.getWordFormProperty("gender");
            wordForm.person = obj.getWordFormProperty("person");
            wordForm.animacy = obj.getWordFormProperty("animacy");
            wordForm.reflexivity = obj.getWordFormProperty("reflexivity");
            wordForm.aspect = obj.getWordFormProperty("aspect");
            wordForm.status = obj.getWordFormProperty("status");
            wordForm.isIrregular = obj.getWordFormProperty("isIrregular");
            wordForm.isVariant = obj.getWordFormProperty("isVariant");
            wordForm.isDifficult = obj.getWordFormProperty("isDifficult");
            wordForm.leadComent = obj.getWordFormProperty("leadComment");
            wordForm.trailingComment = obj.getWordFormProperty("trailingComment");

            paradigm.wordForms.push(JSON.parse(JSON.stringify(wordForm)));
            
        } while (obj.loadNextWordForm());

        var json = JSON.stringify(paradigm);
        console.log(JSON.parse(json));
        response.write(json);               
        response.end();
    }
    catch (e) {
        console.error("NodeJS exception: %s", e.message);
    }
}   // paradigmQuery()

function wordParse(response) {
    console.log("Request handler 'wordParse' was called.");
    response.writeHead(200, { "Content-Type": "text/plain" });
    response.write("============================== wordParse");
    response.end();
}

function Word(incompleteParse, lineBreak, segmentId, seqNum, wordSource, gramHash) {
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
            const word = new Word(segObj.getWordInTextProperty(segNum, at, 'incompleteParse'), 
                                segObj.getWordInTextProperty(segNum, at, 'lineBreak'), 
                                segObj.getWordInTextProperty(segNum, at, 'segmentId'), 
                                segObj.getWordInTextProperty(segNum, at, 'seqNum'), 
                                segObj.getWordInTextProperty(segNum, at, 'wordSource'), 
                                segObj.getWordInTextProperty(segNum, at, 'gramHash'));
            text.push(word);
        }
    }
    catch (e) {
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
            formatSegment(segNum, obj, text)
        }
    } catch (e) {
        console.log ("NodeJS exception: " + e.message);
    }

    response.writeHead(200, { "Content-Type": "text/json; charset=utf-8" });
    var json = JSON.stringify(text);
//    console.log(JSON.parse(json));
    response.write(json);
    response.end();
}

var addon = require('bindings')('zal-web.node');
var obj = new addon.ZalWeb();
//var sDbPath = "/home/konstantin/Zal-Web/data/ZalData_Master_Full.db3";
//obj.setDbPath(sDbPath);

exports.wordQuery = wordQuery;
exports.paradigmQuery = paradigmQuery;
//exports.wordParse = wordParse;
exports.textQuery = textQuery;

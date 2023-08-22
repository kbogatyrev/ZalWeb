var lexemeInfo = {
    id: -1,
    sourceForm: '',
    homonyms: [],
    contexts: [],
    spryazhSm: false,
    mainSymbol: '',
    inflectionSymbol: '',
    comment: '',
    aspectPair: '',
    altAspectPair: '',
    headwordComment: '',
    isPluralOf: false,
    pluralOf: '',
    usage: '',
    seeRef: '',
    trailingComment: '',
    restrictedContexts: '',
    //    commonDeviations : [],
    section: -1,
    hasYoAlternation: false,
    noComparative: false,
    assumedForms: false,
    hasIrregularForms: false,
    hasIrregulatVariants: false,
    noLongForms: false,
    difficultForms: false,
    secondPart: false
};

var inflectionInfo = {
    id: -1,
    primaryInflectionGroup: false,
    inflectionType: -1,
    accentType1: '',
    accentType2: '',
    shortFormsRestricted: false,
    shortFormsIncomplete: false,
    pastParticipleRestricted: false,
    noPassivePastParticiple: false,
    fleetingVowel: false,
    stemAugment: -1
};

function wordQuery(searchString, response) {

    var addon = require('bindings')('addon');

    var obj = new addon.ZalWeb();

    var sDbPath = "/home/konstantin/Zal-Web/data/ZalData_Master.db3";
    obj.setDbPath(sDbPath);

    response.writeHead(200, { "Content-Type": "text/json; charset=utf-8" });
    var bLexemeFound = obj.getLexemesByInitialForm(searchString);
    if (!bLexemeFound) {
        console.log("Lexeme %s not found.", searchString);
        response.write("Lexeme not found");
        response.end();
        return;
    }

    var bLexEnum = obj.createLexemeEnumerator();
    if (!bLexEnum) {
        console.log("Unable to create enumerator for lexeme %s.", searchString);
        response.end();
        return;
    }

    var bLexemeFound = obj.getFirstLexeme();
    if (!bLexemeFound) {
        console.log("Lexeme not found for search string %s.", searchString);
        response.end();
        return;
    }

    do {
        lexemeInfo.sourceForm = obj.getLexemeProperty("sourceForm");
        lexemeInfo.homonyms = obj.getLexemeProperty("homonyms");
        lexemeInfo.contexts = obj.getLexemeProperty("contexts");
        lexemeInfo.spryazhSm = obj.getLexemeProperty("spryazhSm");
        lexemeInfo.mainSymbol = obj.getLexemeProperty("mainSymbol");
        lexemeInfo.inflectionSymbol = obj.getLexemeProperty("inflectionSymbol");
        lexemeInfo.comment = obj.getLexemeProperty("comment");
        lexemeInfo.aspectPair = obj.getLexemeProperty("aspectPair");
        lexemeInfo.altAspectPair = obj.getLexemeProperty("altAspectPair");
        lexemeInfo.headwordComment = obj.getLexemeProperty("headwordComment");
        lexemeInfo.isPluralOf = obj.getLexemeProperty("isPluralOf");
        lexemeInfo.pluralOf = obj.getLexemeProperty("pluralOf");
        lexemeInfo.usage = obj.getLexemeProperty("usage");
        lexemeInfo.seeRef = obj.getLexemeProperty("seeRef");
        lexemeInfo.trailingComment = obj.getLexemeProperty("trailingComment");
        lexemeInfo.restrictedContexts = obj.getLexemeProperty("restrictedContexts");
        //        lexemeInfo.commonDeviations = obj.getLexemeProperty("commonDeviations");
        lexemeInfo.section = obj.getLexemeProperty("section");
        lexemeInfo.hasYoAlternation = obj.getLexemeProperty("hasYoAlternation");
        lexemeInfo.noComparative = obj.getLexemeProperty("noComparative");
        lexemeInfo.assumedForms = obj.getLexemeProperty("assumedForms");
        lexemeInfo.hasIrregularForms = obj.getLexemeProperty("hasIrregularForms");
        lexemeInfo.hasIrregularVariants = obj.getLexemeProperty("hasIrregularVariants");
        lexemeInfo.noLongForms = obj.getLexemeProperty("noLongForms");
        lexemeInfo.difficultForms = obj.getLexemeProperty("difficultForms");
        lexemeInfo.secondPart = obj.getLexemeProperty("secondPart");

        var json = JSON.stringify(lexemeInfo);
        //        console.log(JSON.parse(json));
        response.write(json);

        var bInflEnum = obj.createInflectionEnumerator();
        if (!bInflEnum) {
            console.log("Unable to create inflection enumerator for lexeme %s.", searchString);
            response.end();
            return;
        }

        var bInflectionFound = obj.getFirstInflection();
        if (!bLexemeFound) {
            console.log("Inflection not found for search string %s.", searchString);
            response.end();
            return;
        }

        do {
            primaryInflectionGroup = obj.getInflectionProperty("primaryInflectionGroup");
            inflectionInfo.inflectionType = obj.getInflectionProperty("inflectionType");
            inflectionInfo.accentType1 = obj.getInflectionProperty("accentType1");
            inflectionInfo.accentType2 = obj.getInflectionProperty("accentType2");
            inflectionInfo.shortFormsRestricted = obj.getInflectionProperty("shortFormsRestricted");
            inflectionInfo.shortFormsIncomplete = obj.getInflectionProperty("shortFormsIncomplete");
            inflectionInfo.pastParticipleRestricted = obj.getInflectionProperty("pastParticipleRestricted");
            inflectionInfo.noPassivePastParticiple = obj.getInflectionProperty("noPassivePastParticiple");
            inflectionInfo.fleetingVowel = obj.getInflectionProperty("fleetingVowel");
            inflectionInfo.stemAugment = obj.getInflectionProperty("stemAugment");

            bInflectionFound = obj.getNextInflection();

            var json = JSON.stringify(inflectionInfo);
            //        console.log(JSON.parse(json));
            response.write(json);
        } while (bInflectionFound);

        bLexemeFound = obj.getNextLexeme();

    } while (bLexemeFound);

    response.end();
}

function wordParse(response) {
    console.log("Request handler 'wordParse' was called.");
    response.writeHead(200, { "Content-Type": "text/plain" });
    response.write("============================== wordParse");
    response.end();
}
exports.wordQuery = wordQuery;
exports.wordParse = wordParse;
//# sourceMappingURL=requestHandlers.js.map
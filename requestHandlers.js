var lexemeInfo = {
    id : -1,
    sourceForm : '',
    homonyms : [],
    contexts : [],
    spryazhSm : false,
    mainSymbol : '',
    inflectionSymbol : '',
    comment : '',
    aspectPair : '',
    altAspectPair : '',
    headwordComment : '',
    isPluralOf : false,
    pluralOf : '',
    usage : '',
    seeRef : '',
    trailingComment : '',
    restrictedContexts : '',
    commonDeviations : [],
    section : -1,
    hasYoAlternation : false,
    noComparative : false,
    assumedForms : false,
    hasIrregularForms : false,
    hasIrregulatVariants : false,
    noLongForms : false,
    difficultForms : false,
    secondPart : false
};

var inflectionInfo = {
    id : -1,
    isPrimaryInflectionGroup : false,
    inflectionType : -1,
    accentType1 : '',
    accentType2 : '',
    shortFormsRestricted : false,
    shortFormsIncomplete : false,
    pastParticipleRestricted : false,
    noPastPassiveParticiple : false,
    fleetingVowel : false,
    stemAugment : -1
};

function wordQuery(searchString, response) {

    var addon = require('bindings')('addon');

    var obj = new addon.ZalWeb();

    var sDbPath = "/home/konstantin/Zal-Web/data/ZalData_Master.db3";
    obj.setDbPath(sDbPath);

    response.writeHead(200, { "Content-Type": "text/json; charset=utf-8" });
    var bLexemeFound = obj.getLexemesByInitialForm(searchString);
    if (!bLexemeFound) {
        console.log ("Lexeme %s not found.", searchString);
        response.write("Lexeme not found");
        response.end();
        return;
    }

    do {
        lexeme.sourceForm = obj.getProperty("sourceForm");
        lexeme.homonyms = obj.getProperty("homonyms");
        lexeme.contexts = obj.getProperty("contexts");
        lexeme.spryazhSm = obj.getProperty("spryazhSm");
        lexeme.mainSymbol = obj.getProperty("mainSymbol");
        lexeme.inflectionSymbol = obj.getProperty("inflectionSymbol");
        lexeme.comment = obj.getProperty("comment");
        lexeme.aspectPair = obj.getProperty("aspectPair");
        lexeme.altAspectPair = obj.getProperty("altAspectPair");
        lexeme.headwordComment = obj.getProperty("headwordComment");
        lexeme.isPluralOf = obj.getProperty("isPluralOf");
        lexeme.pluralOf = obj.getProperty("pluralOf");
        lexeme.usage = obj.getProperty("usage");
        lexeme.seeRef = obj.getProperty("seeRef");
        lexeme.trailingComment = obj.getProperty("trailingComment");
        lexeme.restrictedContexts = obj.getProperty("restrictedContexts");
        lexeme.commonDeviations = obj.getProperty("commonDeviations");
        lexeme.section = obj.getProperty("section");
        lexeme.hasYoAlternation = obj.getProperty("hasYoAlternation");
        lexeme.noComparative = obj.getProperty("noComparative");
        lexeme.assumedForms = obj.getProperty("assumedForms");
        lexeme.hasIrregularForms = obj.getProperty("hasIrregularForms");
        lexeme.hasIrregularVariants = obj.getProperty("hasIrregularVariants");
        lexeme.noLongForms = obj.getProperty("noLongForms");
        lexeme.difficultForms = obj.getProperty("difficultForms");
        lexeme.secondPart = obj.getProperty("secondPart");
        
        var json = JSON.stringify(lexeme);
    //        console.log(JSON.parse(json));
        response.write(json);
    }
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
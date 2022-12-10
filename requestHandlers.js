function wordQuery(searchString, response) {

var addon = require('bindings')('addon');

var obj = new addon.ZalWeb();

var sDbPath = "/home/konstantin/Zal-Web/data/ZalData_Master.db3";
obj.setDbPath(sDbPath);

var lexeme = {
    sourceForm : '',
    homonyms : [],
    contexts : [],
    spryazhSm : false,
    mainSymbol : '',
    inflectionSymbol : '',
    inflectionType : -1,
    accentType1 : '',
    accentType2 : '',
    comment : '',
    aspectPair : '',
    altAspectPair : '',
    headwordComment : '',
    isPluralOf : false,
    pluralOf : '',
    usage : '',
    seeRef : '',
    stemAugment : -1,
    trailingComment : '',
    restrictedContexts : '',
    commonDeviations : [],
    section : -1,
    fleetingVowel : false,
    hasYoAlternation : false,
    noComparative : false,
    assumedForms : false,
    hasIrregularForms : false,
    hasIrregulatVariants : false,
    shorFormsRestricted : false,
    shorFormsIncomplete : false,
    noLongForms : false,
    pastParticipleRestricted : false,
    noPastParticiple : false,
    difficultForms : false,
    secondPart : false
};

response.writeHead(200, { "Content-Type": "text/json; charset=utf-8" });
var bLexemeFound = obj.getLexemesByInitialForm(searchString);
if (bLexemeFound) {
    do {
        lexeme.sourceForm = obj.getProperty("sourceForm");
        lexeme.homonyms = obj.getProperty("homonyms");
        lexeme.contexts = obj.getProperty("contexts");
        lexeme.spryazhSm = obj.getProperty("spryazhSm");
        lexeme.mainSymbol = obj.getProperty("mainSymbol");
        lexeme.inflectionSymbol = obj.getProperty("inflectionSymbol");
        lexeme.inflectionType = obj.getProperty("inflectionType");
        lexeme.accentType1 = obj.getProperty("accentType1");
        lexeme.accentType2 = obj.getProperty("accentType2");
        lexeme.comment = obj.getProperty("comment");
        lexeme.aspectPair = obj.getProperty("aspectPair");
        lexeme.altAspectPair = obj.getProperty("altAspectPair");
        lexeme.headwordComment = obj.getProperty("headwordComment");
        lexeme.isPluralOf = obj.getProperty("isPluralOf");
        lexeme.pluralOf = obj.getProperty("pluralOf");
        lexeme.usage = obj.getProperty("usage");
        lexeme.seeRef = obj.getProperty("seeRef");
        lexeme.stemAugment = obj.getProperty("stemAugment");
        lexeme.trailingComment = obj.getProperty("trailingComment");
        lexeme.restrictedContexts = obj.getProperty("restrictedContexts");
        lexeme.commonDeviations = obj.getProperty("commonDeviations");
        lexeme.section = obj.getProperty("section");
        lexeme.hasFleetingVowel = obj.getProperty("hasFleetingVowel");
        lexeme.hasYoAlternation = obj.getProperty("hasYoAlternation");
        lexeme.noComparative = obj.getProperty("noComparative");
        lexeme.assumedForms = obj.getProperty("assumedForms");
        lexeme.hasIrregularForms = obj.getProperty("hasIrregularForms");
        lexeme.hasIrregularVariants = obj.getProperty("hasIrregularVariants");
        lexeme.shortFormsRestricted = obj.getProperty("shortFormsRestricted");
        lexeme.shortFormsIncomplete = obj.getProperty("shortFormsIncomplete");
        lexeme.noLongForms = obj.getProperty("noLongForms");
        lexeme.pastParticipleRestricted = obj.getProperty("pastParticipleRestricted");
        lexeme.noPassivePastParticiple = obj.getProperty("noPassivePastParticiple");
        lexeme.difficultForms = obj.getProperty("difficultForms");
        lexeme.secondPart = obj.getProperty("secondPart");
        
        var json = JSON.stringify(lexeme);
//        console.log(JSON.parse(json));
        response.write(json);
    
        } while (obj.loadNextLexeme());
    } else {
        console.log ("Lexeme %s not found.", searchString);
        response.write("Lexeme not found");
    }
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
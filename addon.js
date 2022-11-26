var addon = require('bindings')('addon');

var obj = new addon.ZalWeb();

var sDbPath = "/home/konstantin/web/data/ZalData_Master.db3";
obj.setDbPath(sDbPath);

let searchString = "баба";
var bLexemeFound = obj.getLexemesByInitialForm(searchString);
if (bLexemeFound) {
    do {
        var sourceForm = obj.getProperty("sourceForm");
        console.log("sourceForm: %s", sourceForm);

        var homonyms = obj.getProperty("homonyms");
        if (!homonyms) {
            console.log("No homonyms.");
        } else {
            console.log(homonyms);
        }

        var contexts = obj.getProperty("contexts");
        console.log("contexts: %s", contexts);

        var spryazhSm = obj.getProperty("spryazhSm");
        if (spryazhSm) {
            console.log('Spryazh. sm.');
        } else {
            console.log('No spryazh. sm.');
        }

        var mainSymbol = obj.getProperty("mainSymbol");
        console.log("mainSymbol: %s", mainSymbol);
/*
        var inflectionSymbol = obj.getProperty("inflectionSymbol");
        console.log("inflectionSymbol: %s", inflectionSymbol);

        var inflectionType = obj.getProperty("inflectionType");
        console.log("inflectionType: %s", inflectionType);

        var accentType1 = obj.getProperty("accentType1");
        console.log("accentType1: %s", accentType1);

        var accentType2 = obj.getProperty("accentType2");
        console.log("accentType2: %s", accentType2);

        var comment = obj.getProperty("comment");
        console.log("comment: %s", comment);

        var aspectPair = obj.getProperty("aspectPair)");
        console.log("aspectPair: %s", aspectPair);

        var altAspectPair = obj.getProperty("altAspectPair");
        console.log("altAspectPair: %s", altAspectPair);

        var headwordComment = obj.getProperty("headwordComment");
        console.log("headwordComment: %s", headwordComment);

        var pluralOf = obj.getProperty("pluralOf");
        console.log("pluralOf: %s", pluralOf);

        var usage = obj.getProperty("usage");
        console.log("usage: %s", usage);

        var seeRef = obj.getProperty("seeRef");
        console.log("seeRef: %s", seeRef);

        var stemAugment = obj.getProperty("stemAugment");
        console.log("stemAugment: %s", stemAugment);

        var trailingComment = obj.getProperty("trailingComment");
        console.log("trailingComment: %s", trailingComment);

        var restrictedContexts = obj.getProperty("restrictedContexts");
        console.log("restrictedContexts: %s", restrictedContexts);

        var commonDeviations = obj.getProperty("commonDeviations");
        console.log("commonDeviations: ");
        console.log(commonDeviations);

        var section = obj.getProperty("section");
        console.log("section: %d", section);

        var hasFleetingVowel = obj.getProperty("hasFleetingVowel");
        console.log("hasFleetingVowel: %d", hasFleetingVowel);

        var hasYoAlternation = obj.getProperty("hasYoAlternation");
        console.log("hasYoAlternation: %d", hasYoAlternation);

        var noComparative = obj.getProperty("noComparative");
        console.log("noComparative: %d", noComparative);

        var assumedPlForms = obj.getProperty("assumedPlForms");
        console.log("assumedPlForms: %d", assumedPlForms);

        var assumedShortPl = obj.getProperty("assumedShortPl");
        console.log("assumedShortPl: %d", assumedShortPl);

        var hasIrregularForms = obj.getProperty("hasIrregularForms");
        console.log("hasIrregularForms: %d", hasIrregularForms);

        var hasIrregularVariants = obj.getProperty("hasIrregularVariants");
        console.log("hasIrregularVariants: %d", hasIrregularVariants);

        var shortFormsRestricted = obj.getProperty("shortFormsRestricted");
        console.log("shortFormsRestricted: %d", shortFormsRestricted);

        var shortFormsIncomplete = obj.getProperty("shortFormsIncomplete");
        console.log("shortFormsIncomplete: %d", shortFormsIncomplete);

        var noLongForms = obj.getProperty("noLongForms");
        console.log("noLongForms: %d", noLongForms);

        var pastParticipleRestricted = obj.getProperty("pastParticipleRestricted");
        console.log("pastParticipleRestricted: %d", pastParticipleRestricted);

        var noPassivePastParticiple = obj.getProperty("noPassivePastParticiple");
        console.log("noPassivePastParticiple: %d", noPassivePastParticiple);

        var difficultForms = obj.getProperty("difficultForms");
        console.log("difficultForms: %s", difficultForms);

        var secondPart = obj.getProperty("secondPart");
        console.log("secondPart: %s", secondPart);
*/        

    } while (obj.loadNextLexeme());
} else {
        console.log ("Lexeme %s not found.", searchString);
    }

//console.log(obj.getProperty("property1"));
//console.log(obj.setProperty("property1", "value1"));
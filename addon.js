var addon = require('bindings')('addon');

var obj = new addon.ZalWeb();

var sDbPath = "/home/konstantin/zal-demo/ZalData_demo.db3";
obj.setDbPath(sDbPath);

var bLexemeFound = obj.getLexemesByInitialForm("спеть");
console.log(bLexemeFound);
if (bLexemeFound) {
    // get initial form with stress

    // get homonyms if any
    var homonyms = obj.getProperty("homonyms");
    console.log(homonyms);

    var contexts = obj.getProperty("contexts");
    console.log(contexts);

    var spryazhSm = obj.getProperty("spryazhSm");
    console.log("SpryazhSm: %d.", spryazhSm);
}

//console.log(obj.getProperty("property1"));
//console.log(obj.setProperty("property1", "value1"));
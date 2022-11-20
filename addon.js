var addon = require('bindings')('addon');

var obj = new addon.ZalWeb();

var sDbPath = "/home/konstantin/zal-demo/ZalData_demo.db3";
obj.setDbPath(sDbPath);

var bLexemeFound = obj.getLexemesByInitialForm("мама");

//console.log(obj.getProperty("property1"));
//console.log(obj.setProperty("property1", "value1"));
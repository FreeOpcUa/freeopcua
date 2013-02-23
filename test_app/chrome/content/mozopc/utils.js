function createArray()
{
  return Components.classes["@mozilla.org/array;1"].createInstance(Components.interfaces.nsIMutableArray);
}

function createVariant()
{
  return Components.classes["@mozilla.org/variant;1"].createInstance(Components.interfaces.nsIWritableVariant);
}


function createPRUint32()
{
  return Components.classes["@mozilla.org/supports-PRUint32;1"].createInstance(Components.interfaces.nsISupportsPRUint32);
}

function createString()
{
 return Components.classes["@mozilla.org/supports-string;1"].createInstance(Components.interfaces.nsISupportsString);
}


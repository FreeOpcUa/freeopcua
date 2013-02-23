
function onOK()
{
  try
  {
    var treeView = window.arguments[0];
//    alert (treeView);

    var serverHandle = treeView.getSelectedItemServerHandle()
    var serverHandles = createArray();
    serverHandles.appendElement(serverHandle, false);
 
    var value = document.getElementById("new_value").value;
//    alert(value);
    var variant = createVariant();
    variant.setAsString(value);
    var values = createArray();
    values.appendElement(variant, false);

    var opcGroup = treeView.getCurrentOPCGroup();
//    alert (opcGroup);
    var syncIO = opcGroup.QueryInterface(Components.interfaces.nsIOPCSyncIO);
    syncIO.Write(serverHandles, values);
    treeView.fillItemsList();
  }
  catch(e)
  {
    alert(e.toString());
  }
}

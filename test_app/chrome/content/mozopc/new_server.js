function onOK()
{
  try
  {
    var treeView = window.arguments[0];
    var contractID = document.getElementById("contractID").value;
    treeView.addServer(contractID);
  }
  catch(e)
  {
    alert(e.toString());
    return false;
  }
}

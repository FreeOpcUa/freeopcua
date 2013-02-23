function onOK()
{
  try
  {
    var treeView = window.arguments[0];
    var groupName = document.getElementById("groupName").value;
    treeView.addGroup(groupName);
  }
  catch(e)
  {
    alert(e.toString());
    return false;
  }
}

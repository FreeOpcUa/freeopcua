/// @brief class definition for server item

function alertError(e)
{
  alert("error: " + e.toString());
}

function serverItemClass (contractID)
{
  this.opcServer = Components.classes[contractID].createInstance(Components.interfaces.nsIOPCServer);

  // item interface
  this.isServer = true;
  this.isVisible = true;
  this.name = contractID;
  this.parentIndex = -1; // for server is always null
  this.isOpen = true;
}

serverItemClass.prototype.finalizeXPCOM = function()
{
  alert("serverItemClass.finalizeXPCOM");
  this.opcServer = null;
}

/// @brief class definition for group item;
function groupItemClass(opcServer, groupName, parentIndex)
{
  // item interface
  this.isServer = false;
  this.name = groupName;
  this.isVisible = true;
  this.parentIndex = parentIndex;

  // group item interface
  var isActive = true;
  var updateRate = 100;
  var timeBias = 0;
  var deadband = 0;
  this.opcGroup = opcServer.addGroup(groupName, isActive, updateRate, timeBias, deadband);
  this.opcServer = opcServer;
}
groupItemClass.prototype.finalizeXPCOM = function()
{
  alert("groupItemClass.finalizeXPCOM");
  this.opcGroup = null;
  this.opcServer = null;
}


/// controller of tree control element.
function serverTree()
{
  this.Items = [];
  this.treeBox = null;
  this.selection = null;
}

serverTree.prototype.ThrowIfServerExist = function(contractID)
{
  var i = 0;
  for (i = 0; i < this.Items.length; i++)
  {
    if (this.Items[i].name == contractID)
    {
      throw "Server with contractID '" + contractID + "' already added.";
    }
  }
}

serverTree.prototype.addServer = function(serverContractID)
{
  try
  {
    this.ThrowIfServerExist(serverContractID);
    this.Items.push(new serverItemClass(serverContractID));
    this.treeBox.rowCountChanged(this.Items.length, 1);
  }
  catch (e)
  {
    alertError("Failed to add server with contract id '" + serverContractID + "':\n" + e.toString());
  }
}

serverTree.prototype.removeServer = function(serverIndex)
{
  if (serverIndex == -1 || !this.Items[serverIndex].isServer)
  {
    throw "No server item selected.";
  }
  var groupIndex = serverIndex + 1;
  while (groupIndex < this.Items.length && !this.Items[groupIndex].isServer)
  {
    this.removeGroup(groupIndex);
  }
  this.Items[serverIndex].finalizeXPCOM(); // this need to unlink OPCServer from array elements and gc should delete it
  // TODO removing server.
  this.Items.splice(serverIndex, 1);
  this.treeBox.rowCountChanged(serverIndex, -1);
  //TODO log that server deleted;
}

serverTree.prototype.getSelectedItemIndex = function()
{
  return document.getElementById("serverTree").currentIndex;
}


serverTree.prototype.getSelectedItemServerHandle = function()
{
  var listItems = document.getElementById("itemsList");
//  alert("listItems = " + listItems);
  var listRow = listItems.selectedItem;
//  alert("listRow = " + listRow);
  var id = listRow.id;
//  alert("id = " + id);
  var prUint32 = createPRUint32();
  prUint32.data = id;
  return prUint32;
}

serverTree.prototype.getCurrentOPCServer = function()
{
  var serverIndex = this.getSelectedItemIndex();
  var item = this.Items[serverIndex];
  return item.opcServer; 
}

serverTree.prototype.isGroupSelected = function()
{
  var item = this.Items[this.getSelectedItemIndex()];
  return !item.isServer;
}

serverTree.prototype.getCurrentOPCGroup = function()
{
  if (!this.isGroupSelected())
  {
    throw "Selected item is now a server item."
  }
  var item = this.Items[this.getSelectedItemIndex()];
  return item.opcGroup; 
}

serverTree.prototype.addGroup = function(groupName)
{
  var serverIndex = this.getSelectedItemIndex();
  var serverItem = this.getServerItem(serverIndex);
  this.Items.splice(serverIndex + 1, 0, new groupItemClass(serverItem.opcServer, groupName, serverIndex));
  this.treeBox.rowCountChanged(serverIndex, 1);
}

serverTree.prototype.removeGroup = function(groupIndex)
{
  if (groupIndex == -1)
  {
    throw "Invalid group index '" + groupIndex + "'";
  }
  if (this.Items[groupIndex].isServer)
  {
    throw "Index '" + groupIndex + "' point to server.";
  }
  // TODO log starting removing group.
  groupItem = this.Items[groupIndex];
  groupItem.opcServer.removeGroup(groupItem.opcGroup);
  groupItem.finalizeXPCOM();
  this.Items.splice(groupIndex, 1);
  this.treeBox.rowCountChanged(groupIndex, -1);
  // TODO log group removed
}

serverTree.prototype.addItems = function(itemIDs)
{
  try
  {
//    alert("addItems");
    var opcGroup = this.getCurrentOPCGroup().QueryInterface(Components.interfaces.nsIOPCItemMgt);
    var factory = opcGroup.QueryInterface(Components.interfaces.nsIOPCGroupInterfacesFactory);
    var itemDefs = Components.classes["@mozilla.org/array;1"].createInstance(Components.interfaces.nsIMutableArray);
    for (var id = 0; id < itemIDs.length; ++id)
    {
      var itemDef = factory.CreateItemDef();
      itemDef.ItemID = itemIDs[id];
      var isWeak = false;
      itemDefs.appendElement(itemDef, isWeak);
    }

    opcGroup.addItems(itemDefs);
    this.fillItemsList();
    return;
  }
  catch (e)
  {
    alertError(e);
  }
  alertError(this.getCurrentOPCServer().getLastError());
}


serverTree.prototype.clearItemsList = function()
{
  var listItems = document.getElementById("itemsList");
  while(listItems.getRowCount())
  {
    listItems.removeItemAt(0);
  }
}

function CreateItemCell(label, id)
{
  var cell = document.createElement("listcell");
  cell.setAttribute("label", label);
  cell.setAttribute("id", id);
  return cell;
}

function CreateItemRow(itemAttributes)
{
  var row = document.createElement("listitem");
  row.appendChild(CreateItemCell(itemAttributes.ItemID, "itemID_" + itemAttributes.ServerHandle));
  row.appendChild(CreateItemCell(itemAttributes.IsActive, "isActive_" + itemAttributes.ServerHandle));
  row.appendChild(CreateItemCell(1, "value_" + itemAttributes.ServerHandle));
  row.setAttribute("id", itemAttributes.ServerHandle);
  return row; 
}

serverTree.prototype.fillItemsList = function()
{
  try
  {
//    alert("fillItemsList");
    this.clearItemsList();

    if (!this.isGroupSelected())
    {
      return;
    }

    var opcGroup = this.getCurrentOPCGroup().QueryInterface(Components.interfaces.nsIOPCItemMgt);
    var itemsEnum = opcGroup.createEnumerator();
    var itemsList = document.getElementById("itemsList");
    var serverHandles = Components.classes["@mozilla.org/array;1"].createInstance(Components.interfaces.nsIMutableArray);
    while (itemsEnum.hasMoreElements())
    {
      var itemAttributes = itemsEnum.getNext().QueryInterface(Components.interfaces.nsIOPCItemAttributes);
//      alert(itemAttributes.ItemID);
      itemsList.appendChild(CreateItemRow(itemAttributes));

      var handle = Components.classes["@mozilla.org/supports-PRUint32;1"].createInstance(Components.interfaces.nsISupportsPRUint32);
//      alert("serverHandle = " + itemAttributes.ServerHandle);
      handle.data = itemAttributes.ServerHandle;
//      alert ("handle = " + handle);
      serverHandles.appendElement(handle, false); 
    }
//    alert("end");
    var fromCache = true;
    this.fillItemsValues(serverHandles, fromCache);
  }
  catch (e)
  {
    alertError(e);
  }
}

serverTree.prototype.fillItemsValues = function(serverHandles, fromCache)
{
  try
  {
    var syncIO = this.getCurrentOPCGroup().QueryInterface(Components.interfaces.nsIOPCSyncIO);
//    alert(syncIO);
    var values = syncIO.Read(true, serverHandles);
//    alert(values);
    for (var i = 0; i < values.length; ++i)
    {
      var itemState = values.queryElementAt(i, Components.interfaces.nsIOPCItemState);
//      alert(itemState);
//      alert("value = " + itemState.Data);
//      var variant = element.QueryInterface(Components.interfaces.nsIVariant);
//      alert(itemState.ServerHandle);
      document.getElementById("value_" + itemState.ServerHandle).setAttribute("label", itemState.Data);
    }
  }
  catch (e)
  {
    alert("Failed to read item values: " + e.toString() + "\n" + this.getCurrentOPCServer().getLastError());
  }
}


serverTree.prototype.showMenu = function()
{
  var tree = document.getElementById("serverTree");
  var idx = tree.currentIndex;
  var isServerSelected = (idx == -1 || this.Items[idx].isServer);
  document.getElementById("deleteServerMenu").hidden = !isServerSelected;
  document.getElementById("addGroupMenu").hidden = (idx == -1 || !isServerSelected);
  document.getElementById("deleteGroupMenu").hidden = isServerSelected;
}

serverTree.prototype.getServerItem = function(idx)
{
  if (idx == -1)
  {
    throw "No server selected.";
  }
  var serverItem = this.Items[idx];
  if (!serverItem.isServer)
  {
    throw "Selected item '" + serverItem.name + "' is not a server item";
  }
  return serverItem;
}

serverTree.prototype.getNextServerIndex = function(currentIndex)
{
  if (currentIndex < 0)
  {
    return -1;
  }

  var index = index + 1;
  while (index < this.Items.length)
  {
    if (this.Items[index].isServer)
    {
      return index;
    }
    ++index;
  }
  return groupIndex -1;
}

serverTree.prototype.__defineGetter__("rowCount", function() { return this.Items.length; })

serverTree.prototype.setTree = function(treeBox)
{
  this.treeBox = treeBox; 
}

serverTree.prototype.getCellText = function(idx, column)
{
  var text = this.Items[idx].name;
  return text; 
}

serverTree.prototype.isContainer = function(idx)
{
  var isServer = this.Items[idx].isServer;
  return isServer ? 1 : 0;
}

serverTree.prototype.isContainerOpen = function(idx)
{
  if (!this.Items[idx].isServer)
  {
    throw "Item with index '" + idx + "' is not a server";
  }
  return this.Items[idx].isOpen;
}

serverTree.prototype.isContainerEmpty = function(idx)
{
  if (!this.Items[idx].isServer)
  { 
    throw "isContainerEmpty: Item '" + idx + "' is not a container."
  }
  if (idx >= this.Items.length - 1)
  {
    return true;
  }
  if (!this.Items[idx + 1].isServer)
  {
    return false;
  }
  return true;
}

serverTree.prototype.isSeparator = function(idx)
{
  alert("isSeparator");
  return false;
}

serverTree.prototype.isSorted = function()
{
  return true;
}

serverTree.prototype.isEditable = function(idx, column)
{
  alert("isEditable");
  return false;
}

serverTree.prototype.getParentIndex = function(idx)
{
  alert("getParentIndex");
  if (this.isContainer(idx))
  {
    return -1;
  }

  for (var t = idx - 1; t >= 0 ; t--)
  {
    if (this.isContainer(t)) return t;
  }
}

serverTree.prototype.getLevel = function(idx)
{
  var isServer = this.Items[idx].isServer;
  return isServer ? 0 : 1;
}

serverTree.prototype.hasNextSibling = function(idx, after)
{
  alert("hasNextSibling");
  var thisLevel = this.getLevel(idx);
  for (var t = idx + 1; t < this.Itemsa.length; ++t)
  {
    var nextLevel = this.getLevel(t)
    if (nextLevel == thisLevel)
    {
      return true;
    }
    else if (nextLevel < thisLevel)
    {
      return false;
    }
  }
}

serverTree.prototype.toggleOpenState = function(idx)
{
  alert("toggleOpenStat");
}

serverTree.prototype.getImageSrc = function(idx, column)
{
  return "";
}

serverTree.prototype.getProgressMode = function(idx,column)
{
  alert("getProgressMode");
}

serverTree.prototype.getCellValue = function(idx, column)
{
  alert("getCellValue");
}

serverTree.prototype.cycleHeader = function(col, elem)
{
  alert("cycleHeader");
}

serverTree.prototype.selectionChanged = function()
{
  alert("selectionChanged");
}

serverTree.prototype.cycleCell = function(idx, column)
{
  alert("cycleCell");
}

serverTree.prototype.performAction = function(action)
{
  alert("performAction");
}

serverTree.prototype.performActionOnCell = function(action, index, column)
{
  alert("performActionCell");
}

serverTree.prototype.getRowProperties = function(idx, column, prop)
{
  alert("getRowProperties");
}

serverTree.prototype.getCellProperties = function(idx, column, prop)
{
//  alert("getCellProperties");
}

serverTree.prototype.getColumnProperties = function(column, element, prop)
{
  alert("getColumnProperties");
}

var treeView;
function init() 
{
  treeView = new serverTree;
  document.getElementById("serverTree").view = treeView;
}


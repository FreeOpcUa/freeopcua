var ItemID = 0;

function alertError(e)
{
  alert("error: " + e.toString());
}

/// @brief class definition for server item
function ItemClass (opcBrowseElement, parentID)
{
  // item interfacie
  this.ID = ++ItemID;
  this.isOpen = false;
  this.parentID = parentID;
  this.isContainer = !opcBrowseElement.isItem;
  this.itemName = opcBrowseElement.name;
  this.opcID = opcBrowseElement.itemID;
  this.hasChildren = opcBrowseElement.hasChildren;
};

/// controller of tree control element.
function serverTree(treeView)
{
  this.Items = [];
  this.treeBox = null;
  this.selection = null;
  
  var opcServer = treeView.getCurrentOPCServer();
  this.opcBrowse = opcServer.QueryInterface(Components.interfaces.nsIOPCBrowse);
 
  try
  {
    var en = this.opcBrowse.browse("", Components.interfaces.nsIOPCBrowse.OPC_BROWSE_FILTER_ALL, "", "", false, false, null);
    while (en.hasMoreElements())
    {
      var supports = en.getNext();
      var browseElement = supports.QueryInterface(Components.interfaces.nsIOPCBrowseElement);
      this.Items.push(new ItemClass(browseElement, -1));
    }
  }
  catch (e)
  {
    alertError(e);
  }
}

serverTree.prototype.finalizeXPCOM = function()
{
  this.opcBrowse = null;
}

serverTree.prototype.__defineGetter__("rowCount", function() { return this.Items.length; })

serverTree.prototype.setTree = function(treeBox)
{
  this.treeBox = treeBox; 
}

serverTree.prototype.getCellText = function(idx, column)
{
  try
  {
    return  this.Items[idx].itemName;
  }
  catch (e)
  {
    alertError(e);
  }
}

serverTree.prototype.isContainer = function(idx)
{
  try
  {
    return this.Items[idx].isContainer;
  }
  catch(e)
  {
    alertError(e);
  }
}

serverTree.prototype.isContainerOpen = function(idx)
{
  try
  {
    return this.Items[idx].isOpen;
  }
  catch (e)
  {
    alertError(e);
  }
}

serverTree.prototype.isContainerEmpty = function(idx)
{
  try
  {
    return !this.Items[idx].hasChildren;
  }
  catch (e)
  {
    alertError(e);
  }
}

serverTree.prototype.isSeparator = function(idx)
{
  return false;
}

serverTree.prototype.isSorted = function()
{
  return true;
}

serverTree.prototype.isEditable = function(idx, column)
{
  return false;
}

serverTree.prototype.isSelectable = function(idx, column)
{
  alert ("isSelectable");
  return !this.Items[idx].isContainer;
}

serverTree.prototype.getParentIndex = function(idx)
{
  alert("getParentIndex");
  try
  {
    for (var i = 0; i < idx; ++i)
    {
      if (this.Items[idx].patentID == this.Items[i].ID)
      {
        return i;
      }
    }
    return -1;
  }
  catch (e)
  {
    alertError(e);
  }
}

serverTree.prototype.getLevel = function(idx)
{
  try
  {
    var opcID = this.Items[idx].opcID;
    var level = 0;
    for (var i = 0; i < opcID.length; i++)
    {
      if (i != 0 && opcID.charAt(i) == "/")
      {
        ++level;
      }
    }
    return level;
  }
  catch (e)
  {
    alertError(e);
  }
}

serverTree.prototype.hasNextSibling = function(idx, after)
{
  try
  {
    for (var t = idx + 1; t < this.Items.length; ++t)
    {
      if (this.Items[t].parentID == this.Items[idx].parentID)
      {
        return true;
      }
    }
   return false;
  }
  catch (e)
  {
    alertError(e);
  }
}

serverTree.prototype.toggleOpenState = function(idx)
{
  try
  {
    // if group item is closed - open it
    if (!this.Items[idx].isOpen)
    {
//      alert("adding items");
      var newItemsCount = 0;
      var en = this.opcBrowse.browse(this.Items[idx].opcID, Components.interfaces.nsIOPCBrowse.OPC_BROWSE_FILTER_ALL, "", "", false, false, null);
      while (en.hasMoreElements())
      {
        var supports = en.getNext();
        var browseElement = supports.QueryInterface(Components.interfaces.nsIOPCBrowseElement);
        this.Items.splice(idx + 1, 0, new ItemClass(browseElement, this.Items[idx].ID));
        ++newItemsCount;
      }
//      alert("newItems.length = " + newItemsCount);
      this.Items[idx].isOpen = true;
//      alert("isOpen = " + this.Items[idx].isOpen);
      this.treeBox.rowCountChanged(idx, newItemsCount);
      return;
    } 

//    alert("deleting items items");
    var countForDelete = 0;
    for (var i = idx + 1; i < this.Items.length; ++i)
    {
      if (this.Items[i].parentID == this.Items[idx].parentID)
      {
        break;
      }
      ++countForDelete;
    }
//    alert("countForDelete = " + countForDelete);
    this.Items[idx].isOpen = false;
    this.Items.splice(idx + 1, countForDelete);
    this.treeBox.rowCountChanged(idx + 1, countForDelete);
  }
  catch (e)
  {
    alertError(e);
  }
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
}

serverTree.prototype.getColumnProperties = function(column, element, prop)
{
  alert("getColumnProperties");
}

var itemsTree = null;

function init() 
{
  try
  {
    var treeView = window.arguments[0];
    itemsTree = new serverTree(treeView);
    document.getElementById("itemsTree").view = itemsTree;
  }
  catch(e)
  {
    alertError(e);
    return false;
  }
}

function onOK()
{
  try
  {
    var selection = document.getElementById("itemsTree").view.selection;
    var itemIDs = [];
//    alert (selection);
    for (var i = 0; i < itemsTree.Items.length; ++i)
    {
      if (selection.isSelected(i) && !itemsTree.Items[i].isContainer)
      {
        itemIDs.push(itemsTree.Items[i].opcID);
      }
    }
    itemsTree.finalizeXPCOM();
    itemsTree = null;
    var treeView = window.arguments[0];
    treeView.addItems(itemIDs);
   return true;
  }
  catch (e)
  {
    alertError(e);
  }
}

function onCancel()
{
  itemsTree.finalizeXPCOM();
  itemsTree = null;
  return true;
}


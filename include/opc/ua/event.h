
#pragma once

#include <opc/ua/protocol/nodeid.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/variant.h>
#include <opc/ua/protocol/attribute.h>

#include <map>



namespace OpcUa
{
  typedef std::map<std::vector<QualifiedName>, Variant> PathMap;
  typedef std::map<AttributeID, Variant> AttributeMap;

  class Event
  {

    public:
      //Instanciate a new event object
      //The optional argument is the type of the event
      //If not set BaseEventType is used
      //If you want to send custom data, you need custom event type on server
      Event(const NodeID& type);
      Event();
      //Set value of a variable(or object)
      //This value will be used when the event is fired
      //You can set arbitrary data, but clients will not be able to discover them thus subscribe to them
      void SetValue(const std::vector<QualifiedName>& path, Variant value);
      void SetValue(AttributeID attribute, Variant value);
      void SetValue(const std::string& qualifiedname, Variant value); //helper method for the most common case
      
      //Return value og variable identified by its relative path
      //or value of attribute identified by its ID
      //returns null variant if no match
      Variant GetValue(const std::vector<QualifiedName>& path);
      Variant GetValue(AttributeID attribute);
      Variant GetValue(const std::string& qualifiedname); //helper method for the most common case

    protected:
      NodeID Type;
      PathMap PathValues; 
      AttributeMap AttributeValues; 
  };
}


#pragma once

#include <opc/ua/node.h>
#include <opc/ua/protocol/nodeid.h>
#include <opc/ua/protocol/types.h>
#include <opc/ua/protocol/variant.h>
#include <opc/ua/protocol/protocol.h>

#include <map>



namespace OpcUa
{
  typedef std::map<std::vector<QualifiedName>, Variant> PathMap;
  typedef std::map<AttributeId, Variant> AttributeMap;

  class Event
  {

    public:
      //Instanciate a new event object
      //The optional argument is the type of the event
      //If not set BaseEventType is used
      //If you want to send custom data, you need custom event type on server
      //The attributes of BaseEventType are accesible as class member,
      //Other attributes must be set using SetValue and accessed using GetValue
      // This API is a draft, it WILL change
      Event(const Node& type);
      Event(const NodeId& type);
      Event();
      
      //Attributes of an BaseEventType
      ByteString EventId;     //unique id
      NodeId EventType;       //The type of event, defines attribues, connections, ...
      DateTime LocalTime;     //Time zone information
      DateTime ReceiveTime;   //Time at which the server received the event
      DateTime Time;          //Time at which the event occured, should come from underlying system
      LocalizedText Message;  //human readable event description
      uint16_t Severity = 1;  //Priority, from 1 to 1000. 1 is lowest priority
      NodeId SourceNode;      //The node that has generated the event
      std::string SourceName; //Description of the source


      //Set value of a variable(or object)
      //This value will be used when the event is fired
      //You can set arbitrary data, but clients will not be able to discover them thus subscribe to them
      void SetValue(const std::vector<QualifiedName>& path, Variant value);
      void SetValue(AttributeId attribute, Variant value);
      void SetValue(const QualifiedName& path, Variant value);
      void SetValue(const std::string& qualifiedname, Variant value); //helper method for the most common case
      
      //Return value og variable identified by its relative path
      //or value of attribute identified by its Id
      //returns null variant if no match
      Variant GetValue(const std::vector<QualifiedName>& path) const;
      Variant GetValue(AttributeId attribute) const;
      Variant GetValue(const QualifiedName& path) const;
      Variant GetValue(const std::string& qualifiedname) const; //helper method for the most common case

      //Get the list of available values in this event
      std::vector<std::vector<QualifiedName>> GetValueKeys();

    protected:
      PathMap PathValues; 
      AttributeMap AttributeValues; 
  };

  std::string ToString(const Event& event);

  inline std::ostream& operator<<(std::ostream& os, const Event& event)
  {
   os << "{" << ToString(event) << "}";
   return os;
  }

}

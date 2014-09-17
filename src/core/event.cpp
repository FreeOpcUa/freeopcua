
#include <iostream> //Debug

#include <opc/ua/event.h>
#include <opc/ua/protocol/string_utils.h>

namespace OpcUa
{

  Event::Event() : EventType(ObjectID::BaseEventType) 
  {
  }

  Event::Event(const NodeID& type) : EventType(type) {}

  void Event::SetValue(const QualifiedName& path, Variant value)
  {
    SetValue(std::vector<QualifiedName>({path}), value);
  }

  void Event::SetValue(const std::vector<QualifiedName>& path, Variant value)
  {
    PathValues[path] = value;
  }

  void Event::SetValue(AttributeID attribute, Variant value)
  {
    AttributeValues[attribute] = value;
  }

  void Event::SetValue(const std::string& name, Variant value)
  {
    std::vector<QualifiedName> path;
    path.push_back(ToQualifiedName(name));
    return SetValue(path, value);
  }

  Variant Event::GetValue(const QualifiedName& path) const
  {
    return GetValue(std::vector<QualifiedName>({path}));
  }

  Variant Event::GetValue(const std::vector<QualifiedName>& path) const
  {
    PathMap::const_iterator it = PathValues.find(path);
      std::cout << "Looking for Event value: ";
      for ( auto qn : path ){
        std::cout << qn ;
      }
      std::cout << std::endl;
    if ( it == PathValues.end() )
    {
      std::cout << "Bad Not found ";
      return Variant();
    }
    else
    {
      std::cout << "ok" << std::endl;
      return it->second;
    }
      std::cout << std::endl;
  }

  Variant Event::GetValue(const std::string& qualifiedname) const
  {
    std::vector<QualifiedName> path;
    path.push_back(ToQualifiedName(qualifiedname));
    return GetValue(path);
  }

  Variant Event::GetValue(AttributeID attribute) const
  {
    AttributeMap::const_iterator it = AttributeValues.find(attribute);
    if ( it == AttributeValues.end() )
    {
      return Variant();
    }
    else
    {
      return it->second;
    }
  }

}

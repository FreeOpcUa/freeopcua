

#include <opc/ua/event.h>
#include <opc/ua/protocol/string_utils.h>

namespace OpcUa
{

  Event::Event() : Type(ObjectID::EventType) {}

  Event::Event(const NodeID& type) : Type(type) {}

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

  Variant Event::GetValue(const std::vector<QualifiedName>& path) const
  {
    PathMap::const_iterator it = PathValues.find(path);
    if ( it == PathValues.end() )
    {
      return Variant();
    }
    else
    {
      return it->second;
    }
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

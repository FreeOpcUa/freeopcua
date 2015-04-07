
#include <iostream> //Debug

#include <opc/ua/event.h>
#include <opc/ua/protocol/string_utils.h>

namespace OpcUa
{

  Event::Event() : EventType(ObjectId::BaseEventType) 
  {
  }

  Event::Event(const NodeId& type) : EventType(type) {}

  Event::Event(const Node& type) : EventType(type.GetId()) {}

  std::vector<std::vector<QualifiedName>> Event::GetValueKeys()
  {
    std::vector<std::vector<QualifiedName>> qns;
    for (auto qn : PathValues)
    {
      qns.push_back(qn.first);
    }
    return qns;
  }

  void Event::SetValue(const QualifiedName& path, Variant value)
  {
    SetValue(std::vector<QualifiedName>({path}), value);
  }

  void Event::SetValue(const std::vector<QualifiedName>& path, Variant value)
  {
    PathValues[path] = value;
  }

  void Event::SetValue(AttributeId attribute, Variant value)
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
      std::cout << "Bad Not found " << std::endl;
      return Variant();
    }
    else
    {
      std::cout << "ok" << std::endl;
      return it->second;
    }
  }

  Variant Event::GetValue(const std::string& qualifiedname) const
  {
    std::vector<QualifiedName> path;
    path.push_back(ToQualifiedName(qualifiedname));
    return GetValue(path);
  }

  Variant Event::GetValue(AttributeId attribute) const
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

  std::string ToString(const Event& event)
  {
    std::stringstream stream;
    stream << "Event(type:" << event.EventType << ", time:" << event.Time << ", source:" << event.SourceNode << ", severity:" << event.Severity << ", message:" << event.Message << ")" ;
    return stream.str();
  }
}

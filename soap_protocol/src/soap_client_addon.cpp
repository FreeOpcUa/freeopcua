/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief OPC UA Address space part.
/// @license GNU GPL
///
/// Distributed under the GNU GPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/gpl.html)
///

#include "serialization/deserialize.h"
#include "serialization/serialize.h"


#include <opc/ua/client/addon.h>

#include <soapBasicHttpBinding_USCOREIDiscoveryEndpointProxy.h>
#include <soapBasicHttpBinding_USCOREISessionEndpointProxy.h>

#include <sstream>

namespace
{

  class SoapEndpoints : public OpcUa::Remote::EndpointServices
  {
  public:
    SoapEndpoints(const std::string& url)
      : Url(url)
    {
    }

    virtual std::vector<OpcUa::ApplicationDescription> FindServers(const OpcUa::FindServersParameters& params) const
    {
      return std::vector<OpcUa::ApplicationDescription>();
    }

    virtual std::vector<OpcUa::EndpointDescription> GetEndpoints(const OpcUa::EndpointsFilter& filter) const
    {
      BasicHttpBinding_USCOREIDiscoveryEndpointProxy service(filter.EndpointURL.c_str());
      OpcUa::GetEndpointsRequest request;
      request.Filter = filter;
      ns3__GetEndpointsRequest* soapRequest = OpcUa::Soap::Serialize(&service, request);
      ns3__GetEndpointsResponse* soapResponse = soap_new_ns3__GetEndpointsResponse(&service, 1);

      const int soapError = service.GetEndpoints(Url.c_str(), 0, soapRequest, soapResponse);
      if (soapError != SOAP_OK)
      {
        std::stringstream stream;
        service.soap_stream_fault(stream);
        throw std::logic_error(stream.str());
      }

      OpcUa::GetEndpointsResponse resp = OpcUa::Soap::Deserialize(soapResponse);

      return resp.Endpoints;
    }

    virtual void RegisterServer(const OpcUa::Remote::ServerParameters& parameters)
    {

    }

  private:
    ns3__GetEndpointsRequest* GetEndpointsRequest(soap* service, const OpcUa::EndpointsFilter& filter) const
    {
      ns3__GetEndpointsRequest* request = soap_new_ns3__GetEndpointsRequest(service, 1);
      return request;
    }


  public:
    std::string Url;
  };

  class SoapViews : public OpcUa::Remote::ViewServices
  {
  public:
    SoapViews(const std::string& url)
      : Url(url)
    {
    }

    virtual std::vector<OpcUa::ReferenceDescription> Browse(const OpcUa::NodesQuery& query) const
    {
      OpcUa::BrowseRequest request;
      request.Query = query;
      BasicHttpBinding_USCOREISessionEndpointProxy service(Url.c_str());
      ns3__BrowseRequest* soapRequest = OpcUa::Soap::Serialize(&service, request);
      ns3__BrowseResponse* soapResponse = soap_new_ns3__BrowseResponse(&service, 1);
      const int soapError = service.Browse(soapRequest, soapResponse);
      if (soapError != SOAP_OK)
      {
        std::stringstream stream;
        service.soap_stream_fault(stream);
        throw std::logic_error(stream.str());
      }
      const OpcUa::BrowseResponse response = OpcUa::Soap::Deserialize(soapResponse);
      if (response.Results.empty())
      {
        return std::vector<OpcUa::ReferenceDescription>();
      }
      //TODO Must return list of BrowseResult
      return response.Results.front().Referencies;
    }

    virtual std::vector<OpcUa::ReferenceDescription> BrowseNext() const
    {
      return std::vector<OpcUa::ReferenceDescription>();
    }

  private:
    std::string Url;
  };

  class SoapAttributes : public OpcUa::Remote::AttributeServices
  {
  public:
    virtual std::vector<OpcUa::DataValue> Read(const OpcUa::ReadParameters& filter) const
    {
      return std::vector<OpcUa::DataValue>();
    }

    virtual std::vector<OpcUa::StatusCode> Write(const std::vector<OpcUa::WriteValue>& filter)
    {
      return std::vector<OpcUa::StatusCode>();
    }
  };

  class SoapSubscriptions : public OpcUa::Remote::SubscriptionServices
  {
  public:
    virtual OpcUa::SubscriptionData CreateSubscription(const OpcUa::SubscriptionParameters& parameters)
    {
      return OpcUa::SubscriptionData();
    }
  };

  class SoapComputer : public OpcUa::Remote::Computer
  {
  public:
    SoapComputer(const std::string& url)
      : Url(url)
    {

    }

    virtual void CreateSession(const OpcUa::Remote::SessionParameters& parameters)
    {

    }

    virtual void ActivateSession()
    {

    }

    virtual void CloseSession()
    {

    }

    virtual OpcUa::Remote::EndpointServices::SharedPtr Endpoints() const
    {
      return OpcUa::Remote::EndpointServices::SharedPtr(new SoapEndpoints(Url));
    }

    virtual OpcUa::Remote::ViewServices::SharedPtr Views() const
    {
      return OpcUa::Remote::ViewServices::SharedPtr(new SoapViews(Url));
    }

    virtual OpcUa::Remote::AttributeServices::SharedPtr Attributes() const
    {
      return OpcUa::Remote::AttributeServices::SharedPtr();
    }

    virtual OpcUa::Remote::SubscriptionServices::SharedPtr Subscriptions() const
    {
      return OpcUa::Remote::SubscriptionServices::SharedPtr();
    }

  public:
    std::string Url;
  };


  class SoapClientAddon : public OpcUa::Client::Addon
  {
  public:
    virtual void Initialize(Common::AddonsManager& manager, const Common::AddonParameters& parameters)
    {

    }

    virtual void Stop()
    {

    }

  public:
    virtual std::string GetProtocol() const
    {
      return "http";
    }

    virtual OpcUa::Remote::Computer::SharedPtr Connect(const std::string& url)
    {
      return OpcUa::Remote::Computer::SharedPtr(new SoapComputer(url));
    }
  };
}

extern "C" Common::Addon::UniquePtr CreateAddon()
{
  return Common::Addon::UniquePtr(new SoapClientAddon());
}


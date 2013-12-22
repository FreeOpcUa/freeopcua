/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Class which implements Discovery service over soap.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include "endpoint_service.h"

#include "serialization/serialize.h"
#include "serialization/deserialize.h"

namespace OpcUa
{
  namespace Soap
  {
    BasicHttpBinding_USCOREISessionEndpointService *EndpointService::copy()
    {
      return new EndpointService(Computer, Debug);
    }

    int EndpointService::CreateSession(ns3__CreateSessionRequest *ns3__CreateSessionRequest_, ns3__CreateSessionResponse *ns3__CreateSessionResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::ActivateSession(ns3__ActivateSessionRequest *ns3__ActivateSessionRequest_, ns3__ActivateSessionResponse *ns3__ActivateSessionResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::CloseSession(ns3__CloseSessionRequest *ns3__CloseSessionRequest_, ns3__CloseSessionResponse *ns3__CloseSessionResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::Cancel(ns3__CancelRequest *ns3__CancelRequest_, ns3__CancelResponse *ns3__CancelResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::AddNodes(ns3__AddNodesRequest *ns3__AddNodesRequest_, ns3__AddNodesResponse *ns3__AddNodesResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::AddReferences(ns3__AddReferencesRequest *ns3__AddReferencesRequest_, ns3__AddReferencesResponse *ns3__AddReferencesResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::DeleteNodes(ns3__DeleteNodesRequest *ns3__DeleteNodesRequest_, ns3__DeleteNodesResponse *ns3__DeleteNodesResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::DeleteReferences(ns3__DeleteReferencesRequest *ns3__DeleteReferencesRequest_, ns3__DeleteReferencesResponse *ns3__DeleteReferencesResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::Browse(ns3__BrowseRequest *ns3__BrowseRequest_, ns3__BrowseResponse *ns3__BrowseResponse_)
    {
      if (Debug) std::clog << "SOAP: Received BrowseRequest." << std::endl;

      const OpcUa::BrowseRequest request = OpcUa::Soap::Deserialize(ns3__BrowseRequest_);
      BrowseResult result;
      result.Referencies = Computer->Views()->Browse(request.Query);
      OpcUa::BrowseResponse response;
      response.Results.push_back(result);
      *ns3__BrowseResponse_ = *OpcUa::Soap::Serialize(this, response);

      if (Debug) std::clog << "SOAP: Processed BrowseRequest." << std::endl;
      return SOAP_OK;
    }

    int EndpointService::BrowseNext(ns3__BrowseNextRequest *ns3__BrowseNextRequest_, ns3__BrowseNextResponse *ns3__BrowseNextResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::TranslateBrowsePathsToNodeIds(ns3__TranslateBrowsePathsToNodeIdsRequest *ns3__TranslateBrowsePathsToNodeIdsRequest_, ns3__TranslateBrowsePathsToNodeIdsResponse *ns3__TranslateBrowsePathsToNodeIdsResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::RegisterNodes(ns3__RegisterNodesRequest *ns3__RegisterNodesRequest_, ns3__RegisterNodesResponse *ns3__RegisterNodesResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::UnregisterNodes(ns3__UnregisterNodesRequest *ns3__UnregisterNodesRequest_, ns3__UnregisterNodesResponse *ns3__UnregisterNodesResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::QueryFirst(ns3__QueryFirstRequest *ns3__QueryFirstRequest_, ns3__QueryFirstResponse *ns3__QueryFirstResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::QueryNext(ns3__QueryNextRequest *ns3__QueryNextRequest_, ns3__QueryNextResponse *ns3__QueryNextResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::Read(ns3__ReadRequest *ns3__ReadRequest_, ns3__ReadResponse *ns3__ReadResponse_)
    {
      if (Debug) std::clog << "SOAP: Received ReadRequest." << std::endl;

      const OpcUa::ReadRequest request = OpcUa::Soap::Deserialize(ns3__ReadRequest_);
      OpcUa::ReadResponse response;
      response.Result.Results = Computer->Attributes()->Read(request.Parameters);
      *ns3__ReadResponse_ = *OpcUa::Soap::Serialize(this, response);

      if (Debug) std::clog << "SOAP: Processed ReadRequest." << std::endl;
      return SOAP_OK;
    }

    int EndpointService::HistoryRead(ns3__HistoryReadRequest *ns3__HistoryReadRequest_, ns3__HistoryReadResponse *ns3__HistoryReadResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::Write(ns3__WriteRequest *ns3__WriteRequest_, ns3__WriteResponse *ns3__WriteResponse_)
    {
      if (Debug) std::clog << "SOAP: Received WriteRequest." << std::endl;

      const OpcUa::WriteRequest request = OpcUa::Soap::Deserialize(ns3__WriteRequest_);
      OpcUa::WriteResponse response;
      response.Result.StatusCodes = Computer->Attributes()->Write(request.Parameters.NodesToWrite);
      *ns3__WriteResponse_ = *OpcUa::Soap::Serialize(this, response);

      if (Debug) std::clog << "SOAP: Processed WriteRequest." << std::endl;
      return SOAP_OK;
    }

    int EndpointService::HistoryUpdate(ns3__HistoryUpdateRequest *ns3__HistoryUpdateRequest_, ns3__HistoryUpdateResponse *ns3__HistoryUpdateResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::Call(ns3__CallRequest *ns3__CallRequest_, ns3__CallResponse *ns3__CallResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::CreateMonitoredItems(ns3__CreateMonitoredItemsRequest *ns3__CreateMonitoredItemsRequest_, ns3__CreateMonitoredItemsResponse *ns3__CreateMonitoredItemsResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::ModifyMonitoredItems(ns3__ModifyMonitoredItemsRequest *ns3__ModifyMonitoredItemsRequest_, ns3__ModifyMonitoredItemsResponse *ns3__ModifyMonitoredItemsResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::SetMonitoringMode(ns3__SetMonitoringModeRequest *ns3__SetMonitoringModeRequest_, ns3__SetMonitoringModeResponse *ns3__SetMonitoringModeResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::SetTriggering(ns3__SetTriggeringRequest *ns3__SetTriggeringRequest_, ns3__SetTriggeringResponse *ns3__SetTriggeringResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::DeleteMonitoredItems(ns3__DeleteMonitoredItemsRequest *ns3__DeleteMonitoredItemsRequest_, ns3__DeleteMonitoredItemsResponse *ns3__DeleteMonitoredItemsResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::CreateSubscription(ns3__CreateSubscriptionRequest *ns3__CreateSubscriptionRequest_, ns3__CreateSubscriptionResponse *ns3__CreateSubscriptionResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::ModifySubscription(ns3__ModifySubscriptionRequest *ns3__ModifySubscriptionRequest_, ns3__ModifySubscriptionResponse *ns3__ModifySubscriptionResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::SetPublishingMode(ns3__SetPublishingModeRequest *ns3__SetPublishingModeRequest_, ns3__SetPublishingModeResponse *ns3__SetPublishingModeResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::Publish(ns3__PublishRequest *ns3__PublishRequest_, ns3__PublishResponse *ns3__PublishResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::Republish(ns3__RepublishRequest *ns3__RepublishRequest_, ns3__RepublishResponse *ns3__RepublishResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::TransferSubscriptions(ns3__TransferSubscriptionsRequest *ns3__TransferSubscriptionsRequest_, ns3__TransferSubscriptionsResponse *ns3__TransferSubscriptionsResponse_)
    {
      return SOAP_OK;
    }

    int EndpointService::DeleteSubscriptions(ns3__DeleteSubscriptionsRequest *ns3__DeleteSubscriptionsRequest_, ns3__DeleteSubscriptionsResponse *ns3__DeleteSubscriptionsResponse_)
    {
      return SOAP_OK;
    }

  } // namespace Impl
} // namespace OpcUa

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

namespace OpcUa
{
  namespace Impl
  {
    BasicHttpBinding_USCOREISessionEndpointService *SoapEndpointService::copy()
    {
      return new SoapEndpointService(Computer, Debug);
    }

    int SoapEndpointService::CreateSession(ns3__CreateSessionRequest *ns3__CreateSessionRequest_, ns3__CreateSessionResponse *ns3__CreateSessionResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::ActivateSession(ns3__ActivateSessionRequest *ns3__ActivateSessionRequest_, ns3__ActivateSessionResponse *ns3__ActivateSessionResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::CloseSession(ns3__CloseSessionRequest *ns3__CloseSessionRequest_, ns3__CloseSessionResponse *ns3__CloseSessionResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::Cancel(ns3__CancelRequest *ns3__CancelRequest_, ns3__CancelResponse *ns3__CancelResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::AddNodes(ns3__AddNodesRequest *ns3__AddNodesRequest_, ns3__AddNodesResponse *ns3__AddNodesResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::AddReferences(ns3__AddReferencesRequest *ns3__AddReferencesRequest_, ns3__AddReferencesResponse *ns3__AddReferencesResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::DeleteNodes(ns3__DeleteNodesRequest *ns3__DeleteNodesRequest_, ns3__DeleteNodesResponse *ns3__DeleteNodesResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::DeleteReferences(ns3__DeleteReferencesRequest *ns3__DeleteReferencesRequest_, ns3__DeleteReferencesResponse *ns3__DeleteReferencesResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::Browse(ns3__BrowseRequest *ns3__BrowseRequest_, ns3__BrowseResponse *ns3__BrowseResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::BrowseNext(ns3__BrowseNextRequest *ns3__BrowseNextRequest_, ns3__BrowseNextResponse *ns3__BrowseNextResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::TranslateBrowsePathsToNodeIds(ns3__TranslateBrowsePathsToNodeIdsRequest *ns3__TranslateBrowsePathsToNodeIdsRequest_, ns3__TranslateBrowsePathsToNodeIdsResponse *ns3__TranslateBrowsePathsToNodeIdsResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::RegisterNodes(ns3__RegisterNodesRequest *ns3__RegisterNodesRequest_, ns3__RegisterNodesResponse *ns3__RegisterNodesResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::UnregisterNodes(ns3__UnregisterNodesRequest *ns3__UnregisterNodesRequest_, ns3__UnregisterNodesResponse *ns3__UnregisterNodesResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::QueryFirst(ns3__QueryFirstRequest *ns3__QueryFirstRequest_, ns3__QueryFirstResponse *ns3__QueryFirstResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::QueryNext(ns3__QueryNextRequest *ns3__QueryNextRequest_, ns3__QueryNextResponse *ns3__QueryNextResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::Read(ns3__ReadRequest *ns3__ReadRequest_, ns3__ReadResponse *ns3__ReadResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::HistoryRead(ns3__HistoryReadRequest *ns3__HistoryReadRequest_, ns3__HistoryReadResponse *ns3__HistoryReadResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::Write(ns3__WriteRequest *ns3__WriteRequest_, ns3__WriteResponse *ns3__WriteResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::HistoryUpdate(ns3__HistoryUpdateRequest *ns3__HistoryUpdateRequest_, ns3__HistoryUpdateResponse *ns3__HistoryUpdateResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::Call(ns3__CallRequest *ns3__CallRequest_, ns3__CallResponse *ns3__CallResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::CreateMonitoredItems(ns3__CreateMonitoredItemsRequest *ns3__CreateMonitoredItemsRequest_, ns3__CreateMonitoredItemsResponse *ns3__CreateMonitoredItemsResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::ModifyMonitoredItems(ns3__ModifyMonitoredItemsRequest *ns3__ModifyMonitoredItemsRequest_, ns3__ModifyMonitoredItemsResponse *ns3__ModifyMonitoredItemsResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::SetMonitoringMode(ns3__SetMonitoringModeRequest *ns3__SetMonitoringModeRequest_, ns3__SetMonitoringModeResponse *ns3__SetMonitoringModeResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::SetTriggering(ns3__SetTriggeringRequest *ns3__SetTriggeringRequest_, ns3__SetTriggeringResponse *ns3__SetTriggeringResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::DeleteMonitoredItems(ns3__DeleteMonitoredItemsRequest *ns3__DeleteMonitoredItemsRequest_, ns3__DeleteMonitoredItemsResponse *ns3__DeleteMonitoredItemsResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::CreateSubscription(ns3__CreateSubscriptionRequest *ns3__CreateSubscriptionRequest_, ns3__CreateSubscriptionResponse *ns3__CreateSubscriptionResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::ModifySubscription(ns3__ModifySubscriptionRequest *ns3__ModifySubscriptionRequest_, ns3__ModifySubscriptionResponse *ns3__ModifySubscriptionResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::SetPublishingMode(ns3__SetPublishingModeRequest *ns3__SetPublishingModeRequest_, ns3__SetPublishingModeResponse *ns3__SetPublishingModeResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::Publish(ns3__PublishRequest *ns3__PublishRequest_, ns3__PublishResponse *ns3__PublishResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::Republish(ns3__RepublishRequest *ns3__RepublishRequest_, ns3__RepublishResponse *ns3__RepublishResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::TransferSubscriptions(ns3__TransferSubscriptionsRequest *ns3__TransferSubscriptionsRequest_, ns3__TransferSubscriptionsResponse *ns3__TransferSubscriptionsResponse_)
    {
      return SOAP_OK;
    }

    int SoapEndpointService::DeleteSubscriptions(ns3__DeleteSubscriptionsRequest *ns3__DeleteSubscriptionsRequest_, ns3__DeleteSubscriptionsResponse *ns3__DeleteSubscriptionsResponse_)
    {
      return SOAP_OK;
    }

  } // namespace Impl
} // namespace OpcUa

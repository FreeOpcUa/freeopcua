/// @author Alexander Rykovanov 2013
/// @email rykovanov.as@gmail.com
/// @brief Class which implements Discovery service over soap.
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#pragma once

#include <opc/ua/computer.h>
#include <soapBasicHttpBinding_USCOREISessionEndpointService.h>


namespace OpcUa
{
  namespace Soap
  {
    class EndpointService : public BasicHttpBinding_USCOREISessionEndpointService
    {
      typedef BasicHttpBinding_USCOREISessionEndpointService ParentType;

    public:
      EndpointService(OpcUa::Remote::Computer::SharedPtr computer, bool debug)
        : Computer(computer)
        , Debug(debug)
      {
      }

      EndpointService(OpcUa::Remote::Computer::SharedPtr computer, bool debug, struct soap& s)
        : ParentType(s)
        , Computer(computer)
        , Debug(debug)
      {
      }

      EndpointService(OpcUa::Remote::Computer::SharedPtr computer, bool debug, soap_mode iomode)
        : ParentType(iomode)
        , Computer(computer)
        , Debug(debug)
      {
      }

      EndpointService(OpcUa::Remote::Computer::SharedPtr computer, bool debug, soap_mode imode, soap_mode omode)
        : ParentType(imode, omode)
        , Computer(computer)
        , Debug(debug)
      {
      }

      virtual BasicHttpBinding_USCOREISessionEndpointService *copy();
      virtual int CreateSession(ns3__CreateSessionRequest *ns3__CreateSessionRequest_, ns3__CreateSessionResponse *ns3__CreateSessionResponse_);
      virtual int ActivateSession(ns3__ActivateSessionRequest *ns3__ActivateSessionRequest_, ns3__ActivateSessionResponse *ns3__ActivateSessionResponse_);
      virtual int CloseSession(ns3__CloseSessionRequest *ns3__CloseSessionRequest_, ns3__CloseSessionResponse *ns3__CloseSessionResponse_);
      virtual int Cancel(ns3__CancelRequest *ns3__CancelRequest_, ns3__CancelResponse *ns3__CancelResponse_);
      virtual int AddNodes(ns3__AddNodesRequest *ns3__AddNodesRequest_, ns3__AddNodesResponse *ns3__AddNodesResponse_);
      virtual int AddReferences(ns3__AddReferencesRequest *ns3__AddReferencesRequest_, ns3__AddReferencesResponse *ns3__AddReferencesResponse_);
      virtual int DeleteNodes(ns3__DeleteNodesRequest *ns3__DeleteNodesRequest_, ns3__DeleteNodesResponse *ns3__DeleteNodesResponse_);
      virtual int DeleteReferences(ns3__DeleteReferencesRequest *ns3__DeleteReferencesRequest_, ns3__DeleteReferencesResponse *ns3__DeleteReferencesResponse_);
      virtual int Browse(ns3__BrowseRequest *ns3__BrowseRequest_, ns3__BrowseResponse *ns3__BrowseResponse_);
      virtual int BrowseNext(ns3__BrowseNextRequest *ns3__BrowseNextRequest_, ns3__BrowseNextResponse *ns3__BrowseNextResponse_);
      virtual int TranslateBrowsePathsToNodeIds(ns3__TranslateBrowsePathsToNodeIdsRequest *ns3__TranslateBrowsePathsToNodeIdsRequest_, ns3__TranslateBrowsePathsToNodeIdsResponse *ns3__TranslateBrowsePathsToNodeIdsResponse_);
      virtual int RegisterNodes(ns3__RegisterNodesRequest *ns3__RegisterNodesRequest_, ns3__RegisterNodesResponse *ns3__RegisterNodesResponse_);
      virtual int UnregisterNodes(ns3__UnregisterNodesRequest *ns3__UnregisterNodesRequest_, ns3__UnregisterNodesResponse *ns3__UnregisterNodesResponse_);
      virtual int QueryFirst(ns3__QueryFirstRequest *ns3__QueryFirstRequest_, ns3__QueryFirstResponse *ns3__QueryFirstResponse_);
      virtual int QueryNext(ns3__QueryNextRequest *ns3__QueryNextRequest_, ns3__QueryNextResponse *ns3__QueryNextResponse_);
      virtual int Read(ns3__ReadRequest *ns3__ReadRequest_, ns3__ReadResponse *ns3__ReadResponse_);
      virtual int HistoryRead(ns3__HistoryReadRequest *ns3__HistoryReadRequest_, ns3__HistoryReadResponse *ns3__HistoryReadResponse_);
      virtual int Write(ns3__WriteRequest *ns3__WriteRequest_, ns3__WriteResponse *ns3__WriteResponse_);
      virtual int HistoryUpdate(ns3__HistoryUpdateRequest *ns3__HistoryUpdateRequest_, ns3__HistoryUpdateResponse *ns3__HistoryUpdateResponse_);
      virtual int Call(ns3__CallRequest *ns3__CallRequest_, ns3__CallResponse *ns3__CallResponse_);
      virtual int CreateMonitoredItems(ns3__CreateMonitoredItemsRequest *ns3__CreateMonitoredItemsRequest_, ns3__CreateMonitoredItemsResponse *ns3__CreateMonitoredItemsResponse_);
      virtual int ModifyMonitoredItems(ns3__ModifyMonitoredItemsRequest *ns3__ModifyMonitoredItemsRequest_, ns3__ModifyMonitoredItemsResponse *ns3__ModifyMonitoredItemsResponse_);
      virtual int SetMonitoringMode(ns3__SetMonitoringModeRequest *ns3__SetMonitoringModeRequest_, ns3__SetMonitoringModeResponse *ns3__SetMonitoringModeResponse_);
      virtual int SetTriggering(ns3__SetTriggeringRequest *ns3__SetTriggeringRequest_, ns3__SetTriggeringResponse *ns3__SetTriggeringResponse_);
      virtual int DeleteMonitoredItems(ns3__DeleteMonitoredItemsRequest *ns3__DeleteMonitoredItemsRequest_, ns3__DeleteMonitoredItemsResponse *ns3__DeleteMonitoredItemsResponse_);
      virtual int CreateSubscription(ns3__CreateSubscriptionRequest *ns3__CreateSubscriptionRequest_, ns3__CreateSubscriptionResponse *ns3__CreateSubscriptionResponse_);
      virtual int ModifySubscription(ns3__ModifySubscriptionRequest *ns3__ModifySubscriptionRequest_, ns3__ModifySubscriptionResponse *ns3__ModifySubscriptionResponse_);
      virtual int SetPublishingMode(ns3__SetPublishingModeRequest *ns3__SetPublishingModeRequest_, ns3__SetPublishingModeResponse *ns3__SetPublishingModeResponse_);
      virtual int Publish(ns3__PublishRequest *ns3__PublishRequest_, ns3__PublishResponse *ns3__PublishResponse_);
      virtual int Republish(ns3__RepublishRequest *ns3__RepublishRequest_, ns3__RepublishResponse *ns3__RepublishResponse_);
      virtual int TransferSubscriptions(ns3__TransferSubscriptionsRequest *ns3__TransferSubscriptionsRequest_, ns3__TransferSubscriptionsResponse *ns3__TransferSubscriptionsResponse_);
      virtual int DeleteSubscriptions(ns3__DeleteSubscriptionsRequest *ns3__DeleteSubscriptionsRequest_, ns3__DeleteSubscriptionsResponse *ns3__DeleteSubscriptionsResponse_);

    private:
      OpcUa::Remote::Computer::SharedPtr Computer;
      bool Debug;
    };
  }
}



/// @author Olivier Roulet-Dubonnet 
/// @email olivier@sintef.no 
/// @brief Opc Ua Binary. 
/// @license GNU LGPL
///
/// Distributed under the GNU LGPL License
/// (See accompanying file LICENSE or copy at
/// http://www.gnu.org/licenses/lgpl.html)
///

#include <opc/ua/protocol/protocol_auto.h>
#include <opc/ua/protocol/object_ids.h>

namespace OpcUa
{   

     RequestHeader::RequestHeader()
        : TypeId(ObjectId::RequestHeader_Encoding_DefaultBinary)
    {
    }

     ResponseHeader::ResponseHeader()
        : TypeId(ObjectId::ResponseHeader_Encoding_DefaultBinary)
    {
    }

     ServiceFault::ServiceFault()
        : TypeId(ObjectId::ServiceFault_Encoding_DefaultBinary)
    {
    }

     FindServersRequest::FindServersRequest()
        : TypeId(ObjectId::FindServersRequest_Encoding_DefaultBinary)
    {
    }

     FindServersResponse::FindServersResponse()
        : TypeId(ObjectId::FindServersResponse_Encoding_DefaultBinary)
    {
    }

     GetEndpointsRequest::GetEndpointsRequest()
        : TypeId(ObjectId::GetEndpointsRequest_Encoding_DefaultBinary)
    {
    }

     GetEndpointsResponse::GetEndpointsResponse()
        : TypeId(ObjectId::GetEndpointsResponse_Encoding_DefaultBinary)
    {
    }

     RegisterServerRequest::RegisterServerRequest()
        : TypeId(ObjectId::RegisterServerRequest_Encoding_DefaultBinary)
    {
    }

     RegisterServerResponse::RegisterServerResponse()
        : TypeId(ObjectId::RegisterServerResponse_Encoding_DefaultBinary)
    {
    }

     OpenSecureChannelParameters::OpenSecureChannelParameters()
        : TypeId(ObjectId::OpenSecureChannelParameters_Encoding_DefaultBinary)
    {
    }

     OpenSecureChannelRequest::OpenSecureChannelRequest()
        : TypeId(ObjectId::OpenSecureChannelRequest_Encoding_DefaultBinary)
    {
    }

     OpenSecureChannelResponse::OpenSecureChannelResponse()
        : TypeId(ObjectId::OpenSecureChannelResponse_Encoding_DefaultBinary)
    {
    }

     CloseSecureChannelRequest::CloseSecureChannelRequest()
        : TypeId(ObjectId::CloseSecureChannelRequest_Encoding_DefaultBinary)
    {
    }

     CloseSecureChannelResponse::CloseSecureChannelResponse()
        : TypeId(ObjectId::CloseSecureChannelResponse_Encoding_DefaultBinary)
    {
    }

     CreateSessionRequest::CreateSessionRequest()
        : TypeId(ObjectId::CreateSessionRequest_Encoding_DefaultBinary)
    {
    }

     CreateSessionResponse::CreateSessionResponse()
        : TypeId(ObjectId::CreateSessionResponse_Encoding_DefaultBinary)
    {
    }

     UserIdentityToken::UserIdentityToken()
        : TypeId(ObjectId::UserIdentityToken_Encoding_DefaultBinary)
    {
    }

     ActivateSessionRequest::ActivateSessionRequest()
        : TypeId(ObjectId::ActivateSessionRequest_Encoding_DefaultBinary)
    {
    }

     ActivateSessionResponse::ActivateSessionResponse()
        : TypeId(ObjectId::ActivateSessionResponse_Encoding_DefaultBinary)
    {
    }

     CloseSessionRequest::CloseSessionRequest()
        : TypeId(ObjectId::CloseSessionRequest_Encoding_DefaultBinary)
    {
    }

     CloseSessionResponse::CloseSessionResponse()
        : TypeId(ObjectId::CloseSessionResponse_Encoding_DefaultBinary)
    {
    }

     CancelRequest::CancelRequest()
        : TypeId(ObjectId::CancelRequest_Encoding_DefaultBinary)
    {
    }

     CancelResponse::CancelResponse()
        : TypeId(ObjectId::CancelResponse_Encoding_DefaultBinary)
    {
    }

     AddNodesRequest::AddNodesRequest()
        : TypeId(ObjectId::AddNodesRequest_Encoding_DefaultBinary)
    {
    }

     AddNodesResponse::AddNodesResponse()
        : TypeId(ObjectId::AddNodesResponse_Encoding_DefaultBinary)
    {
    }

     AddReferencesRequest::AddReferencesRequest()
        : TypeId(ObjectId::AddReferencesRequest_Encoding_DefaultBinary)
    {
    }

     AddReferencesResponse::AddReferencesResponse()
        : TypeId(ObjectId::AddReferencesResponse_Encoding_DefaultBinary)
    {
    }

     DeleteNodesRequest::DeleteNodesRequest()
        : TypeId(ObjectId::DeleteNodesRequest_Encoding_DefaultBinary)
    {
    }

     DeleteNodesResponse::DeleteNodesResponse()
        : TypeId(ObjectId::DeleteNodesResponse_Encoding_DefaultBinary)
    {
    }

     DeleteReferencesRequest::DeleteReferencesRequest()
        : TypeId(ObjectId::DeleteReferencesRequest_Encoding_DefaultBinary)
    {
    }

     DeleteReferencesResponse::DeleteReferencesResponse()
        : TypeId(ObjectId::DeleteReferencesResponse_Encoding_DefaultBinary)
    {
    }

     BrowseDescription::BrowseDescription()
        : TypeId(ObjectId::BrowseDescription_Encoding_DefaultBinary)
    {
    }

     ReferenceDescription::ReferenceDescription()
        : TypeId(ObjectId::ReferenceDescription_Encoding_DefaultBinary)
    {
    }

     BrowseRequest::BrowseRequest()
        : TypeId(ObjectId::BrowseRequest_Encoding_DefaultBinary)
    {
    }

     BrowseResponse::BrowseResponse()
        : TypeId(ObjectId::BrowseResponse_Encoding_DefaultBinary)
    {
    }

     BrowseNextRequest::BrowseNextRequest()
        : TypeId(ObjectId::BrowseNextRequest_Encoding_DefaultBinary)
    {
    }

     BrowseNextResponse::BrowseNextResponse()
        : TypeId(ObjectId::BrowseNextResponse_Encoding_DefaultBinary)
    {
    }

     RelativePathElement::RelativePathElement()
        : TypeId(ObjectId::RelativePathElement_Encoding_DefaultBinary)
    {
    }

     TranslateBrowsePathsToNodeIdsRequest::TranslateBrowsePathsToNodeIdsRequest()
        : TypeId(ObjectId::TranslateBrowsePathsToNodeIdsRequest_Encoding_DefaultBinary)
    {
    }

     TranslateBrowsePathsToNodeIdsResponse::TranslateBrowsePathsToNodeIdsResponse()
        : TypeId(ObjectId::TranslateBrowsePathsToNodeIdsResponse_Encoding_DefaultBinary)
    {
    }

     RegisterNodesRequest::RegisterNodesRequest()
        : TypeId(ObjectId::RegisterNodesRequest_Encoding_DefaultBinary)
    {
    }

     RegisterNodesResponse::RegisterNodesResponse()
        : TypeId(ObjectId::RegisterNodesResponse_Encoding_DefaultBinary)
    {
    }

     UnregisterNodesRequest::UnregisterNodesRequest()
        : TypeId(ObjectId::UnregisterNodesRequest_Encoding_DefaultBinary)
    {
    }

     UnregisterNodesResponse::UnregisterNodesResponse()
        : TypeId(ObjectId::UnregisterNodesResponse_Encoding_DefaultBinary)
    {
    }

     QueryFirstRequest::QueryFirstRequest()
        : TypeId(ObjectId::QueryFirstRequest_Encoding_DefaultBinary)
    {
    }

     QueryFirstResponse::QueryFirstResponse()
        : TypeId(ObjectId::QueryFirstResponse_Encoding_DefaultBinary)
    {
    }

     QueryNextRequest::QueryNextRequest()
        : TypeId(ObjectId::QueryNextRequest_Encoding_DefaultBinary)
    {
    }

     QueryNextResponse::QueryNextResponse()
        : TypeId(ObjectId::QueryNextResponse_Encoding_DefaultBinary)
    {
    }

     ReadValueId::ReadValueId()
        : TypeId(ObjectId::ReadValueId_Encoding_DefaultBinary)
    {
    }

     ReadParameters::ReadParameters()
        : TypeId(ObjectId::ReadParameters_Encoding_DefaultBinary)
    {
    }

     ReadRequest::ReadRequest()
        : TypeId(ObjectId::ReadRequest_Encoding_DefaultBinary)
    {
    }

     ReadResponse::ReadResponse()
        : TypeId(ObjectId::ReadResponse_Encoding_DefaultBinary)
    {
    }

     HistoryReadRequest::HistoryReadRequest()
        : TypeId(ObjectId::HistoryReadRequest_Encoding_DefaultBinary)
    {
    }

     HistoryReadResponse::HistoryReadResponse()
        : TypeId(ObjectId::HistoryReadResponse_Encoding_DefaultBinary)
    {
    }

     WriteRequest::WriteRequest()
        : TypeId(ObjectId::WriteRequest_Encoding_DefaultBinary)
    {
    }

     WriteResponse::WriteResponse()
        : TypeId(ObjectId::WriteResponse_Encoding_DefaultBinary)
    {
    }

     HistoryUpdateRequest::HistoryUpdateRequest()
        : TypeId(ObjectId::HistoryUpdateRequest_Encoding_DefaultBinary)
    {
    }

     HistoryUpdateResponse::HistoryUpdateResponse()
        : TypeId(ObjectId::HistoryUpdateResponse_Encoding_DefaultBinary)
    {
    }

     CallRequest::CallRequest()
        : TypeId(ObjectId::CallRequest_Encoding_DefaultBinary)
    {
    }

     CallResponse::CallResponse()
        : TypeId(ObjectId::CallResponse_Encoding_DefaultBinary)
    {
    }

     CreateMonitoredItemsRequest::CreateMonitoredItemsRequest()
        : TypeId(ObjectId::CreateMonitoredItemsRequest_Encoding_DefaultBinary)
    {
    }

     CreateMonitoredItemsResponse::CreateMonitoredItemsResponse()
        : TypeId(ObjectId::CreateMonitoredItemsResponse_Encoding_DefaultBinary)
    {
    }

     ModifyMonitoredItemsRequest::ModifyMonitoredItemsRequest()
        : TypeId(ObjectId::ModifyMonitoredItemsRequest_Encoding_DefaultBinary)
    {
    }

     ModifyMonitoredItemsResponse::ModifyMonitoredItemsResponse()
        : TypeId(ObjectId::ModifyMonitoredItemsResponse_Encoding_DefaultBinary)
    {
    }

     SetMonitoringModeRequest::SetMonitoringModeRequest()
        : TypeId(ObjectId::SetMonitoringModeRequest_Encoding_DefaultBinary)
    {
    }

     SetMonitoringModeResponse::SetMonitoringModeResponse()
        : TypeId(ObjectId::SetMonitoringModeResponse_Encoding_DefaultBinary)
    {
    }

     SetTriggeringRequest::SetTriggeringRequest()
        : TypeId(ObjectId::SetTriggeringRequest_Encoding_DefaultBinary)
    {
    }

     SetTriggeringResponse::SetTriggeringResponse()
        : TypeId(ObjectId::SetTriggeringResponse_Encoding_DefaultBinary)
    {
    }

     DeleteMonitoredItemsRequest::DeleteMonitoredItemsRequest()
        : TypeId(ObjectId::DeleteMonitoredItemsRequest_Encoding_DefaultBinary)
    {
    }

     DeleteMonitoredItemsResponse::DeleteMonitoredItemsResponse()
        : TypeId(ObjectId::DeleteMonitoredItemsResponse_Encoding_DefaultBinary)
    {
    }

     CreateSubscriptionParameters::CreateSubscriptionParameters()
        : TypeId(ObjectId::CreateSubscriptionParameters_Encoding_DefaultBinary)
    {
    }

     CreateSubscriptionRequest::CreateSubscriptionRequest()
        : TypeId(ObjectId::CreateSubscriptionRequest_Encoding_DefaultBinary)
    {
    }

     CreateSubscriptionResponse::CreateSubscriptionResponse()
        : TypeId(ObjectId::CreateSubscriptionResponse_Encoding_DefaultBinary)
    {
    }

     ModifySubscriptionRequest::ModifySubscriptionRequest()
        : TypeId(ObjectId::ModifySubscriptionRequest_Encoding_DefaultBinary)
    {
    }

     ModifySubscriptionResponse::ModifySubscriptionResponse()
        : TypeId(ObjectId::ModifySubscriptionResponse_Encoding_DefaultBinary)
    {
    }

     SetPublishingModeParameters::SetPublishingModeParameters()
        : TypeId(ObjectId::SetPublishingModeParameters_Encoding_DefaultBinary)
    {
    }

     SetPublishingModeRequest::SetPublishingModeRequest()
        : TypeId(ObjectId::SetPublishingModeRequest_Encoding_DefaultBinary)
    {
    }

     SetPublishingModeResponse::SetPublishingModeResponse()
        : TypeId(ObjectId::SetPublishingModeResponse_Encoding_DefaultBinary)
    {
    }

     NotificationMessage::NotificationMessage()
        : TypeId(ObjectId::NotificationMessage_Encoding_DefaultBinary)
    {
    }

     PublishRequest::PublishRequest()
        : TypeId(ObjectId::PublishRequest_Encoding_DefaultBinary)
    {
    }

     PublishResult::PublishResult()
        : TypeId(ObjectId::PublishResult_Encoding_DefaultBinary)
    {
    }

     PublishResponse::PublishResponse()
        : TypeId(ObjectId::PublishResponse_Encoding_DefaultBinary)
    {
    }

     RepublishRequest::RepublishRequest()
        : TypeId(ObjectId::RepublishRequest_Encoding_DefaultBinary)
    {
    }

     RepublishResponse::RepublishResponse()
        : TypeId(ObjectId::RepublishResponse_Encoding_DefaultBinary)
    {
    }

     TransferSubscriptionsRequest::TransferSubscriptionsRequest()
        : TypeId(ObjectId::TransferSubscriptionsRequest_Encoding_DefaultBinary)
    {
    }

     TransferSubscriptionsResponse::TransferSubscriptionsResponse()
        : TypeId(ObjectId::TransferSubscriptionsResponse_Encoding_DefaultBinary)
    {
    }

     DeleteSubscriptionsRequest::DeleteSubscriptionsRequest()
        : TypeId(ObjectId::DeleteSubscriptionsRequest_Encoding_DefaultBinary)
    {
    }

     DeleteSubscriptionsResponse::DeleteSubscriptionsResponse()
        : TypeId(ObjectId::DeleteSubscriptionsResponse_Encoding_DefaultBinary)
    {
    }

     TestStackRequest::TestStackRequest()
        : TypeId(ObjectId::TestStackRequest_Encoding_DefaultBinary)
    {
    }

     TestStackResponse::TestStackResponse()
        : TypeId(ObjectId::TestStackResponse_Encoding_DefaultBinary)
    {
    }

     TestStackExRequest::TestStackExRequest()
        : TypeId(ObjectId::TestStackExRequest_Encoding_DefaultBinary)
    {
    }

     TestStackExResponse::TestStackExResponse()
        : TypeId(ObjectId::TestStackExResponse_Encoding_DefaultBinary)
    {
    }

} // namespace
    

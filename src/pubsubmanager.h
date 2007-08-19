/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/

#ifndef PUBSUBMANAGER_H__
#define PUBSUBMANAGER_H__

#include "pubsub.h"
#include "iqhandler.h"
#include "discohandler.h"
#include "messagehandler.h"
#include "pubsubnodehandler.h"

#include <map>
#include <string>

namespace gloox
{

  class ClientBase;
  class DataForm;

  namespace PubSub
  {

    class ServiceHandler;
    class NodeHandler;
    class ItemHandler;
    class DiscoHandler;
    class EventHandler;

    /**
     * @brief This manager is used to interact with PubSub services (XEP-0060).
     *
     * This manager acts in concert with 5 different handlers:
     *
     * - EventHandler is responsible for receiving the PubSub event notifications.
     *   Register as much as you need with the Manager.
     *
     * - DiscoHandler, ServiceHandler, NodeHandler and ItemHandler interfaces are
     *   used to receive notifications of a request's result, depending on the
     *   context.
     *
     * \note Many PubSub queries will both trigger a notification from registered
     *       EventHandler's and from the handler specific to the query.
     *
     * To get started with PubSub in gloox, create a Manager, implement the
     * PubSub::EventHandler virtuals and register an instance with a Manager.
     * This will get you notified of PubSub events sent to you.
     *
     * Next, to be able to interact with PubSub services, you will need to
     * implement the DiscoHandler, NodeHandler, ItemHandler and ServiceHandler
     * interfaces to be notified of the result of disco, node, item and service
     * related requests and pass these along to these requests (null handlers
     * are not allowed).
     *
     * Here's an example.
     *
     * EventHandler::handleItemPublication() can be called with or without the actual
     * tag, depending on if the notification actually includes the payload. From there
     * you could only record the event and be done with it, or decide to retrieve the
     * full payload. Eg:
     *
     * @code
     *
     * class MyEventHandler : public gloox::EventHandler
     * {
     *   // ...
     * };
     *
     * void MyEventHandler::handleItemPublication( const JID& service,
     *                                             const std::string& node,
     *                                             const std::string& item,
     *                                             const Tag* entry )
     * {
     *   // we want to retrieve the payload everytime
     *   if( !entry )
     *   {
     *     m_manager->requestItem( service, node, item, myItemHandler );
     *   }
     *   else
     *   {
     *     do_something_useful( entry );
     *   }
     * }
     *
     * @endcode
     *
     * In response to this request, MyItemHandler::handleItem() will be called
     * 
     * \note PubSub support in gloox is still relatively young and you are most
     *       welcome to ask questions, critique the API and so on. For contact
     *       informations, see below.
     *
     *
     * @author Vincent Thomasset <vthomasset@gmail.com>
     * @todo Subscription request management is currently missing.
     *
     * XEP Version: 1.9
     */
    class GLOOX_API Manager : public IqHandler,
                              public gloox::DiscoHandler,
                              public MessageHandler
    {
      public:

        /**
         * Initialize the manager.
         * @param parent Client to which this manager belongs to.
         */
        Manager( ClientBase* parent );

        /**
         * Default virtual destructor.
         */
        virtual ~Manager() {}

private:
        /**
         * Performs a Disco query to a service or node.
         * @param service Service to query.
         * @param node ID of the node to query. If empty, the service will be queried.
         * @param handler DiscoHandler to notify when receiving a response.
         */
        void discoverInfos( const JID& service, const std::string& node,
                                                PubSub::DiscoHandler* handler );

public:
        /**
         * Performs a Disco query to a service.
         * @param service Service to query.
         * @param handler DiscoHandler to notify when receiving a response
         */
        void discoverServiceInfos( const JID& service, PubSub::DiscoHandler* handler )
          { discoverInfos( service, "", handler ); }

        /**
         * Performs a Disco query to a node.
         * @param service Service hosting the node to query.
         * @param node ID of the node to query. If empty, the root node will be queried.
         * @param handler DiscoHandler to notify when receiving a response
         */
        void discoverNodeInfos( const JID& service, const std::string& node,
                                PubSub::DiscoHandler* handler )
          { discoverInfos( service, node, handler ); }

        /**
         * Ask for the list children of a node.
         * @param service Service hosting the node.
         * @param node ID of the node to ask for subnodes. If empty, the root node
         * will be queried.
         * @param handler DiscoHandler to notify when receiving a response
         */
        void discoverNodeItems( const JID& service, const std::string& node,
                                PubSub::DiscoHandler* handler );

        /**
         * Subscribe to a node.
         * @param service Service hosting the node.
         * @param nodeid ID of the node to subscribe to.
         * @param jid JID to subscribe. If empty, the client's JID will be used
         *            (ie self subscription).
         * @param type SibscriptionType of the subscription (Collections only!).
         * @param depth Subscription depth. For 'all', use 0 (Collections only!).
         */
        void subscribe( const JID& service, const std::string& nodeid,
                        NodeHandler* handler, const JID& jid = JID(),
                        SubscriptionObject type = SubscriptionNodes,
                        int depth = 1 );

        /**
         * Unsubscribe from a node.
         * @param service Service hosting the node.
         * @param node ID of the node to unsubscribe from.
         * @param handler NodeHandler receiving the result notification.
         * @param jid JID to unsubscribe. If empty, the client's JID will be
         *            used (ie self unsubscription).
         */
        void unsubscribe( const JID& service,
                          const std::string& node,
                          NodeHandler* handler,
                          const JID& jid = JID() );

        /**
         * Requests the subscription list from a service.
         * @param service Service to query.
         * @param handler The ServiceHandler to handle the result.
         */
        void requestSubscriptionList( const JID& service, ServiceHandler* handler );

        /**
         * Requests the affiliation list from a service.
         * @param service Service to query.
         * @param handler The ServiceHandler to handle the result.
         */
        void requestAffiliationList( const JID& service, ServiceHandler* handler );

private:
        void subscriptionOptions( const JID& service,
                                  const JID& jid,
                                  const std::string& node,
                                  NodeHandler* handler,
                                  const DataForm* df );

public:
        /**
         * Requests subscription options.
         * @param service Service to query.
         * @param jid Subscribed entity.
         * @param node Node ID of the node.
         * @param handler Node ID of the node.
         * @param slh The SubscriptionListHandler to handle the result.
         */
        void requestSubscriptionOptions( const JID& service,
                                         const JID& jid,
                                         const std::string& node,
                                         NodeHandler* handler)
        { subscriptionOptions( service, jid, node, handler, 0 ); }

        /**
         * Modifies subscription options.
         * @param service Service to query.
         * @param jid Subscribed entity.
         * @param node Node ID of the node.
         * @param df New configuration.
         */
        void setSubscriptionOptions( const JID& service,
                                     const JID& jid,
                                     const std::string& node,
                                     const DataForm& df,
                                     NodeHandler* handler )
        { subscriptionOptions( service, jid, node, handler, &df ); }

        /**
         * Requests the affiliation list for a node.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler The AffiliationListHandler to handle the result.
         */
        void requestAffiliationList( const JID& service,
                                     const std::string& node,
                                     ServiceHandler* handler );

        /**
         * Publish an item to a node. The Tag to publish is destroyed
         * by the function before returning.
         * @param service Service hosting the node.
         * @param node ID of the node to delete the item from.
         * @param item The item to publish.
         */
        void publishItem( const JID& service,
                          const std::string& node,
                          Tag* item,
                          ItemHandler* handler );

        /**
         * Delete an item from a node.
         * @param service Service hosting the node.
         * @param node ID of the node to delete the item from.
         * @param item ID of the item in the node.
         */
        void deleteItem( const JID& service,
                         const std::string& node,
                         const std::string& item,
                         ItemHandler* handler );

        /**
         * Ask for the item list of a specific node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param handler ItemHandler to send the result to.
         */
        void requestItems( const JID& service,
                           const std::string& nodeid,
                           ItemHandler* handler );

        /**
         * Creates a new node.
         * @param type The type of the new node.
         * @param service Service where to create the new node.
         * @param node The ID of the new node.
         * @param name The name of the new node.
         * @param parent ID of the parent node. If empty, the node will be located at the root of
         * the service.
         * @param access The node's access model.
         * @param config A map of further node configuration options. The keys of the map should be in
         * the form of 'pubsub#name', where 'name' is a valid pubsub option. Do not use this map to
         * include an access model config option, use the @c access parameter instead.
         */
        void createNode( NodeType type, const JID& service,
                                        const std::string& node,
                                        NodeHandler* handler,
                                        const std::string& name = "",
                                        const std::string& parent = "",
                                        AccessModel access = AccessDefault,
                                        const StringMap* config = 0 );

        /**
         * Creates a new leaf node.
         * @param service Service where to create the new node.
         * @param node The ID of the new node.
         * @param name The name of the new node.
         * @param parent ID of the parent node. If empty, the node will be located at the root of
         * the service.
         * @param access The node's access model.
         * @param config A map of further node configuration options. The keys of the map should be in
         * the form of 'pubsub#name', where 'name' is a valid pubsub option. Do not use this map to
         * include an access model config option, use the @c access parameter instead.
         */
        void createLeafNode( const JID& service,
                             const std::string& node,
                             NodeHandler* handler,
                             const std::string& name,
                             const std::string& parent = "",
                             AccessModel access = AccessDefault,
                             const StringMap* config = 0 )
          { createNode( NodeLeaf, service, node, handler, name, parent, access, config ); }

        /**
         * Creates a new collection node.
         * @param service Service where to create the new node.
         * @param node The ID of the new node.
         * @param name The name of the new node.
         * @param parent ID of the parent node. If empty, the node will be located at the root of
         * the service.
         * @param access The node's access model.
         * @param config A map of further node configuration options. The keys of the map should be in
         * the form of 'pubsub#name', where 'name' is a valid pubsub option. It is not necessary to
         * include an access model config option. Use the @c access parameter instead.
         */
        void createCollectionNode( const JID& service,
                                   const std::string& node,
                                   NodeHandler* handler,
                                   const std::string& name,
                                   const std::string& parent = "",
                                   AccessModel access = AccessDefault,
                                   const StringMap* config = 0 )
          { createNode( NodeCollection, service, node, handler, name, parent, access, config ); }

        /**
         * Deletes a node.
         * @param service Service where to create the new node.
         * @param node Node ID of the new node.
         */
        void deleteNode( const JID& service,
                         const std::string& node,
                         NodeHandler* handler );

/*
        void associateNode( const JID& service,
                            const std::string& node,
                            const std::string& collection );

        void disassociateNode( const JID& service,
                               const std::string& node,
                               const std::string& collection );
*/
        /**
         * Retrieves the default configuration for a specific NodeType.
         * @param service The queried service.
         * @param type NodeType to get default configuration for.
         * @param handler ServiceHandler.
         */
        void getDefaultNodeConfig( const JID& service, NodeType type, ServiceHandler* handler );

        /**
         * Requests the subscriber list for a node.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler NodeHandler.
         */
        void purgeNodeItems( const JID& service, const std::string& node, NodeHandler* handler );

        /**
         * Requests the subscriber list for a node.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler NodeHandler.
         */
        void requestSubscriberList( const JID& service, const std::string& node, NodeHandler* handler )
          { subscriberList( service, node, 0, handler ); }

        /**
         * Modifies the subscriber list for a node. This function SHOULD only set the
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param list NodeHandler.
         */
        void setSubscriberList( const JID& service,
                                const std::string& node,
                                const SubscriberList& list,
                                NodeHandler* handler )
          { subscriberList( service, node, &list, handler ); }

        /**
         * Requests the affiliate list for a node.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler NodeHandler.
         */
        void requestAffiliateList( const JID& service,
                                   const std::string& node,
                                   NodeHandler* handler )
          { affiliateList( service, node, 0, handler ); }

        /**
         * Modifies the affiliate list for a node.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param list NodeHandler.
         */
        void setAffiliateList( const JID& service,
                               const std::string& node,
                               const AffiliateList& list,
                               NodeHandler* handler )
          { affiliateList( service, node, &list, handler ); }

        /**
         * Retrieve the configuration (options) of a node.
         * @param service Service hosting the node.
         * @param node ID of the node.
         * @param handler NodeHandler responsible to handle the request result.
         */
        void requestNodeConfig( const JID& service,
                                const std::string& node,
                                NodeHandler* handler )
          { nodeConfig( service, node, 0, handler ); }

        /**
         * Changes a node's configuration (options).
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param handler NodeHandler responsible to handle the request result.
         */
        void setNodeConfig( const JID& service,
                            const std::string& node,
                            const DataForm& config,
                            NodeHandler* handler  )
          { nodeConfig( service, node, &config, handler ); }

        /**
         * Registers an handler to receive notification of events.
         * @param handler EventHandler to register.
         */
        void registerEventHandler( EventHandler* handler )
          { m_eventHandlerList.push_back( handler ); }

        /**
         * Removes an handler from the list of event handlers.
         * @param handler EventHandler to remove.
         */
        void removeEventHandler( EventHandler* handler )
          { m_eventHandlerList.remove( handler ); }

        // reimplemented from DiscoHandler
        void handleDiscoInfoResult( IQ* iq, int context );
        void handleDiscoItemsResult( IQ* iq, int context );
        void handleDiscoError( IQ* iq, int context );
        bool handleDiscoSet( IQ* ) { return 0; }

        // reimplemented from MessageHandler
        void handleMessage( Message* msg, MessageSession * );

        // reimplemented from IqHandler
        bool handleIq  ( IQ* ) { return 0; }
        void handleIqID( IQ* iq, int context );

      private:

        /**
         * This function sets or requests a node's configuration form
         * (depending on arguments). Does the actual work for requestNodeConfig
         * and setNodeConfig.
         * Requests or changes a node's configuration.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param config If not NULL, the function will request the node config.
         *               Otherwise, it will set the config based on the form.
         * @param handler NodeHandler responsible to handle the request result.
         */
        void nodeConfig( const JID& service, const std::string& node,
                         const DataForm* config, NodeHandler* handler );

        /**
         * This function sets or requests a node's subscribers list form
         * (depending on arguments). Does the actual work for
         * requestSubscriberList and setSubscriberList.
         * Requests or changes a node's configuration.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param config If not NULL, the function will request the node config.
         *               Otherwise, it will set the config based on the form.
         * @param handler NodeHandler responsible to handle the request result.
         */
        void subscriberList( const JID& service, const std::string& node,
                             const SubscriberList* config, NodeHandler* handler );

        /**
         * This function sets or requests a node's affiliates list
         * (depending on arguments). Does the actual work for
         * requestAffiliateList and setAffiliateList.
         * Requests or changes a node's configuration.
         * @param service Service to query.
         * @param node Node ID of the node.
         * @param config If not NULL, the function will request the node config.
         *               Otherwise, it will set the config based on the form.
         * @param handler NodeHandler responsible to handle the request result.
         */
        void affiliateList( const JID& service, const std::string& node,
                            const AffiliateList* config, NodeHandler* handler );

        typedef std::pair< std::string, std::string > TrackedItem;
        typedef std::map < std::string, TrackedItem > ItemOperationTrackMap;
        typedef std::map < std::string, std::string > NodeOperationTrackMap;

        typedef std::map < std::string, PubSub::DiscoHandler* > DiscoHandlerTrackMap;
        typedef std::map < std::string, ServiceHandler* > ServiceHandlerTrackMap;
        typedef std::map < std::string, NodeHandler* > NodeHandlerTrackMap;
        typedef std::map < std::string, ItemHandler* > ItemHandlerTrackMap;
        typedef std::list< EventHandler* > EventHandlerList;

        ClientBase* m_parent;

        ItemOperationTrackMap  m_iopTrackMap;
        NodeOperationTrackMap  m_nopTrackMap;

        ServiceHandlerTrackMap m_serviceHandlerTrackMap;
        ItemHandlerTrackMap    m_itemHandlerTrackMap;
        NodeHandlerTrackMap    m_nodeHandlerTrackMap;
        DiscoHandlerTrackMap   m_discoHandlerTrackMap;
        EventHandlerList       m_eventHandlerList;
    };

  }

}

#endif /* PUBSUBMANAGER_H__ */

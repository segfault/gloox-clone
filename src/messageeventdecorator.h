/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#ifndef MESSAGEVENTDECORATOR
#define MESSAGEVENTDECORATOR

#include "sessiondecorator.h"
#include "messagesession.h"
#include "messagehandler.h"

namespace gloox
{

  class Tag;
  class MessageEventHandler;

  /**
   * @brief This class adds Message Event (JEP-0022) support to a MessageSession.
   *
   * Usage:
   * @code
   * MessageSession *msgSess = new MessageSession( clntBase );
   * msgSess = new MessageEventDecorator( msgSess );
   * @endcode
   * And use it like you would use a MessageSession.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class GLOOX_EXPORT MessageEventDecorator : public SessionDecorator, MessageHandler
  {
    public:
      /**
       * The possible Message Events according to JEP-0022.
       */
      enum MessageEventType
      {
        MESSAGE_EVENT_OFFLINE   = 1, /**< Indicates that the message has been stored offline by the
                                      * intended recipient's server. */
        MESSAGE_EVENT_DELIVERED = 2, /**< Indicates that the message has been delivered to the
                                      * recipient. */
        MESSAGE_EVENT_DISPLAYED = 4, /**< Indicates that the message has been displayed */
        MESSAGE_EVENT_COMPOSING = 8  /**< Indicates that the recipient is composing a reply to a
                                      * message. */
      };

      /**
       * Contstructs a new Message Event decorator for a MessageSession.
       * You should use the newly created decorator and forget about the old
       * MessageSession.
       * @param ms The MessageSession to decorate.
       */
      MessageEventDecorator( MessageSession *ms );

      /**
       * Virtual destructor.
       */
      virtual ~MessageEventDecorator();

      /**
       * Use this function to raise an event as defined in JEP-0022.
       * @note The Spec states that Message Events shall not be sent to an entity
       * which did not request them. Reasonable effort istaken in this function to
       * avoid spurious event sending. You should be safe to call this even if Message
       * Events were not requested by the remote entity.
       * @param event The event to raise.
       */
      void raiseEvent( MessageEventType event );

      /**
       * The MessageEventHandler registered here will receive Message Events according
       * to JEP-0022.
       * @param meh The MessageEventHandler to register.
       */
      void registerMessageEventHandler( MessageEventHandler *meh );

      /**
       * This function clears the internal pointer to the MessageEventHandler.
       * Message Events will not be delivered anymore after calling this function until another
       * MessageEventHandler is registered.
       */
      void removeMessageEventHandler();

      /**
       * @copydoc MessageSession::registerMessageHandler().
       * @param mh The MessageHandler to register.
       */
      void registerMessageHandler( MessageHandler *mh );

      /**
       * @copydoc MessageSession::removeMessageHandler().
       */
      void removeMessageHandler();

      // reimplemented from Session
      virtual void send( Tag *tag );

      // reimplemented from MessageHandler
      virtual void handleMessage( Stanza *stanza );

    private:
      MessageSession *m_parent;
      MessageEventHandler *m_messageEventHandler;
      std::string m_lastID;
      int m_events;

  };
}

#endif // MESSAGEVENTDECORATOR

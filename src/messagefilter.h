/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef MESSAGEFILTER_H__
#define MESSAGEFILTER_H__

namespace gloox
{

  class MessageSession;
  class Stanza;
  class Tag;

  /**
   * @brief Virtual base class for message filters.
   *
   * A message filter is fed with all messages passing through a MessageSession. It can
   * modify the XML/XMPP structure and/or the message content at will. Messages arriving
   * from the server as well as messages sent to the server can be altered.
   *
   * Messages to be sent out are presented to the filter via the decorate() function, incoming
   * messages can be filtered in the -- filter() method.
   *
   * @author Jakob Schroeter <js@camaya.net>
   * @since 0.8
   */
  class MessageFilter
  {

    public:
      /**
       * Constructor.
       */
      MessageFilter( MessageSession *parent ) : m_parent( parent ) {};

      /**
       * Virtual Destructor.
       */
      virtual ~MessageFilter() {};

      /**
       * This function receives a message right before it is sent out (there may be other filters
       * which get to see the message after this filter, though).
       * @param tag The tag to decorate. It contains the message to be sent.
       */
      virtual void decorate( Tag *tag ) = 0;

      /**
       * This function receives a message stanza right after it was received (there may be other filters
       * which got to see the stanza before this filter, though).
       * @param stanza The complete message stanza.
       */
      virtual void filter( Stanza *stanza ) = 0;

    protected:
      MessageSession *m_parent;

  };

}

#endif // MESSAGEFILTER_H__
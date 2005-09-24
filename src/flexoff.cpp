/*
  Copyright (c) 2004-2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warrenty.
*/


#include "flexoff.h"
#include "dataform.h"

namespace gloox
{

  FlexibleOffline::FlexibleOffline( ClientBase *parent, Disco *disco )
  : m_parent( parent ), m_disco( disco )
  {
    if( m_disco )
      m_disco->registerDiscoHandler( this );
  }

  FlexibleOffline::~FlexibleOffline()
  {
    if( m_disco )
      m_disco->removeDiscoHandler( this );
  }

  void FlexibleOffline::checkSupport()
  {
    m_disco->getDiscoInfo( m_parent->jid().server(), "", this, FO_CHECK_SUPPORT );
  }

  void FlexibleOffline::getMsgCount()
  {
    m_disco->getDiscoInfo( m_parent->jid().server(), XMLNS_OFFLINE, this, FO_REQUEST_NUM );
  }

  void FlexibleOffline::fetchHeaders()
  {
    m_disco->getDiscoItems( m_parent->jid().server(), XMLNS_OFFLINE, this, FO_REQUEST_HEADERS );
  }

  void FlexibleOffline::fetchMessages( const StringList& msgs )
  {
    const std::string id = m_parent->getID();
    Tag *iq = new Tag( "iq" );
    iq->addAttrib( "type", "get" );
    iq->addAttrib( "id", id );
    Tag *o = new Tag( "offline" );
    o->addAttrib( "xmlns", XMLNS_OFFLINE );

    if( msgs.size() == 0 )
      o->addChild( new Tag( "fetch" ) );
    else
    {
      StringList::const_iterator it = msgs.begin();
      for( it; it != msgs.end(); ++it )
      {
        Tag *i = new Tag( o, "item" );
        i->addAttrib( "action", "view" );
        i->addAttrib( "node", (*it) );
      }
    }

    m_parent->trackID( this, id, FO_REQUEST_MSGS );
    m_parent->send( iq );
  }

  void FlexibleOffline::removeMessages( const StringList& msgs )
  {
    const std::string id = m_parent->getID();
    Tag *iq = new Tag( "iq" );
    iq->addAttrib( "type", "get" );
    iq->addAttrib( "id", id );
    Tag *o = new Tag( "offline" );
    o->addAttrib( "xmlns", XMLNS_OFFLINE );

    if( msgs.size() == 0 )
      o->addChild( new Tag( "purge" ) );
    else
    {
      StringList::const_iterator it = msgs.begin();
      for( it; it != msgs.end(); ++it )
      {
        Tag *i = new Tag( o, "item" );
        i->addAttrib( "action", "remove" );
        i->addAttrib( "node", (*it) );
      }
    }

    m_parent->trackID( this, id, FO_REMOVE_MSGS );
    m_parent->send( iq );
  }

  void FlexibleOffline::registerFlexibleOfflineHandler( FlexibleOfflineHandler *foh )
  {
    m_flexibleOfflineHandler = foh;
  }

  void FlexibleOffline::removeFlexibleOfflineHandler()
  {
    m_flexibleOfflineHandler = 0;
  }

  void FlexibleOffline::handleDiscoInfoResult( Stanza *stanza, int context )
  {
    if( !m_flexibleOfflineHandler )
      return;

    switch( context )
    {
      case FO_CHECK_SUPPORT:
        m_flexibleOfflineHandler->handleFlexibleOfflineSupport(
            stanza->findChild( "query" )->hasChild( "feature", "var", XMLNS_OFFLINE ) );
        break;

      case FO_REQUEST_NUM:
        int num = -1;
        DataForm f( stanza->findChild( "query" )->findChild( "x" ) );
        if( f.hasField( "number_of_messages" ) )
          num = atoi( f.field( "number_of_messages" ).value().c_str() );

        m_flexibleOfflineHandler->handleFlexibleOfflineMsgNum( num );
        break;
    }
  }

  void FlexibleOffline::handleDiscoItemsResult( Stanza *stanza, int context )
  {
    if( context == FO_REQUEST_HEADERS )
    {

    }
  }

  bool FlexibleOffline::handleIqID( Stanza *stanza, int context )
  {
    switch( context )
    {
      case FO_REQUEST_MSGS:
        break;
      case FO_REMOVE_MSGS:
        break;
    }
  }

};

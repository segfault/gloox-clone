/*
  Copyright (c) 2005 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA
*/



#include "config.h"

#include "clientbase.h"
#include "connection.h"
#include "parser.h"
#include "tag.h"
#include "stanza.h"

#include <string>
#include <map>
#include <list>


namespace gloox
{

  ClientBase::ClientBase( const std::string& ns )
    : m_namespace( ns ), m_port( -1 ),
      m_connection( 0 ), m_parser( 0 ),
      m_tls( true ), m_sasl( true ), m_idCount( 0 )
  {
  }

  ClientBase::ClientBase( const std::string& ns, const std::string& password, int port )
    : m_namespace( ns ), m_password( password ), m_port( port ),
      m_connection( 0 ), m_parser( 0 ),
      m_tls( true ), m_sasl( true ), m_idCount( 0 )
  {
  }

  ClientBase::ClientBase( const std::string& ns, const std::string& password,
                          const std::string& server, int port )
    : m_namespace( ns ), m_password( password ), m_port( port ),
      m_connection( 0 ), m_parser( 0 ),
      m_tls( true ), m_sasl( true ), m_idCount( 0 )
  {
    m_jid.setServer( server );
  }

  ClientBase::~ClientBase()
  {
    delete m_connection;
    delete m_parser;
    printf( "deleting Parser & Connection in ~ClientBase()\n" );
  }

  bool ClientBase::connect()
  {
    if( m_jid.server().empty() )
      return false;

    if( !m_parser )
      m_parser = new Parser( this, m_namespace );

    if( !m_connection )
      m_connection = new Connection( m_parser, m_jid.server(), m_port );

    int ret = m_connection->connect();
    if( ret == STATE_CONNECTED )
    {
      header();
      m_connection->receive();
    }

    return true;
  }

  void ClientBase::filter( int type, const Tag& tag )
  {
    if( tag.empty() )
      return;

#ifdef DEBUG
    log( tag, true );
#endif

    switch( type )
    {
      case( IKS_NODE_START ):
        m_sid = tag.findAttribute( "id" );
        handleStartNode();
        break;
      case IKS_NODE_NORMAL:
        if( !handleNormalNode( tag ) )
        {
          Stanza stanza( tag );

          switch( stanza.type() )
          {
            case STANZA_IQ:
              notifyIqHandlers( stanza );
              break;
            case STANZA_PRESENCE:
              notifyPresenceHandlers( stanza );
              break;
            case STANZA_S10N:
              notifySubscriptionHandlers( stanza );
              break;
            case STANZA_MESSAGE:
              notifyMessageHandlers( stanza );
              break;
          }
        }
        break;
      case IKS_NODE_ERROR:
#ifdef DEBUG
        printf( "stream error received\n" );
#endif
        disconnect( STATE_ERROR );
        break;
      case IKS_NODE_STOP:
#ifdef DEBUG
        printf( "stream closed\n" );
#endif
        disconnect( STATE_DISCONNECTED );
        break;
    }
  }

  void ClientBase::disconnect( ConnectionState reason )
  {
    if( m_connection )
    {
      m_connection->setState( reason );
      m_connection->disconnect();
    }
  }

  void ClientBase::header()
  {
    std::string xml = "<?xml version='1.0'?>";
    xml += "<stream:stream to='" + streamTo()+  "' xmlns='" + m_namespace + "' ";
    xml += "xmlns:stream='http://etherx.jabber.org/streams' ";
    xml += "version='1.0'>";
    send( xml );
  }

  void ClientBase::send( const Tag& tag )
  {
#ifdef DEBUG
    log( tag, false );
#endif
    if( m_connection )
      m_connection->send( tag.xml() );
  }

  void ClientBase::send( const std::string& xml )
  {
    if( m_connection )
      m_connection->send( xml );
  }

  void ClientBase::send( iks *x )
  {
    printf( "Another user of send( iks *x )" );
    char *str = iks_string( 0, x );
    if( str )
      m_connection->send( str );
    iks_free( str );
    iks_delete( x );
  }

  ConnectionState ClientBase::state() const{
    if( m_connection )
      return m_connection->state();
    else
      return STATE_DISCONNECTED;
  }

  void ClientBase::setState( ConnectionState state )
  {
    if( m_connection )
      m_connection->setState( state );
  }

  void ClientBase::log( const Tag& tag, bool incoming )
  {
#ifdef DEBUG
    if ( m_connection->isSecure() )
      printf( "Sec" );

    if( incoming )
      printf( "RECV " );
    else
      printf( "SEND " );

    const std::string data;
    printf( "[%s]", data.c_str() );
    if( data.substr(data.length()-1, 1 ) != "\n" )
      printf( "\n" );
#endif
  }

  void ClientBase::registerPresenceHandler( PresenceHandler* ph )
  {
    m_presenceHandlers.push_back( ph );
  }

  void ClientBase::removePresenceHandler( PresenceHandler* ph )
  {
    m_presenceHandlers.remove( ph );
  }

  void ClientBase::registerIqHandler( IqHandler* ih, const std::string& xmlns )
  {
    m_iqNSHandlers[xmlns] = ih;
  }

  void ClientBase::trackID( IqHandler* ih, const std::string& id, int context )
  {
    TrackStruct track;
    track.ih = ih;
    track.context = context;
    m_iqIDHandlers[id] = track;
  }

  void ClientBase::removeIqHandler( const std::string& xmlns )
  {
    m_iqNSHandlers.erase( xmlns );
  }

  void ClientBase::registerMessageHandler( MessageHandler* mh )
  {
    m_messageHandlers.push_back( mh );
  }

  void ClientBase::removeMessageHandler( MessageHandler* mh )
  {
    m_messageHandlers.remove( mh );
  }

  void ClientBase::registerSubscriptionHandler( SubscriptionHandler* sh )
  {
    m_subscriptionHandlers.push_back( sh );
  }

  void ClientBase::removeSubscriptionHandler( SubscriptionHandler* sh )
  {
    m_subscriptionHandlers.remove( sh );
  }

  void ClientBase::registerConnectionListener( ConnectionListener* cl )
  {
    m_connectionListeners.push_back( cl );
  }

  void ClientBase::removeConnectionListener( ConnectionListener* cl )
  {
    m_connectionListeners.remove( cl );
  }

  void ClientBase::notifyOnConnect()
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( it; it != m_connectionListeners.end(); it++ )
    {
      (*it)->onConnect();
    }
  }

  void ClientBase::notifyOnDisconnect()
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( it; it != m_connectionListeners.end(); it++ )
    {
      (*it)->onDisconnect();
    }
  }

  void ClientBase::notifyOnResourceBindError( ConnectionListener::ResourceBindError error )
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( it; it != m_connectionListeners.end(); it++ )
    {
      (*it)->onResourceBindError( error );
    }
  }

  void ClientBase::notifyOnSessionCreateError( ConnectionListener::SessionCreateError error )
  {
    ConnectionListenerList::const_iterator it = m_connectionListeners.begin();
    for( it; it != m_connectionListeners.end(); it++ )
    {
      (*it)->onSessionCreateError( error );
    }
  }

  void ClientBase::notifyPresenceHandlers( const Stanza& stanza )
  {
    PresenceHandlerList::const_iterator it = m_presenceHandlers.begin();
    for( it; it != m_presenceHandlers.end(); it++ )
    {
      (*it)->handlePresence( stanza );
    }
  }

  void ClientBase::notifySubscriptionHandlers( const Stanza& stanza )
  {
    SubscriptionHandlerList::const_iterator it = m_subscriptionHandlers.begin();
    for( it; it != m_subscriptionHandlers.end(); it++ )
    {
      (*it)->handleSubscription( stanza );
    }
  }

  void ClientBase::notifyIqHandlers( const Stanza& stanza )
  {
    bool res = false;

    IqHandlerMap::const_iterator it_ns = m_iqNSHandlers.begin();
    for( it_ns; it_ns != m_iqNSHandlers.end(); it_ns++ )
    {
      if( stanza.hasChildWithAttrib( "xmlns", (*it_ns).first ) )
      {
        if( (*it_ns).second->handleIq( stanza ) )
          res = true;
      }
    }

    IqTrackMap::iterator it_id = m_iqIDHandlers.find( stanza.id() );
    if( it_id != m_iqIDHandlers.end() )
    {
      if( (*it_id).second.ih->handleIqID( stanza, (*it_id).second.context ) )
        res = true;
      m_iqIDHandlers.erase( it_id );
    }

    if( !res && ( stanza.type() == STANZA_IQ ) &&
         ( ( stanza.subtype() == STANZA_IQ_GET ) || ( stanza.subtype() == STANZA_IQ_SET ) ) )
    {
      Tag iq( "iq" );
      iq.addAttrib( "type", "error" );
      iq.addAttrib( "id", stanza.id() );
      iq.addAttrib( "to", stanza.from().full() );
      send( iq );
    }
  }

  void ClientBase::notifyMessageHandlers( const Stanza& stanza )
  {
    MessageHandlerList::const_iterator it = m_messageHandlers.begin();
    for( it; it != m_messageHandlers.end(); it++ )
    {
      (*it)->handleMessage( stanza );
    }
  }

};

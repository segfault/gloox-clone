/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "siprofileft.h"

#include "siprofilefthandler.h"
#include "simanager.h"
#include "dataform.h"
#include "socks5bytestream.h"
#include "socks5bytestreammanager.h"

namespace gloox
{

  SIProfileFT::SIProfileFT( ClientBase* parent, SIProfileFTHandler* sipfth, SIManager* manager,
                            SOCKS5BytestreamManager* s5Manager )
    : m_parent( parent ), m_manager( manager ), m_handler( sipfth ),
      m_socks5Manager( s5Manager ), m_delManager( false ), m_delS5Manager( false )
  {
    if( !m_manager )
    {
      m_delManager = true;
      m_manager = new SIManager( m_parent );
    }

    m_manager->registerProfile( XMLNS_SI_FT, this );

    if( !m_socks5Manager )
    {
      m_socks5Manager = new SOCKS5BytestreamManager( m_parent, this );
      m_delS5Manager = true;
    }
  }

  SIProfileFT::~SIProfileFT()
  {
    m_manager->removeProfile( XMLNS_SI_FT );

    if( m_delManager )
      delete m_manager;

    if( m_socks5Manager && m_delS5Manager )
      delete m_socks5Manager;
  }

  void SIProfileFT::requestFT( const JID& to, const std::string& name, int size, const std::string& hash,
                               const std::string& desc, const std::string& date, const std::string& mimetype )
  {
    if( name.empty() || size <= 0 || !m_manager )
      return;

    Tag* file = new Tag( "file", "xmlns", XMLNS_SI_FT );
    file->addAttribute( "name", name );
    file->addAttribute( "size", size );
    if( !hash.empty() )
      file->addAttribute( "hash", hash );
    if( !date.empty() )
      file->addAttribute( "date", date );
    if( !desc.empty() )
      new Tag( file, "desc", desc );

    Tag* feature = new Tag( "feature", "xmlns", XMLNS_FEATURE_NEG );
    DataFormField* dff = new DataFormField( "stream-method", "", "", DataFormField::FieldTypeListSingle );
    StringMap sm;
    sm["s5b"] = XMLNS_BYTESTREAMS;
//     sm["ibb"] = XMLNS_IBB;
//     sm["oob"] = XMLNS_IQ_OOB;
    dff->setOptions( sm );
    DataForm df( DataForm::FormTypeForm );
    df.addField( dff );
    feature->addChild( df.tag() );

    m_manager->requestSI( this, to, XMLNS_SI_FT, file, feature, mimetype );
  }

  void SIProfileFT::acceptFT( const JID& to, const std::string& id, StreamType type )
  {
    if( !m_manager )
      return;
    printf( "accepting si\n" );

    Tag* feature = new Tag( "feature", "xmlns", XMLNS_FEATURE_NEG );
    DataFormField* dff = new DataFormField( "stream-method" );
    switch( type )
    {
      case FTTypeS5B:
        dff->setValue( XMLNS_BYTESTREAMS );
        break;
/*      case FTTypeIBB:
        dff->setValue( XMLNS_IBB );
        break;
      case FTTypeOOB:
        dff->setValue( XMLNS_IQ_OOB );
        break;*/
    }
    DataForm df( DataForm::FormTypeSubmit );
    df.addField( dff );
    feature->addChild( df.tag() );

    m_manager->acceptSI( to, id, 0, feature );
  }

  void SIProfileFT::declineFT( const JID& to, const std::string& id, SIManager::SIError reason,
                               const std::string& text )
  {
    printf( "declining si\n" );
    if( !m_manager )
      return;

    m_manager->declineSI( to, id, reason, text );
  }

  void SIProfileFT::dispose( SOCKS5Bytestream* s5b )
  {
    if( m_socks5Manager )
      m_socks5Manager->dispose( s5b );
  }

  void SIProfileFT::setStreamHosts( StreamHostList hosts )
  {
    if( m_socks5Manager )
      m_socks5Manager->setStreamHosts( hosts );
  }

  void SIProfileFT::addStreamHost( const JID& jid, const std::string& host, int port )
  {
    if( m_socks5Manager )
      m_socks5Manager->addStreamHost( jid, host, port );
  }

  void SIProfileFT::handleSIRequest( const JID& from, const std::string& id, const std::string& profile,
                                     Tag* si, Tag* ptag, Tag* /*fneg*/ )
  {
    if( profile != XMLNS_SI_FT || !ptag || !si )
      return;
printf( "handleSIRequest\n" );
    if( m_handler )
    {
      std::string desc;
      if( ptag->hasChild( "desc" ) )
        desc = ptag->findChild( "desc" )->cdata();
      const std::string& mt = si->findAttribute( "mime-type" );
      m_handler->handleFTRequest( from, id, ptag->findAttribute( "name" ), ptag->findAttribute( "size" ),
                                  ptag->findAttribute( "hash" ), ptag->findAttribute( "date" ),
                                  mt.empty() ? "binary/octet-stream" : mt, desc, FTTypeS5B );
    }
  }

  void SIProfileFT::handleSIRequestResult( const JID& from, const std::string& sid,
                                           Tag* /*si*/, Tag* /*ptag*/, Tag* fneg )
  {

    if( m_socks5Manager && fneg && fneg->hasChild( "x", "xmlns", XMLNS_X_DATA ) )
    {
      DataForm df( fneg->findChild( "x", "xmlns", XMLNS_X_DATA ) );
      DataFormField* dff = df.field( "stream-method" );
      if( dff && dff->value() == XMLNS_BYTESTREAMS )
      {
        printf( "requesting new SOCKS5Bytestream\n" );
        // check return value:
        m_socks5Manager->requestSOCKS5Bytestream( from, SOCKS5BytestreamManager::S5BTCP, sid );
      }
    }

//     if( m_handler )
//       m_handler->handleFTRequestResult( from, sid );
  }

  void SIProfileFT::handleSIRequestError( Stanza* stanza )
  {
    if( m_handler )
      m_handler->handleFTRequestError( stanza );
  }

  void SIProfileFT::handleIncomingSOCKS5BytestreamRequest( const std::string& sid, const JID& from )
  {
    printf( "SIProfileFT::handleIncomingSOCKS5BytestreamRequest dummy impl\n" );
    // check for valid sid/from tuple
    m_socks5Manager->acceptSOCKS5Bytestream( sid );
  }

  void SIProfileFT::handleIncomingSOCKS5Bytestream( const std::string& sid, SOCKS5Bytestream* s5b )
  {
    printf( "SIProfileFT::handleIncomingSOCKS5Bytestream\n" );
    if( m_handler )
      m_handler->handleFTSOCKS5Bytestream( s5b );
  }

  void SIProfileFT::handleOutgoingSOCKS5Bytestream( const JID& to, SOCKS5Bytestream *s5b )
  {
    printf( "SIProfileFT::handleOutgoingSOCKS5Bytestream\n" );
    if( m_handler )
      m_handler->handleFTSOCKS5Bytestream( s5b );
  }

  void SIProfileFT::handleSOCKS5BytestreamError( const JID& remote, StanzaError se )
  {
  }

}

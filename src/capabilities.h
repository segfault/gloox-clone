/*
  Copyright (c) 2007 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#ifndef CAPABILITIES_H__
#define CAPABILITIES_H__

#include "stanzaextension.h"
#include "tag.h"

#include <string>

namespace gloox
{

  class Disco;
  class Tag;

  /**
   * @brief This is an implementation of XEP-0115 (Entity Capabilities).
   *
   * XEP Version: 1.4
   * @author Jakob Schroeter <js@camaya.net>
   * @since 1.0
   */
  class Capabilities : public StanzaExtension
  {

    public:
      /**
       * Constructs a new object and fills it according to the parameters.
       * @param disco The current Client's Disco object.
       */
      Capabilities( Disco* disco );

      /**
       * Constructs a new object from the given Tag.
       * @param tag The Tag to parse.
       */
      Capabilities( const Tag *tag );

      /**
       * Virtual Destructor.
       */
      virtual ~Capabilities();

      /**
       * Returns the client's identifying node.
       * @return The node.
       */
      const std::string& node() const { return m_node; }

      /**
       * Sets the client's identifying node.
       * @param node The node.
       */
      void setNode( const std::string& node ) { m_node = node; }

      /**
       * Returns the client's identifying ver string.
       * @return The ver string.
       */
      const std::string ver() const;

      // reimplemented from StanzaExtension
      virtual Tag* tag() const;

    private:
      Disco* m_disco;
      std::string m_node;
      std::string m_ver;
      bool m_valid;
  };

}

#endif // CAPABILITIES_H__

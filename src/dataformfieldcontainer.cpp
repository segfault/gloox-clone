/*
  Copyright (c) 2005-2008 by Jakob Schroeter <js@camaya.net>
  This file is part of the gloox library. http://camaya.net/gloox

  This software is distributed under a license. The full license
  agreement can be found in the file LICENSE in this distribution.
  This software may not be copied, modified, sold or distributed
  other than expressed in the named license agreement.

  This software is distributed without any warranty.
*/


#include "dataformfieldcontainer.h"
#include "util.h"


namespace gloox
{

  DataFormFieldContainer::DataFormFieldContainer()
  {
  }

  DataFormFieldContainer::~DataFormFieldContainer()
  {
//     util::clearList( m_fields );
    // FIXME
    FieldList::iterator it = m_fields.begin();
    FieldList::iterator it2;
    while( it != m_fields.end() )
    {
      it2 = it++;
      delete (*it2);
      m_fields.erase( it2 );
    }
    // ~
  }

  DataFormField* DataFormFieldContainer::field( const std::string& field ) const
  {
    FieldList::const_iterator it = m_fields.begin();
    for( ; it != m_fields.end() && (*it)->name() != field; ++it )
      ;
    return it != m_fields.end() ? (*it) : 0;
  }

}

/***************************************************************************
                          avlistviewitem.cpp  -  description
                             -------------------
    begin                : Wed Mar 20 2002
    copyright            : (C) 2002 by Jason Wood
    email                : jasonwood@blueyonder.co.uk
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "avlistviewitem.h"
#include "avfile.h"

#include <klocale.h>
#include <kdenlivedoc.h>

#include <math.h>

AVListViewItem::AVListViewItem(KdenliveDoc *doc, QListView *parent, AVFile *clip) : KListViewItem(parent) {
	m_listView = parent;
	m_clip = clip;
  m_doc = doc;
}

AVListViewItem::~AVListViewItem() {
}

QString AVListViewItem::text ( int column ) const {
 	if(m_listView->columnText(column) == i18n("Filename")) {
 		return m_clip->name();
	}
	
	if(m_listView->columnText(column) == i18n("Duration")) {
    if(m_clip->durationKnown()) {
  		return QString::number(((int)m_clip->duration().seconds())/3600).rightJustify(2, '0', FALSE) + ":" +
  			QString::number((((int)m_clip->duration().seconds())/60) % 60).rightJustify(2, '0', FALSE) + ":" +
  			QString::number(((int)m_clip->duration().seconds())%60).rightJustify(2, '0', FALSE) + "." +        
  			QString::number((int)(((int)(m_clip->duration().ms())%1000) * (m_doc->framesPerSecond() / 1000.0))).leftJustify(2, '0', TRUE);
    } else {
      return "unknown";
    }
	}
	
	if(m_listView->columnText(column) == i18n("Size")) {			
		long fileSize = m_clip->fileSize();
		long tenth;
		if(fileSize < 1024) {		
			return QString::number(fileSize) + i18n(" Bytes");
		}
		
		fileSize = (int)floor((fileSize / 1024.0)+0.5);
		
		if(fileSize < 1024) {
			return QString::number(fileSize) + i18n(" Kb");
		}
		
		fileSize = (int)floor((fileSize / 102.4) + 0.5);
		
		tenth = fileSize % 10;
		fileSize /= 10;
		
		return QString::number(fileSize) + "." + QString::number(tenth) + i18n(" Mb");			
	}
	
	if(m_listView->columnText(column) == i18n("Type")) {
	}
	
	if(m_listView->columnText(column) == i18n("Usage Count")) {
		return QString::number(m_clip->numReferences());
	}	
	
	return i18n("n/a");	
}

AVFile *AVListViewItem::clip() const
{
	return m_clip;
}

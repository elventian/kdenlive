/***************************************************************************
                          kmmtrackkeyframepanel.cpp  -  description
                             -------------------
    begin                : Sun Dec 1 2002
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

#include "kmmtrackkeyframepanel.h"
#include "kdenlivedoc.h"
#include "kmmtimeline.h"
#include "kresizecommand.h"
#include "kdebug.h"

KMMTrackKeyFramePanel::KMMTrackKeyFramePanel(KMMTimeLine *timeline,
                                            DocTrackBase *doc,
                                            QWidget *parent,
                                            const char *name )
																				: KMMTrackPanel(timeline, doc, parent,name)
{
  setMinimumHeight(30);
	setMaximumHeight(30);
}

KMMTrackKeyFramePanel::~KMMTrackKeyFramePanel()
{
}

void KMMTrackKeyFramePanel::paintClip(QPainter & painter, DocClipBase * clip, QRect & rect, bool selected)
{
	int clipsx = (int)timeLine()->mapValueToLocal(clip->trackStart().frames(m_docTrack->document()->framesPerSecond()));
	int clipex = (int)timeLine()->mapValueToLocal(clip->trackEnd().frames(m_docTrack->document()->framesPerSecond()));
	int clipWidth = clipex-clipsx;

  int sx = clipsx;
  int width = clipex;

  if(sx < rect.x()) sx = rect.x();
  if(width > rect.x() + rect.width()) width = rect.x() + rect.width();
  width -= sx;

	painter.setClipping(true);
	painter.setClipRect(sx, rect.y(), width, rect.height());

	QColor col = selected ? QColor(128, 128, 128) : QColor(200, 200, 200);

	painter.fillRect( clipsx, rect.y(),
										clipWidth, rect.height(),
										col);
	painter.drawRect( clipsx, rect.y(),
										clipWidth, rect.height());

  painter.setClipping(false);    
}

/** A mouse press event has occured. Perform relevant mouse operations. */
bool KMMTrackKeyFramePanel::mousePressed(QMouseEvent *event)
{
	GenTime mouseTime(m_timeline->mapLocalToValue(event->x()), m_docTrack->document()->framesPerSecond());
  GenTime roundedMouseTime = m_timeline->timeUnderMouse(event->x());
	m_clipUnderMouse = 0;

	switch(m_timeline->editMode()) {
		case KdenliveApp::Move :
										m_clipUnderMouse = docTrack()->getClipAt(mouseTime);
										if(m_clipUnderMouse) {
											if(m_resizeState != None) {
                        m_timeline->selectClipAt(*m_docTrack, (m_clipUnderMouse->trackStart() + m_clipUnderMouse->trackEnd())/2.0);
												m_resizeCommand = new KResizeCommand(m_docTrack->document(), m_clipUnderMouse);
											}

											if (event->state() & ControlButton) {
											  m_timeline->toggleSelectClipAt(*m_docTrack, mouseTime);
											} else if(event->state() & ShiftButton) {
											  m_timeline->selectClipAt(*m_docTrack, mouseTime);
											}
											return true;
										} else {
											m_timeline->addCommand(m_timeline->selectNone(), true);
										}
										break;


		case KdenliveApp::Razor :
										m_clipUnderMouse = docTrack()->getClipAt(mouseTime);
										if(m_clipUnderMouse) {
											if (event->state() & ShiftButton) {
												m_timeline->razorAllClipsAt(roundedMouseTime);
											} else {
												m_timeline->addCommand(m_timeline->razorClipAt(*m_docTrack, roundedMouseTime), true);
											}
											return true;
										}

										break;


		case KdenliveApp::Spacer :
										if (event->state() & ShiftButton) {
			                m_timeline->addCommand(m_timeline->selectLaterClips(mouseTime, true), true);
			       				} else {
			                m_timeline->addCommand(m_timeline->selectLaterClips(mouseTime, false), true);
			           		}
										return true;
										break;
		default :
										kdWarning() << "Error, mouse pressed over clips, unknown mode" << endl;
	}

	return false;
}

bool KMMTrackKeyFramePanel::mouseReleased(QMouseEvent *event)
{
 	GenTime mouseTime = m_timeline->timeUnderMouse(event->x());

	switch(m_timeline->editMode()) {
		case KdenliveApp::Move :
										if(m_resizeState != None) {
											m_resizeCommand->setEndSize(m_clipUnderMouse);
											m_timeline->addCommand(m_resizeCommand, false);
											m_resizeCommand = 0;
										} else {
											if(m_dragging) {
												m_dragging = false;
											} else {
												if(m_clipUnderMouse) {
													if (event->state() & ControlButton) {
													} else if(event->state() & ShiftButton) {
													} else {
														m_timeline->addCommand(m_timeline->selectNone(), true);
													  m_timeline->selectClipAt(*m_docTrack, mouseTime);
													}
												} else {
													m_timeline->addCommand(m_timeline->selectNone(), true);
												}
											}
										}
										break;
		case KdenliveApp::Razor :
										break;
		case KdenliveApp::Spacer :
										break;
		default :
										kdWarning() << "Error, mouse released over clips, unknown mode" << endl;
	}

	m_clipUnderMouse = 0;
	return true;
}

/** Set the cursor to an appropriate shape, relative to the position on the track. */
QCursor KMMTrackKeyFramePanel::getMouseCursor(QMouseEvent *event)
{
	GenTime mouseTime(m_timeline->mapLocalToValue(event->x()), m_docTrack->document()->framesPerSecond());
	DocClipBase *clip;

	m_resizeState = None;

	switch(m_timeline->editMode()) {
		case KdenliveApp::Move :
								clip = docTrack()->getClipAt(mouseTime);
								if(clip) {
									if( fabs(m_timeline->mapValueToLocal(clip->trackStart().frames(m_docTrack->document()->framesPerSecond())) - event->x()) < resizeTolerance) {
										m_resizeState = Start;
										return QCursor(Qt::SizeHorCursor);
									}
									if( fabs(m_timeline->mapValueToLocal((clip->trackEnd()).frames(m_docTrack->document()->framesPerSecond())) - event->x()) < resizeTolerance) {
										m_resizeState = End;
										return QCursor(Qt::SizeHorCursor);
									}
								}
								return QCursor(Qt::ArrowCursor);
		case KdenliveApp::Razor :
               clip = docTrack()->getClipAt(mouseTime);
                if(clip) {
                  emit lookingAtClip(clip, mouseTime - clip->trackStart() + clip->cropStartTime());
                }
								return QCursor(Qt::SplitVCursor);
		case KdenliveApp::Spacer :
								return QCursor(Qt::SizeHorCursor);
		default:
								return QCursor(Qt::ArrowCursor);
	}
}

bool KMMTrackKeyFramePanel::mouseMoved(QMouseEvent *event)
{
	GenTime mouseTime = m_timeline->timeUnderMouse(event->x());

	switch(m_timeline->editMode()) {
		case KdenliveApp::Move :
								if(m_clipUnderMouse) {
									if(m_resizeState != None) {
										if(m_resizeState == Start) {
											m_docTrack->resizeClipTrackStart(m_clipUnderMouse, mouseTime);
                      emit signalClipCropStartChanged(m_clipUnderMouse->cropStartTime());
										} else if(m_resizeState == End) {
											m_docTrack->resizeClipTrackEnd(m_clipUnderMouse, mouseTime);
                      emit signalClipCropEndChanged(m_clipUnderMouse->cropStartTime() + m_clipUnderMouse->cropDuration());
										} else {
											kdError() << "Unknown resize state reached in KMMTimeLineTrackView::mouseMoveEvent()" << endl;
											kdError() << "(this message should never be seen!)" << endl;
										}
									} else {
										if(!m_timeline->clipSelected(m_clipUnderMouse)) {
											if ((event->state() & ControlButton) || (event->state() & ShiftButton)) {
												m_timeline->selectClipAt(*m_docTrack,mouseTime);
											} else {
												m_timeline->addCommand(m_timeline->selectNone(), true);
												m_timeline->selectClipAt(*m_docTrack,mouseTime);
											}
										}
										m_dragging = true;
										m_timeline->initiateDrag(m_clipUnderMouse, mouseTime - m_clipUnderMouse->trackStart());
									}
								}
								break;
		case KdenliveApp::Razor :
								break;
		case KdenliveApp::Spacer :
								break;
		default:
								kdWarning() << "Error, mouse moved over clips, unknown mode" << endl;
	}

	return true;
}

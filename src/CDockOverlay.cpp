/*
 * Qt Advanced Docking System
 * Copyright (C) 2017 Uwe Kindler
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; If not, see <http://www.gnu.org/licenses/>.
 */

#include "CDockOverlay.h"

#include <QCursor>
#include <QDebug>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QMap>
#include <QMoveEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QPointer>
#include <QResizeEvent>
#include <QWindow>
#include <QtGlobal>

#include <iostream>

#include "CAutoHideSideBar.h"
#include "CDockAreaTabBar.h"
#include "CDockAreaTitleBar.h"
#include "CDockAreaWidget.h"
#include "CDockContainerWidget.h"
#include "CDockManager.h"

#include "CDockOverlayCross.h"
#include "CDockOverlay_p.h"

#include "CDockOverlayCross_p.h"

namespace ads
{

DockOverlayPrivate::DockOverlayPrivate(CDockOverlay* _public)
	: _this(_public)
	, Cross(nullptr)
{}

int DockOverlayPrivate::sideBarOverlaySize(SideBarLocation sideBarLocation)
{
	auto Container = qobject_cast<CDockContainerWidget*>(TargetWidget.data());
	auto SideBar = Container->autoHideSideBar(sideBarLocation);
	if (!SideBar || !SideBar->isVisibleTo(Container))
	{
		return AutoHideAreaWidth;
	}
	else
	{
		return (SideBar->orientation() == Qt::Horizontal) ? SideBar->height() : SideBar->width();
	}
}

int DockOverlayPrivate::sideBarMouseZone(SideBarLocation sideBarLocation)
{
	auto Container = qobject_cast<CDockContainerWidget*>(TargetWidget.data());
	auto SideBar = Container->autoHideSideBar(sideBarLocation);
	if (!SideBar || !SideBar->isVisibleTo(Container))
	{
		return AutoHideAreaMouseZone;
	}
	else
	{
		return (SideBar->orientation() == Qt::Horizontal) ? SideBar->height() : SideBar->width();
	}
}

CDockOverlay::CDockOverlay(QWidget* parent, eMode Mode)
	:// Wayland: the overlay must be a child widget of the target window. Top
	// level windows cannot be positioned in screen coordinates on Wayland,
	// so a Qt::Tool overlay would not align over the target dock area
	QFrame(internal::isWayland() && parent ? parent->window() : parent)
	, d(new DockOverlayPrivate(this))
{
	d->Mode = Mode;
	d->Cross = new CDockOverlayCross(this);
	if (internal::isWayland())
	{
		// Remember the stable home window so hideOverlay() can reparent the
		// overlay back to it (see HomeWindow), keeping it off transient
		// floating windows that may be destroyed.
		d->HomeWindow = parentWidget();
		// A translucent child widget would become a native window
		// (wl_subsurface) that takes part in the compositors drag and drop
		// target picking and steals the drop focus from the dock container.
		// A child widget composites with its parent without the attribute.
		// WA_TransparentForMouseEvents lets a release on a drop indicator
		// fall through to it
		setAttribute(Qt::WA_TransparentForMouseEvents);
	}
	else
	{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
#else
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
#endif
		setWindowOpacity(1);
		setAttribute(Qt::WA_TranslucentBackground);
	}
	setWindowTitle("DockOverlay");
	setAttribute(Qt::WA_NoSystemBackground);

	d->Cross->setVisible(false);
	setVisible(false);
}

CDockOverlay::~CDockOverlay()
{
	delete d;
}

void CDockOverlay::setAllowedAreas(DockWidgetAreas areas)
{
	if (areas == d->AllowedAreas)
	{
		return;
	}
	d->AllowedAreas = areas;
	d->Cross->reset();
}

void CDockOverlay::setAllowedArea(DockWidgetArea area, bool Enable)
{
	auto AreasOld = d->AllowedAreas;
	d->AllowedAreas.setFlag(area, Enable);
	if (AreasOld != d->AllowedAreas)
	{
		d->Cross->reset();
	}
}

DockWidgetAreas CDockOverlay::allowedAreas() const
{
	return d->AllowedAreas;
}

DockWidgetArea CDockOverlay::dropAreaUnderCursor() const
{
	return dropAreaUnderCursor(QCursor::pos());
}

DockWidgetArea CDockOverlay::dropAreaUnderCursor(const QPoint& GlobalPos) const
{
	d->TabIndex = InvalidTabIndex;
	if (!d->TargetWidget)
	{
		return InvalidDockWidgetArea;
	}

	DockWidgetArea Result = d->Cross->cursorLocation(GlobalPos);
	if (Result != InvalidDockWidgetArea)
	{
		return Result;
	}

	auto CursorPos = GlobalPos;
	auto DockArea = qobject_cast<CDockAreaWidget*>(d->TargetWidget.data());
	if (!DockArea && CDockManager::autoHideConfigFlags().testFlag(CDockManager::AutoHideFeatureEnabled))
	{
		auto Rect = rect();
		const QPoint pos = mapFromGlobal(GlobalPos);
		if ((pos.x() < d->sideBarMouseZone(SideBarLocation::SideBarLeft)) && d->AllowedAreas.testFlag(LeftAutoHideArea))
		{
			Result = LeftAutoHideArea;
		}
		else if (pos.x() > (Rect.width() - d->sideBarMouseZone(SideBarLocation::SideBarRight)) && d->AllowedAreas.testFlag(RightAutoHideArea))
		{
			Result = RightAutoHideArea;
		}
		else if (pos.y() < d->sideBarMouseZone(SideBarLocation::SideBarTop) && d->AllowedAreas.testFlag(TopAutoHideArea))
		{
			Result = TopAutoHideArea;
		}
		else if (pos.y() > (Rect.height() - d->sideBarMouseZone(SideBarLocation::SideBarBottom)) && d->AllowedAreas.testFlag(BottomAutoHideArea))
		{
			Result = BottomAutoHideArea;
		}

		auto SideBarLocation = ads::internal::toSideBarLocation(Result);
		if (SideBarLocation != SideBarLocation::SideBarNone)
		{
			auto Container = qobject_cast<CDockContainerWidget*>(d->TargetWidget.data());
			auto SideBar = Container->autoHideSideBar(SideBarLocation);
			if (SideBar->isVisible())
			{
				d->TabIndex = SideBar->tabInsertIndexAt(SideBar->mapFromGlobal(CursorPos));
			}
		}
		return Result;
	}
	else if (!DockArea)
	{
		return Result;
	}

	if (
		DockArea->allowedAreas().testFlag(CenterDockWidgetArea) && !DockArea->titleBar()->isHidden() &&
		DockArea->titleBarGeometry().contains(DockArea->mapFromGlobal(CursorPos))
	)
	{
		auto TabBar = DockArea->titleBar()->tabBar();
		d->TabIndex = TabBar->tabInsertIndexAt(TabBar->mapFromGlobal(CursorPos));
		return CenterDockWidgetArea;
	}

	return Result;
}

int CDockOverlay::tabIndexUnderCursor() const
{
	return d->TabIndex;
}

DockWidgetArea CDockOverlay::visibleDropAreaUnderCursor() const
{
	return visibleDropAreaUnderCursor(QCursor::pos());
}

DockWidgetArea CDockOverlay::visibleDropAreaUnderCursor(const QPoint& GlobalPos) const
{
	if (isHidden() || !d->DropPreviewEnabled)
	{
		return InvalidDockWidgetArea;
	}
	else
	{
		return dropAreaUnderCursor(GlobalPos);
	}
}

DockWidgetArea CDockOverlay::showOverlay(QWidget* target)
{
	return showOverlay(target, QCursor::pos());
}

DockWidgetArea CDockOverlay::showOverlay(QWidget* target, const QPoint& GlobalPos)
{
	// Cache the drag position that drives this overlay's drop preview. On
	// Wayland paintEvent() reads it back because the global cursor position is
	// not available during a platform drag; it must be current before the
	// show()/repaint() below triggers a paint
	d->LastGlobalPos = GlobalPos;

	// Wayland: a cross window drop (e.g. into a floating widget) requires the
	// overlay to be a child of the target window because a widget cannot be
	// positioned over a foreign window
	if (internal::isWayland() && parentWidget() != target->window())
	{
		setParent(target->window());
		d->Cross->setParent(target->window());
		d->TargetWidget = nullptr;
	}

	if (d->TargetWidget == target)
	{
		// Hint: We could update geometry of overlay here.
		DockWidgetArea da = dropAreaUnderCursor(GlobalPos);
		if (da != d->LastLocation)
		{
			repaint();
			d->LastLocation = da;
		}
		return da;
	}

	d->TargetWidget = target;
	d->LastLocation = InvalidDockWidgetArea;

	// Move it over the target.
	hide();
	resize(target->size());
	QPoint TopLeft = target->mapToGlobal(target->rect().topLeft());
	// Wayland: the overlay is a child of the target window, so the target top
	// left has to be mapped into the parent coordinate system
	if (internal::isWayland() && parentWidget())
	{
		move(parentWidget()->mapFromGlobal(TopLeft));
	}
	else
	{
		move(TopLeft);
	}
	show();
	if (internal::isWayland())
	{
		// paint the child overlay above its sibling dock widgets
		raise();
	}
	d->Cross->updatePosition();
	d->Cross->updateOverlayIcons();
	return dropAreaUnderCursor(GlobalPos);
}

void CDockOverlay::hideOverlay()
{
	hide();
	// Wayland: showOverlay() reparents this overlay (and its cross) into the
	// top level window it is shown over. Reparent them back to the stable home
	// window now that the overlay is hidden, so the dock-manager-owned overlay
	// is never left as a child of a transient floating window - destroying that
	// window would otherwise delete the overlay and leave the dock manager with
	// a dangling pointer.
	if (internal::isWayland() && d->HomeWindow && parentWidget() != d->HomeWindow)
	{
		setParent(d->HomeWindow);
		d->Cross->setParent(d->HomeWindow);
	}
	d->TargetWidget.clear();
	d->LastLocation = InvalidDockWidgetArea;
	d->DropAreaRect = QRect();
}

void CDockOverlay::enableDropPreview(bool Enable)
{
	d->DropPreviewEnabled = Enable;
	update();
}

bool CDockOverlay::dropPreviewEnabled() const
{
	return d->DropPreviewEnabled;
}

void CDockOverlay::paintEvent(QPaintEvent* event)
{
	Q_UNUSED(event);

	// Draw rect based on location
	if (!d->DropPreviewEnabled)
	{
		d->DropAreaRect = QRect();
		return;
	}

	QRect r = rect();
	// Wayland: use the cached drag position because the global cursor
	// position is not available during a platform drag
	const DockWidgetArea da = internal::isWayland() ? dropAreaUnderCursor(d->LastGlobalPos) : dropAreaUnderCursor();
	double Factor = (CDockOverlay::ModeContainerOverlay == d->Mode) ? 3 : 2;

	switch (da)
	{
		case TopDockWidgetArea:
			r.setHeight(r.height() / Factor);
			break;
		case RightDockWidgetArea:
			r.setX(r.width() * (1 - 1 / Factor));
			break;
		case BottomDockWidgetArea:
			r.setY(r.height() * (1 - 1 / Factor));
			break;
		case LeftDockWidgetArea:
			r.setWidth(r.width() / Factor);
			break;
		case CenterDockWidgetArea:
			r = rect();
			break;
		case LeftAutoHideArea:
			r.setWidth(d->sideBarOverlaySize(SideBarLeft));
			break;
		case RightAutoHideArea:
			r.setX(r.width() - d->sideBarOverlaySize(SideBarRight));
			break;
		case TopAutoHideArea:
			r.setHeight(d->sideBarOverlaySize(SideBarTop));
			break;
		case BottomAutoHideArea:
			r.setY(r.height() - d->sideBarOverlaySize(SideBarBottom));
			break;
		default:
			return;
	}

	QPainter painter(this);
	QColor Color = palette().color(QPalette::Active, QPalette::Highlight);
	QPen Pen = painter.pen();
	Pen.setColor(Color.darker(120));
	Pen.setStyle(Qt::SolidLine);
	Pen.setWidth(1);
	Pen.setCosmetic(true);
	painter.setPen(Pen);
	Color = Color.lighter(130);
	Color.setAlpha(64);
	painter.setBrush(Color);
	painter.drawRect(r.adjusted(0, 0, -1, -1));
	d->DropAreaRect = r;
}

QRect CDockOverlay::dropOverlayRect() const
{
	return d->DropAreaRect;
}

void CDockOverlay::showEvent(QShowEvent* e)
{
	d->Cross->show();
	QFrame::showEvent(e);
}

void CDockOverlay::hideEvent(QHideEvent* e)
{
	d->Cross->hide();
	QFrame::hideEvent(e);
}

bool CDockOverlay::event(QEvent* e)
{
	bool Result = Super::event(e);
	if (e->type() == QEvent::Polish)
	{
		d->Cross->setupOverlayCross(d->Mode);
	}
	return Result;
}

}// namespace ads

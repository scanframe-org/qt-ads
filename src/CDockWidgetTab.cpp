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

/// @file   CDockWidgetTab.cpp
/// @author Uwe Kindler
/// @date   27.02.2017
/// @brief  Implementation of CDockWidgetTab class

#include "CDockWidgetTab.h"

#include <QMouseEvent>
#include <QStyle>
#include "CDockAreaWidget.h"
#include "CDockFocusController.h"
#include "CDockManager.h"
#include "CDockOverlay.h"
#include "CDockWidget.h"
#include "CElidingLabel.h"
#include "CFloatingDockContainer.h"
#include "CFloatingDragPreview.h"
#include "ads_globals.h"
#include "CDockWidgetTab_p.h"

namespace ads
{

// struct DockWidgetTabPrivate

DockWidgetTabPrivate::DockWidgetTabPrivate(CDockWidgetTab* _public)
	: _this(_public)
	, DockWidget(nullptr)
	, TitleLabel(nullptr)
	, IconTextSpacer(nullptr)
{}

void DockWidgetTabPrivate::createLayout()
{
	TitleLabel = new tTabLabel();
	if (CDockManager::testConfigFlag(CDockManager::DisableTabTextEliding))
	{
		TitleLabel->setElideMode(Qt::ElideNone);
	}
	else
	{
		TitleLabel->setElideMode(Qt::ElideRight);
	}
	TitleLabel->setText(DockWidget->windowTitle());
	TitleLabel->setObjectName("dockWidgetTabLabel");
	TitleLabel->setAlignment(Qt::AlignCenter);
	_this->connect(TitleLabel, SIGNAL(elidedChanged(bool)), SIGNAL(elidedChanged(bool)));

	CloseButton = createCloseButton();
	CloseButton->setObjectName("tabCloseButton");
	internal::setButtonIcon(CloseButton, QStyle::SP_TitleBarCloseButton, TabCloseIcon);
	CloseButton->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
	CloseButton->setFocusPolicy(Qt::NoFocus);
	updateCloseButtonSizePolicy();
	internal::setToolTip(CloseButton, QObject::tr("Close Tab"));
	_this->connect(CloseButton, SIGNAL(clicked()), SIGNAL(closeRequested()));

	QFontMetrics fm(TitleLabel->font());
	int Spacing = qRound(fm.height() / 4.0);

	// Fill the layout
	QBoxLayout* Layout = new QBoxLayout(QBoxLayout::LeftToRight);
	Layout->setContentsMargins(2 * Spacing, 0, 0, 0);
	Layout->setSpacing(0);
	_this->setLayout(Layout);
	Layout->addWidget(TitleLabel, 1);
	Layout->addSpacing(Spacing);
	Layout->addWidget(CloseButton);
	Layout->addSpacing(qRound(Spacing * 4.0 / 3.0));
	Layout->setAlignment(Qt::AlignCenter);

	TitleLabel->setVisible(true);
}

void DockWidgetTabPrivate::moveTab(QMouseEvent* ev)
{
	ev->accept();
	QPoint Distance = internal::globalPositionOf(ev) - GlobalDragStartMousePosition;
	Distance.setY(0);
	auto TargetPos = Distance + TabDragStartPosition;
	TargetPos.rx() = qMax(TargetPos.x(), 0);
	TargetPos.rx() = qMin(_this->parentWidget()->rect().right() - _this->width() + 1, TargetPos.rx());
	_this->move(TargetPos);
	_this->raise();
}

bool DockWidgetTabPrivate::startFloating(eDragState DraggingState)
{
	auto dockContainer = DockWidget->dockContainer();
	ADS_PRINT("isFloating " << dockContainer->isFloating());
	ADS_PRINT("areaCount " << dockContainer->dockAreaCount());
	ADS_PRINT("widgetCount " << DockWidget->dockAreaWidget()->dockWidgetsCount());
	// On Wayland, dragging the tab of the single dock widget of a
	// floating widget drags the existing floating widget, so the user
	// can dock it into another container
	if (
		internal::isWayland() && dockContainer->isFloating() && (dockContainer->visibleDockAreaCount() == 1) &&
		(DockWidget->dockAreaWidget()->dockWidgetsCount() == 1)
	)
	{
		if (DraggingFloatingWidget == DraggingState)
		{
			auto FloatingContainer = dockContainer->floatingWidget();
			if (FloatingContainer)
			{
				DragState = DraggingInactive;
				CFloatingDockContainer::startPlatformDrag(FloatingContainer, GlobalDragStartMousePosition, _this);
				return true;
			}
		}
		return false;
	}

	ADS_PRINT("startFloating");
	DragState = DraggingState;
	IFloatingWidget* FloatingWidget = nullptr;
	// On Wayland the drag preview is an in-window child widget (driven by event
	// coordinates, confined to the source container); it converts to a native
	// compositor platform drag only when the cursor leaves the source window
	// (see waylandPreviewMove()). So a drag uses a preview on every platform.
	bool CreateContainer = (DraggingFloatingWidget != DraggingState);

	// If section widget has multiple tabs, we take only one tab
	// If it has only one single tab, we can move the complete
	// dock area into floating widget
	QSize Size;
	if (DockArea->dockWidgetsCount() > 1)
	{
		FloatingWidget = createFloatingWidget(DockWidget, CreateContainer);
		Size = DockWidget->size();
	}
	else
	{
		FloatingWidget = createFloatingWidget(DockArea, CreateContainer);
		Size = DockArea->size();
	}

	if (DraggingFloatingWidget == DraggingState)
	{
		FloatingWidget->startFloating(DragStartMousePosition, Size, DraggingFloatingWidget, _this);
		auto DockManager = DockWidget->dockManager();
		auto Overlay = DockManager->containerOverlay();
		Overlay->setAllowedAreas(OuterDockAreas);
		this->FloatingWidget = FloatingWidget;
		if (internal::isWayland())
		{
			// Confine the in-window preview to the source container; its
			// position and the drop overlays are driven by event coordinates
			// delivered to the tab (see waylandPreviewMove()). FloatingWidget is
			// a CFloatingDragPreview here because CreateContainer was false.
			auto Preview = static_cast<CFloatingDragPreview*>(FloatingWidget);
			Preview->setSourceContainer(dockContainer);
		}
		qApp->postEvent(DockWidget, new QEvent((QEvent::Type) internal::DockedWidgetDragStartEvent));
	}
	else
	{
		FloatingWidget->startFloating(DragStartMousePosition, Size, DraggingInactive, nullptr);
	}

	return true;
}

void DockWidgetTabPrivate::waylandPreviewMove(QMouseEvent* ev)
{
	const QPoint GlobalPos = internal::globalPositionOf(ev);

	// FloatingWidget is a CFloatingDragPreview during the in-window phase
	// (DraggingFloatingWidget state on Wayland; see startFloating()). While the
	// cursor stays inside the source window the preview just follows it.  If
	// the window is not floatable and the cursor is outside of the source window,
	// then the preview will be frozen until the drag is cancelled or the cursor
	// reenters the source window.
	auto Preview = static_cast<CFloatingDragPreview*>(FloatingWidget);
	if (CFloatingDockContainer::waylandMoveOrLeaveInWindowPreview(Preview, _this->window(), GlobalPos, _this->isFloatable()))
	{
		return;
	}

	// Boundary cross: the preview was torn down, convert the gesture into a
	// native compositor platform drag of a freshly created floating widget.
	FloatingWidget = nullptr;
	DragState = DraggingInactive;

	QSize Size;
	IFloatingWidget* RealFloating = nullptr;
	if (DockArea->dockWidgetsCount() > 1)
	{
		RealFloating = createFloatingWidget(DockWidget, true);
		Size = DockWidget->size();
	}
	else
	{
		RealFloating = createFloatingWidget(DockArea, true);
		Size = DockArea->size();
	}
	CFloatingDockContainer::startPlatformDragForFloatingWidget(RealFloating, DragStartMousePosition, Size, GlobalDragStartMousePosition, _this);
}

CDockWidgetTab::CDockWidgetTab(CDockWidget* DockWidget, QWidget* parent)
	: QFrame(parent)
	, d(new DockWidgetTabPrivate(this))
{
	setAttribute(Qt::WA_NoMousePropagation, true);
	d->DockWidget = DockWidget;
	d->createLayout();
	setFocusPolicy(Qt::NoFocus);
}

CDockWidgetTab::~CDockWidgetTab()
{
	ADS_PRINT("~CDockWidgetTab()");
	delete d;
}

void CDockWidgetTab::mousePressEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		ev->accept();
		d->saveDragStartMousePosition(internal::globalPositionOf(ev));
		d->DragState = DraggingMousePressed;
		if (CDockManager::testConfigFlag(CDockManager::FocusHighlighting))
		{
			d->focusController()->setDockWidgetTabPressed(true);
			d->focusController()->setDockWidgetTabFocused(this);
		}
		Q_EMIT clicked();
		return;
	}
	Super::mousePressEvent(ev);
}

void CDockWidgetTab::mouseReleaseEvent(QMouseEvent* ev)
{
	if (ev->button() == Qt::LeftButton)
	{
		auto CurrentDragState = d->DragState;
		d->GlobalDragStartMousePosition = QPoint();
		d->DragStartMousePosition = QPoint();
		d->DragState = DraggingInactive;

		switch (CurrentDragState)
		{
			case DraggingTab:
				// End of tab moving, emit signal
				if (d->DockArea)
				{
					ev->accept();
					Q_EMIT moved(internal::globalPositionOf(ev));
				}
				break;

			case DraggingFloatingWidget:
				ev->accept();
				d->FloatingWidget->finishDragging();
				break;

			default:
				break;
		}

		if (CDockManager::testConfigFlag(CDockManager::FocusHighlighting))
		{
			d->focusController()->setDockWidgetTabPressed(false);
		}
	}
	else if (ev->button() == Qt::MiddleButton)
	{
		if (CDockManager::testConfigFlag(CDockManager::MiddleMouseButtonClosesTab) && d->DockWidget->features().testFlag(CDockWidget::DockWidgetClosable))
		{
			// Only attempt to close if the mouse is still
			// on top of the widget, to allow the user to cancel.
			if (rect().contains(mapFromGlobal(QCursor::pos())))
			{
				ev->accept();
				Q_EMIT closeRequested();
			}
		}
	}

	Super::mouseReleaseEvent(ev);
}

void CDockWidgetTab::mouseMoveEvent(QMouseEvent* ev)
{
	if (!(ev->buttons() & Qt::LeftButton) || d->isDraggingState(DraggingInactive))
	{
		d->DragState = DraggingInactive;
		Super::mouseMoveEvent(ev);
		return;
	}

	// move floating window
	if (d->isDraggingState(DraggingFloatingWidget))
	{
		if (internal::isWayland())
		{
			// Wayland hybrid drag: in-window preview until the cursor leaves
			// the source window, then convert to a native platform drag.
			d->waylandPreviewMove(ev);
		}
		else
		{
			d->FloatingWidget->moveFloating();
		}
		Super::mouseMoveEvent(ev);
		return;
	}

	// move tab
	if (d->isDraggingState(DraggingTab))
	{
		// Moving the tab is always allowed because it does not mean moving the
		// dock widget around
		d->moveTab(ev);
	}

	auto MappedPos = mapToParent(ev->pos());
	bool MouseOutsideBar = (MappedPos.x() < 0) || (MappedPos.x() > parentWidget()->rect().right());
	// Maybe a fixed drag distance is better here ?
	int DragDistanceY = qAbs(d->GlobalDragStartMousePosition.y() - internal::globalPositionOf(ev).y());
	if (DragDistanceY >= CDockManager::startDragDistance() || MouseOutsideBar)
	{
		// Floating is only allowed for widgets that are floatable
		// We can create the drag preview if the widget is movable.
		auto Features = d->DockWidget->features();
		if (Features.testFlag(CDockWidget::DockWidgetFloatable) || (Features.testFlag(CDockWidget::DockWidgetMovable)))
		{
			// If we undock, we need to restore the initial position of this
			// tab because it looks strange if it remains on its dragged position
			if (d->isDraggingState(DraggingTab))
			{
				parentWidget()->layout()->update();
			}
			d->startFloating();
		}
		return;
	}
	else if (
		d->DockArea->openDockWidgetsCount() > 1 &&
		(internal::globalPositionOf(ev) - d->GlobalDragStartMousePosition).manhattanLength() >= QApplication::startDragDistance()
	)// Wait a few pixels
	// before start moving
	{
		// If we start dragging the tab, we save its initial position to
		// restore it later
		if (DraggingTab != d->DragState)
		{
			d->TabDragStartPosition = this->pos();
		}
		d->DragState = DraggingTab;
		return;
	}

	Super::mouseMoveEvent(ev);
}

void CDockWidgetTab::contextMenuEvent(QContextMenuEvent* ev)
{
	ev->accept();
	if (d->isDraggingState(DraggingFloatingWidget))
	{
		return;
	}

	auto Menu = buildContextMenu(nullptr);
	d->saveDragStartMousePosition(ev->globalPos());
	Menu->exec(ev->globalPos());
}

QMenu* CDockWidgetTab::buildContextMenu(QMenu* Menu)
{
	if (Menu == nullptr)
	{
		Menu = new QMenu(this);
	}

	ADS_PRINT("CDockWidgetTab::buildContextMenu");
	const bool isFloatable = d->DockWidget->features().testFlag(CDockWidget::DockWidgetFloatable);
	const bool isTopLevelArea = d->DockArea->isTopLevelArea();
	const bool isFloating = d->DockWidget->isFloating();
	const bool isDetachable = isFloatable && !isFloating;
	QAction* Action;

	if (!(isTopLevelArea && isFloating))
	{
		Action = Menu->addAction(tr("Detach"), this, SLOT(detachDockWidget()));
		Action->setEnabled(isDetachable);
		if (CDockManager::testAutoHideConfigFlag(CDockManager::AutoHideFeatureEnabled))
		{
			Action = Menu->addAction(tr("Pin"), this, SLOT(autoHideDockWidget()));
			auto IsPinnable = d->DockWidget->features().testFlag(CDockWidget::DockWidgetPinnable);
			Action->setEnabled(IsPinnable);

			auto menu = Menu->addMenu(tr("Pin To..."));
			menu->setEnabled(IsPinnable);
			d->createAutoHideToAction(tr("Top"), SideBarTop, menu);
			d->createAutoHideToAction(tr("Left"), SideBarLeft, menu);
			d->createAutoHideToAction(tr("Right"), SideBarRight, menu);
			d->createAutoHideToAction(tr("Bottom"), SideBarBottom, menu);
		}
	}

	Menu->addSeparator();
	Action = Menu->addAction(tr("Close"), this, SIGNAL(closeRequested()));
	Action->setEnabled(isClosable());
	if (d->DockArea->openDockWidgetsCount() > 1)
	{
		Menu->addAction(tr("Close Others"), this, SIGNAL(closeOtherTabsRequested()));
	}

	return Menu;
}

bool CDockWidgetTab::isActiveTab() const
{
	return d->IsActiveTab;
}

void CDockWidgetTab::setActiveTab(bool active)
{
	d->updateCloseButtonVisibility(active);

	if (CDockManager::testConfigFlag(CDockManager::ShowTabTextOnlyForActiveTab) && !d->Icon.isNull())
	{
		if (active)
			d->TitleLabel->setVisible(true);
		else
			d->TitleLabel->setVisible(false);
	}

	// Focus related stuff
	if (CDockManager::testConfigFlag(CDockManager::FocusHighlighting) && !d->DockWidget->dockManager()->isRestoringState())
	{
		bool UpdateFocusStyle = false;
		// Update the focus only, if this the dock area of this tab is the focused
		// dock area
		if (active && !hasFocus() && (d->focusController()->focusedDockArea() == this->dockAreaWidget()))
		{
			d->focusController()->setDockWidgetTabFocused(this);
			UpdateFocusStyle = true;
		}

		if (d->IsActiveTab == active)
		{
			if (UpdateFocusStyle)
			{
				updateStyle();
			}
			return;
		}
	}
	else if (d->IsActiveTab == active)
	{
		return;
	}

	d->IsActiveTab = active;
	updateStyle();
	update();
	updateGeometry();

	Q_EMIT activeTabChanged();
}

CDockWidget* CDockWidgetTab::dockWidget() const
{
	return d->DockWidget;
}

void CDockWidgetTab::setDockAreaWidget(CDockAreaWidget* DockArea)
{
	d->DockArea = DockArea;
}

CDockAreaWidget* CDockWidgetTab::dockAreaWidget() const
{
	return d->DockArea;
}

void CDockWidgetTab::setIcon(const QIcon& Icon)
{
	QBoxLayout* Layout = qobject_cast<QBoxLayout*>(layout());
	if (!d->IconLabel && Icon.isNull())
	{
		return;
	}

	if (!d->IconLabel)
	{
		d->IconLabel = new QLabel();
		d->IconLabel->setAlignment(Qt::AlignVCenter);
		d->IconLabel->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Preferred);
		internal::setToolTip(d->IconLabel, d->TitleLabel->toolTip());
		Layout->insertWidget(0, d->IconLabel, Qt::AlignVCenter);
		Layout->insertSpacing(1, qRound(1.5 * Layout->contentsMargins().left() / 2.0));
	}
	else if (Icon.isNull())
	{
		// Remove icon label and spacer item
		Layout->removeWidget(d->IconLabel);
		Layout->removeItem(Layout->itemAt(0));
		delete d->IconLabel;
		d->IconLabel = nullptr;
	}

	d->Icon = Icon;
	d->updateIcon();
}

const QIcon& CDockWidgetTab::icon() const
{
	return d->Icon;
}

QString CDockWidgetTab::text() const
{
	return d->TitleLabel->text();
}

void CDockWidgetTab::mouseDoubleClickEvent(QMouseEvent* event)
{
	if (event->button() == Qt::LeftButton && CDockManager::testConfigFlag(CDockManager::DoubleClickUndocksWidget))
	{
		// If this is the last dock area in a dock container it does not make
		// sense to move it to a new floating widget and leave this one
		// empty
		if (
			(!d->DockArea->dockContainer()->isFloating() || d->DockArea->dockWidgetsCount() > 1) &&
			d->DockWidget->features().testFlag(CDockWidget::DockWidgetFloatable)
		)
		{
			event->accept();
			d->saveDragStartMousePosition(internal::globalPositionOf(event));
			d->startFloating(DraggingInactive);
		}
	}

	Super::mouseDoubleClickEvent(event);
}

void CDockWidgetTab::setVisible(bool visible)
{
	visible &= !d->DockWidget->features().testFlag(CDockWidget::NoTab);
	Super::setVisible(visible);
}

void CDockWidgetTab::setText(const QString& title)
{
	d->TitleLabel->setText(title);
}

bool CDockWidgetTab::isTitleElided() const
{
	return d->TitleLabel->isElided();
}

bool CDockWidgetTab::isClosable() const
{
	return d->DockWidget && d->DockWidget->features().testFlag(CDockWidget::DockWidgetClosable);
}

bool CDockWidgetTab::isFloatable() const
{
	return d->DockWidget && d->DockWidget->features().testFlag(CDockWidget::DockWidgetFloatable);
}

void CDockWidgetTab::detachDockWidget()
{
	if (!d->DockWidget->features().testFlag(CDockWidget::DockWidgetFloatable))
	{
		return;
	}

	d->saveDragStartMousePosition(QCursor::pos());
	d->startFloating(DraggingInactive);
}

void CDockWidgetTab::autoHideDockWidget()
{
	d->DockWidget->setAutoHide(true);
}

void CDockWidgetTab::onAutoHideToActionClicked()
{
	int Location = sender()->property(internal::LocationProperty).toInt();
	d->DockWidget->toggleAutoHide((SideBarLocation) Location);
}

bool CDockWidgetTab::event(QEvent* e)
{
#ifndef QT_NO_TOOLTIP
	if (e->type() == QEvent::ToolTipChange)
	{
		const auto text = toolTip();
		d->TitleLabel->setToolTip(text);
		if (d->IconLabel)
		{
			d->IconLabel->setToolTip(text);
		}
	}
#endif
	if (e->type() == QEvent::StyleChange)
	{
		d->updateIcon();
	}
	return Super::event(e);
}

eDragState CDockWidgetTab::dragState() const
{
	return d->DragState;
}

void CDockWidgetTab::onDockWidgetFeaturesChanged()
{
	d->updateCloseButtonSizePolicy();
	d->updateCloseButtonVisibility(isActiveTab());
}

void CDockWidgetTab::setElideMode(Qt::TextElideMode mode)
{
	d->TitleLabel->setElideMode(mode);
}

void CDockWidgetTab::updateStyle()
{
	internal::repolishStyle(this, internal::RepolishDirectChildren);
}

QSize CDockWidgetTab::iconSize() const
{
	return d->IconSize;
}

void CDockWidgetTab::setIconSize(const QSize& Size)
{
	d->IconSize = Size;
	d->updateIcon();
}

}// namespace ads

// EOF CDockWidgetTab.cpp

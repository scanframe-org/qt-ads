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

/// @file   CFloatingDockContainer.cpp
/// @author Uwe Kindler
/// @date   01.03.2017
/// @brief  Implementation of CFloatingDockContainer class

#include "CFloatingDockContainer.h"

#include <QAbstractButton>
#include <QAction>
#include <QApplication>
#include <QBoxLayout>
#include <QDataStream>
#include <QDebug>
#include <QDrag>
#include <QElapsedTimer>
#include <QMimeData>
#include <QMouseEvent>
#include <QPointer>
#include <QTime>
#include <QWindow>

#include <iostream>

#include "CDockAreaWidget.h"
#include "CDockContainerWidget.h"
#include "CDockManager.h"
#include "CDockOverlay.h"
#include "CDockWidget.h"
#include "CFloatingDragPreview.h"

#ifdef Q_OS_WIN
	#include <windows.h>
	#ifdef _MSC_VER
		#pragma comment(lib, "User32.lib")
	#endif
#endif
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
	#include <xcb/xcb.h>

	#include "linux/CFloatingWidgetTitleBar.h"
#elif defined(Q_OS_WIN)
	#include "linux/CFloatingWidgetTitleBar.h"
#endif

#include "CFloatingDockContainer_p.h"

namespace ads
{

// struct FloatingDockContainerPrivate

FloatingDockContainerPrivate::FloatingDockContainerPrivate(CFloatingDockContainer* _public)
	: _this(_public)
	, DockContainer(nullptr)
{}

void FloatingDockContainerPrivate::titleMouseReleaseEvent()
{
	setState(DraggingInactive);
	if (!DropContainer)
	{
		return;
	}

	// processEvents() below runs a nested event loop. The floating widget,
	// the drop container or the dock manager may be deleted during it, so
	// everything needed afterwards is held in guarded pointers and 'this' is
	// not touched after the nested loop.
	QPointer<CFloatingDockContainer> Floating(_this);
	QPointer<CDockContainerWidget> Target(DropContainer);
	QPointer<CDockManager> Manager(DockManager);
	if (!Manager)
	{
		return;
	}

	if (Manager->dockAreaOverlay()->dropAreaUnderCursor() != InvalidDockWidgetArea || Manager->containerOverlay()->dropAreaUnderCursor() != InvalidDockWidgetArea)
	{
		CDockOverlay* Overlay = Manager->containerOverlay();
		if (!Overlay->dropOverlayRect().isValid())
		{
			Overlay = Manager->dockAreaOverlay();
		}

		// Do not resize if we drop into an autohide sidebar area to preserve
		// the dock area size for the initial size of the auto hide area
		if (!ads::internal::isSideBarArea(Overlay->dropAreaUnderCursor()))
		{
			// Resize the floating widget to the size of the highlighted drop area
			// rectangle
			QRect Rect = Overlay->dropOverlayRect();
			int FrameWidth = (Floating->frameSize().width() - Floating->rect().width()) / 2;
			int TitleBarHeight = Floating->frameSize().height() - Floating->rect().height() - FrameWidth;
			if (Rect.isValid())
			{
				QPoint TopLeft = Overlay->mapToGlobal(Rect.topLeft());
				TopLeft.ry() += TitleBarHeight;
				Floating->setGeometry(QRect(TopLeft, QSize(Rect.width(), Rect.height() - TitleBarHeight)));
				QApplication::processEvents();
				// 'Overlay' must not be used past this point
			}
		}

		// The nested loop may have deleted either target, so check both
		if (Floating && Target)
		{
			// DockManager will be unlinked from this within dropFloatingWidget
			Target->dropFloatingWidget(Floating, QCursor::pos());
		}
	}

	if (Manager)
	{
		Manager->containerOverlay()->hideOverlay();
		Manager->dockAreaOverlay()->hideOverlay();
	}
}

void FloatingDockContainerPrivate::updateDropOverlays(const QPoint& GlobalPos)
{
	if (!_this->isVisible() || !DockManager)
	{
		return;
	}

#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
	// Prevent display of drop overlays and docking as long as a model dialog
	// is active
	if (qApp->activeModalWidget())
	{
		return;
	}
#endif

	auto Containers = DockManager->dockContainers();
	CDockContainerWidget* TopContainer = nullptr;
	for (auto ContainerWidget: Containers)
	{
		if (!ContainerWidget->isVisible())
		{
			continue;
		}

		if (DockContainer == ContainerWidget)
		{
			continue;
		}

		QPoint MappedPos = ContainerWidget->mapFromGlobal(GlobalPos);
		if (ContainerWidget->rect().contains(MappedPos))
		{
			if (!TopContainer || ContainerWidget->isInFrontOf(TopContainer))
			{
				TopContainer = ContainerWidget;
			}
		}
	}

	DropContainer = TopContainer;
	if (!TopContainer)
	{
		DockManager->containerOverlay()->hideOverlay();
		DockManager->dockAreaOverlay()->hideOverlay();
		return;
	}

	CDockContainerWidget::showDropOverlays(DockManager, TopContainer, GlobalPos, DockContainer->features().testFlag(CDockWidget::DockWidgetPinnable));
}

void FloatingDockContainerPrivate::handleEscapeKey()
{
	ADS_PRINT("FloatingDockContainerPrivate::handleEscapeKey()");
	setState(DraggingInactive);
	DockManager->containerOverlay()->hideOverlay();
	DockManager->dockAreaOverlay()->hideOverlay();
}

// Wayland: a floating widget that is a child of the dock manager makes Qt
// turn the dock manager and all of its child widgets into native windows
// when the floating widget is shown. Native child widgets are
// wl_subsurfaces that take part in the compositors drag and drop target
// picking and steal the drop focus from the dock container during a
// platform drag. The dock manager deletes the registered floating widgets
// in its destructor, so the floating widget does not need a parent for
// memory management.
CFloatingDockContainer::CFloatingDockContainer(CDockManager* DockManager)
	: tFloatingWidgetBase(internal::isWayland() ? nullptr : static_cast<QWidget*>(DockManager))
	, d(new FloatingDockContainerPrivate(this))
{
	d->DockManager = DockManager;
	d->DockContainer = new CDockContainerWidget(DockManager, this);
	connect(d->DockContainer, SIGNAL(dockAreasAdded()), this, SLOT(onDockAreasAddedOrRemoved()));
	connect(d->DockContainer, SIGNAL(dockAreasRemoved()), this, SLOT(onDockAreasAddedOrRemoved()));

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
	setWidget(d->DockContainer);
	setFeatures(DockWidgetClosable | DockWidgetMovable | DockWidgetFloatable);

	bool native_window = true;

	// FloatingContainerForce*TitleBar is overwritten by the "ADS_UseNativeTitle"
	// environment variable if set.
	auto env = qgetenv("ADS_UseNativeTitle").toUpper();
	if (env == "1")
	{
		native_window = true;
	}
	else if (env == "0")
	{
		native_window = false;
	}
	else if (CDockManager::testConfigFlag(CDockManager::FloatingContainerForceNativeTitleBar))
	{
		native_window = true;
	}
	else if (CDockManager::testConfigFlag(CDockManager::FloatingContainerForceQWidgetTitleBar))
	{
		native_window = false;
	}
	else
	{
		// KDE doesn't seem to fire MoveEvents while moving windows, so for now no
		// native titlebar for everything using KWin.
		QString window_manager = internal::windowManager().toUpper().split(" ")[0];
		native_window = window_manager != "KWIN";
	}

	// Wayland does not allow clients to move windows, so the custom title
	// bar cannot work there - dragging it would not move the window. Always
	// use a native window so that the user can move the floating widget
	// via the window decoration provided by the compositor or by Qt.
	if (internal::isWayland())
	{
		native_window = true;
	}

	if (native_window)
	{
		setTitleBarWidget(new QWidget());
		setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::CustomizeWindowHint | Qt::WindowCloseButtonHint);
	}
	else
	{
		d->TitleBar = new CFloatingWidgetTitleBar(this);
		setTitleBarWidget(d->TitleBar);
		setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint);
		d->TitleBar->enableCloseButton(isClosable());
		connect(d->TitleBar, SIGNAL(closeRequested()), SLOT(close()));
		connect(d->TitleBar, &CFloatingWidgetTitleBar::maximizeRequested, this, &CFloatingDockContainer::onMaximizeRequest);
	}
	setMouseTracking(true);
	d->DockContainer->installEventFilter(this);
#elif defined(Q_OS_WIN)
	if (internal::isWine())
	{
		// Under Wine, native window borders don't work properly.
		// Use a frameless window with a custom QWidget title bar instead.
		d->TitleBar = new CFloatingWidgetTitleBar(this);
		setWindowFlags(Qt::Window | Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint);
		d->TitleBar->enableCloseButton(isClosable());
		connect(d->TitleBar, SIGNAL(closeRequested()), SLOT(close()));
		connect(d->TitleBar, &CFloatingWidgetTitleBar::maximizeRequested, this, &CFloatingDockContainer::onMaximizeRequest);
		QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
		l->setContentsMargins(0, 0, 0, 0);
		l->setSpacing(0);
		setLayout(l);
		l->addWidget(d->TitleBar);
		l->addWidget(d->DockContainer);
		setMouseTracking(true);
		d->DockContainer->installEventFilter(this);
	}
	else
	{
		// Native Windows: use native window borders
		setWindowFlags(Qt::Window | Qt::WindowMaximizeButtonHint | Qt::WindowCloseButtonHint);
		QBoxLayout* l = new QBoxLayout(QBoxLayout::TopToBottom);
		l->setContentsMargins(0, 0, 0, 0);
		l->setSpacing(0);
		setLayout(l);
		l->addWidget(d->DockContainer);
	}
#endif

	if (CDockManager::testConfigFlag(CDockManager::UseNativeWindows))
	{
		winId();
	}

	// Wayland: the floating widget has no parent widget (see above), so it
	// does not inherit the dock manager's effective style sheet through the
	// widget hierarchy. Apply the style sheets along the dock manager parent
	// chain explicitly, so the floating widget looks like the docked content.
	// CDockManager::changeEvent() keeps this up to date when the style sheet
	// changes while the widget is already floating.
	if (internal::isWayland())
	{
		const QString StyleSheet = waylandInheritedStyleSheet(DockManager);
		if (!StyleSheet.isEmpty())
		{
			setStyleSheet(StyleSheet);
		}
	}

	DockManager->registerFloatingWidget(this);
}

CFloatingDockContainer::CFloatingDockContainer(CDockAreaWidget* DockArea)
	: CFloatingDockContainer(DockArea->dockManager())
{
	d->DockContainer->addDockArea(DockArea);

	auto TopLevelDockWidget = topLevelDockWidget();
	if (TopLevelDockWidget)
	{
		TopLevelDockWidget->emitTopLevelChanged(true);
	}

	d->DockManager->notifyWidgetOrAreaRelocation(DockArea);
}

CFloatingDockContainer::CFloatingDockContainer(CDockWidget* DockWidget)
	: CFloatingDockContainer(DockWidget->dockManager())
{
	d->DockContainer->addDockWidget(CenterDockWidgetArea, DockWidget);
	auto TopLevelDockWidget = topLevelDockWidget();
	if (TopLevelDockWidget)
	{
		TopLevelDockWidget->emitTopLevelChanged(true);
	}

	d->DockManager->notifyWidgetOrAreaRelocation(DockWidget);
}

CFloatingDockContainer::~CFloatingDockContainer()
{
	ADS_PRINT("~CFloatingDockContainer");
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
	if (!hasNativeTitleBar())
	{
		qApp->removeEventFilter(this);
		updateCursorFor(Qt::Edges());
	}
#endif
	if (d->DockManager)
	{
		d->DockManager->removeFloatingWidget(this);
	}
	delete d;
}

void CFloatingDockContainer::deleteContent()
{
	std::vector<QPointer<ads::CDockAreaWidget>> areas;
	for (int i = 0; i != dockContainer()->dockAreaCount(); ++i)
	{
		areas.push_back(dockContainer()->dockArea(i));
	}
	for (auto area: areas)
	{
		if (!area)
		{
			continue;
		}

		// QPointer delete safety - just in case some dock widget in destruction
		// deletes another related/twin or child dock widget.
		std::vector<QPointer<QWidget>> deleteWidgets;
		for (auto widget: area->dockWidgets())
		{
			deleteWidgets.push_back(widget);
		}
		for (const auto& ptrWdg: deleteWidgets)
		{
			delete ptrWdg;
		}
	}
}

CDockContainerWidget* CFloatingDockContainer::dockContainer() const
{
	return d->DockContainer;
}

bool CFloatingDockContainer::isDraggingActive() const
{
	return d->isState(DraggingFloatingWidget);
}

void CFloatingDockContainer::changeEvent(QEvent* event)
{
	Super::changeEvent(event);
	switch (event->type())
	{
		case QEvent::ActivationChange:
			if (isActiveWindow())
			{
				ADS_PRINT("FloatingWidget::changeEvent QEvent::ActivationChange ");
				d->zOrderIndex = ++zOrderCounterFloating;

#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
				if (!hasNativeTitleBar() && d->DraggingState == DraggingFloatingWidget)
				{
					d->titleMouseReleaseEvent();
					// d->DraggingState = DraggingInactive;
				}
#endif
			}
			break;

		case QEvent::WindowStateChange:
			// If the DockManager window is restored from minimized on Windows
			// then the FloatingWidgets are not properly restored to maximized but
			// to normal state.
			// We simply check here, if the FloatingWidget was maximized before
			// and if the DockManager is just leaving the minimized state. In this
			// case, we restore the maximized state of this floating widget.
			// DockManager is cleared in finishDropOperation() before the deferred
			// delete, so state change events may still arrive without one.
			if (d->DockManager && d->DockManager->isLeavingMinimizedState())
			{
				auto ev = static_cast<QWindowStateChangeEvent*>(event);
				if (ev->oldState().testFlag(Qt::WindowMaximized))
				{
					this->showMaximized();
				}
			}
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
			if (d->TitleBar)
			{
				d->TitleBar->setVisible(!(isFloating() && isFullScreen()));
			}
#elif defined(Q_OS_WIN)
			if (d->TitleBar)
			{
				d->TitleBar->setVisible(!isFullScreen());
			}
#endif
			break;

		default:
			// do nothing
			break;
	}
}

#ifdef Q_OS_WIN
	#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
bool CFloatingDockContainer::nativeEvent(const QByteArray& eventType, void* message, long* result)
	#else
bool CFloatingDockContainer::nativeEvent(const QByteArray& eventType, void* message, qintptr* result)
	#endif
{
	QWidget::nativeEvent(eventType, message, result);
	MSG* msg = static_cast<MSG*>(message);
	switch (msg->message)
	{
		case WM_MOVING:
		{
			if (d->isState(DraggingFloatingWidget))
			{
				d->updateDropOverlays(QCursor::pos());
			}
		}
		break;

		case WM_NCLBUTTONDOWN:
			if (msg->wParam == HTCAPTION && d->isState(DraggingInactive))
			{
				ADS_PRINT("CFloatingDockContainer::nativeEvent WM_NCLBUTTONDOWN");
				d->DragStartPos = pos();
				d->setState(DraggingMousePressed);
			}
			break;

		case WM_NCLBUTTONDBLCLK:
			d->setState(DraggingInactive);
			break;

		case WM_ENTERSIZEMOVE:
			if (d->isState(DraggingMousePressed))
			{
				ADS_PRINT("CFloatingDockContainer::nativeEvent WM_ENTERSIZEMOVE");
				d->setState(DraggingFloatingWidget);
				d->updateDropOverlays(QCursor::pos());
			}
			break;

		case WM_EXITSIZEMOVE:
			if (d->isState(DraggingFloatingWidget))
			{
				ADS_PRINT("CFloatingDockContainer::nativeEvent WM_EXITSIZEMOVE");
				if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
				{
					d->handleEscapeKey();
				}
				else
				{
					d->titleMouseReleaseEvent();
				}
			}
			break;
	}
	return false;
}
#endif

void CFloatingDockContainer::closeEvent(QCloseEvent* event)
{
	ADS_PRINT("CFloatingDockContainer closeEvent");
	d->setState(DraggingInactive);
	event->ignore();
	if (!isClosable())
	{
		return;
	}

	bool HasOpenDockWidgets = false;
	for (auto DockWidget: d->DockContainer->openedDockWidgets())
	{
		if (DockWidget->features().testFlag(CDockWidget::DockWidgetDeleteOnClose) || DockWidget->features().testFlag(CDockWidget::CustomCloseHandling))
		{
			bool Closed = DockWidget->closeDockWidgetInternal();
			if (!Closed)
			{
				HasOpenDockWidgets = true;
			}
		}
		else
		{
			DockWidget->toggleView(false);
		}
	}

	if (HasOpenDockWidgets)
	{
		return;
	}

	// New bug (QWebEngineView reload side effect):
	// when a WebEngine-based dock is tabified into a floating container, the
	// embedded native/web process can trigger delayed hide/show cycles on the
	// floating window. If every non-spontaneous hide propagates to
	// DockWidget->toggleView(false), unrelated tabs are marked closed and seem
	// to "disappear". We therefore arm HideContentOnNextHide only for the
	// explicit close path.
	d->HideContentOnNextHide = true;

	// In Qt version after 5.9.2 there seems to be a bug that causes the
	// QWidget::event() function to not receive any NonClientArea mouse
	// events anymore after a close/show cycle. The bug is reported here:
	// https://bugreports.qt.io/browse/QTBUG-73295
	// The following code is a workaround for Qt versions > 5.9.2 that seems
	// to work
	// Starting from Qt version 5.12.2 this seems to work again. But
	// now the QEvent::NonClientAreaMouseButtonPress function returns always
	// Qt::RightButton even if the left button was pressed
	this->hide();
}

void CFloatingDockContainer::hideEvent(QHideEvent* event)
{
	Super::hideEvent(event);
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
	if (!hasNativeTitleBar())
	{
		qApp->removeEventFilter(this);
		updateCursorFor(Qt::Edges());
	}
#endif
	if (event->spontaneous())
	{
		return;
	}

	// Prevent toogleView() events during restore state. Without a DockManager
	// the container has been unlinked and is pending deletion.
	if (!d->DockManager || d->DockManager->isRestoringState())
	{
		return;
	}

	// Only a close operation should propagate hide->toggleView(false) to
	// child dock widgets. Generic hide/show cycles (e.g. from platform or
	// embedded native content) must not change dock open/closed state.
	if (!d->HideContentOnNextHide)
	{
		return;
	}
	d->HideContentOnNextHide = false;

	if (d->AutoHideChildren)
	{
		d->Hiding = true;
		for (auto DockArea: d->DockContainer->openedDockAreas())
		{
			for (auto DockWidget: DockArea->openedDockWidgets())
			{
				DockWidget->toggleView(false);
			}
		}
		d->Hiding = false;
	}
}

void CFloatingDockContainer::showEvent(QShowEvent* event)
{
	Super::showEvent(event);
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
	if (!hasNativeTitleBar())
	{
		qApp->removeEventFilter(this);
		qApp->installEventFilter(this);
	}
	if (CDockManager::testConfigFlag(CDockManager::FocusHighlighting))
	{
		this->window()->activateWindow();
	}
#endif
}

void CFloatingDockContainer::startFloating(const QPoint& DragStartMousePos, const QSize& Size, eDragState DragState, QWidget* MouseEventHandler)
{
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
	if (hasNativeTitleBar())
	{
		// Native title bar on Windows: use simple path
		resize(Size);
		d->DragStartMousePosition = DragStartMousePos;
		d->setState(DragState);
		moveFloating();
		show();
	}
	else
	{
		// Custom title bar (Linux non-native or Windows under Wine)
		if (!isMaximized())
		{
			resize(Size);
			d->DragStartMousePosition = DragStartMousePos;
		}
		d->setState(DragState);
		if (DraggingFloatingWidget == DragState)
		{
			d->MouseEventHandler = MouseEventHandler;
			if (d->MouseEventHandler)
			{
				d->MouseEventHandler->grabMouse();
			}
		}
		if (!isMaximized())
		{
			moveFloating();
		}
		show();
	}
#else
	Q_UNUSED(MouseEventHandler)
	resize(Size);
	d->DragStartMousePosition = DragStartMousePos;
	d->setState(DragState);
	moveFloating();
	show();
#endif
}

void CFloatingDockContainer::moveFloating()
{
	int BorderSize = (frameSize().width() - size().width()) / 2;
	const QPoint moveToPos = QCursor::pos() - d->DragStartMousePosition - QPoint(BorderSize, 0);
	move(moveToPos);
	switch (d->DraggingState)
	{
		case DraggingMousePressed:
			d->setState(DraggingFloatingWidget);
			d->updateDropOverlays(QCursor::pos());
			break;

		case DraggingFloatingWidget:
			d->updateDropOverlays(QCursor::pos());
#ifdef Q_OS_MACOS
			// In OSX when hiding the DockAreaOverlay the application would set
			// the main window as the active window for some reason. This fixes
			// that by resetting the active window to the floating widget after
			// updating the overlays.
			activateWindow();
#endif
			break;
		default:
			break;
	}
}

bool CFloatingDockContainer::isClosable() const
{
	return d->DockContainer->features().testFlag(CDockWidget::DockWidgetClosable);
}

void CFloatingDockContainer::onDockAreasAddedOrRemoved()
{
	ADS_PRINT("CFloatingDockContainer::onDockAreasAddedOrRemoved()");
	auto TopLevelDockArea = d->DockContainer->topLevelDockArea();
	if (TopLevelDockArea)
	{
		d->SingleDockArea = TopLevelDockArea;
		CDockWidget* CurrentWidget = d->SingleDockArea->currentDockWidget();
		d->reflectCurrentWidget(CurrentWidget);
		connect(d->SingleDockArea, SIGNAL(currentChanged(int)), this, SLOT(onDockAreaCurrentChanged(int)));
	}
	else
	{
		if (d->SingleDockArea)
		{
			disconnect(d->SingleDockArea, SIGNAL(currentChanged(int)), this, SLOT(onDockAreaCurrentChanged(int)));
			d->SingleDockArea = nullptr;
		}
		d->setWindowTitle(FloatingDockContainerPrivate::floatingContainersTitle());
		setWindowIcon(QApplication::windowIcon());
	}

	if (d->DockContainer->dockAreaCount() == 0)
	{
		deleteLater();
	}
}

void CFloatingDockContainer::updateWindowTitle()
{
	// If this floating container will be hidden, then updating the window
	// tile is not required anymore
	if (d->Hiding)
	{
		return;
	}

	auto TopLevelDockArea = d->DockContainer->topLevelDockArea();
	if (TopLevelDockArea)
	{
		CDockWidget* CurrentWidget = TopLevelDockArea->currentDockWidget();
		if (CurrentWidget)
		{
			d->reflectCurrentWidget(CurrentWidget);
		}
	}
	else
	{
		d->setWindowTitle(FloatingDockContainerPrivate::floatingContainersTitle());
		setWindowIcon(QApplication::windowIcon());
	}
}

void CFloatingDockContainer::onDockAreaCurrentChanged(int Index)
{
	Q_UNUSED(Index);
	CDockWidget* CurrentWidget = d->SingleDockArea->currentDockWidget();
	d->reflectCurrentWidget(CurrentWidget);
}

bool CFloatingDockContainer::restoreState(CDockingStateReader& Stream, bool Testing)
{
	if (!d->DockContainer->restoreState(Stream, Testing))
	{
		return false;
	}
	onDockAreasAddedOrRemoved();
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
	if (!hasNativeTitleBar() && d->TitleBar)
	{
		d->TitleBar->setMaximizedIcon(windowState() == Qt::WindowMaximized);
	}
#endif
	return true;
}

bool CFloatingDockContainer::hasTopLevelDockWidget() const
{
	return d->DockContainer->hasTopLevelDockWidget();
}

CDockWidget* CFloatingDockContainer::topLevelDockWidget() const
{
	return d->DockContainer->topLevelDockWidget();
}

QList<CDockWidget*> CFloatingDockContainer::dockWidgets() const
{
	return d->DockContainer->dockWidgets();
}

void CFloatingDockContainer::finishDropOperation()
{
	// Widget has been redocked, so it must be hidden right way (see
	// https://github.com/githubuser0xFFFF/Qt-Advanced-Docking-System/issues/351)
	// but AutoHideChildren must be set to false because "this" still contains
	// dock widgets that shall not be toggled hidden.
	d->AutoHideChildren = false;
	hide();
	// The floating widget will be deleted now. Ensure, that the destructor
	// of the floating widget does not delete any dock areas that have been
	// moved to a new container - simply remove all dock areas before deleting
	// the floating widget
	d->DockContainer->removeAllDockAreas();
	deleteLater();
	if (d->DockManager)
	{
		d->DockManager->removeFloatingWidget(this);
		d->DockManager.clear();
	}
	this->dockContainer()->removeFromDockManager();
}

void CFloatingDockContainer::finishDragging()
{
	ADS_PRINT("CFloatingDockContainer::finishDragging");
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
	if (!hasNativeTitleBar())
	{
		setWindowOpacity(1);
		activateWindow();
		if (d->MouseEventHandler)
		{
			d->MouseEventHandler->releaseMouse();
			d->MouseEventHandler = nullptr;
		}
	}
#endif
	d->titleMouseReleaseEvent();
}

// MIME type that transports the CFloatingDockContainer pointer between the
// drag source and the CDockContainerWidget drop targets within this
// application
static const char* const FloatingWidgetMimeType = "application/x-ads-floating-dock-container";

// MIME types that QWaylandDataDevice translates into an
// xdg_toplevel_drag_v1 request so that the compositor attaches the floating
// widget window to the drag cursor. The format matches
// QMainWindowLayout::performPlatformWidgetDrag() in qtbase
static const char* const PlatformDragWindowMimeType = "application/x-qt-mainwindowdrag-window";
static const char* const PlatformDragPositionMimeType = "application/x-qt-mainwindowdrag-position";

// Maximum age of the recorded drop candidate that startPlatformDrag() still
// accepts as a drop. It must be longer than the interval between drag move
// events (so a drop right after the last move counts) but short enough to
// reject a release far away from the last drop area
static const int PlatformDragDropCandidateTimeoutMs = 400;

// Records the last drop candidate during a Wayland platform drag. Some
// compositors do not deliver a drop event when the dragged window overlaps
// the drop target (they send a leave event instead), so startPlatformDrag()
// falls back to this candidate when no drop event was delivered.
struct PlatformDragDropCandidate
{
		QPointer<CDockContainerWidget> Container;
		QPoint GlobalPos;
		bool ValidDropArea = false;
		bool DropHandled = false;
		QElapsedTimer SinceLastUpdate;
};

static PlatformDragDropCandidate s_PlatformDragDropCandidate;

void CFloatingDockContainer::platformDragUpdateDropCandidate(CDockContainerWidget* Container, const QPoint& GlobalPos, bool ValidDropArea)
{
	s_PlatformDragDropCandidate.Container = Container;
	s_PlatformDragDropCandidate.GlobalPos = GlobalPos;
	s_PlatformDragDropCandidate.ValidDropArea = ValidDropArea;
	s_PlatformDragDropCandidate.SinceLastUpdate.start();
}

void CFloatingDockContainer::platformDragNotifyDropHandled()
{
	s_PlatformDragDropCandidate.DropHandled = true;
}

QString CFloatingDockContainer::waylandInheritedStyleSheet(QWidget* DockManager)
{
	QString StyleSheet;
	for (QWidget* Widget = DockManager; Widget; Widget = Widget->parentWidget())
	{
		const QString WidgetStyleSheet = Widget->styleSheet();
		if (!WidgetStyleSheet.isEmpty())
		{
			StyleSheet = WidgetStyleSheet + QLatin1Char('\n') + StyleSheet;
		}
	}
	return StyleSheet;
}

Qt::DropAction
CFloatingDockContainer::startPlatformDrag(CFloatingDockContainer* FloatingWidget, const QPoint& GlobalPressPos, QWidget* DragSource, const QPoint* DragOffset)
{
	auto serialize = [](const auto& object) {
		QByteArray Data;
		QDataStream DataStream(&Data, QIODevice::WriteOnly);
		DataStream << object;
		return Data;
	};

	// xdg_toplevel_drag_v1: the attach offset that places the new window under
	// the cursor is only honoured by the compositor while the toplevel is still
	// UNMAPPED. The protocol says the client "issues a
	// xdg_toplevel_drag_v1.attach request before mapping it", and the offset
	// "might only be used when an unmapped window is attached". Qt's
	// QWaylandDataDevice::startDrag() performs the attach when the data drag
	// starts, reading the xdg_toplevel surface role off the window handle.
	//
	// The caller already show()'d the floating widget, which creates the
	// xdg_toplevel role, but the surface is only mapped (a buffer committed)
	// once the event loop is pumped. We therefore deliberately do NOT wait for
	// exposure here: pumping events would map the window before exec() runs the
	// attach, after which the compositor ignores the offset and leaves the
	// window at its default position (typically screen center). Proceeding
	// straight to exec() keeps the toplevel created-but-unmapped so the attach
	// offset positions it under the cursor.

	// The offset is the press position relative to the floating widget top
	// left corner, so the compositor keeps the cursor at the same spot on
	// the window during the drag. For a freshly created floating window the
	// caller supplies it explicitly (DragOffset): on Wayland the new window has
	// no meaningful geometry yet, so mapFromGlobal() would give a wrong/random
	// offset and the compositor would place the window randomly.
	QPoint DragStartOffset;
	if (DragOffset)
	{
		DragStartOffset = *DragOffset;
		// The new floating window is a native window on Wayland (forced in the
		// constructor), so it has a title bar / decoration around the content -
		// drawn either by the compositor or by Qt's client-side decoration.
		// The supplied offset is relative to the source tab/area content, so
		// shift it by the window frame's top and left margins to keep the
		// grabbed content point - not the decoration - under the cursor. With
		// server-side decorations the margins are 0 and no shift is applied.
		if (FloatingWidget->windowHandle())
		{
			const QMargins Frame = FloatingWidget->windowHandle()->frameMargins();
			DragStartOffset.rx() += Frame.left();
			DragStartOffset.ry() += Frame.top();
		}
	}
	else
	{
		DragStartOffset = FloatingWidget->mapFromGlobal(GlobalPressPos);
	}

	auto Drag = new QDrag(DragSource);
	auto MimeData = new QMimeData();
	MimeData->setData(QLatin1String(PlatformDragWindowMimeType), serialize(reinterpret_cast<qintptr>(FloatingWidget->windowHandle())));
	MimeData->setData(QLatin1String(PlatformDragPositionMimeType), serialize(DragStartOffset));
	MimeData->setData(QLatin1String(FloatingWidgetMimeType), serialize(reinterpret_cast<qintptr>(FloatingWidget)));
	Drag->setMimeData(MimeData);

	// The drop targets dock the floating widget in their dropEvent() handler,
	// which deletes the floating widget before exec() returns
	QPointer<CFloatingDockContainer> GuardedFloatingWidget(FloatingWidget);
	auto DockManager = FloatingWidget->dockContainer()->dockManager();
	s_PlatformDragDropCandidate = PlatformDragDropCandidate();
	// The QDrag::exec() return value does not reliably encode whether the
	// floating widget was docked on Wayland (e.g. compositors that send a
	// leave event instead of a drop event report Qt::IgnoreAction even though
	// the drag ended over a drop area). Track the docking outcome explicitly
	// instead and translate it into the documented return value below.
	Drag->exec();

	// Some compositors (e.g. Mutter) do not deliver a drop event to the
	// target container when the dragged window overlaps it - they send a
	// leave event on release instead. If no drop event was delivered, but
	// the drag ended over a valid drop area, dock the floating widget into
	// the recorded candidate. A short timeout distinguishes a drop on a
	// drop area from a release outside of any drop area to keep the widget
	// floating
	auto& Candidate = s_PlatformDragDropCandidate;
	bool Docked = Candidate.DropHandled;
	if (
		!Candidate.DropHandled && !GuardedFloatingWidget.isNull() && Candidate.Container && Candidate.ValidDropArea && Candidate.SinceLastUpdate.isValid() &&
		Candidate.SinceLastUpdate.elapsed() < PlatformDragDropCandidateTimeoutMs
	)
	{
		Candidate.Container->dropFloatingWidget(GuardedFloatingWidget, Candidate.GlobalPos);
		Docked = true;
	}
	s_PlatformDragDropCandidate = PlatformDragDropCandidate();
	DockManager->containerOverlay()->hideOverlay();
	DockManager->dockAreaOverlay()->hideOverlay();
	Drag->deleteLater();

	if (GuardedFloatingWidget)
	{
		GuardedFloatingWidget->d->setState(DraggingInactive);
	}

	return Docked ? Qt::MoveAction : Qt::IgnoreAction;
}

bool CFloatingDockContainer::waylandMoveOrLeaveInWindowPreview(
	CFloatingDragPreview* Preview, QWidget* SourceWindow, const QPoint& GlobalPos, const bool Floatable
)
{
	const QPoint InWindow = SourceWindow->mapFromGlobal(GlobalPos);
	if (SourceWindow->rect().contains(InWindow))
	{
		if (Preview)
		{
			Preview->moveFloating(GlobalPos);
		}
		return true;
	}

	// Boundary cross: tear down the in-window preview without performing a
	// drop. The caller converts the gesture into a native compositor platform
	// drag while the implicit pointer grab from the original press is still
	// active, which is what QDrag::exec() / xdg_toplevel_drag_v1 requires.
	if (Preview && Floatable)
	{
		Preview->cancelDraggingSilently();

		return false;
	}
	return true;
}

Qt::DropAction CFloatingDockContainer::startPlatformDragForFloatingWidget(
	IFloatingWidget* FloatingWidget, const QPoint& GrabOffset, const QSize& Size, const QPoint& GlobalGrabPos, QWidget* DragSource
)
{
	FloatingWidget->startFloating(GrabOffset, Size, DraggingInactive, nullptr);
	// Keep the grab point under the cursor across the conversion. A client
	// cannot position a Wayland top-level and the new window has no usable
	// geometry yet, so pass the surface-local grab offset explicitly rather
	// than letting startPlatformDrag() derive it from the press via
	// mapFromGlobal() (which would be random on Wayland). We must NOT move()
	// the window here either: besides being ignored by the compositor, a
	// geometry change between show() and exec() risks mapping the surface
	// before the attach, which would discard the offset.
	QPoint Offset = GrabOffset;
	return startPlatformDrag(static_cast<CFloatingDockContainer*>(FloatingWidget), GlobalGrabPos, DragSource, &Offset);
}

CFloatingDockContainer* CFloatingDockContainer::floatingWidgetFromMimeData(const QMimeData* MimeData)
{
	if (!MimeData || !MimeData->hasFormat(QLatin1String(FloatingWidgetMimeType)))
	{
		return nullptr;
	}

	qintptr FloatingWidgetPtr = 0;
	QDataStream DataStream(MimeData->data(QLatin1String(FloatingWidgetMimeType)));
	DataStream >> FloatingWidgetPtr;
	return reinterpret_cast<CFloatingDockContainer*>(FloatingWidgetPtr);
}

#ifdef Q_OS_MACOS
bool CFloatingDockContainer::event(QEvent* e)
{
	switch (d->DraggingState)
	{
		case DraggingInactive:
		{
			// Normally we would check here, if the left mouse button is pressed.
			// But from QT version 5.12.2 on the mouse events from
			// QEvent::NonClientAreaMouseButtonPress return the wrong mouse button
			// The event always returns Qt::RightButton even if the left button
			// is clicked.
			// It is really great to work around the whole NonClientMouseArea
			// bugs
	#if (QT_VERSION >= QT_VERSION_CHECK(5, 12, 2))
			if (e->type() == QEvent::NonClientAreaMouseButtonPress /*&& QGuiApplication::mouseButtons().testFlag(Qt::LeftButton)*/)
	#else
			if (e->type() == QEvent::NonClientAreaMouseButtonPress && QGuiApplication::mouseButtons().testFlag(Qt::LeftButton))
	#endif
			{
				ADS_PRINT("FloatingWidget::event Event::NonClientAreaMouseButtonPress" << e->type());
				d->DragStartPos = pos();
				d->setState(DraggingMousePressed);
			}
		}
		break;

		case DraggingMousePressed:
			switch (e->type())
			{
				case QEvent::NonClientAreaMouseButtonDblClick:
					ADS_PRINT("FloatingWidget::event QEvent::NonClientAreaMouseButtonDblClick");
					d->setState(DraggingInactive);
					break;

				case QEvent::Resize:
					// If the first event after the mouse press is a resize event, then
					// the user resizes the window instead of dragging it around.
					// But there is one exception. If the window is maximized,
					// then dragging the window via title bar will cause the widget to
					// leave the maximized state. This in turn will trigger a resize
					// event. To know, if the resize event was triggered by user via
					// moving a corner of the window frame or if it was caused by a
					// windows state change, we check, if we are not in maximized state.
					if (!isMaximized())
					{
						d->setState(DraggingInactive);
					}
					break;

				default:
					break;
			}
			break;

		case DraggingFloatingWidget:
			if (e->type() == QEvent::NonClientAreaMouseButtonRelease)
			{
				ADS_PRINT("FloatingWidget::event QEvent::NonClientAreaMouseButtonRelease");
				d->titleMouseReleaseEvent();
			}
			break;

		default:
			break;
	}

	#if (ADS_DEBUG_LEVEL > 0)
	qDebug() << QTime::currentTime() << "CFloatingDockContainer::event " << e->type();
	#endif
	return QWidget::event(e);
}

void CFloatingDockContainer::moveEvent(QMoveEvent* event)
{
	QWidget::moveEvent(event);
	switch (d->DraggingState)
	{
		case DraggingMousePressed:
			d->setState(DraggingFloatingWidget);
			d->updateDropOverlays(QCursor::pos());
			break;

		case DraggingFloatingWidget:
			d->updateDropOverlays(QCursor::pos());
			// In OSX when hiding the DockAreaOverlay the application would set
			// the main window as the active window for some reason. This fixes
			// that by resetting the active window to the floating widget after
			// updating the overlays.
			activateWindow();
			break;
		default:
			break;
	}
}
#endif

#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
void CFloatingDockContainer::onMaximizeRequest()
{
	if (windowState() == Qt::WindowMaximized)
	{
		showNormal();
	}
	else
	{
		showMaximized();
	}
}

void CFloatingDockContainer::showNormal(bool fixGeometry)
{
	if ((windowState() & Qt::WindowMaximized) != 0 || (windowState() & Qt::WindowFullScreen) != 0)
	{
		QRect oldNormal = normalGeometry();
		Super::showNormal();
		if (fixGeometry)
		{
			setGeometry(oldNormal);
		}
	}
	if (d->TitleBar)
	{
		d->TitleBar->setMaximizedIcon(false);
	}
}

void CFloatingDockContainer::showMaximized()
{
	Super::showMaximized();
	if (d->TitleBar)
	{
		d->TitleBar->setMaximizedIcon(true);
	}
}

bool CFloatingDockContainer::isMaximized() const
{
	return windowState() == Qt::WindowMaximized;
}
#endif

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
void CFloatingDockContainer::show()
{
	// Prevent this window from showing in the taskbar and pager (alt+tab)
	internal::xcb_add_prop(true, winId(), "_NET_WM_STATE", "_NET_WM_STATE_SKIP_TASKBAR");
	internal::xcb_add_prop(true, winId(), "_NET_WM_STATE", "_NET_WM_STATE_SKIP_PAGER");
	Super::show();
}

void CFloatingDockContainer::resizeEvent(QResizeEvent* event)
{
	d->IsResizing = true;
	Super::resizeEvent(event);
}

void CFloatingDockContainer::moveEvent(QMoveEvent* event)
{
	Super::moveEvent(event);
	// On Wayland, the global cursor position is not available and the
	// compositor does not report window moves, so the docking via window
	// moves cannot work. Floating widgets are docked via the platform drag
	// and drop implemented in CDockContainerWidget instead
	if (!d->IsResizing && event->spontaneous() && d->MousePressed && !internal::isWayland())
	{
		d->setState(DraggingFloatingWidget);
		d->updateDropOverlays(QCursor::pos());
	}
	d->IsResizing = false;
}

bool CFloatingDockContainer::event(QEvent* e)
{
	bool result = Super::event(e);
	switch (e->type())
	{
		case QEvent::WindowActivate:
			d->MousePressed = false;
			break;
		case QEvent::WindowDeactivate:
			d->MousePressed = true;
			break;
		default:
			break;
	}
	return result;
}

// Q_OS_UNIX && !Q_OS_MACOS
#endif

#ifdef Q_OS_WIN
void CFloatingDockContainer::resizeEvent(QResizeEvent* event)
{
	if (!hasNativeTitleBar())
	{
		d->IsResizing = true;
	}
	Super::resizeEvent(event);
	if (!hasNativeTitleBar())
	{
		d->IsResizing = false;
	}
}

void CFloatingDockContainer::moveEvent(QMoveEvent* event)
{
	Super::moveEvent(event);
	// Only handle drag detection for custom title bar (Wine mode).
	// Native Windows uses nativeEvent() for drag detection.
	if (!hasNativeTitleBar() && !d->IsResizing && event->spontaneous() && d->MousePressed)
	{
		d->setState(DraggingFloatingWidget);
		d->updateDropOverlays(QCursor::pos());
	}
}

bool CFloatingDockContainer::event(QEvent* e)
{
	bool result = Super::event(e);
	// Only track mouse press state for custom title bar (Wine mode).
	// Native Windows uses nativeEvent() for drag detection.
	// DeferredDelete may destroy this in Super::event(), so don't access d for
	// other events.
	switch (e->type())
	{
		case QEvent::WindowActivate:
			if (!hasNativeTitleBar())
			{
				d->MousePressed = false;
			}
			break;
		case QEvent::WindowDeactivate:
			if (!hasNativeTitleBar())
			{
				d->MousePressed = true;
			}
			break;
		default:
			break;
	}
	return result;
}

// Q_OS_WIN
#endif

#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
bool CFloatingDockContainer::hasNativeTitleBar()
{
	return d->TitleBar == nullptr;
}

int CFloatingDockContainer::gripWidth() const
{
	return d->_gripWidth;
}

// Edge/corner resize support (Linux and Windows under Wine)
namespace
{
struct EventHelper
{
		explicit EventHelper(QEvent* event)
			: _mouseEvent(dynamic_cast<QMouseEvent*>(event))
		{}

		QMouseEvent* _mouseEvent;

		[[nodiscard]] QPointF position() const
		{
	#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
			return _mouseEvent->position();
	#else
			return _mouseEvent->localPos();
	#endif
		}

		[[nodiscard]] QPointF globalPosition() const
		{
	#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
			return _mouseEvent->globalPosition();
	#else
			return _mouseEvent->screenPos();
	#endif
		}
};
}// namespace

Qt::Edges CFloatingDockContainer::edgesAt(const QPointF& pos) const
{
	Qt::Edges edges;
	if (pos.x() <= d->_gripWidth)
		edges |= Qt::LeftEdge;
	if (pos.x() >= width() - d->_gripWidth)
		edges |= Qt::RightEdge;
	if (pos.y() <= d->_gripWidth)
		edges |= Qt::TopEdge;
	if (pos.y() >= height() - d->_gripWidth)
		edges |= Qt::BottomEdge;
	return edges;
}

void CFloatingDockContainer::updateCursorFor(Qt::Edges edges)
{
	constexpr Qt::Edges tl = Qt::LeftEdge | Qt::TopEdge;
	constexpr Qt::Edges br = Qt::RightEdge | Qt::BottomEdge;
	constexpr Qt::Edges tr = Qt::RightEdge | Qt::TopEdge;
	constexpr Qt::Edges bl = Qt::LeftEdge | Qt::BottomEdge;

	Qt::CursorShape targetShape = Qt::ArrowCursor;
	if (edges == tl || edges == br)
		targetShape = Qt::SizeFDiagCursor;
	else if (edges == tr || edges == bl)
		targetShape = Qt::SizeBDiagCursor;
	else if (edges & (Qt::LeftEdge | Qt::RightEdge))
		targetShape = Qt::SizeHorCursor;
	else if (edges & (Qt::TopEdge | Qt::BottomEdge))
		targetShape = Qt::SizeVerCursor;
	else
	{
		if (d->_edgeCursorActive)
		{
			QGuiApplication::restoreOverrideCursor();
			d->_edgeCursorActive = false;
		}
		return;
	}

	if (d->_edgeCursorActive)
		QGuiApplication::changeOverrideCursor(QCursor(targetShape));
	else
	{
		QGuiApplication::setOverrideCursor(QCursor(targetShape));
		d->_edgeCursorActive = true;
	}
}

void CFloatingDockContainer::performManualResize(const QPointF& globalPos)
{
	const QPointF d_pos = globalPos - d->_pressGlobalPos;
	QRect g = d->_startGeometry;

	if (d->_resizeEdges & Qt::LeftEdge)
		g.setLeft(g.left() + static_cast<int>(d_pos.x()));
	if (d->_resizeEdges & Qt::RightEdge)
		g.setRight(g.right() + static_cast<int>(d_pos.x()));
	if (d->_resizeEdges & Qt::TopEdge)
		g.setTop(g.top() + static_cast<int>(d_pos.y()));
	if (d->_resizeEdges & Qt::BottomEdge)
		g.setBottom(g.bottom() + static_cast<int>(d_pos.y()));

	// Enforce the minimum size while keeping the opposite edge anchored.
	if (g.width() < minimumWidth())
	{
		if (d->_resizeEdges & Qt::LeftEdge)
			g.setLeft(g.right() - minimumWidth() + 1);
		else
			g.setRight(g.left() + minimumWidth() - 1);
	}
	if (g.height() < minimumHeight())
	{
		if (d->_resizeEdges & Qt::TopEdge)
			g.setTop(g.bottom() - minimumHeight() + 1);
		else
			g.setBottom(g.top() + minimumHeight() - 1);
	}

	setGeometry(g);
}

void CFloatingDockContainer::mousePressEvent(QMouseEvent* event)
{
	if (hasNativeTitleBar())
	{
		Super::mousePressEvent(event);
		return;
	}
	if (event->button() != Qt::LeftButton)
	{
		Super::mousePressEvent(event);
		return;
	}
	QWindow* wh = windowHandle();
	if (const Qt::Edges edges = edgesAt(EventHelper(event).position()))
	{
	#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
		// Preferred: let the window manager / compositor do the resize.
		if (wh && wh->startSystemResize(edges))
			return;
	#endif
		// Fallback: resize manually.
		d->_manualResizing = true;
		d->_resizeEdges = edges;
		d->_pressGlobalPos = EventHelper(event).globalPosition();
		d->_startGeometry = geometry();
		return;
	}
	// Let the base class handle title bar dragging and other interactions.
	Super::mousePressEvent(event);
}

void CFloatingDockContainer::mouseMoveEvent(QMouseEvent* event)
{
	if (hasNativeTitleBar())
	{
		Super::mouseMoveEvent(event);
		return;
	}
	if (d->_manualResizing)
	{
		performManualResize(EventHelper(event).globalPosition());
		return;
	}

	updateCursorFor(edgesAt(EventHelper(event).position()));
	Super::mouseMoveEvent(event);
}

void CFloatingDockContainer::mouseReleaseEvent(QMouseEvent* event)
{
	if (hasNativeTitleBar())
	{
		Super::mouseReleaseEvent(event);
		return;
	}
	d->_manualResizing = false;
	Super::mouseReleaseEvent(event);
}

void CFloatingDockContainer::leaveEvent(QEvent* event)
{
	if (hasNativeTitleBar())
	{
		Super::leaveEvent(event);
		return;
	}
	if (!d->_manualResizing)
		updateCursorFor(Qt::Edges());
	Super::leaveEvent(event);
}

bool CFloatingDockContainer::eventFilter(QObject* watched, QEvent* event)
{
	if (hasNativeTitleBar())
	{
		return Super::eventFilter(watched, event);
	}
	const QEvent::Type type = event->type();
	if (type != QEvent::MouseMove && type != QEvent::HoverMove && type != QEvent::Enter && type != QEvent::Leave && type != QEvent::MouseButtonPress)
	{
		return Super::eventFilter(watched, event);
	}

	QWidget* w = qobject_cast<QWidget*>(watched);
	if (!w || !isVisible() || d->_manualResizing || QApplication::mouseButtons() != Qt::NoButton)
	{
		if (type != QEvent::MouseButtonPress || !w)
			return Super::eventFilter(watched, event);
	}
	if (w != this && !isAncestorOf(w))
	{
		return Super::eventFilter(watched, event);
	}

	const QPoint globalPos = QCursor::pos();
	const Qt::Edges edges = edgesAt(mapFromGlobal(globalPos));

	if (type == QEvent::MouseButtonPress)
	{
		QMouseEvent* me = static_cast<QMouseEvent*>(event);
		if (me->button() == Qt::LeftButton && edges && !d->_manualResizing)
		{
			updateCursorFor(Qt::Edges());
			QWindow* wh = windowHandle();
	#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
			if (wh && wh->startSystemResize(edges))
				return true;
	#endif
			d->_manualResizing = true;
			d->_resizeEdges = edges;
			d->_pressGlobalPos = EventHelper(me).globalPosition();
			d->_startGeometry = geometry();
			return true;
		}
		return Super::eventFilter(watched, event);
	}

	if (d->_manualResizing || QApplication::mouseButtons() != Qt::NoButton)
		return Super::eventFilter(watched, event);

	updateCursorFor(type == QEvent::Leave ? Qt::Edges() : edges);
	return Super::eventFilter(watched, event);
}

#endif

}// namespace ads

// EOF CFloatingDockContainer.cpp

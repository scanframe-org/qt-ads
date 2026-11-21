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

/// @file   CDockAreaWidget.cpp
/// @author Uwe Kindler
/// @date   24.02.2017
/// @brief  Implementation of CDockAreaWidget class

#include "CDockAreaWidget.h"

#include <QDebug>
#include <QList>
#include <QMenu>
#include <QPushButton>
#include <QScrollBar>
#include <QStackedLayout>
#include <QStyle>
#include <QXmlStreamWriter>

#include <CAutoHideDockContainer.h>
#include <CAutoHideTab.h>

#include "CDockAreaTabBar.h"
#include "CDockAreaTitleBar.h"
#include "CDockComponentsFactory.h"
#include "CDockContainerWidget.h"
#include "CDockManager.h"
#include "CDockOverlay.h"
#include "CDockSplitter.h"
#include "CDockWidget.h"
#include "CDockWidgetTab.h"
#include "CDockingStateReader.h"
#include "CElidingLabel.h"
#include "CFloatingDockContainer.h"

#include "CDockAreaWidget_p.h"

namespace ads
{

DockAreaWidgetPrivate::DockAreaWidgetPrivate(CDockAreaWidget* _public)
	: _this(_public)
{}

void DockAreaWidgetPrivate::createTitleBar()
{
	TitleBar = componentsFactory()->createDockAreaTitleBar(_this);
	Layout->addWidget(TitleBar);
	if (CDockManager::testConfigFlag(CDockManager::TabsAtBottom))
	{
		// Title bar will be index 0, container widgets will be index 1,
		// so tabs will always be at the end of the layout.
		Layout->addWidget(tabBar());
		tabBar()->setVisible(CDockManager::testConfigFlag(CDockManager::AlwaysShowTabs));
	}

	QObject::connect(tabBar(), &CDockAreaTabBar::tabCloseRequested, _this, &CDockAreaWidget::onTabCloseRequested);
	QObject::connect(TitleBar, &CDockAreaTitleBar::tabBarClicked, _this, &CDockAreaWidget::setCurrentIndex);
	QObject::connect(tabBar(), &CDockAreaTabBar::tabMoved, _this, &CDockAreaWidget::reorderDockWidget);
}

void DockAreaWidgetPrivate::updateTitleBarButtonStates()
{
	if (_this->isHidden())
	{
		UpdateTitleBarButtons = true;
		return;
	}

	if (_this->isAutoHide())
	{
		if (CDockManager::testAutoHideConfigFlag(CDockManager::AutoHideHasCloseButton))
		{
			TitleBar->button(TitleBarButtonClose)->setEnabled(_this->features().testFlag(CDockWidget::DockWidgetClosable));
		}
	}
	else
	{
		TitleBar->button(TitleBarButtonUndock)->setEnabled(_this->features().testFlag(CDockWidget::DockWidgetFloatable));
		TitleBar->button(TitleBarButtonClose)->setEnabled(_this->features().testFlag(CDockWidget::DockWidgetClosable));
	}
	TitleBar->button(TitleBarButtonAutoHide)->setEnabled(_this->features().testFlag(CDockWidget::DockWidgetPinnable));
	TitleBar->updateDockWidgetActionsButtons();
	UpdateTitleBarButtons = false;
}

void DockAreaWidgetPrivate::updateTitleBarButtonVisibility(bool IsTopLevel)
{
	auto* const container = _this->dockContainer();
	if (!container)
	{
		return;
	}

	bool IsAutoHide = _this->isAutoHide();
	if (IsAutoHide)
	{
		bool ShowCloseButton = CDockManager::testAutoHideConfigFlag(CDockManager::AutoHideHasCloseButton);
		TitleBar->button(TitleBarButtonClose)->setVisible(ShowCloseButton);
		TitleBar->button(TitleBarButtonAutoHide)->setVisible(true);
		TitleBar->button(TitleBarButtonUndock)->setVisible(false);
		TitleBar->button(TitleBarButtonTabsMenu)->setVisible(false);
	}
	else if (IsTopLevel)
	{
		TitleBar->button(TitleBarButtonClose)->setVisible(!container->isFloating());
		TitleBar->button(TitleBarButtonAutoHide)->setVisible(!container->isFloating());
		// Undock and tabs should never show when auto hidden
		TitleBar->button(TitleBarButtonUndock)->setVisible(!container->isFloating());
		TitleBar->button(TitleBarButtonTabsMenu)->setVisible(true);
	}
	else
	{
		TitleBar->button(TitleBarButtonClose)->setVisible(true);
		bool ShowAutoHideButton = CDockManager::testAutoHideConfigFlag(CDockManager::DockAreaHasAutoHideButton);
		TitleBar->button(TitleBarButtonAutoHide)->setVisible(ShowAutoHideButton);
		TitleBar->button(TitleBarButtonUndock)->setVisible(true);
		TitleBar->button(TitleBarButtonTabsMenu)->setVisible(true);
	}
}

CDockAreaWidget::CDockAreaWidget(CDockManager* DockManager, CDockContainerWidget* parent)
	: QFrame(parent)
	, d(new DockAreaWidgetPrivate(this))
{
	d->DockManager = DockManager;
	d->Layout = new QBoxLayout(QBoxLayout::TopToBottom);
	d->Layout->setContentsMargins(0, 0, 0, 0);
	d->Layout->setSpacing(0);
	setLayout(d->Layout);

	d->createTitleBar();
	d->ContentsLayout = new DockAreaLayout(d->Layout);

	if (CDockManager::testConfigFlag(CDockManager::UseNativeWindows))
	{
		winId();
	}

	if (d->DockManager)
	{
		Q_EMIT d->DockManager->dockAreaCreated(this);
	}
}

CDockAreaWidget::~CDockAreaWidget()
{
	ADS_PRINT("~CDockAreaWidget()");
	delete d->ContentsLayout;
	delete d;
}

CDockManager* CDockAreaWidget::dockManager() const
{
	return d->DockManager;
}

CDockContainerWidget* CDockAreaWidget::dockContainer() const
{
	return internal::findParent<CDockContainerWidget*>(this);
}

CAutoHideDockContainer* CDockAreaWidget::autoHideDockContainer() const
{
	return d->AutoHideDockContainer;
}

CDockSplitter* CDockAreaWidget::parentSplitter() const
{
	return internal::findParent<CDockSplitter*>(this);
}

bool CDockAreaWidget::isAutoHide() const
{
	return d->AutoHideDockContainer != nullptr;
}

void CDockAreaWidget::setAutoHideDockContainer(CAutoHideDockContainer* AutoHideDockContainer)
{
	d->AutoHideDockContainer = AutoHideDockContainer;
	updateAutoHideButtonCheckState();
	updateTitleBarButtonsToolTips();
	d->TitleBar->button(TitleBarButtonAutoHide)->setShowInTitleBar(true);
}

void CDockAreaWidget::addDockWidget(CDockWidget* DockWidget)
{
	insertDockWidget(d->ContentsLayout->count(), DockWidget);
}

void CDockAreaWidget::insertDockWidget(int index, CDockWidget* DockWidget, bool Activate)
{
	if (index < 0 || index > d->ContentsLayout->count())
	{
		index = d->ContentsLayout->count();
	}
	d->ContentsLayout->insertWidget(index, DockWidget);
	DockWidget->setDockArea(this);
	DockWidget->tabWidget()->setDockAreaWidget(this);
	auto TabWidget = DockWidget->tabWidget();
	// Inserting the tab will change the current index which in turn will
	// make the tab widget visible in the slot
	d->tabBar()->blockSignals(true);
	d->tabBar()->insertTab(index, TabWidget);
	d->tabBar()->blockSignals(false);
	TabWidget->setVisible(!DockWidget->isClosed());
	d->TitleBar->autoHideTitleLabel()->setText(DockWidget->windowTitle());
	DockWidget->setProperty(INDEX_PROPERTY, index);
	d->MinSizeHint.setHeight(qMax(d->MinSizeHint.height(), DockWidget->minimumSizeHint().height()));
	d->MinSizeHint.setWidth(qMax(d->MinSizeHint.width(), DockWidget->minimumSizeHint().width()));
	if (Activate)
	{
		setCurrentIndex(index);
		// Set current index can show the
		// widget without changing the close
		// state, added to keep the close
		// state consistent
		DockWidget->setClosedState(false);
		if (!DockWidget->isClosed())
		{
			TabWidget->setVisible(true);
		}
	}
	// If this dock area is hidden, then we need to make it visible again
	// by calling DockWidget->toggleViewInternal(true);
	if (!this->isVisible() && d->ContentsLayout->count() > 1 && !dockManager()->isRestoringState())
	{
		DockWidget->toggleViewInternal(true);
	}
	d->updateTitleBarButtonStates();
	updateTitleBarVisibility();
}

void CDockAreaWidget::removeDockWidget(CDockWidget* DockWidget)
{
	ADS_PRINT("CDockAreaWidget::removeDockWidget");
	if (!DockWidget)
	{
		return;
	}

	// If this dock area is in a auto hide container, then we can delete
	// the auto hide container now
	if (isAutoHide())
	{
		autoHideDockContainer()->cleanupAndDelete();
		return;
	}

	auto CurrentDockWidget = currentDockWidget();
	auto NextOpenDockWidget = (DockWidget == CurrentDockWidget) ? nextOpenDockWidget(DockWidget) : nullptr;

	d->ContentsLayout->removeWidget(DockWidget);
	auto TabWidget = DockWidget->tabWidget();
	TabWidget->hide();
	d->tabBar()->removeTab(TabWidget);
	TabWidget->setParent(DockWidget);
	DockWidget->setDockArea(nullptr);
	CDockContainerWidget* DockContainer = dockContainer();
	if (NextOpenDockWidget)
	{
		setCurrentDockWidget(NextOpenDockWidget);
	}
	else if (d->ContentsLayout->isEmpty() && DockContainer->dockAreaCount() >= 1)
	{
		ADS_PRINT("Dock Area empty");
		DockContainer->removeDockArea(this);
		this->deleteLater();
		if (DockContainer->dockAreaCount() == 0)
		{
			if (CFloatingDockContainer* FloatingDockContainer = DockContainer->floatingWidget())
			{
				FloatingDockContainer->finishDropOperation();
			}
		}
	}
	else if (DockWidget == CurrentDockWidget)
	{
		// if contents layout is not empty but there are no more open dock
		// widgets, then we need to hide the dock area because it does not
		// contain any visible content
		hideAreaWithNoVisibleContent();
	}

	d->updateTitleBarButtonStates();
	updateTitleBarVisibility();
	d->updateMinimumSizeHint();
	auto TopLevelDockWidget = DockContainer->topLevelDockWidget();
	if (TopLevelDockWidget)
	{
		TopLevelDockWidget->emitTopLevelChanged(true);
	}

#if (ADS_DEBUG_LEVEL > 0)
	DockContainer->dumpLayout();
#endif
}

void CDockAreaWidget::hideAreaWithNoVisibleContent()
{
	this->toggleView(false);

	// Hide empty parent splitters
	auto Splitter = parentSplitter();
	internal::hideEmptyParentSplitters(Splitter);

	// Hide empty floating widget
	CDockContainerWidget* Container = this->dockContainer();
	if (!Container->isFloating() && !CDockManager::testConfigFlag(CDockManager::HideSingleCentralWidgetTitleBar))
	{
		return;
	}

	updateTitleBarVisibility();
	auto TopLevelWidget = Container->topLevelDockWidget();
	auto FloatingWidget = Container->floatingWidget();
	if (TopLevelWidget)
	{
		if (FloatingWidget)
		{
			FloatingWidget->updateWindowTitle();
		}
		CDockWidget::emitTopLevelEventForWidget(TopLevelWidget, true);
	}
	else if (Container->openedDockAreas().isEmpty() && FloatingWidget)
	{
		FloatingWidget->hide();
	}
	if (isAutoHide())
	{
		autoHideDockContainer()->hide();
	}
}

void CDockAreaWidget::onTabCloseRequested(int Index)
{
	ADS_PRINT("CDockAreaWidget::onTabCloseRequested " << Index);
	dockWidget(Index)->requestCloseDockWidget();
}

CDockWidget* CDockAreaWidget::currentDockWidget() const
{
	int CurrentIndex = currentIndex();
	if (CurrentIndex < 0)
	{
		return nullptr;
	}

	return dockWidget(CurrentIndex);
}

void CDockAreaWidget::setCurrentDockWidget(CDockWidget* DockWidget)
{
	if (dockManager()->isRestoringState())
	{
		return;
	}

	internalSetCurrentDockWidget(DockWidget);
}

void CDockAreaWidget::internalSetCurrentDockWidget(CDockWidget* DockWidget)
{
	int Index = index(DockWidget);
	if (Index < 0)
	{
		return;
	}

	setCurrentIndex(Index);
	// Set current index can show the widget
	// without changing the close state, added
	// to keep the close state consistent
	DockWidget->setClosedState(false);
}

void CDockAreaWidget::setCurrentIndex(int index)
{
	auto TabBar = d->tabBar();
	if (index < 0 || index > (TabBar->count() - 1))
	{
		qWarning() << Q_FUNC_INFO << "Invalid index" << index;
		return;
	}

	auto cw = d->ContentsLayout->currentWidget();
	auto nw = d->ContentsLayout->widget(index);
	if (cw == nw && !nw->isHidden())
	{
		return;
	}

	Q_EMIT currentChanging(index);
	TabBar->setCurrentIndex(index);
	d->ContentsLayout->setCurrentIndex(index);
	d->ContentsLayout->currentWidget()->show();
	d->TitleBar->autoHideTitleLabel()->setText(d->ContentsLayout->currentWidget()->windowTitle());
	Q_EMIT currentChanged(index);
}

int CDockAreaWidget::currentIndex() const
{
	return d->ContentsLayout->currentIndex();
}

QRect CDockAreaWidget::titleBarGeometry() const
{
	return d->TitleBar->geometry();
}

QRect CDockAreaWidget::contentAreaGeometry() const
{
	return d->ContentsLayout->geometry();
}

int CDockAreaWidget::index(CDockWidget* DockWidget)
{
	return d->ContentsLayout->indexOf(DockWidget);
}

QList<CDockWidget*> CDockAreaWidget::dockWidgets() const
{
	QList<CDockWidget*> DockWidgetList;
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		DockWidgetList.append(dockWidget(i));
	}
	return DockWidgetList;
}

int CDockAreaWidget::openDockWidgetsCount() const
{
	int Count = 0;
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		if (dockWidget(i) && !dockWidget(i)->isClosed())
		{
			++Count;
		}
	}
	return Count;
}

QList<CDockWidget*> CDockAreaWidget::openedDockWidgets() const
{
	QList<CDockWidget*> DockWidgetList;
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		CDockWidget* DockWidget = dockWidget(i);
		if (DockWidget && !DockWidget->isClosed())
		{
			DockWidgetList.append(dockWidget(i));
		}
	}
	return DockWidgetList;
}

int CDockAreaWidget::indexOfFirstOpenDockWidget() const
{
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		if (dockWidget(i) && !dockWidget(i)->isClosed())
		{
			return i;
		}
	}

	return -1;
}

int CDockAreaWidget::dockWidgetsCount() const
{
	return d->ContentsLayout->count();
}

CDockWidget* CDockAreaWidget::dockWidget(int Index) const
{
	return qobject_cast<CDockWidget*>(d->ContentsLayout->widget(Index));
}

void CDockAreaWidget::reorderDockWidget(int fromIndex, int toIndex)
{
	ADS_PRINT("CDockAreaWidget::reorderDockWidget");
	if (fromIndex >= d->ContentsLayout->count() || fromIndex < 0 || toIndex >= d->ContentsLayout->count() || toIndex < 0 || fromIndex == toIndex)
	{
		ADS_PRINT("Invalid index for tab movement" << fromIndex << toIndex);
		return;
	}

	auto Widget = d->ContentsLayout->widget(fromIndex);
	d->ContentsLayout->removeWidget(Widget);
	d->ContentsLayout->insertWidget(toIndex, Widget);
	setCurrentIndex(toIndex);
}

void CDockAreaWidget::toggleDockWidgetView(CDockWidget* DockWidget, bool Open)
{
	Q_UNUSED(DockWidget);
	Q_UNUSED(Open);
	updateTitleBarVisibility();
}

void CDockAreaWidget::updateTitleBarVisibility()
{
	CDockContainerWidget* Container = dockContainer();
	if (!Container)
	{
		return;
	}

	if (!d->TitleBar)
	{
		return;
	}

	bool IsAutoHide = isAutoHide();
	if (!CDockManager::testConfigFlag(CDockManager::AlwaysShowTabs))
	{
		bool Hidden = false;
		// Wayland: the title bar of a floating container must always stay
		// visible because it is the only handle that can drag the dock
		// widget back into a dock container. The window decoration is owned
		// by the compositor and only moves the window
		bool IsWaylandFloating = internal::isWayland() && Container->isFloating();
		// Titlebar must always be visible
		// when auto hidden so it can be
		// dragged
		if (!IsAutoHide && !IsWaylandFloating)
		{
			if (Container->isFloating() || CDockManager::testConfigFlag(CDockManager::HideSingleCentralWidgetTitleBar))
			{
				// Always show title bar if it contains title bar actions
				if (CDockWidget* TopLevelWidget = Container->topLevelDockWidget())
				{
					Hidden |= TopLevelWidget->titleBarActions().empty();
				}
				else if (CDockManager::testConfigFlag(CDockManager::TabsAtBottom))
				{
					Hidden = true;

					for (CDockWidget* DockWidget: Container->openedDockWidgets())
					{
						if (!DockWidget->titleBarActions().empty())
						{
							Hidden = false;
							break;
						}
					}
				}
			}
			if (!Hidden && d->Flags.testFlag(HideSingleWidgetTitleBar))
			{
				// Always show title bar if it contains title bar actions
				auto DockWidgets = openedDockWidgets();
				Hidden |= (DockWidgets.size() == 1) && DockWidgets.front()->titleBarActions().empty();
			}
		}
		d->TitleBar->setVisible(!Hidden);
		if (CDockManager::testConfigFlag(CDockManager::TabsAtBottom))
			d->TitleBar->tabBar()->setVisible(openDockWidgetsCount() > 1);
	}

	if (isAutoHideFeatureEnabled())
	{
		d->TitleBar->showAutoHideControls(IsAutoHide);
		updateTitleBarButtonVisibility(Container->topLevelDockArea() == this);
	}
}

void CDockAreaWidget::updateWindowTitle()
{
	auto currentWidget = d->ContentsLayout->currentWidget();
	if (d->TitleBar && currentWidget)
	{
		d->TitleBar->autoHideTitleLabel()->setText(currentWidget->windowTitle());
	}

	markTitleBarMenuOutdated();
}

void CDockAreaWidget::markTitleBarMenuOutdated()
{
	if (d->TitleBar)
	{
		d->TitleBar->markTabsMenuOutdated();
	}
}

void CDockAreaWidget::updateAutoHideButtonCheckState()
{
	auto autoHideButton = titleBarButton(TitleBarButtonAutoHide);
	autoHideButton->blockSignals(true);
	autoHideButton->setChecked(isAutoHide());
	autoHideButton->blockSignals(false);
}

void CDockAreaWidget::updateTitleBarButtonVisibility(bool IsTopLevel) const
{
	d->updateTitleBarButtonVisibility(IsTopLevel);
}

void CDockAreaWidget::updateTitleBarButtonsToolTips()
{
	internal::setToolTip(titleBarButton(TitleBarButtonClose), titleBar()->titleBarButtonToolTip(TitleBarButtonClose));
	internal::setToolTip(titleBarButton(TitleBarButtonAutoHide), titleBar()->titleBarButtonToolTip(TitleBarButtonAutoHide));
}

void CDockAreaWidget::saveState(QXmlStreamWriter& s) const
{
	s.writeStartElement("Area");
	s.writeAttribute("Tabs", QString::number(d->ContentsLayout->count()));
	auto CurrentDockWidget = currentDockWidget();
	QString Name = CurrentDockWidget ? CurrentDockWidget->objectName() : "";
	s.writeAttribute("Current", Name);

	if (d->AllowedAreas != DefaultAllowedAreas)
	{
		s.writeAttribute("AllowedAreas", QString::number(d->AllowedAreas, 16));
	}

	if (d->Flags != DefaultFlags)
	{
		s.writeAttribute("Flags", QString::number(d->Flags, 16));
	}
	ADS_PRINT("CDockAreaWidget::saveState TabCount: " << d->ContentsLayout->count() << " Current: " << Name);
	for (int i = 0; i < d->ContentsLayout->count(); ++i)
	{
		dockWidget(i)->saveState(s);
	}
	s.writeEndElement();
}

bool CDockAreaWidget::restoreState(CDockingStateReader& s, CDockAreaWidget*& CreatedWidget, bool Testing, CDockContainerWidget* Container)
{
	bool Ok;
#ifdef ADS_DEBUG_PRINT
	int Tabs = s.attributes().value("Tabs").toInt(&Ok);
	if (!Ok)
	{
		return false;
	}
#endif

	QString CurrentDockWidget = s.attributes().value("Current").toString();
	ADS_PRINT("Restore NodeDockArea Tabs: " << Tabs << " Current: " << CurrentDockWidget);

	auto DockManager = Container->dockManager();
	CDockAreaWidget* DockArea = nullptr;
	if (!Testing)
	{
		DockArea = new CDockAreaWidget(DockManager, Container);
		const auto AllowedAreasAttribute = s.attributes().value("AllowedAreas");
		if (!AllowedAreasAttribute.isEmpty())
		{
			DockArea->setAllowedAreas((DockWidgetArea) AllowedAreasAttribute.toInt(nullptr, 16));
		}

		const auto FlagsAttribute = s.attributes().value("Flags");
		if (!FlagsAttribute.isEmpty())
		{
			DockArea->setDockAreaFlags((CDockAreaWidget::DockAreaFlags) FlagsAttribute.toInt(nullptr, 16));
		}
	}

	while (s.readNextStartElement())
	{
		if (s.name() != QLatin1String("Widget"))
		{
			continue;
		}

		auto ObjectName = s.attributes().value("Name");
		if (ObjectName.isEmpty())
		{
			return false;
		}

		bool Closed = s.attributes().value("Closed").toInt(&Ok);
		if (!Ok)
		{
			return false;
		}

		s.skipCurrentElement();
		CDockWidget* DockWidget = DockManager->findDockWidget(ObjectName.toString());
		if (!DockWidget || Testing)
		{
			continue;
		}

		ADS_PRINT("Dock Widget found - parent " << DockWidget->parent());
		if (DockWidget->autoHideDockContainer())
		{
			DockWidget->autoHideDockContainer()->cleanupAndDelete();
		}

		// We hide the DockArea here to prevent the short display (the flashing)
		// of the dock areas during application startup
		DockArea->hide();
		DockArea->addDockWidget(DockWidget);
		DockWidget->setToggleViewActionChecked(!Closed);
		DockWidget->setClosedState(Closed);
		DockWidget->setProperty(internal::ClosedProperty, Closed);
		DockWidget->setProperty(internal::DirtyProperty, false);
	}

	if (Testing)
	{
		return true;
	}

	if (!DockArea->dockWidgetsCount())
	{
		delete DockArea;
		DockArea = nullptr;
	}
	else
	{
		DockArea->setProperty("currentDockWidget", CurrentDockWidget);
	}

	CreatedWidget = DockArea;
	return true;
}

CDockWidget* CDockAreaWidget::nextOpenDockWidget(CDockWidget* DockWidget) const
{
	auto OpenDockWidgets = openedDockWidgets();
	if (OpenDockWidgets.count() > 1 || (OpenDockWidgets.count() == 1 && OpenDockWidgets[0] != DockWidget))
	{
		if (OpenDockWidgets.last() == DockWidget)
		{
			CDockWidget* NextDockWidget = OpenDockWidgets[OpenDockWidgets.count() - 2];
			// search backwards for widget with tab
			for (int i = OpenDockWidgets.count() - 2; i >= 0; --i)
			{
				auto dw = OpenDockWidgets[i];
				if (!dw->features().testFlag(CDockWidget::NoTab))
				{
					return dw;
				}
			}

			// return widget without tab
			return NextDockWidget;
		}
		else
		{
			int IndexOfDockWidget = OpenDockWidgets.indexOf(DockWidget);
			CDockWidget* NextDockWidget = OpenDockWidgets[IndexOfDockWidget + 1];
			// search forwards for widget with tab
			for (int i = IndexOfDockWidget + 1; i < OpenDockWidgets.count(); ++i)
			{
				auto dw = OpenDockWidgets[i];
				if (!dw->features().testFlag(CDockWidget::NoTab))
				{
					return dw;
				}
			}

			// search backwards for widget with tab
			for (int i = IndexOfDockWidget - 1; i >= 0; --i)
			{
				auto dw = OpenDockWidgets[i];
				if (!dw->features().testFlag(CDockWidget::NoTab))
				{
					return dw;
				}
			}

			// return widget without tab
			return NextDockWidget;
		}
	}
	else
	{
		return nullptr;
	}
}

CDockWidget::DockWidgetFeatures CDockAreaWidget::features(eBitwiseOperator Mode) const
{
	if (BitwiseAnd == Mode)
	{
		CDockWidget::DockWidgetFeatures Features(CDockWidget::AllDockWidgetFeatures);
		for (const auto DockWidget: dockWidgets())
		{
			Features &= DockWidget->features();
		}
		return Features;
	}
	else
	{
		CDockWidget::DockWidgetFeatures Features(CDockWidget::NoDockWidgetFeatures);
		for (const auto DockWidget: dockWidgets())
		{
			Features |= DockWidget->features();
		}
		return Features;
	}
}

void CDockAreaWidget::toggleView(bool Open)
{
	setVisible(Open);

	Q_EMIT viewToggled(Open);
}

void CDockAreaWidget::setVisible(bool Visible)
{
	Super::setVisible(Visible);
	if (d->UpdateTitleBarButtons)
	{
		d->updateTitleBarButtonStates();
	}
}

void CDockAreaWidget::setAllowedAreas(DockWidgetAreas areas)
{
	d->AllowedAreas = areas;
}

DockWidgetAreas CDockAreaWidget::allowedAreas() const
{
	return d->AllowedAreas;
}

CDockAreaWidget::DockAreaFlags CDockAreaWidget::dockAreaFlags() const
{
	return d->Flags;
}

void CDockAreaWidget::setDockAreaFlags(DockAreaFlags Flags)
{
	auto ChangedFlags = d->Flags ^ Flags;
	d->Flags = Flags;
	if (ChangedFlags.testFlag(HideSingleWidgetTitleBar))
	{
		updateTitleBarVisibility();
	}
}

void CDockAreaWidget::setDockAreaFlag(eDockAreaFlag Flag, bool On)
{
	auto flags = dockAreaFlags();
	internal::setFlag(flags, Flag, On);
	setDockAreaFlags(flags);
}

QAbstractButton* CDockAreaWidget::titleBarButton(TitleBarButton which) const
{
	return d->TitleBar->button(which);
}

void CDockAreaWidget::closeArea()
{
	// If there is only one single dock widget and this widget has the
	// DeleteOnClose feature or CustomCloseHandling, then we delete the dock
	// widget now; in the case of CustomCloseHandling, the CDockWidget class will
	// emit its closeRequested signal and not actually delete unless the signal is
	// handled in a way that deletes it
	auto OpenDockWidgets = openedDockWidgets();
	if (
		OpenDockWidgets.count() == 1 &&
		(OpenDockWidgets[0]->features().testFlag(CDockWidget::DockWidgetDeleteOnClose) ||
		 OpenDockWidgets[0]->features().testFlag(CDockWidget::CustomCloseHandling)) &&
		!isAutoHide()
	)
	{
		OpenDockWidgets[0]->closeDockWidgetInternal();
	}
	else
	{
		for (auto DockWidget: openedDockWidgets())
		{
			if (
				(DockWidget->features().testFlag(CDockWidget::DockWidgetDeleteOnClose) && DockWidget->features().testFlag(CDockWidget::DockWidgetForceCloseWithArea)) ||
				DockWidget->features().testFlag(CDockWidget::CustomCloseHandling)
			)
			{
				DockWidget->closeDockWidgetInternal();
			}
			else if (DockWidget->features().testFlag(CDockWidget::DockWidgetDeleteOnClose) && isAutoHide())
			{
				DockWidget->closeDockWidgetInternal();
			}
			else
			{
				DockWidget->toggleView(false);
			}
		}
	}
}

enum eBorderLocation
{
	BorderNone = 0,
	BorderLeft = 0x01,
	BorderRight = 0x02,
	BorderTop = 0x04,
	BorderBottom = 0x08,
	BorderVertical = BorderLeft | BorderRight,
	BorderHorizontal = BorderTop | BorderBottom,
	BorderTopLeft = BorderTop | BorderLeft,
	BorderTopRight = BorderTop | BorderRight,
	BorderBottomLeft = BorderBottom | BorderLeft,
	BorderBottomRight = BorderBottom | BorderRight,
	BorderVerticalBottom = BorderVertical | BorderBottom,
	BorderVerticalTop = BorderVertical | BorderTop,
	BorderHorizontalLeft = BorderHorizontal | BorderLeft,
	BorderHorizontalRight = BorderHorizontal | BorderRight,
	BorderAll = BorderVertical | BorderHorizontal
};

SideBarLocation CDockAreaWidget::calculateSideTabBarArea() const
{
	auto Container = dockContainer();
	auto ContentRect = Container->contentRect();

	// contains all borders that are touched by the
	// dock ware
	int borders = BorderNone;
	auto DockAreaTopLeft = mapTo(Container, rect().topLeft());
	auto DockAreaRect = rect();
	DockAreaRect.moveTo(DockAreaTopLeft);
	const qreal aspectRatio = DockAreaRect.width() / (qMax(1, DockAreaRect.height()) * 1.0);
	const qreal sizeRatio = (qreal) ContentRect.width() / DockAreaRect.width();
	static const int MinBorderDistance = 16;
	bool HorizontalOrientation = (aspectRatio > 1.0) && (sizeRatio < 3.0);

	// measure border distances - a distance less than 16 px means we touch the
	// border
	int BorderDistance[4];

	int Distance = qAbs(ContentRect.topLeft().y() - DockAreaRect.topLeft().y());
	BorderDistance[SideBarLocation::SideBarTop] = (Distance < MinBorderDistance) ? 0 : Distance;
	if (!BorderDistance[SideBarLocation::SideBarTop])
	{
		borders |= BorderTop;
	}

	Distance = qAbs(ContentRect.bottomRight().y() - DockAreaRect.bottomRight().y());
	BorderDistance[SideBarLocation::SideBarBottom] = (Distance < MinBorderDistance) ? 0 : Distance;
	if (!BorderDistance[SideBarLocation::SideBarBottom])
	{
		borders |= BorderBottom;
	}

	Distance = qAbs(ContentRect.topLeft().x() - DockAreaRect.topLeft().x());
	BorderDistance[SideBarLocation::SideBarLeft] = (Distance < MinBorderDistance) ? 0 : Distance;
	if (!BorderDistance[SideBarLocation::SideBarLeft])
	{
		borders |= BorderLeft;
	}

	Distance = qAbs(ContentRect.bottomRight().x() - DockAreaRect.bottomRight().x());
	BorderDistance[SideBarLocation::SideBarRight] = (Distance < MinBorderDistance) ? 0 : Distance;
	if (!BorderDistance[SideBarLocation::SideBarRight])
	{
		borders |= BorderRight;
	}

	auto SideTab = SideBarLocation::SideBarRight;
	switch (borders)
	{
		// 1. It's touching all borders
		case BorderAll:
			SideTab = HorizontalOrientation ? SideBarLocation::SideBarBottom : SideBarLocation::SideBarRight;
			break;

		// 2. It's touching 3 borders
		case BorderVerticalBottom:
			SideTab = SideBarLocation::SideBarBottom;
			break;
		case BorderVerticalTop:
			SideTab = SideBarLocation::SideBarTop;
			break;
		case BorderHorizontalLeft:
			SideTab = SideBarLocation::SideBarLeft;
			break;
		case BorderHorizontalRight:
			SideTab = SideBarLocation::SideBarRight;
			break;

		// 3. It's touching horizontal or vertical borders
		case BorderVertical:
			SideTab = SideBarLocation::SideBarBottom;
			break;
		case BorderHorizontal:
			SideTab = SideBarLocation::SideBarRight;
			break;

		// 4. It's in a corner
		case BorderTopLeft:
			SideTab = HorizontalOrientation ? SideBarLocation::SideBarTop : SideBarLocation::SideBarLeft;
			break;
		case BorderTopRight:
			SideTab = HorizontalOrientation ? SideBarLocation::SideBarTop : SideBarLocation::SideBarRight;
			break;
		case BorderBottomLeft:
			SideTab = HorizontalOrientation ? SideBarLocation::SideBarBottom : SideBarLocation::SideBarLeft;
			break;
		case BorderBottomRight:
			SideTab = HorizontalOrientation ? SideBarLocation::SideBarBottom : SideBarLocation::SideBarRight;
			break;

		// 5. It's touching only one border
		case BorderLeft:
			SideTab = SideBarLocation::SideBarLeft;
			break;
		case BorderRight:
			SideTab = SideBarLocation::SideBarRight;
			break;
		case BorderTop:
			SideTab = SideBarLocation::SideBarTop;
			break;
		case BorderBottom:
			SideTab = SideBarLocation::SideBarBottom;
			break;
	}

	return SideTab;
}

void CDockAreaWidget::setAutoHide(bool Enable, SideBarLocation Location, int TabIndex)
{
	if (!isAutoHideFeatureEnabled())
	{
		return;
	}

	if (!Enable)
	{
		if (isAutoHide())
		{
			d->AutoHideDockContainer->moveContentsToParent();
		}
		return;
	}

	// If this is already an auto hide container, then move it to new location
	if (isAutoHide())
	{
		d->AutoHideDockContainer->moveToNewSideBarLocation(Location, TabIndex);
		return;
	}

	auto area = (SideBarNone == Location) ? calculateSideTabBarArea() : Location;
	for (const auto DockWidget: openedDockWidgets())
	{
		if (Enable == isAutoHide())
		{
			continue;
		}

		if (!DockWidget->features().testFlag(CDockWidget::DockWidgetPinnable))
		{
			continue;
		}

		dockContainer()->createAndSetupAutoHideContainer(area, DockWidget, TabIndex++);
	}
}

void CDockAreaWidget::toggleAutoHide(SideBarLocation Location)
{
	if (!isAutoHideFeatureEnabled())
	{
		return;
	}

	setAutoHide(!isAutoHide(), Location);
}

void CDockAreaWidget::closeOtherAreas()
{
	dockContainer()->closeOtherAreas(this);
}

CDockAreaTitleBar* CDockAreaWidget::titleBar() const
{
	return d->TitleBar;
}

bool CDockAreaWidget::isCentralWidgetArea() const
{
	if (dockWidgetsCount() != 1)
	{
		return false;
	}

	return dockManager()->centralWidget() == dockWidgets().constFirst();
}

bool CDockAreaWidget::containsCentralWidget() const
{
	auto centralWidget = dockManager()->centralWidget();
	for (const auto& dockWidget: dockWidgets())
	{
		if (dockWidget == centralWidget)
		{
			return true;
		}
	}

	return false;
}

QSize CDockAreaWidget::minimumSizeHint() const
{
	if (!d->MinSizeHint.isValid())
	{
		return Super::minimumSizeHint();
	}

	int extraHeight = 0;
	if (d->TitleBar->isVisible())
	{
		extraHeight += d->TitleBar->minimumSizeHint().height();
	}

	if (CDockManager::testConfigFlag(CDockManager::TabsAtBottom) && d->tabBar()->isVisible())
	{
		extraHeight += d->tabBar()->minimumSizeHint().height();
	}

	return d->MinSizeHint + QSize(0, extraHeight);
}

void CDockAreaWidget::onDockWidgetFeaturesChanged()
{
	if (d->TitleBar)
	{
		d->updateTitleBarButtonStates();
	}
}

bool CDockAreaWidget::isTopLevelArea() const
{
	auto Container = dockContainer();
	if (!Container)
	{
		return false;
	}

	return (Container->topLevelDockArea() == this);
}

void CDockAreaWidget::setFloating()
{
	d->TitleBar->setAreaFloating();
}

#ifdef Q_OS_WIN
bool CDockAreaWidget::event(QEvent* e)
{
	switch (e->type())
	{
		case QEvent::PlatformSurface:
			return true;
		default:
			break;
	}

	return Super::event(e);
}
#endif

}// namespace ads

// EOF CDockAreaWidget.cpp

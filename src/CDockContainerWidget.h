#pragma once

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

/// @file   CDockContainerWidget.h
/// @author Uwe Kindler
/// @date   24.02.2017
/// @brief  Declaration of CDockContainerWidget class.

#include <QFrame>

#include "CAutoHideTab.h"
#include "CDockWidget.h"
#include "ads_globals.h"

QT_FORWARD_DECLARE_CLASS(QXmlStreamWriter)

namespace ads
{
class DockContainerWidgetPrivate;
class CDockAreaWidget;
class CDockWidget;
class CDockManager;
struct DockManagerPrivate;
class CFloatingDockContainer;
struct FloatingDockContainerPrivate;
class CFloatingDragPreview;
struct FloatingDragPreviewPrivate;
class CDockingStateReader;
class CAutoHideSideBar;
class CAutoHideTab;
class CDockSplitter;
struct AutoHideTabPrivate;
struct AutoHideDockContainerPrivate;

/**
 * @brief Container that manages a number of dock areas with single dock widgets
 * or tabified dock widgets in each area.
 * Each window that support docking has a DockContainerWidget. That means
 * the main application window and all floating windows contain a
 * DockContainerWidget instance.
 */
class ADS_EXPORT CDockContainerWidget : public QFrame
{
		Q_OBJECT

	public:
		/**
		 * @brief Default Constructor.
		 */
		CDockContainerWidget(CDockManager* DockManager, QWidget* parent = 0);

		/**
		 * @brief Virtual Destructor.
		 */
		~CDockContainerWidget() override;

	private Q_SLOTS:
		/**
		 * @brief Removes the container from its dock manager.
		 */
		void removeFromDockManager();

	protected:
		/**
		 * @brief Handles activation events to update zOrderIndex.
		 */
		bool event(QEvent* e) override;

		/**
		 * @brief The drag and drop events below implement docking of floating widgets
		 * on Wayland, where CFloatingDockContainer::startPlatformDrag() executes
		 * a compositor driven drag instead of the mouse tracked dragging that is
		 * used on the other platforms.
		 * Accepts the drag, if it transports a floating widget of the same
		 * dock manager.
		 */
		void dragEnterEvent(QDragEnterEvent* e) override;

		/**
		 * @brief Updates the drop overlays for the drag position.
		 */
		void dragMoveEvent(QDragMoveEvent* e) override;

		/**
		 * @brief Hides the drop overlays, if the drag leaves this container.
		 */
		void dragLeaveEvent(QDragLeaveEvent* e) override;

		/**
		 * @brief Docks the dragged floating widget into the drop area under the drop
		 * position.
		 */
		void dropEvent(QDropEvent* e) override;

		/**
		 * @brief Returns the floating widget transported by the given platform drag
		 * and drop event, if the drag was started by this application and the
		 * floating widget belongs to the given dock manager; otherwise nullptr.
		 * The floating widget pointer from the mime data is only dereferenced
		 * after it was found in the dock manager floating widget list, so drags
		 * from other applications cannot inject an invalid pointer.
		 */
		static CFloatingDockContainer* floatingWidgetFromDropEvent(QDropEvent* e, CDockManager* DockManager);

		/**
		 * @brief Configures and shows the container and dock area drop overlays for a
		 * drag that hovers over TopContainer at the given global position.
		 * Shared by the mouse tracked dragging (CFloatingDragPreview /
		 * CFloatingDockContainer) and the Wayland platform drag and drop.
		 * ContentPinnable is true, if the dragged content can be auto hidden.
		 */
		static void showDropOverlays(CDockManager* DockManager, CDockContainerWidget* TopContainer, const QPoint& GlobalPos, bool ContentPinnable);

		/**
		 * @brief Access function for the internal root splitter.
		 */
		CDockSplitter* rootSplitter() const;

		/**
		 * @brief Creates and initializes a dockwidget auto hide container into the given
		 * area. Initializing inserts the tabs into the side tab widget and hides it
		 * Returns nullptr if you try and insert into an area where the configuration
		 * is not enabled.
		 */
		CAutoHideDockContainer* createAndSetupAutoHideContainer(SideBarLocation area, CDockWidget* DockWidget, int TabIndex = -1);

		/**
		 * @brief Helper function for creation of the root splitter.
		 */
		void createRootSplitter();

		/**
		 * @brief Helper function for creation of the side tab bar widgets.
		 */
		void createSideTabBarWidgets();

		/**
		 * @brief Drop floating widget into the container.
		 */
		void dropFloatingWidget(CFloatingDockContainer* FloatingWidget, const QPoint& TargetPos);

		/**
		 * @brief Drop a dock area or a dock widget given in widget parameter.
		 * If the TargetAreaWidget is a nullptr, then the DropArea indicates
		 * the drop area for the container. If the given TargetAreaWidget is not
		 * a nullptr, then the DropArea indicates the drop area in the given
		 * TargetAreaWidget.
		 */
		void dropWidget(QWidget* Widget, DockWidgetArea DropArea, CDockAreaWidget* TargetAreaWidget, int TabIndex = -1);

		/**
		 * @brief Adds the given dock area to this container widget.
		 */
		void addDockArea(CDockAreaWidget* DockAreaWidget, DockWidgetArea area = ads::CenterDockWidgetArea);

		/**
		 * @brief Removes the given dock area from this container.
		 */
		void removeDockArea(CDockAreaWidget* area);

		/**
		 * @brief Remove all dock areas and returns the list of removed dock areas.
		 */
		QList<QPointer<CDockAreaWidget>> removeAllDockAreas();

		/**
		 * @brief Saves the state into the given stream.
		 */
		void saveState(QXmlStreamWriter& Stream) const;

		/**
		 * @brief Restores the state from given stream.
		 * If Testing is true, the function only parses the data from the given
		 * stream but does not restore anything. You can use this check for
		 * faulty files before you start restoring the state.
		 */
		bool restoreState(CDockingStateReader& Stream, bool Testing);

		/**
		 * @brief This function returns the last added dock area widget for the given
		 * area identifier or 0 if no dock area widget has been added for the given
		 * area.
		 */
		CDockAreaWidget* lastAddedDockAreaWidget(DockWidgetArea area) const;

		/**
		 * @brief If hasSingleVisibleDockWidget() returns true, this function returns the
		 * one and only visible dock widget. Otherwise it returns a nullptr.
		 */
		CDockWidget* topLevelDockWidget() const;

		/**
		 * @brief If the container has only one single visible dock area, then this
		 * functions returns this top level dock area.
		 */
		CDockAreaWidget* topLevelDockArea() const;

		/**
		 * @brief This function returns a list of all dock widgets in this floating widget.
		 * It may be possible, depending on the implementation, that dock widgets,
		 * that are not visible to the user have no parent widget. Therefore simply
		 * calling findChildren() would not work here. Therefore this function
		 * iterates over all dock areas and creates a list that contains all
		 * dock widgets returned from all dock areas.
		 */
		QList<CDockWidget*> dockWidgets() const;

		/**
		 * @brief This function forces the dock container widget to update handles of
		 * splitters based on resize modes of dock widgets contained in the container.
		 */
		void updateSplitterHandles(QSplitter* splitter);

		/**
		 * @brief Registers the given floating widget in the internal list of
		 * auto hide widgets.
		 */
		void registerAutoHideWidget(CAutoHideDockContainer* AutoHideWidget);

		/**
		 * @brief Remove the given auto hide widget from the list of registered auto hide
		 * widgets.
		 */
		void removeAutoHideWidget(CAutoHideDockContainer* AutoHideWidget);

		/**
		 * @brief Handles widget events of auto hide widgets to trigger delayed show
		 * or hide actions for auto hide container on auto hide tab mouse over.
		 */
		void handleAutoHideWidgetEvent(QEvent* e, QWidget* w);

	public:
		/**
		 * @brief Adds dockwidget into the given area.
		 * If DockAreaWidget is not null, then the area parameter indicates the area
		 * into the DockAreaWidget. If DockAreaWidget is null, the Dockwidget will
		 * be dropped into the container.
		 * @return Returns the dock area widget that contains the new DockWidget.
		 */
		CDockAreaWidget* addDockWidget(DockWidgetArea area, CDockWidget* Dockwidget, CDockAreaWidget* DockAreaWidget = nullptr, int Index = -1);

		/**
		 * @brief Removes dockwidget.
		 */
		void removeDockWidget(CDockWidget* Dockwidget);

		/**
		 * @brief Returns the current zOrderIndex.
		 */
		virtual unsigned int zOrderIndex() const;

		/**
		 * @brief This function returns true if this container widgets z order index is
		 * higher than the index of the container widget given in Other parameter.
		 */
		bool isInFrontOf(CDockContainerWidget* Other) const;

		/**
		 * @brief Returns the dock area at the given global position or 0 if there is no
		 * dock area at this position.
		 */
		CDockAreaWidget* dockAreaAt(const QPoint& GlobalPos) const;

		/**
		 * @brief Returns the dock area at the given Index or 0 if the index is out of
		 * range.
		 */
		CDockAreaWidget* dockArea(int Index) const;

		/**
		 * @brief Returns the list of dock areas that are not closed
		 * If all dock widgets in a dock area are closed, the dock area will be closed.
		 */
		QList<CDockAreaWidget*> openedDockAreas() const;

		/**
		 * @brief Returns a list for all open dock widgets in all open dock areas.
		 */
		QList<CDockWidget*> openedDockWidgets() const;

		/**
		 * @brief This function returns true, if the container has open dock areas.
		 * This functions is a little bit faster than calling
		 * openedDockAreas().isEmpty() because it returns as soon as it finds an open
		 * dock area.
		 */
		bool hasOpenDockAreas() const;

		/**
		 * @brief This function returns true if this dock area has only one single
		 * visible dock widget.
		 * A top level widget is a real floating widget. Only the isFloating()
		 * function of top level widgets may returns true.
		 */
		bool hasTopLevelDockWidget() const;

		/**
		 * @brief Returns the number of dock areas in this container.
		 */
		int dockAreaCount() const;

		/**
		 * @brief Returns the number of visible dock areas.
		 */
		int visibleDockAreaCount() const;

		/**
		 * @brief This function returns true, if this container is in a floating widget.
		 */
		bool isFloating() const;

		/**
		 * @brief Dumps the layout for debugging purposes.
		 */
		void dumpLayout();

		/**
		 * @brief This functions returns the dock widget features of all dock widget in
		 * this container.
		 * A bitwise and is used to combine the flags of all dock widgets. That
		 * means, if only dock widget does not support a certain flag, the whole
		 * dock are does not support the flag.
		 */
		CDockWidget::DockWidgetFeatures features() const;

		/**
		 * @brief If this dock container is in a floating widget, this function returns
		 * the floating widget.
		 * Else, it returns a nullptr.
		 */
		CFloatingDockContainer* floatingWidget() const;

		/**
		 * @brief Call this function to close all dock areas except the KeepOpenArea.
		 */
		void closeOtherAreas(CDockAreaWidget* KeepOpenArea);

		/**
		 * @brief Returns the side tab widget for the given area.
		 */
		CAutoHideSideBar* autoHideSideBar(SideBarLocation area) const;

		/**
		 * @brief Access function for auto hide widgets.
		 */
		QList<CAutoHideDockContainer*> autoHideWidgets() const;

		/**
		 * @brief Returns the content rectangle without the autohide side bars.
		 */
		QRect contentRect() const;

		/**
		 * @brief Returns the content rectangle mapped to global space.
		 * The content rectangle is the rectangle of the dock manager widget minus
		 * the auto hide side bars. So that means, it is the geometry of the
		 * internal root splitter mapped to global space.
		 */
		QRect contentRectGlobal() const;

		/**
		 * @brief Returns the dock manager that owns this container.
		 */
		CDockManager* dockManager() const;

	Q_SIGNALS:
		/**
		 * @brief This signal is emitted if one or multiple dock areas has been added to
		 * the internal list of dock areas.
		 * If multiple dock areas are inserted, this signal is emitted only once.
		 */
		void dockAreasAdded();

		/**
		 * @brief This signal is emitted, if a new auto hide widget has been created.
		 */
		void autoHideWidgetCreated(ads::CAutoHideDockContainer* AutoHideWidget);

		/**
		 * @brief This signal is emitted if one or multiple dock areas has been removed.
		 */
		void dockAreasRemoved();

		/**
		 * @brief This signal is emitted if a dock area is opened or closed via
		 * toggleView() function.
		 */
		void dockAreaViewToggled(ads::CDockAreaWidget* DockArea, bool Open);

	private:
		friend class DockContainerWidgetPrivate;
		friend class CDockManager;
		friend struct DockManagerPrivate;
		friend class CDockAreaWidget;
		friend struct DockAreaWidgetPrivate;
		friend class CFloatingDockContainer;
		friend struct FloatingDockContainerPrivate;
		friend class CDockWidget;
		friend class CFloatingDragPreview;
		friend struct FloatingDragPreviewPrivate;
		/// @brief The c auto hide dock container.
		friend CAutoHideDockContainer;
		/// @brief The c auto hide tab.
		friend CAutoHideTab;
		/// @brief The auto hide tab private.
		friend AutoHideTabPrivate;
		/// @brief The auto hide dock container private.
		friend AutoHideDockContainerPrivate;
		/// @brief The c auto hide side bar.
		friend CAutoHideSideBar;
		/// Private data (pimpl).
		DockContainerWidgetPrivate* d;
};
}// namespace ads

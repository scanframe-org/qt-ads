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

/// @file   CDockWidget.h
/// @author Uwe Kindler
/// @date   26.02.2017
/// @brief  Declaration of CDockWidget class.

#include <QFrame>

#include "ads_globals.h"

QT_FORWARD_DECLARE_CLASS(QToolBar)
QT_FORWARD_DECLARE_CLASS(QXmlStreamWriter)

namespace ads
{
struct DockWidgetPrivate;
class CDockWidgetTab;
class CDockManager;
class CDockContainerWidget;
class CDockAreaWidget;
class DockContainerWidgetPrivate;
class CFloatingDockContainer;
class CAutoHideTab;
class CAutoHideDockContainer;
class CAutoHideSideBar;

/**
 * @brief The QDockWidget class provides a widget that can be docked inside a
 * CDockManager or floated as a top-level window on the desktop.
 */
class ADS_EXPORT CDockWidget : public QFrame
{
		Q_OBJECT

	public:
		/**
		 * @brief This constructor creates a dock widget for the given dock manager with the
		 * provided title.
		 *
		 * @param manager Pointer to the dock manager that owns the dock widget.
		 * @param title The title is the text that is shown in the window title when
		 * the dock widget is floating, and it is the title that is shown in the
		 * titlebar or the tab of this dock widget if it is tabified.
		 * @param parent Pointer to the parent widget, defaults to nullptr.
		 *
		 * @note The object name of the dock widget is also set to the title. The
		 * object name is required by the dock manager to properly save and restore
		 * the state of the dock widget. That means, the title needs to be unique. If
		 * the title is not unique or if you would like to change the title during
		 * runtime, you need to set a unique object name explicitly by calling
		 * setObjectName() after construction. Use the layoutFlags to configure the
		 * layout of the dock widget.
		 *
		 * @note This constructor is preferred over the two argument version,
		 * especially when custom factories are in use. Indeed, it will use the Dock
		 * Manager factory, and not the default factory. For this reason, the original
		 * constructor should be deprecated in favour of this version.
		 */
		CDockWidget(CDockManager* manager, const QString& title, QWidget* parent = nullptr);

		/**
		 * @brief Virtual Destructor.
		 */
		~CDockWidget() override;

	private Q_SLOTS:
		/**
		 * @brief Adjusts the toolbar icon sizes according to the floating state.
		 */
		void setToolbarFloatingStyle(bool topLevel);

	protected:
		/**
		 * @brief Assigns the dock manager that manages this dock widget.
		 */
		void setDockManager(CDockManager* DockManager);

		/**
		 * @brief If this dock widget is inserted into a dock area, the dock area will
		 * be registered on this widget via this function. If a dock widget is
		 * removed from a dock area, this function will be called with nullptr
		 * value.
		 */
		void setDockArea(CDockAreaWidget* DockArea);

		/**
		 * @brief This function changes the toggle view action without emitting any
		 * signal.
		 */
		void setToggleViewActionChecked(bool Checked);

		/**
		 * @brief Saves the state into the given stream.
		 */
		void saveState(QXmlStreamWriter& Stream) const;

		/**
		 * @brief This is a helper function for the dock manager to flag this widget
		 * as unassigned.
		 * When calling the restore function, it may happen, that the saved state
		 * contains less dock widgets then currently available. All widgets whose
		 * data is not contained in the saved state, are flagged as unassigned
		 * after the restore process. If the user shows an unassigned dock widget,
		 * a floating widget will be created to take up the dock widget.
		 */
		void flagAsUnassigned();

		/**
		 * @brief Call this function to emit a topLevelChanged() signal and to update
		 * the dock area tool bar visibility.
		 */
		static void emitTopLevelEventForWidget(CDockWidget* TopLevelDockWidget, bool Floating);

		/**
		 * @brief Use this function to emit a top level changed event.
		 * Do never use emit topLevelChanged(). Always use this function because
		 * it only emits a signal if the floating state has really changed.
		 */
		void emitTopLevelChanged(bool Floating);

		/**
		 * @brief Internal function for modifying the closed state when restoring
		 * a saved docking state.
		 */
		void setClosedState(bool Closed);

		/**
		 * @brief Internal toggle view function that does not check if the widget
		 * already is in the given state.
		 */
		void toggleViewInternal(bool Open);

		/**
		 * @brief Internal close dock widget implementation.
		 * The function returns true if the dock widget has been closed or hidden.
		 */
		bool closeDockWidgetInternal(bool ForceClose = false);

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = QFrame;

		/**
		 * @brief Enumeration of the dock widget feature values.
		 */
		enum DockWidgetFeature
		{
			/// @brief The dock widget has a close button.
			DockWidgetClosable = 0x001,
			/// @brief The dock widget is movable and can be moved to a new position in the current dock container.
			DockWidgetMovable = 0x002,
			/// @brief The dock widget can be dragged into a floating window.
			DockWidgetFloatable = 0x004,
			/// @brief Deletes the dock widget when it is closed.
			DockWidgetDeleteOnClose = 0x008,
			/// @brief Clicking the close button will not close the dock widget but emits the closeRequested() signal instead.
			CustomCloseHandling = 0x010,
			/// @brief If this is enabled, a dock widget can get focus highlighting.
			DockWidgetFocusable = 0x020,
			/// @brief The dock widget will be closed when the dock area hosting it is closed.
			DockWidgetForceCloseWithArea = 0x040,
			/// @brief The dock widget tab will never be shown if this flag is set.
			NoTab = 0x080,
			/// @brief Deletes only the contained widget on close, keeping the dock widget intact and in place. Attempts to rebuild the contents widget on show if there is a widget factory set.
			DeleteContentOnClose = 0x100,
			/// @brief The dock widget can be pinned and added to an auto hide dock container.
			DockWidgetPinnable = 0x200,
			DefaultDockWidgetFeatures = DockWidgetClosable | DockWidgetMovable | DockWidgetFloatable | DockWidgetFocusable | DockWidgetPinnable,
			AllDockWidgetFeatures = DefaultDockWidgetFeatures | DockWidgetDeleteOnClose | CustomCloseHandling,
			DockWidgetAlwaysCloseAndDelete = DockWidgetForceCloseWithArea | DockWidgetDeleteOnClose,
			GloballyLockableFeatures = DockWidgetClosable | DockWidgetMovable | DockWidgetFloatable | DockWidgetPinnable,
			NoDockWidgetFeatures = 0x000
		};
		Q_DECLARE_FLAGS(DockWidgetFeatures, DockWidgetFeature)

		/**
		 * @brief Enumeration of the state values.
		 */
		enum eState
		{
			StateHidden,
			StateDocked,
			StateFloating
		};

		/**
		 * @brief Enumeration of the tool bar style source values.
		 */
		enum eToolBarStyleSource
		{
			ToolBarStyleFromDockManager,
			ToolBarStyleFromDockWidget
		};

		/**
		 * @brief Sets the widget for the dock widget to widget.
		 * The InsertMode defines how the widget is inserted into the dock widget.
		 * The content of a dock widget should be resizable do a very small size to
		 * prevent the dock widget from blocking the resizing. To ensure, that a
		 * dock widget can be resized very well, it is better to insert the content+
		 * widget into a scroll area or to provide a widget that is already a scroll
		 * area or that contains a scroll area.
		 * If the InsertMode is AutoScrollArea, the DockWidget tries to automatically
		 * detect how to insert the given widget. If the widget is derived from
		 * QScrollArea (i.e. an QAbstractItemView), then the widget is inserted
		 * directly. If the given widget is not a scroll area, the widget will be
		 * inserted into a scroll area.
		 * To force insertion into a scroll area, you can also provide the InsertMode
		 * ForceScrollArea. To prevent insertion into a scroll area, you can
		 * provide the InsertMode ForceNoScrollArea.
		 */
		enum eInsertMode
		{
			AutoScrollArea,
			ForceScrollArea,
			ForceNoScrollArea
		};

		/**
		 * @brief The mode of the minimumSizeHint() that is returned by the DockWidget
		 * minimumSizeHint() function.
		 * To ensure, that a dock widget does not block resizing, the dock widget
		 * reimplements minimumSizeHint() function to return a very small minimum
		 * size hint. If you would like to adhere the minimumSizeHint() from the
		 * content widget, then set the minimumSizeHintMode() to
		 * MinimumSizeHintFromContent. If you would like to use the minimumSize()
		 * value of the content widget or the dock widget, then you can use the
		 * MinimumSizeHintFromDockWidgetMinimumSize or
		 * MinimumSizeHintFromContentMinimumSize modes.
		 */
		enum eMinimumSizeHintMode
		{
			MinimumSizeHintFromDockWidget,
			MinimumSizeHintFromContent,
			MinimumSizeHintFromDockWidgetMinimumSize,
			MinimumSizeHintFromContentMinimumSize,
		};

		/**
		 * @brief This mode configures the behavior of the toggle view action.
		 * If the mode if ActionModeToggle, then the toggle view action is
		 * a checkable action to show / hide the dock widget. If the mode
		 * is ActionModeShow, then the action is not checkable an it will
		 * always show the dock widget if clicked. If the mode is ActionModeShow,
		 * the user can only close the DockWidget with the close button.
		 */
		enum eToggleViewActionMode
		{
			// !< ActionModeToggle
			ActionModeToggle,
			// !< ActionModeShow
			ActionModeShow
		};

		/**
		 * @brief This constructor creates a dock widget with the given title.
		 * The title is the text that is shown in the window title when the dock
		 * widget is floating and it is the title that is shown in the titlebar
		 * or the tab of this dock widget if it is tabified.
		 * The object name of the dock widget is also set to the title. The
		 * object name is required by the dock manager to properly save and restore
		 * the state of the dock widget. That means, the title needs to be unique.
		 * If your title is not unique or if you would like to change the title
		 * during runtime, you need to set a unique object name explicitly
		 * by calling setObjectName() after construction.
		 * Use the layoutFlags to configure the layout of the dock widget.
		 * @note If you would like to use custom TabWidget implementations, you need
		 * to use the constructor with the CDockManager argument.
		 */
		Q_DECL_DEPRECATED explicit CDockWidget(const QString& title, QWidget* parent = nullptr);

		/**
		 * @brief We return a fixed minimum size hint or the size hint of the content
		 * widget if minimum size hint mode is MinimumSizeHintFromContent.
		 */
		QSize minimumSizeHint() const override;

		/**
		 * @brief Sets the widget for the dock widget to widget.
		 * The InsertMode defines how the widget is inserted into the dock widget.
		 * The content of a dock widget should be resizable to a very small size to
		 * prevent the dock widget from blocking the resizing. To ensure, that a
		 * dock widget can be resized very well, it is better to insert the content+
		 * widget into a scroll area or to provide a widget that is already a scroll
		 * area or that contains a scroll area.
		 * If the InsertMode is AutoScrollArea, the DockWidget tries to automatically
		 * detect how to insert the given widget. If the widget is derived from
		 * QScrollArea (i.e. an QAbstractItemView), then the widget is inserted
		 * directly. If the given widget is not a scroll area, the widget will be
		 * inserted into a scroll area.
		 * To force insertion into a scroll area, you can also provide the InsertMode
		 * ForceScrollArea. To prevent insertion into a scroll area, you can
		 * provide the InsertMode ForceNoScrollArea.
		 */
		void setWidget(QWidget* widget, eInsertMode InsertMode = AutoScrollArea);

		/**
		 * @brief Only used when the feature flag DeleteContentOnClose is set.
		 * Using the flag and setting a widget factory allows to free the resources
		 * of the widget of your application while retaining the position the next
		 * time you want to show your widget, unlike the flag DockWidgetDeleteOnClose
		 * which deletes the dock widget itself. Since we keep the dock widget, all
		 * regular features of ADS should work as normal, including saving and
		 * restoring the state of the docking system and using perspectives.
		 */
		using FactoryFunc = std::function<QWidget*(QWidget*)>;
		/**
		 * @brief Sets the widget factory.
		 */
		void setWidgetFactory(FactoryFunc createWidget, eInsertMode InsertMode = AutoScrollArea);

		/**
		 * @brief Remove the widget from the dock and give ownership back to the caller.
		 */
		QWidget* takeWidget();

		/**
		 * @brief Returns the widget for the dock widget. This function returns zero if
		 * the widget has not been set.
		 */
		QWidget* widget() const;

		/**
		 * @brief Returns the tab widget of this dock widget that is shown in the dock
		 * area title bar.
		 */
		CDockWidgetTab* tabWidget() const;

		/**
		 * @brief Sets, whether the dock widget is movable, closable, and floatable.
		 */
		void setFeatures(DockWidgetFeatures features);

		/**
		 * @brief Sets the feature flag for this dock widget if on is true; otherwise
		 * clears the flag.
		 */
		void setFeature(DockWidgetFeature flag, bool on);

		/**
		 * @brief This property holds whether the dock widget is movable, closable, and
		 * floatable.
		 * By default, this property is set to a combination of DockWidgetClosable,
		 * DockWidgetMovable and DockWidgetFloatable.
		 */
		DockWidgetFeatures features() const;

		/**
		 * @brief Triggers notification of feature change signals and functions.
		 */
		void notifyFeaturesChanged();

		/**
		 * @brief Returns the dock manager that manages the dock widget or 0 if the widget
		 * has not been assigned to any dock manager yet.
		 */
		CDockManager* dockManager() const;

		/**
		 * @brief Returns the dock container widget this dock area widget belongs to or 0
		 * if this dock widget has not been docked yet.
		 */
		CDockContainerWidget* dockContainer() const;

		/**
		 * @brief This function return the floating DockContainer if is isFloating() is true
		 * and a nullptr if this dock widget is not floating.
		 */
		CFloatingDockContainer* floatingDockContainer() const;

		/**
		 * @brief Returns the dock area widget this dock widget belongs to or 0
		 * if this dock widget has not been docked yet.
		 */
		CDockAreaWidget* dockAreaWidget() const;

		/**
		 * @brief Returns the side tab widget for this dock, if this dock widget is in
		 * a auto hide container. If it is not in a auto hide container, then this
		 * function returns a nullptr,.
		 */
		CAutoHideTab* sideTabWidget() const;

		/**
		 * @brief Assign a side tab widget if this dock widget is an auto hide container.
		 */
		void setSideTabWidget(CAutoHideTab* SideTab) const;

		/**
		 * @brief Returns true, if this dock widget is in an auto hide container.
		 */
		bool isAutoHide() const;

		/**
		 * @brief Returns the auto hide dock container of this dock widget
		 * or 0 if there is none.
		 */
		CAutoHideDockContainer* autoHideDockContainer() const;

		/**
		 * @brief Returns the auto hide side bar location or SideBarNone if, this is not
		 * an autohide dock widget.
		 */
		SideBarLocation autoHideLocation() const;

		/**
		 * @brief Sets the preferred auto-hide sidebar location for this dock widget.
		 * When set to a value other than SideBarNone, the pin button will place
		 * this widget in the specified sidebar instead of using geometry-based
		 * detection. When unpinning, widgets with the same preferred location
		 * will be merged as tabs in the same dock area.
		 * Set to SideBarNone (default) to use the original geometry-based behavior.
		 */
		void setPreferredAutoHideSideBarLocation(SideBarLocation Location);

		/**
		 * @brief Returns the preferred auto-hide sidebar location, or SideBarNone
		 * if no preference is set (geometry-based detection will be used).
		 */
		SideBarLocation preferredAutoHideSideBarLocation() const;

		/**
		 * @brief This property holds whether the dock widget is floating.
		 * A dock widget is only floating, if it is the one and only widget inside
		 * of a floating container. If there are more than one dock widget in a
		 * floating container, the all dock widgets are docked and not floating.
		 */
		bool isFloating() const;

		/**
		 * @brief This function returns true, if this dock widget is in a floating.
		 * The function returns true, if the dock widget is floating and it also
		 * returns true if it is docked inside of a floating container.
		 */
		bool isInFloatingContainer() const;

		/**
		 * @brief Returns true, if this dock widget is closed.
		 */
		bool isClosed() const;

		/**
		 * @brief Returns a checkable action that can be used to show or close this dock
		 * widget. The action's text is set to the dock widget's window title.
		 */
		QAction* toggleViewAction() const;

		/**
		 * @brief Use provided action to be the default toggle view action for this dock
		 * widget. This dock widget now owns the action.
		 */
		void setToggleViewAction(QAction* action);

		/**
		 * @brief Configures the behavior of the toggle view action.
		 * @see eToggleViewActionMode for a detailed description
		 */
		void setToggleViewActionMode(eToggleViewActionMode Mode);

		/**
		 * @brief Configures the minimum size hint that is returned by the
		 * minimumSizeHint() function.
		 * @see eMinimumSizeHintMode for a detailed description
		 */
		void setMinimumSizeHintMode(eMinimumSizeHintMode Mode);

		/**
		 * @brief Get the minimum size hint mode configured by setMinimumSizeHintMode.
		 */
		eMinimumSizeHintMode minimumSizeHintMode() const;

		/**
		 * @brief Returns true if the dock widget is set as central widget of it's dock
		 * manager.
		 */
		bool isCentralWidget() const;

		/**
		 * @brief Sets the dock widget icon that is shown in tabs and in toggle view
		 * actions.
		 */
		void setIcon(const QIcon& Icon);

		/**
		 * @brief Returns the icon that has been assigned to the dock widget.
		 */
		QIcon icon() const;

		/**
		 * @brief This function returns the dock widget top tool bar.
		 * If no toolbar is assigned, this function returns nullptr. To get a valid
		 * toolbar you either need to create a default empty toolbar via
		 * createDefaultToolBar() function or you need to assign your custom
		 * toolbar via setToolBar().
		 */
		QToolBar* toolBar() const;

		/**
		 * @brief If you would like to use the default top tool bar, then call this
		 * function to create the default tool bar.
		 * After this function the toolBar() function will return a valid toolBar()
		 * object.
		 */
		QToolBar* createDefaultToolBar();

		/**
		 * @brief Assign a new tool bar that is shown above the content widget.
		 * The dock widget will become the owner of the tool bar and deletes it
		 * on destruction.
		 */
		void setToolBar(QToolBar* ToolBar);

		/**
		 * @brief Configures, if the dock widget uses the global tool bar styles from
		 * dock manager or if it uses its own tool bar style.
		 */
		void setToolBarStyleSource(eToolBarStyleSource Source);

		/**
		 * @brief Returns the configured tool bar style source.
		 */
		eToolBarStyleSource toolBarStyleSource() const;

		/**
		 * @brief This function sets the tool button style for the given dock widget state.
		 * It is possible to switch the tool button style depending on the state.
		 * If a dock widget is floating, then here are more space and it is
		 * possible to select a style that requires more space like
		 * Qt::ToolButtonTextUnderIcon. For the docked state Qt::ToolButtonIconOnly
		 * might be better.
		 */
		void setToolBarStyle(Qt::ToolButtonStyle Style, eState State);

		/**
		 * @brief Returns the tool button style for the given docking state.
		 * @see setToolBarStyle()
		 */
		Qt::ToolButtonStyle toolBarStyle(eState State) const;

		/**
		 * @brief This function sets the tool button icon size for the given state.
		 * If a dock widget is floating, there is more space an increasing the
		 * icon size is possible. For docked widgets, small icon sizes, eg. 16 x 16
		 * might be better.
		 */
		void setToolBarIconSize(const QSize& IconSize, eState State);

		/**
		 * @brief Returns the icon size for a given docking state.
		 * @see setToolBarIconSize()
		 */
		QSize toolBarIconSize(eState State) const;

		/**
		 * @brief Set the actions that will be shown in the dock area title bar
		 * if this dock widget is the active tab.
		 * You should not add to many actions to the title bar, because this
		 * will remove the available space for the tabs. If you have a number
		 * of actions, just add an action with a menu to show a popup menu
		 * button in the title bar.
		 */
		void setTitleBarActions(QList<QAction*> actions);

		/**
		 * @brief Returns a list of actions that will be inserted into the dock area title
		 * bar if this dock widget becomes the current widget.
		 */
		virtual QList<QAction*> titleBarActions() const;

#ifndef QT_NO_TOOLTIP
		/**
		 * @brief This is function sets text tooltip for title bar widget
		 * and tooltip for toggle view action.
		 */
		void setTabToolTip(const QString& text);
#endif

		/**
		 * @brief Returns true if the dock widget is floating and if the floating dock
		 * container is full screen.
		 * Hides a non-virtual function.
		 */
		bool isFullScreen() const;

		/**
		 * @brief Returns true if this dock widget is in a dock area, that contains at
		 * least 2 opened dock widgets.
		 */
		bool isTabbed() const;

		/**
		 * @brief Returns true if this dock widget is the current one in the dock
		 * area widget that contains it.
		 * If the dock widget is the only opened dock widget in a dock area,
		 * `true` is returned.
		 */
		bool isCurrentTab() const;

	protected:
		/**
		 * @brief Emits titleChanged signal if title change event occurs.
		 */
		bool event(QEvent* e) override;

	public Q_SLOTS:
		/**
		 * @brief This property controls whether the dock widget is open or closed.
		 * The toggleViewAction triggers this slot.
		 */
		void toggleView(bool Open = true);

		/**
		 * @brief Makes this dock widget the current tab in its dock area.
		 * The function only has an effect, if the dock widget is open. A call
		 * to this function will not toggle the view, so if it is closed,
		 * nothing will happen.
		 */
		void setAsCurrentTab();

		/**
		 * @brief Brings the dock widget to the front.
		 * This means:
		 * 	- If the dock widget is tabbed with other dock widgets but its tab is not
		 * current, it's made current.
		 * 	- If the dock widget is floating, QWindow::raise() is called.
		 * 	This only applies if the dock widget is already open. If closed, does
		 * nothing.
		 * Hides a non-virtual function.
		 */
		void raise();

		/**
		 * @brief This function will make a docked widget floating.
		 */
		void setFloating();

		/**
		 * @brief This function will delete the dock widget and its content from the
		 * docking system.
		 */
		void deleteDockWidget();

		/**
		 * @brief Closes the dock widget.
		 * The function forces closing of the dock widget even for
		 * CustomCloseHandling.
		 */
		void closeDockWidget();

		/**
		 * @brief Request closing of the dock widget.
		 * For DockWidget with default close handling, the function does the same
		 * as clodeDockWidget() but if the flags CustomCloseHandling is set,
		 * the function only emits the closeRequested() signal.
		 */
		void requestCloseDockWidget();

		/**
		 * @brief Shows the widget in full-screen mode.
		 * Normally this function only affects windows. To make the interface
		 * compatible to QDockWidget, this function also maximizes a floating
		 * dock widget.
		 *
		 * @note Full-screen mode works fine under Windows, but has certain
		 * problems (does not work) under X (Linux). These problems are due to
		 * limitations of the ICCCM protocol that specifies the communication
		 * between X11 clients and the window manager. ICCCM simply does not
		 * understand the concept of non-decorated full-screen windows.
		 * Hides a non-virtual function.
		 */
		void showFullScreen();

		/**
		 * @brief This function complements showFullScreen() to restore the widget
		 * after it has been in full screen mode.
		 * Hides a non-virtual function.
		 */
		void showNormal();

		/**
		 * @brief Sets the dock widget into auto hide mode if this feature is enabled
		 * via CDockManager::setAutoHideFlags(CDockManager::AutoHideFeatureEnabled).
		 */
		void setAutoHide(bool Enable, SideBarLocation Location = SideBarLocation::SideBarNone, int TabIndex = -1);

		/**
		 * @brief Switches the dock widget to auto hide mode or vice versa depending on its
		 * current state.
		 */
		void toggleAutoHide(SideBarLocation Location = SideBarLocation::SideBarNone);

	Q_SIGNALS:
		/**
		 * @brief This signal is emitted if the dock widget is opened or closed.
		 */
		void viewToggled(bool Open);

		/**
		 * @brief This signal is emitted if the dock widget is closed.
		 */
		void closed();

		/**
		 * @brief This signal is emitted if the window title of this dock widget
		 * changed.
		 */
		void titleChanged(const QString& Title);

		/**
		 * @brief This signal is emitted when the floating property changes.
		 * The topLevel parameter is true if the dock widget is now floating;
		 * otherwise it is false.
		 */
		void topLevelChanged(bool topLevel);

		/**
		 * @brief This signal is emitted, if close is requested.
		 */
		void closeRequested();

		/**
		 * @brief This signal is emitted when the dock widget becomes visible (or invisible).
		 * This happens when the widget is hidden or shown, as well as when it is
		 * docked in a tabbed dock area and its tab becomes selected or unselected.
		 */
		void visibilityChanged(bool visible);

		/**
		 * @brief This signal is emitted when the features property changes.
		 * The features parameter gives the new value of the property.
		 */
		void featuresChanged(ads::CDockWidget::DockWidgetFeatures features);

	private:
		friend struct DockWidgetPrivate;
		friend class CDockContainerWidget;
		friend class CDockAreaWidget;
		friend class CFloatingDockContainer;
		friend class CDockManager;
		friend struct DockManagerPrivate;
		friend class DockContainerWidgetPrivate;
		friend class CDockAreaTabBar;
		friend class CDockWidgetTab;
		friend struct DockWidgetTabPrivate;
		friend struct DockAreaTitleBarPrivate;
		friend class CAutoHideDockContainer;
		/// @brief The c auto hide sidebar.
		friend CAutoHideSideBar;
		/// Private data (pimpl).
		DockWidgetPrivate* d;
};
}// namespace ads

Q_DECLARE_OPERATORS_FOR_FLAGS(ads::CDockWidget::DockWidgetFeatures)

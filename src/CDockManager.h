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

/// @file   CDockManager.h
/// @author Uwe Kindler
/// @date   26.02.2017
/// @brief  Declaration of CDockManager class.

#include "CDockContainerWidget.h"
#include "CDockWidget.h"
#include "CFloatingDockContainer.h"
#include "ads_globals.h"

QT_FORWARD_DECLARE_CLASS(QSettings)
QT_FORWARD_DECLARE_CLASS(QMenu)

namespace ads
{
struct DockManagerPrivate;
class CFloatingDockContainer;
struct FloatingDockContainerPrivate;
class CDockContainerWidget;
class DockContainerWidgetPrivate;
class CDockOverlay;
class CDockAreaTabBar;
class CDockWidgetTab;
struct DockWidgetTabPrivate;
struct DockAreaWidgetPrivate;
class CIconProvider;
class CDockComponentsFactory;
class CDockFocusController;
class CAutoHideSideBar;
class CAutoHideTab;
struct AutoHideTabPrivate;

/**
 * @brief The central dock manager that maintains the complete docking system.
 * With the configuration flags you can globally control the functionality
 * of the docking system. The dock manager uses an internal stylesheet to
 * style its components like splitters, tabs and buttons. If you want to
 * disable this stylesheet because your application uses its own,
 * you can either set the DisableStylesheet config flag before creating
 * the dock manager:.
 * \code
 * CDockManager::setConfigFlag(CDockManager::DisableStylesheet, true);
 * \endcode
 * Or call the function for settings the stylesheet with an empty string
 * after creating the dock manager:.
 * \code
 * DockManager->setStyleSheet("");
 * \endcode
 **/
class ADS_EXPORT CDockManager : public CDockContainerWidget
{
		Q_OBJECT

	public:
		/**
		 * @brief Default Constructor.
		 * If the given parent is a QMainWindow, the dock manager sets itself as the
		 * central widget.
		 * Before you create any dock widgets, you should properly setup the
		 * configuration flags via setConfigFlags().
		 */
		CDockManager(QWidget* parent = nullptr);

		/**
		 * @brief Virtual Destructor.
		 */
		~CDockManager() override;

	public Q_SLOTS:
		/**
		 * @brief Ends the isRestoringFromMinimizedState.
		 */
		void endLeavingMinimizedState();

	protected:
		/**
		 * @brief Registers the given floating widget in the internal list of
		 * floating widgets.
		 */
		void registerFloatingWidget(CFloatingDockContainer* FloatingWidget);

		/**
		 * @brief Remove the given floating widget from the list of registered floating
		 * widgets.
		 */
		void removeFloatingWidget(CFloatingDockContainer* FloatingWidget);

		/**
		 * @brief Registers the given dock container widget.
		 */
		void registerDockContainer(CDockContainerWidget* DockContainer);

		/**
		 * @brief Remove dock container from the internal list of registered dock
		 * containers.
		 */
		void removeDockContainer(CDockContainerWidget* DockContainer);

		/**
		 * @brief Overlay for containers.
		 */
		CDockOverlay* containerOverlay() const;

		/**
		 * @brief Overlay for dock areas.
		 */
		CDockOverlay* dockAreaOverlay() const;

		/**
		 * @brief A container needs to call this function if a widget has been dropped
		 * into it.
		 */
		void notifyWidgetOrAreaRelocation(QWidget* RelocatedWidget);

		/**
		 * @brief This function is called, if a floating widget has been dropped into
		 * an new position.
		 * When this function is called, all dock widgets of the FloatingWidget
		 * are already inserted into its new position.
		 */
		void notifyFloatingWidgetDrop(CFloatingDockContainer* FloatingWidget);

		/**
		 * @brief Show the floating widgets that has been created floating.
		 */
		void showEvent(QShowEvent* event) override;

		/**
		 * @brief Wayland: re-applies the inherited style sheet to all floating widgets
		 * when this dock manager or one of its ancestors changes its style sheet.
		 * Floating widgets have no parent widget on Wayland, so they do not
		 * receive style sheet changes that propagate down the widget hierarchy.
		 */
		void changeEvent(QEvent* event) override;

		/**
		 * @brief Access for the internal dock focus controller.
		 * This function only returns a valid object, if the FocusHighlighting
		 * flag is set.
		 */
		CDockFocusController* dockFocusController() const;

		/**
		 * @brief Restore floating widgets hidden by an earlier call to
		 * hideManagerAndFloatingWidgets.
		 */
		void restoreHiddenFloatingWidgets();

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = CDockContainerWidget;

		/**
		 * @brief Enumeration of the view menu insertion order values.
		 */
		enum eViewMenuInsertionOrder
		{
			MenuSortedByInsertion,
			MenuAlphabeticallySorted
		};

		/**
		 * @brief Enumeration of the color scheme mode values.
		 */
		enum class ColorSchemeMode
		{
			Light,
			Dark,
			FollowPalette
		};

		/**
		 * @brief These global configuration flags configure some global dock manager
		 * settings.
		 * Set the dock manager flags, before you create the dock manager instance.
		 */
		enum eConfigFlag
		{
			/// @brief If this flag is set, the active tab in a tab area has a close button.
			ActiveTabHasCloseButton = 0x0001,
			/// @brief If the flag is set each dock area has a close button.
			DockAreaHasCloseButton = 0x0002,
			/// @brief If the flag is set, the dock area close button closes the active tab, if not set, it closes the complete dock area.
			DockAreaCloseButtonClosesTab = 0x0004,
			/// @brief See QSplitter::setOpaqueResize() documentation.
			OpaqueSplitterResize = 0x0008,
			/// @brief If enabled, the XML writer automatically adds line-breaks and indentation to empty sections between elements (ignorable whitespace).
			XmlAutoFormattingEnabled = 0x0010,
			/// @brief If enabled, the XML output will be compressed and is not human readable anymore.
			XmlCompressionEnabled = 0x0020,
			/// @brief If enabled the tab close buttons will be QToolButtons instead of QPushButtons - disabled by default.
			TabCloseButtonIsToolButton = 0x0040,
			/// @brief If this flag is set, then all tabs that are closable show a close button.
			AllTabsHaveCloseButton = 0x0080,
			/// @brief If this flag is set, the space for the close button is reserved even if the close button is not visible.
			RetainTabSizeWhenCloseButtonHidden = 0x0100,
			/// @brief If opaque undocking is disabled, this flag defines the behavior of the drag preview window, if this flag is enabled, the preview will be adjusted dynamically to the drop area.
			DragPreviewIsDynamic = 0x0400,
			/// @brief If opaque undocking is disabled, the created drag preview window shows a copy of the content of the dock widget / dock area that is dragged.
			DragPreviewShowsContentPixmap = 0x0800,
			/// @brief If opaque undocking is disabled, then this flag configures if the drag preview is frameless or looks like a real window.
			DragPreviewHasWindowFrame = 0x1000,
			/// @brief If this option is enabled, the tab of a dock widget is always displayed - even if it is the only visible dock widget in a floating widget.
			AlwaysShowTabs = 0x2000,
			/// @brief If the flag is set each dock area has an undock button.
			DockAreaHasUndockButton = 0x4000,
			/// @brief If the flag is set each dock area has a tabs menu button.
			DockAreaHasTabsMenuButton = 0x8000,
			/// @brief If the flag is set disabled dock area buttons will not appear on the toolbar at all (enabling them will bring them back).
			DockAreaHideDisabledButtons = 0x10000,
			/// @brief If the flag is set, the tabs menu button will be shown only when it is required - that means, if the tabs are elided. If the tabs are not elided, it is hidden.
			DockAreaDynamicTabsMenuButtonVisibility = 0x20000,
			/// @brief If set, the Floating Widget window title reflects the title of the current dock widget otherwise it displays the title set with `CDockManager::setFloatingContainersTitle` or application name as window title.
			FloatingContainerHasWidgetTitle = 0x40000,
			/// @brief If set, the Floating Widget icon reflects the icon of the current dock widget otherwise it displays application icon.
			FloatingContainerHasWidgetIcon = 0x80000,
			/// @brief If there is only one single visible dock widget in the main dock container (the dock manager) and if this flag is set, then the titlebar of this dock widget will be hidden. This only makes sense for non draggable and non floatable widgets and enables the creation of some kind of "central" widget.
			HideSingleCentralWidgetTitleBar = 0x100000,
			/// @brief Enables styling of focused dock widget tabs or floating widget titlebar.
			FocusHighlighting = 0x200000,
			/// @brief If enabled, the space is equally distributed to all widgets in a splitter.
			EqualSplitOnInsertion = 0x400000,
			/// @brief Linux only ! Forces all FloatingContainer to use the native title bar. This might break docking for FloatinContainer on some Window Managers (like Kwin/KDE). If neither this nor FloatingContainerForceCustomTitleBar is set (the default) native titlebars are used except on known bad systems. Users can overwrite this by setting the environment variable ADS_UseNativeTitle to "1" or "0".
			FloatingContainerForceNativeTitleBar = 0x800000,
			/// @brief Linux only ! Forces all FloatingContainer to use a QWidget based title bar. If neither this nor FloatingContainerForceNativeTitleBar is set (the default) native titlebars are used except on known bad systems. Users can overwrite this by setting the environment variable ADS_UseNativeTitle to "1" or "0".
			FloatingContainerForceQWidgetTitleBar = 0x1000000,
			/// @brief If the flag is set, the user can use the mouse middle button to close the tab under the mouse.
			MiddleMouseButtonClosesTab = 0x2000000,
			/// @brief Set this flag to disable eliding of tab texts in dock area tabs.
			DisableTabTextEliding = 0x4000000,
			/// @brief Set this flag to show label texts in dock area tabs only for active tabs.
			ShowTabTextOnlyForActiveTab = 0x8000000,
			/// @brief If the flag is set, a double click on a tab undocks the widget.
			DoubleClickUndocksWidget = 0x10000000,
			/// @brief If the flag is set, tabs will be shown at the bottom instead of in the title bar.
			TabsAtBottom = 0x20000000,
			/// @brief If the flag is set, windows for the dock and area widgets will be native.
			UseNativeWindows = 0x40000000,
			/// @brief If the flag is set, the dock manager will not apply the default stylesheet.
			DisableStylesheet = 0x80000000,

			/// @brief Default configuration of dock area title bar buttons.
			DefaultDockAreaButtons = DockAreaHasCloseButton | DockAreaHasUndockButton | DockAreaHasTabsMenuButton,
			/// @brief Default base configuration settings.
			DefaultBaseConfig = DefaultDockAreaButtons | ActiveTabHasCloseButton | XmlCompressionEnabled | FloatingContainerHasWidgetTitle | DoubleClickUndocksWidget,
			/// @brief The default configuration for opaque operations.
			DefaultOpaqueConfig = DefaultBaseConfig | OpaqueSplitterResize | DragPreviewShowsContentPixmap,
			/// @brief The default configuration for non opaque operations.
			DefaultNonOpaqueConfig = DefaultBaseConfig | DragPreviewShowsContentPixmap,
			/// @brief The default configuration for non opaque operations that show a real window with frame.
			NonOpaqueWithWindowFrame = DefaultNonOpaqueConfig | DragPreviewHasWindowFrame
		};
		Q_DECLARE_FLAGS(ConfigFlags, eConfigFlag)

		/**
		 * @brief These global configuration flags configure some dock manager auto hide
		 * settings
		 * Set the dock manager flags, before you create the dock manager instance.
		 */
		enum eAutoHideFlag
		{
			/// @brief Enables / disables auto hide feature.
			AutoHideFeatureEnabled = 0x01,
			/// @brief If the flag is set each dock area has a auto hide menu button.
			DockAreaHasAutoHideButton = 0x02,
			/// @brief If the flag is set, the auto hide button enables auto hiding for all dock widgets in an area, if disabled, only the current dock widget will be toggled.
			AutoHideButtonTogglesArea = 0x04,
			/// @brief If the flag is set, the auto hide button will be checked and unchecked depending on the auto hide state. Mainly for styling purposes.
			AutoHideButtonCheckable = 0x08,
			/// @brief Show only icons in auto hide side tab - if a tab has no icon, then the text will be shown.
			AutoHideSideBarsIconOnly = 0x10,
			/// @brief Show the auto hide window on mouse over tab and hide it if mouse leaves auto hide container.
			AutoHideShowOnMouseOver = 0x20,
			/// @brief Close button of an auto hide container collapses the dock instead of hiding it completely.
			AutoHideCloseButtonCollapsesDock = 0x40,
			/// @brief If the flag is set an auto hide title bar has a close button.
			AutoHideHasCloseButton = 0x80,
			/// @brief If this flag is set, the auto hide title bar has a minimize button to collapse the dock widget.
			AutoHideHasMinimizeButton = 0x100,
			/// @brief If this flag is set, dragging hover the tab bar will open the dock.
			AutoHideOpenOnDragHover = 0x200,
			/// @brief If this flag is set, the auto hide dock container will collapse if the user clicks outside of the container, if not set, the auto hide container can be closed only via click on sidebar tab.
			AutoHideCloseOnOutsideMouseClick = 0x400,

			/// @brief Default auto hide configuration.
			DefaultAutoHideConfig = AutoHideFeatureEnabled | DockAreaHasAutoHideButton | AutoHideHasMinimizeButton | AutoHideCloseOnOutsideMouseClick
		};
		Q_DECLARE_FLAGS(AutoHideFlags, eAutoHideFlag)

		/**
		 * @brief Global configuration parameters that you can set via setConfigParam().
		 */
		enum eConfigParam
		{
			/// @brief Delay in ms before the dock opens on drag hover if AutoHideOpenOnDragHover flag is set.
			AutoHideOpenOnDragHoverDelay_ms,
			/// @brief Just a delimiter to count number of config params.
			ConfigParamCount
		};

		/**
		 * @brief Creates a new dock widget with the specified title and optional parent
		 * widget.
		 *
		 * The new dock widget will be managed by the dock manager, and its lifetime
		 * will be tied to the dock manager. If a parent widget is provided, the dock
		 * widget will be created as a child of the parent widget. If no parent widget
		 * is provided, the dock widget will be created as a top-level widget.
		 *
		 * @param title The title of the dock widget.
		 * @param parent The parent widget, if any. Defaults to nullptr.
		 * @return Returns a pointer to the created CDockWidget.
		 */
		CDockWidget* createDockWidget(const QString& title, QWidget* parent = nullptr);

		/**
		 * @brief Returns the dock manager specific factory for creating components of
		 * fock widgets.
		 */
		QSharedPointer<ads::CDockComponentsFactory> componentsFactory() const;

		/**
		 * @brief Sets a custom factory for creating components of dock widgets.
		 * The pointer is stored internally into a shared pointer so you should not
		 * delete the given factory object as long as it is used by the dock manager.
		 */
		void setComponentsFactory(ads::CDockComponentsFactory* Factory);

		/**
		 * @brief Sets a custom factory for creating components of dock widgets.
		 */
		void setComponentsFactory(QSharedPointer<ads::CDockComponentsFactory>);

		/**
		 * @brief This function returns the global configuration flags.
		 */
		static ConfigFlags configFlags();

		/**
		 * @brief This function returns the auto hide configuration flags.
		 */
		static AutoHideFlags autoHideConfigFlags();

		/**
		 * @brief Sets the global configuration flags for the whole docking system.
		 * Call this function before you create the dock manager and before
		 * your create the first dock widget.
		 */
		static void setConfigFlags(const ConfigFlags Flags);

		/**
		 * @brief Sets the global configuration flags for the whole docking system.
		 * Call this function before you create the dock manager and before
		 * your create the first dock widget.
		 */
		static void setAutoHideConfigFlags(const AutoHideFlags Flags);

		/**
		 * @brief Set a certain config flag.
		 * @see setConfigFlags()
		 */
		static void setConfigFlag(eConfigFlag Flag, bool On = true);

		/**
		 * @brief Set a certain overlay config flag.
		 * @see setConfigFlags()
		 */
		static void setAutoHideConfigFlag(eAutoHideFlag Flag, bool On = true);

		/**
		 * @brief Returns true if the given config flag is set.
		 */
		static bool testConfigFlag(eConfigFlag Flag);

		/**
		 * @brief Returns true if the given overlay config flag is set.
		 */
		static bool testAutoHideConfigFlag(eAutoHideFlag Flag);

		/**
		 * @brief Sets the value for the given config parameter.
		 */
		static void setConfigParam(eConfigParam Param, QVariant Value);

		/**
		 * @brief Returns the value for the given config parameter or the default value
		 * if the parameter is not set.
		 */
		static QVariant configParam(eConfigParam Param, QVariant Default);

		/**
		 * @brief Returns the global icon provider.
		 * The icon provider enables the use of custom icons in case using
		 * styleheets for icons is not an option.
		 */
		static CIconProvider& iconProvider();

		/**
		 * @brief Returns if current application palette is dark.
		 */
		static bool isApplicationPaletteDark();

		/**
		 * @brief Returns if currently applied stylesheet is supposed to be dark.
		 */
		bool isDesiredStylesheetDark();

		/**
		 * @brief Adds dockwidget into the given area.
		 * If DockAreaWidget is not null, then the area parameter indicates the area
		 * into the DockAreaWidget. If DockAreaWidget is null, the Dockwidget will
		 * be dropped into the container. If you would like to add a dock widget
		 * tabified, then you need to add it to an existing dock area object
		 * into the CenterDockWidgetArea. The following code shows this:.
		 * \code
		 * DockManager->addDockWidget(ads::CenterDockWidgetArea, NewDockWidget,
		 * 	   ExisitingDockArea);
		 * \endcode
		 * @return Returns the dock area widget that contains the new DockWidget.
		 */
		CDockAreaWidget* addDockWidget(DockWidgetArea area, CDockWidget* Dockwidget, CDockAreaWidget* DockAreaWidget = nullptr, int Index = -1);

		/**
		 * @brief Adds dockwidget into the given container.
		 * This allows you to place the dock widget into a container, even if that
		 * container does not yet contain a DockAreaWidget.
		 * @return Returns the dock area widget that contains the new DockWidget.
		 */
		CDockAreaWidget* addDockWidgetToContainer(DockWidgetArea area, CDockWidget* Dockwidget, CDockContainerWidget* DockContainerWidget);

		/**
		 * @brief Adds an Auto-Hide widget to the dock manager container pinned to
		 * the given side bar location.
		 * @return Returns the CAutoHideDockContainer that contains the new DockWidget.
		 */
		CAutoHideDockContainer* addAutoHideDockWidget(SideBarLocation Location, CDockWidget* Dockwidget);

		/**
		 * @brief Adds an Auto-Hide widget to the given DockContainerWidget pinned to
		 * the given side bar location in this container.
		 * @return Returns the CAutoHideDockContainer that contains the new DockWidget.
		 */
		CAutoHideDockContainer* addAutoHideDockWidgetToContainer(SideBarLocation Location, CDockWidget* Dockwidget, CDockContainerWidget* DockContainerWidget);

		/**
		 * @brief This function will add the given Dockwidget to the given dock area as
		 * a new tab.
		 * If no dock area widget exists for the given area identifier, a new
		 * dock area widget is created.
		 */
		CDockAreaWidget* addDockWidgetTab(DockWidgetArea area, CDockWidget* Dockwidget);

		/**
		 * @brief This function will add the given Dockwidget to the given DockAreaWidget
		 * as a new tab.
		 * If index is out of range, the tab is simply appended. Otherwise it is
		 * inserted at the specified position.
		 */
		CDockAreaWidget* addDockWidgetTabToArea(CDockWidget* Dockwidget, CDockAreaWidget* DockAreaWidget, int Index = -1);

		/**
		 * @brief Adds the given DockWidget floating and returns the created
		 * CFloatingDockContainer instance.
		 */
		CFloatingDockContainer* addDockWidgetFloating(CDockWidget* Dockwidget);

		/**
		 * @brief Searches for a registered doc widget with the given ObjectName.
		 * @return Return the found dock widget or nullptr if a dock widget with the
		 * given name is not registered.
		 */
		CDockWidget* findDockWidget(const QString& ObjectName) const;

		/**
		 * @brief Remove the given Dock from the dock manager.
		 */
		void removeDockWidget(CDockWidget* Dockwidget);

		/**
		 * @brief This function returns a readable reference to the internal dock
		 * widgets map so that it is possible to iterate over all dock widgets.
		 */
		QMap<QString, CDockWidget*> dockWidgetsMap() const;

		/**
		 * @brief Returns the list of all active and visible dock containers
		 * Dock containers are the main dock manager and all floating widgets.
		 */
		const QList<CDockContainerWidget*> dockContainers() const;

		/**
		 * @brief Returns the list of all floating widgets.
		 */
		const QList<CFloatingDockContainer*> floatingWidgets() const;

		/**
		 * @brief This function always return 0 because the main window is always behind
		 * any floating widget.
		 */
		unsigned int zOrderIndex() const override;

		/**
		 * @brief Saves the current state of the dockmanger and all its dock widgets
		 * into the returned QByteArray.
		 * The XmlMode enables / disables the auto formatting for the XmlStreamWriter.
		 * If auto formatting is enabled, the output is intended and line wrapped.
		 * The XmlMode XmlAutoFormattingDisabled is better if you would like to have
		 * a more compact XML output - i.e. for storage in ini files.
		 * The version number is stored as part of the data.
		 * To restore the saved state, pass the return value and version number
		 * to restoreState().
		 * @see restoreState()
		 */
		QByteArray saveState(int version = 0) const;

		/**
		 * @brief Restores the state of this dockmanagers dockwidgets.
		 * The version number is compared with that stored in state. If they do
		 * not match, the dockmanager's state is left unchanged, and this function
		 * returns false; otherwise, the state is restored, and this function
		 * returns true.
		 * @see saveState()
		 */
		bool restoreState(const QByteArray& state, int version = 0);

		/**
		 * @brief Saves the current perspective to the internal list of perspectives.
		 * A perspective is the current state of the dock manager assigned
		 * with a certain name. This makes it possible for the user,
		 * to switch between different perspectives quickly.
		 * If a perspective with the given name already exists, then
		 * it will be overwritten with the new state.
		 */
		void addPerspective(const QString& UniquePrespectiveName);

		/**
		 * @brief Removes the perspective with the given name from the list of perspectives.
		 */
		void removePerspective(const QString& Name);

		/**
		 * @brief Removes the given perspectives from the dock manager.
		 */
		void removePerspectives(const QStringList& Names);

		/**
		 * @brief Returns the names of all available perspectives.
		 */
		QStringList perspectiveNames() const;

		/**
		 * @brief Saves the perspectives to the given settings file.
		 */
		void savePerspectives(QSettings& Settings) const;

		/**
		 * @brief Loads the perspectives from the given settings file.
		 */
		void loadPerspectives(QSettings& Settings);

		/**
		 * @brief This function returns managers central widget or nullptr if no central
		 * widget is set.
		 */
		CDockWidget* centralWidget() const;

		/**
		 * @brief Adds dockwidget widget into the central area and marks it as central
		 * widget. If central widget is set, it will be the only dock widget that will
		 * resize with the dock container. A central widget if not movable, floatable
		 * or closable and the titlebar of the central dock area is not visible. If
		 * the given widget could be set as central widget, the function returns the
		 * created dock area. If the widget could not be set, because there is already
		 * a central widget, this function returns a nullptr. To clear the central
		 * widget, pass a nullptr to the function.
		 * @note Setting a central widget is only possible if no other dock widgets
		 * have been registered before. That means, this function should be the
		 * first function that you call before you add other dock widgets.
		 * @retval != 0 The dock area that contains the central widget
		 * @retval nullptr Indicates that the given widget can not be set as central
		 *         widget because there is already a central widget.
		 */
		CDockAreaWidget* setCentralWidget(CDockWidget* widget);

		/**
		 * @brief Adds a toggle view action to the the internal view menu.
		 * You can either manage the insertion of the toggle view actions in your
		 * application or you can add the actions to the internal view menu and
		 * then simply insert the menu object into your.
		 * @param[in] ToggleViewAction The action to insert. If no group is provided
		 *            the action is directly inserted into the menu. If a group
		 *            is provided, the action is inserted into the group and the
		 *            group is inserted into the menu if it is not existing yet.
		 * @param[in] Group This is the text used for the group menu item.
		 * @param[in] GroupIcon The icon used for grouping the workbenches in the
		 *            view menu. I.e. if there is a workbench for each device
		 *            like for spectrometer devices, it is good to group all these
		 *            workbenches under a menu item.
		 * @return If Group is not empty, this function returns the GroupAction
		 *         for this group. If the group is empty, the function returns
		 *         the given ToggleViewAction.
		 */
		QAction* addToggleViewActionToMenu(QAction* ToggleViewAction, const QString& Group = QString(), const QIcon& GroupIcon = QIcon());

		/**
		 * @brief This function returns the internal view menu.
		 * To fill the view menu, you can use the addToggleViewActionToMenu()
		 * function.
		 */
		QMenu* viewMenu() const;

		/**
		 * @brief Define the insertion order for toggle view menu items.
		 * The order defines how the actions are added to the view menu.
		 * The default insertion order is MenuAlphabeticallySorted to make it
		 * easier for users to find the menu entry for a certain dock widget.
		 * You need to call this function before you insert the first menu item
		 * into the view menu.
		 */
		void setViewMenuInsertionOrder(eViewMenuInsertionOrder Order);

		/**
		 * @brief Define the behavior of stylesheet color scheme selection.
		 * The stylesheet can be fixed to either light or dark scheme,
		 * or it can follow the current application palette (default).
		 * Note: The fixed settings implement legacy behavior (before
		 * dark scheme was implemented) including problems like missing
		 * palette change propagation. They are implemented solely
		 * for compatibility reasons and manual stylesheet switching.
		 */
		void setColorSchemeMode(ColorSchemeMode Mode);

		/**
		 * @brief This function returns true between the restoringState() and
		 * stateRestored() signals.
		 */
		bool isRestoringState() const;

		/**
		 * @brief This function returns true, if the DockManager window is restoring from
		 * minimized state.
		 * The DockManager is in this state starting from the QWindowStateChangeEvent
		 * that signals the state change from minimized to normal until
		 * endLeavingMinimizedState() function is called.
		 */
		bool isLeavingMinimizedState() const;

		/**
		 * @brief The distance the user needs to move the mouse with the left button
		 * hold down before a dock widget start floating.
		 */
		static int startDragDistance();

		/**
		 * @brief Helper function to set focus depending on the configuration of the
		 * FocusStyling flag.
		 */
		template<class QWidgetPtr>
		/**
		 * @brief Sets the widget focus.
		 */
		static void setWidgetFocus(QWidgetPtr widget)
		{
			if (!CDockManager::testConfigFlag(CDockManager::FocusHighlighting))
			{
				return;
			}

			widget->setFocus(Qt::OtherFocusReason);
		}

		/**
		 * @brief Filters events of watched objects.
		 */
		bool eventFilter(QObject* obj, QEvent* e) override;

		/**
		 * @brief Returns the dock widget that has focus style in the ui or a nullptr if
		 * not dock widget is painted focused.
		 * If the flag FocusHighlighting is disabled, this function always returns
		 * nullptr.
		 */
		CDockWidget* focusedDockWidget() const;

		/**
		 * @brief Returns the sizes of the splitter that contains the dock area.
		 *
		 * If there is no splitter that contains the area, an empty list will be
		 * returned.
		 */
		QList<int> splitterSizes(CDockAreaWidget* ContainedArea) const;

		/**
		 * @brief Update the sizes of a splitter
		 * Programmatically updates the sizes of a given splitter by calling
		 * QSplitter::setSizes(). The splitter will be the splitter that
		 * contains the supplied dock area widget. If there is not splitter
		 * that contains the dock area, or the sizes supplied does not match
		 * the number of children of the splitter, this method will have no
		 * effect.
		 */
		void setSplitterSizes(CDockAreaWidget* ContainedArea, const QList<int>& sizes);

		/**
		 * @brief Set a custom title for all FloatingContainer that does not reflect
		 * the title of the current dock widget.
		 */
		static void setFloatingContainersTitle(const QString& Title);

		/**
		 * @brief Returns the title used by all FloatingContainer that does not
		 * reflect the title of the current dock widget.
		 *
		 * If not title was set with setFloatingContainersTitle(), it returns
		 * QGuiApplication::applicationDisplayName().
		 */
		static QString floatingContainersTitle();

		/**
		 * @brief This function sets the tool button style for the given dock widget state.
		 * It is possible to switch the tool button style depending on the state.
		 * If a dock widget is floating, then here are more space and it is
		 * possible to select a style that requires more space like
		 * Qt::ToolButtonTextUnderIcon. For the docked state Qt::ToolButtonIconOnly
		 * might be better.
		 */
		void setDockWidgetToolBarStyle(Qt::ToolButtonStyle Style, CDockWidget::eState State);

		/**
		 * @brief Returns the tool button style for the given docking state.
		 * @see setToolBarStyle()
		 */
		Qt::ToolButtonStyle dockWidgetToolBarStyle(CDockWidget::eState State) const;

		/**
		 * @brief This function sets the tool button icon size for the given state.
		 * If a dock widget is floating, there is more space and increasing the
		 * icon size is possible. For docked widgets, small icon sizes, eg. 16 x 16
		 * might be better.
		 */
		void setDockWidgetToolBarIconSize(const QSize& IconSize, CDockWidget::eState State);

		/**
		 * @brief Returns the icon size for a given docking state.
		 * @see setToolBarIconSize()
		 */
		QSize dockWidgetToolBarIconSize(CDockWidget::eState State) const;

		/**
		 * @brief Returns all dock widget features that are globally locked by the dock
		 * manager.
		 * Globally locked features are removed from the features of all dock
		 * widgets.
		 */
		CDockWidget::DockWidgetFeatures globallyLockedDockWidgetFeatures() const;

		/**
		 * @brief Globally Lock features of all dock widgets to "freeze" the current
		 * workspace layout.
		 * For example, it is now possible to lock the workspace to avoid
		 * accidentally dragging a docked view. Locking wasn’t possible before.
		 * So, users had to manually dock it back to the desired place after
		 * each accidental undock.
		 * You can use a combination of the following feature flags:
		 * - CDockWidget::DockWidgetClosable
		 * - CDockWidget::DockWidgetMovable
		 * - CDockWidget::DockWidgetFloatable
		 * - CDockWidget::DockWidgetPinable.
		 *
		 * To clear the locked features, you can use CDockWidget::NoDockWidgetFeatures
		 * The following code shows how to lock and unlock dock widget features
		 * globally.
		 *
		 * \code
		 * DockManager->lockDockWidgetFeaturesGlobally();
		 * DockManager->lockDockWidgetFeaturesGlobally(CDockWidget::NoDockWidgetFeatures);
		 * \endcode
		 */
		void lockDockWidgetFeaturesGlobally(CDockWidget::DockWidgetFeatures Features = CDockWidget::GloballyLockableFeatures);

	public Q_SLOTS:
		/**
		 * @brief Opens the perspective with the given name.
		 */
		void openPerspective(const QString& PerspectiveName);

		/**
		 * @brief Request a focus change to the given dock widget.
		 * This function only has an effect, if the flag CDockManager::FocusStyling
		 * is enabled.
		 */
		void setDockWidgetFocused(CDockWidget* DockWidget);

		/**
		 * @brief Hide CDockManager and all floating widgets (See Issue #380). Calling
		 * regular QWidget::hide() hides the CDockManager but not the floating
		 * widgets;.
		 */
		void hideManagerAndFloatingWidgets();

		/**
		 * @brief Calls raise() for the widget that hosts this dock manager.
		 * This will bring the widget in front of any other application that is
		 * running.
		 */
		void raise();

	Q_SIGNALS:
		/**
		 * @brief This signal is emitted if the list of perspectives changed.
		 * The list of perspectives changes if perspectives are added, removed
		 * or if the perspective list has been loaded.
		 */
		void perspectiveListChanged();

		/**
		 * @brief This signal is emitted if the perspective list has been loaded.
		 */
		void perspectiveListLoaded();

		/**
		 * @brief This signal is emitted if perspectives have been removed.
		 */
		void perspectivesRemoved();

		/**
		 * @brief This signal is emitted, if the restore function is called, just before
		 * the dock manager starts restoring the state.
		 * If this function is called, nothing has changed yet.
		 */
		void restoringState();

		/**
		 * @brief This signal is emitted if the state changed in restoreState.
		 * The signal is emitted if the restoreState() function is called or
		 * if the openPerspective() function is called.
		 */
		void stateRestored();

		/**
		 * @brief This signal is emitted, if the dock manager starts opening a
		 * perspective.
		 * Opening a perspective may take more than a second if there are
		 * many complex widgets. The application may use this signal
		 * to show some progress indicator or to change the mouse cursor
		 * into a busy cursor.
		 */
		void openingPerspective(const QString& PerspectiveName);

		/**
		 * @brief This signal is emitted if the dock manager finished opening a
		 * perspective.
		 */
		void perspectiveOpened(const QString& PerspectiveName);

		/**
		 * @brief This signal is emitted, if a new floating widget has been created.
		 * An application can use this signal to e.g. subscribe to events of
		 * the newly created window.
		 */
		void floatingWidgetCreated(ads::CFloatingDockContainer* FloatingWidget);

		/**
		 * @brief This signal is emitted, if a new DockArea has been created.
		 * An application can use this signal to set custom icons or custom
		 * tooltips for the DockArea buttons.
		 */
		void dockAreaCreated(ads::CDockAreaWidget* DockArea);

		/**
		 * @brief This signal is emitted if a dock widget has been added to this
		 * dock manager instance.
		 */
		void dockWidgetAdded(ads::CDockWidget* DockWidget);

		/**
		 * @brief This signal is emitted just before the given dock widget is removed
		 * from the dock manager.
		 */
		void dockWidgetAboutToBeRemoved(ads::CDockWidget* DockWidget);

		/**
		 * @brief This signal is emitted if a dock widget has been removed with the remove
		 * removeDockWidget() function.
		 * If this signal is emitted, the dock widget has been removed from the
		 * docking system but it is not deleted yet.
		 */
		void dockWidgetRemoved(ads::CDockWidget* DockWidget);

		/**
		 * @brief This signal is emitted if the focused dock widget changed.
		 * Both old and now can be nullptr.
		 * The focused dock widget is the one that is highlighted in the GUI.
		 */
		void focusedDockWidgetChanged(ads::CDockWidget* old, ads::CDockWidget* now);

	private:
		friend struct DockManagerPrivate;
		friend class CFloatingDockContainer;
		friend struct FloatingDockContainerPrivate;
		friend class CDockContainerWidget;
		friend class DockContainerWidgetPrivate;
		friend class CDockAreaTabBar;
		friend class CDockWidgetTab;
		friend struct DockAreaWidgetPrivate;
		friend struct DockWidgetTabPrivate;
		friend class CFloatingDragPreview;
		friend struct FloatingDragPreviewPrivate;
		friend class CDockAreaTitleBar;
		friend class CAutoHideDockContainer;
		/// @brief The c auto hide side bar.
		friend CAutoHideSideBar;
		/// @brief The c auto hide tab.
		friend CAutoHideTab;
		/// @brief The auto hide tab private.
		friend AutoHideTabPrivate;
		/// Private data (pimpl).
		DockManagerPrivate* d;
};
}// namespace ads

Q_DECLARE_OPERATORS_FOR_FLAGS(ads::CDockManager::ConfigFlags)

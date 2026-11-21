#pragma once

/// @file   CDockFocusController.h
/// @author Uwe Kindler
/// @date   05.06.2020
/// @brief  Declaration of CDockFocusController class.

#include <QObject>

#include "CDockManager.h"
#include "ads_globals.h"

namespace ads
{
struct DockFocusControllerPrivate;
class CDockManager;
class CFloatingDockContainer;

/**
 * @brief Manages focus styling of dock widgets and handling of focus changes.
 */
class ADS_EXPORT CDockFocusController : public QObject
{
		Q_OBJECT

	public:
		/**
		 * @brief Default Constructor.
		 */
		CDockFocusController(CDockManager* DockManager);

		/**
		 * @brief Virtual Destructor.
		 */
		~CDockFocusController() override;

	private Q_SLOTS:
		/**
		 * @brief Slot invoked on application focus changed.
		 */
		void onApplicationFocusChanged(QWidget* old, QWidget* now);
		/**
		 * @brief Slot invoked on focus window changed.
		 */
		void onFocusWindowChanged(QWindow* focusWindow);
		/**
		 * @brief Slot invoked on focused dock area view toggled.
		 */
		void onFocusedDockAreaViewToggled(bool Open);
		/**
		 * @brief Slot handling: state restored.
		 */
		void onStateRestored();
		/**
		 * @brief Slot invoked on dock widget visibility changed.
		 */
		void onDockWidgetVisibilityChanged(bool Visible);

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = QObject;

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
		 * @brief Returns the dock widget that has focus style in the ui or a nullptr if
		 * not dock widget is painted focused.
		 */
		CDockWidget* focusedDockWidget() const;

		/**
		 * @brief Returns the dock area that contains the focusedDockWidget() or nullptr if
		 * the focused dock widget is not in this area.
		 */
		CDockAreaWidget* focusedDockArea() const;

		/**
		 * @brief Request focus highlighting for the given dock widget assigned to the tab
		 * given in Tab parameter.
		 */
		void setDockWidgetTabFocused(CDockWidgetTab* Tab);

		/*
		 * Request clear focus for a dock widget
		 */
		/**
		 * @brief Implements clear dock widget focus.
		 */
		void clearDockWidgetFocus(CDockWidget* dockWidget);

		/**
		 * @brief Notifies the dock focus controller, that a the mouse is pressed or
		 * released.
		 */
		void setDockWidgetTabPressed(bool Value);

	public Q_SLOTS:
		/**
		 * @brief Request a focus change to the given dock widget.
		 */
		void setDockWidgetFocused(CDockWidget* focusedNow);

	private:
		friend struct DockFocusControllerPrivate;
		/// Private data (pimpl).
		DockFocusControllerPrivate* d;
};
}// namespace ads

// namespace ads

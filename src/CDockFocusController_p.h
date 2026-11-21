
#pragma once

/// @file   CDockFocusController_p.h
/// @author Uwe Kindler
/// @date   05.06.2020
/// @brief  Private data of CDockFocusController class.

#include "CDockFocusController.h"

#include <QAbstractButton>
#include <QApplication>
#include <QPointer>
#include <QWindow>

#include <CAutoHideTab.h>

#include <algorithm>
#include <iostream>

#include "CDockAreaTitleBar.h"
#include "CDockAreaWidget.h"
#include "CDockContainerWidget.h"
#include "CDockManager.h"
#include "CDockWidget.h"
#include "CDockWidgetTab.h"
#include "CFloatingDockContainer.h"

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
	#include "linux/CFloatingWidgetTitleBar.h"
#endif

namespace ads
{
/**
 * @brief The focused dock widget property.
 */
static const char* const FocusedDockWidgetProperty = "FocusedDockWidget";

/**
 * @brief Private data class of CDockFocusController class (pimpl).
 */
struct DockFocusControllerPrivate
{

		/**
		 * @brief Private data constructor.
		 */
		DockFocusControllerPrivate(CDockFocusController* _public);

		/**
		 * @brief This function updates the focus style of the given dock widget and
		 * the dock area that it belongs to.
		 */
		void updateDockWidgetFocus(CDockWidget* DockWidget);
		/// @brief The this.
		CDockFocusController* _this;
		/// @brief The focused dock widget.
		QPointer<CDockWidget> FocusedDockWidget = nullptr;
		/// @brief The focused area.
		QPointer<CDockAreaWidget> FocusedArea = nullptr;
		/// @brief The old focused dock widget.
		QPointer<CDockWidget> OldFocusedDockWidget = nullptr;
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
		QPointer<CFloatingDockContainer> FloatingWidget = nullptr;
#endif
		/// @brief The dock manager.
		CDockManager* DockManager;
		/// @brief The force focus changed signal.
		bool ForceFocusChangedSignal = false;
		/// @brief The tab pressed.
		bool TabPressed = false;
};
}// namespace ads

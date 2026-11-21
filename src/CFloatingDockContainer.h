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

/// @file   CFloatingDockContainer.h
/// @author Uwe Kindler
/// @date   01.03.2017
/// @brief  Declaration of CFloatingDockContainer class.

#include "IFloatingWidget.h"
#include "ads_globals.h"

#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
	#include <QDockWidget>
	#define tFloatingWidgetBase QDockWidget
#else
	#include <QWidget>
	/**
	 * @brief Macro for floating widget base.
	 */
	#define tFloatingWidgetBase QWidget
#endif

QT_FORWARD_DECLARE_CLASS(CDockingStateReader);
QT_FORWARD_DECLARE_CLASS(QMimeData)

namespace ads
{
struct FloatingDockContainerPrivate;
class CDockManager;
struct DockManagerPrivate;
class CDockAreaWidget;
class CDockContainerWidget;
class CDockWidget;
class CDockAreaTabBar;
class CDockWidgetTab;
struct DockWidgetTabPrivate;
class CDockAreaTitleBar;
struct DockAreaTitleBarPrivate;
class CFloatingWidgetTitleBar;
class CDockingStateReader;
class CFloatingDragPreview;

/**
 * @brief This implements a floating widget that is a dock container that accepts
 * docking of dock widgets like the main window and that can be docked into
 * another dock container.
 * Every floating window of the docking system is a FloatingDockContainer.
 */
class ADS_EXPORT CFloatingDockContainer
	: public tFloatingWidgetBase
	, public IFloatingWidget
{
		Q_OBJECT
	private:
		/**
		 * @brief Returns the floating widget that is transported in the mime data of
		 * a drag and drop operation started by startPlatformDrag() and nullptr,
		 * if the mime data does not contain a floating widget.
		 */
		static CFloatingDockContainer* floatingWidgetFromMimeData(const QMimeData* MimeData);

		/**
		 * @brief Records the drop candidate while a platform drag is hovering over a
		 * dock container. Called from CDockContainerWidget::dragMoveEvent().
		 * Some compositors (e.g. Mutter) do not deliver a drop event when the
		 * dragged window overlaps the drop target. startPlatformDrag() then docks
		 * the floating widget into the last recorded candidate, if the drag ended
		 * over a valid drop area.
		 */
		static void platformDragUpdateDropCandidate(CDockContainerWidget* Container, const QPoint& GlobalPos, bool ValidDropArea);

		/**
		 * @brief Marks the platform drag drop as handled by a delivered drop event, so
		 * that startPlatformDrag() does not dock the floating widget a second time.
		 */
		static void platformDragNotifyDropHandled();

		/**
		 * @brief Wayland: returns the concatenated style sheets along the dock manager
		 * parent chain. A floating widget has no parent widget on Wayland, so it
		 * does not inherit the dock manager's effective style sheet through the
		 * widget hierarchy and gets this style sheet applied explicitly instead
		 * (an application wide qApp style sheet is still applied automatically by
		 * Qt and is therefore not included here). Used at construction and when
		 * the dock manager re-applies the style sheet after a style change.
		 */
		static QString waylandInheritedStyleSheet(QWidget* DockManager);

	private Q_SLOTS:
		/**
		 * @brief Slot invoked on dock areas added or removed.
		 */
		void onDockAreasAddedOrRemoved();
		/**
		 * @brief Slot invoked on dock area current changed.
		 */
		void onDockAreaCurrentChanged(int Index);

	public:
		/**
		 * @brief Starts floating at the given global position.
		 * Use moveToGlobalPos() to move the widget to a new position
		 * depending on the start position given in Pos parameter.
		 */
		void startFloating(const QPoint& DragStartMousePos, const QSize& Size, eDragState DragState, QWidget* MouseEventHandler) override;

		/**
		 * @brief Call this function if you explicitly want to signal that dragging has
		 * finished.
		 */
		void finishDragging() override;

	protected:
		/**
		 * @brief This function deletes all dock widgets in it.
		 * This functions should be called only from dock manager in its
		 * destructor before deleting the floating widget.
		 */
		void deleteContent();

		/**
		 * @brief Call this function if you just want to initialize the position
		 * and size of the floating widget.
		 */
		void initFloatingGeometry(const QPoint& DragStartMousePos, const QSize& Size)
		{
			startFloating(DragStartMousePos, Size, DraggingInactive, nullptr);
		}

	public:
		/**
		 * @brief Moves the widget to a new position relative to the position given when
		 * startFloating() was called.
		 */
		void moveFloating() override;

	protected:
		/**
		 * @brief Restores the state from given stream.
		 * If Testing is true, the function only parses the data from the given
		 * stream but does not restore anything. You can use this check for
		 * faulty files before you start restoring the state.
		 */
		bool restoreState(CDockingStateReader& Stream, bool Testing);

		/**
		 * @brief Call this function to update the window title.
		 */
		void updateWindowTitle();

		// reimplements QWidget
		/**
		 * @brief Handles the change event.
		 */
		void changeEvent(QEvent* event) override;
		/**
		 * @brief Handles the close event.
		 */
		void closeEvent(QCloseEvent* event) override;
		/**
		 * @brief Handles the hide event.
		 */
		void hideEvent(QHideEvent* event) override;
		/**
		 * @brief Handles the show event.
		 */
		void showEvent(QShowEvent* event) override;

#ifdef Q_OS_MACOS
		/**
		 * @brief Handles generic events.
		 */
		bool event(QEvent* e) override;
		/**
		 * @brief Handles the move event.
		 */
		void moveEvent(QMoveEvent* event) override;
#elif defined(Q_OS_UNIX) || defined(Q_OS_WIN)
		/**
		 * @brief Handles the move event.
		 */
		void moveEvent(QMoveEvent* event) override;
		/**
		 * @brief Handles the resize event.
		 */
		void resizeEvent(QResizeEvent* event) override;
		/**
		 * @brief Handles generic events.
		 */
		bool event(QEvent* e) override;

		// Edge/corner resize support (Linux and Windows under Wine)
		/**
		 * @brief Handles the mouse press event.
		 */
		void mousePressEvent(QMouseEvent* event) override;
		/**
		 * @brief Handles the mouse move event.
		 */
		void mouseMoveEvent(QMouseEvent* event) override;
		/**
		 * @brief Handles the mouse release event.
		 */
		void mouseReleaseEvent(QMouseEvent* event) override;
		/**
		 * @brief Handles the leave event.
		 */
		void leaveEvent(QEvent* event) override;
		/**
		 * @brief Filters events of watched objects.
		 */
		bool eventFilter(QObject* watched, QEvent* event) override;
#endif

#ifdef Q_OS_WIN
		/**
		 * @brief Native event filter for handling WM_MOVING messages on Windows.
		 */
	#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
		/**
		 * @brief Handles the native event.
		 */
		bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;
	#else
		/**
		 * @brief Handles the native event.
		 */
		bool nativeEvent(const QByteArray& eventType, void* message, qintptr* result) override;
	#endif
#endif

	public:
		/**
		 * @brief Alias for the base class.
		 */
		using Super = tFloatingWidgetBase;

		/**
		 * @brief Create empty floating widget - required for restore state.
		 */
		CFloatingDockContainer(CDockManager* DockManager);

		/**
		 * @brief Create floating widget with the given dock area.
		 */
		CFloatingDockContainer(CDockAreaWidget* DockArea);

		/**
		 * @brief Create floating widget with the given dock widget.
		 */
		CFloatingDockContainer(CDockWidget* DockWidget);

		/**
		 * @brief Virtual Destructor.
		 */
		~CFloatingDockContainer() override;

		/**
		 * @brief Access function for the internal dock container.
		 */
		CDockContainerWidget* dockContainer() const;

		/**
		 * @brief Call this function to start dragging the floating widget.
		 */
		void startDragging(const QPoint& DragStartMousePos, const QSize& Size, QWidget* MouseEventHandler)
		{
			startFloating(DragStartMousePos, Size, DraggingFloatingWidget, MouseEventHandler);
		}

		/**
		 * @brief Starts a compositor driven drag of the given floating widget on
		 * Wayland.
		 * Wayland neither reports the global cursor position nor allows clients
		 * to move top level windows, so the mouse tracked dragging that is used
		 * on the other platforms cannot work. Instead, this function executes a
		 * QDrag with the MIME types that QWaylandDataDevice translates into an
		 * xdg_toplevel_drag_v1 request, so the compositor moves the floating
		 * widget with the cursor. The CDockContainerWidget drag and drop event
		 * handlers show the drop overlays and dock the widget on drop.
		 * GlobalPressPos is the mouse press position in global coordinates and
		 * DragSource is the widget that received the mouse press event.
		 * DragOffset, when non-null, is the surface-local point of the floating
		 * widget that should stay under the cursor (the xdg_toplevel_drag attach
		 * offset). It must be supplied for a freshly created floating window: on
		 * Wayland such a window has no meaningful geometry yet, so deriving the
		 * offset from GlobalPressPos via mapFromGlobal() would place it randomly.
		 * When null (e.g. dragging an already-mapped floating window) the offset is
		 * derived from GlobalPressPos.
		 * The function blocks until the user drops or cancels the drag. It
		 * returns Qt::MoveAction, if the floating widget was docked into a
		 * drop area and Qt::IgnoreAction, if the floating widget was dropped
		 * outside of any drop area and remains floating.
		 */
		static Qt::DropAction
		/**
		 * @brief Starts a platform specific drag operation.
		 */
		startPlatformDrag(CFloatingDockContainer* FloatingWidget, const QPoint& GlobalPressPos, QWidget* DragSource, const QPoint* DragOffset = nullptr);

		/**
		 * @brief Wayland hybrid drag helper, shared by the tab and the title bar drag
		 * paths. While the cursor stays inside SourceWindow the in-window Preview
		 * is moved to GlobalPos and this returns true. Once the cursor leaves
		 * SourceWindow the Preview is torn down without performing a drop and this
		 * returns false - the caller then creates the real floating widget and
		 * hands it to startPlatformDragForFloatingWidget(). Preview may be null.
		 * If the window is not floatable and the cursor is outside the contained
		 * source window, the preview drag operation will not be cancelled. It will
		 * return true, leaving the preview window frozen until the cursor reenters
		 * the source window or the drag operation is cancelled.
		 */
		static bool waylandMoveOrLeaveInWindowPreview(CFloatingDragPreview* Preview, QWidget* SourceWindow, const QPoint& GlobalPos, const bool Floatable);

		/**
		 * @brief Wayland hybrid drag helper, shared by the tab and the title bar drag
		 * paths. Floats the freshly created FloatingWidget at the given Size and
		 * starts the compositor driven platform drag, keeping GrabOffset (the
		 * surface-local grab point) under the cursor. See startPlatformDrag() for
		 * why the offset is passed explicitly instead of being derived from the
		 * press position on Wayland.
		 */
		static Qt::DropAction startPlatformDragForFloatingWidget(
			IFloatingWidget* FloatingWidget, const QPoint& GrabOffset, const QSize& Size, const QPoint& GlobalGrabPos, QWidget* DragSource
		);

		/**
		 * @brief This function returns true, if it can be closed.
		 * It can be closed, if all dock widgets in all dock areas can be closed.
		 */
		bool isClosable() const;

		/**
		 * @brief This function returns true, if this floating widget has only one single
		 * visible dock widget in a single visible dock area.
		 * The single dock widget is a real top level floating widget because no
		 * other widgets are docked.
		 */
		bool hasTopLevelDockWidget() const;

		/**
		 * @brief This function returns the first dock widget in the first dock area.
		 * If the function hasSingleDockWidget() returns true, then this function
		 * returns this single dock widget.
		 */
		CDockWidget* topLevelDockWidget() const;

		/**
		 * @brief This function returns a list of all dock widget in this floating widget.
		 * This is a simple convenience function that simply calls the dockWidgets()
		 * function of the internal container widget.
		 */
		QList<CDockWidget*> dockWidgets() const;

		/**
		 * @brief This function hides the floating widget instantly and delete it later.
		 */
		void finishDropOperation();

		/**
		 * @brief Returns true while this floating widget is actively being dragged.
		 */
		bool isDraggingActive() const;

#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
		/**
		 * @brief This is a function that responds to
		 * FloatingWidgetTitleBar::maximizeRequest() Maximize or normalize the
		 * container size.
		 */
		void onMaximizeRequest();

		/**
		 * @brief Normalize (Unmaximize) the window.
		 *	fixGeometry parameter fixes a "bug" in QT where immediately after calling
		 * showNormal geometry is not set properly. Set this true when moving the
		 * window immediately after normalizing.
		 */
		void showNormal(bool fixGeometry = false);

		/**
		 * @brief Maximizes the window.
		 * Hides non-virtual method from QWidget.
		 */
		void showMaximized();

		/**
		 * @brief Returns if the window is currently maximized or not.
		 * Hides non-virtual method from QWidget.
		 */
		bool isMaximized() const;

	#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
		/**
		 * @brief Patched show to prevent the window from appearing in the taskbar.
		 * Hides non-virtual method from QWidget.
		 */
		void show();
	#endif

		/**
		 * @brief Returns true if the floating widget has a native titlebar or false if
		 * the floating widget has a QWidget based title bar.
		 */
		bool hasNativeTitleBar();
#endif

#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
		/**
		 * @brief Returns the resize grip width in pixels (for title bar edge detection).
		 */
		int gripWidth() const;

	private:
		/// @brief Detects which window edge(s) or corner the mouse position is over.
		[[nodiscard]] Qt::Edges edgesAt(const QPointF& pos) const;
		/// @brief Updates the cursor shape based on the detected resize edges.
		void updateCursorFor(Qt::Edges edges);
		/// @brief Performs manual window resize when startSystemResize() is.
		/// Unavailable.
		void performManualResize(const QPointF& globalPos);
#endif

	private:
		/// Private data (pimpl).
		FloatingDockContainerPrivate* d;
		friend struct FloatingDockContainerPrivate;
		friend class CDockManager;
		friend struct DockManagerPrivate;
		friend class CDockAreaTabBar;
		friend struct DockWidgetTabPrivate;
		friend class CDockWidgetTab;
		friend class CDockAreaTitleBar;
		friend struct DockAreaTitleBarPrivate;
		friend class CDockWidget;
		friend class CDockAreaWidget;
		friend class CFloatingWidgetTitleBar;
		friend class CDockContainerWidget;
};
}// namespace ads

// namespace ads

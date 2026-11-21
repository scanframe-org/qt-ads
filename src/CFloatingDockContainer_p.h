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

#pragma once

/// @file   CFloatingDockContainer_p.h
/// @author Uwe Kindler
/// @date   01.03.2017
/// @brief  Private data of CFloatingDockContainer class.

#include "CFloatingDockContainer.h"

#include <QApplication>
#include <QMouseEvent>
#include <QPointer>
#include "CDockAreaWidget.h"
#include "CDockContainerWidget.h"
#include "CDockManager.h"
#include "CDockWidget.h"

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

namespace ads
{
#ifdef Q_OS_WIN
	// set to 1 if you need this function for debugging
	#if 0
/**
 * @brief Just for debugging to convert windows message identifiers to strings.
 */
static const char* windowsMessageString(int MessageId)
{
	switch (MessageId)
	{
    case 0: return "WM_NULL";
    case 1: return "WM_CREATE";
    case 2: return "WM_DESTROY";
    case 3: return "WM_MOVE";
    case 5: return "WM_SIZE";
    case 6: return "WM_ACTIVATE";
    case 7: return "WM_SETFOCUS";
    case 8: return "WM_KILLFOCUS";
    case 10: return "WM_ENABLE";
    case 11: return "WM_SETREDRAW";
    case 12: return "WM_SETTEXT";
    case 13: return "WM_GETTEXT";
    case 14: return "WM_GETTEXTLENGTH";
    case 15: return "WM_PAINT";
    case 16: return "WM_CLOSE";
    case 17: return "WM_QUERYENDSESSION";
    case 18: return "WM_QUIT";
    case 19: return "WM_QUERYOPEN";
    case 20: return "WM_ERASEBKGND";
    case 21: return "WM_SYSCOLORCHANGE";
    case 22: return "WM_ENDSESSION";
    case 24: return "WM_SHOWWINDOW";
    case 25: return "WM_CTLCOLOR";
    case 26: return "WM_WININICHANGE";
    case 27: return "WM_DEVMODECHANGE";
    case 28: return "WM_ACTIVATEAPP";
    case 29: return "WM_FONTCHANGE";
    case 30: return "WM_TIMECHANGE";
    case 31: return "WM_CANCELMODE";
    case 32: return "WM_SETCURSOR";
    case 33: return "WM_MOUSEACTIVATE";
    case 34: return "WM_CHILDACTIVATE";
    case 35: return "WM_QUEUESYNC";
    case 36: return "WM_GETMINMAXINFO";
    case 38: return "WM_PAINTICON";
    case 39: return "WM_ICONERASEBKGND";
    case 40: return "WM_NEXTDLGCTL";
    case 42: return "WM_SPOOLERSTATUS";
    case 43: return "WM_DRAWITEM";
    case 44: return "WM_MEASUREITEM";
    case 45: return "WM_DELETEITEM";
    case 46: return "WM_VKEYTOITEM";
    case 47: return "WM_CHARTOITEM";
    case 48: return "WM_SETFONT";
    case 49: return "WM_GETFONT";
    case 50: return "WM_SETHOTKEY";
    case 51: return "WM_GETHOTKEY";
    case 55: return "WM_QUERYDRAGICON";
    case 57: return "WM_COMPAREITEM";
    case 61: return "WM_GETOBJECT";
    case 65: return "WM_COMPACTING";
    case 68: return "WM_COMMNOTIFY";
    case 70: return "WM_WINDOWPOSCHANGING";
    case 71: return "WM_WINDOWPOSCHANGED";
    case 72: return "WM_POWER";
    case 73: return "WM_COPYGLOBALDATA";
    case 74: return "WM_COPYDATA";
    case 75: return "WM_CANCELJOURNAL";
    case 78: return "WM_NOTIFY";
    case 80: return "WM_INPUTLANGCHANGEREQUEST";
    case 81: return "WM_INPUTLANGCHANGE";
    case 82: return "WM_TCARD";
    case 83: return "WM_HELP";
    case 84: return "WM_USERCHANGED";
    case 85: return "WM_NOTIFYFORMAT";
    case 123: return "WM_CONTEXTMENU";
    case 124: return "WM_STYLECHANGING";
    case 125: return "WM_STYLECHANGED";
    case 126: return "WM_DISPLAYCHANGE";
    case 127: return "WM_GETICON";
    case 128: return "WM_SETICON";
    case 129: return "WM_NCCREATE";
    case 130: return "WM_NCDESTROY";
    case 131: return "WM_NCCALCSIZE";
    case 132: return "WM_NCHITTEST";
    case 133: return "WM_NCPAINT";
    case 134: return "WM_NCACTIVATE";
    case 135: return "WM_GETDLGCODE";
    case 136: return "WM_SYNCPAINT";
    case 160: return "WM_NCMOUSEMOVE";
    case 161: return "WM_NCLBUTTONDOWN";
    case 162: return "WM_NCLBUTTONUP";
    case 163: return "WM_NCLBUTTONDBLCLK";
    case 164: return "WM_NCRBUTTONDOWN";
    case 165: return "WM_NCRBUTTONUP";
    case 166: return "WM_NCRBUTTONDBLCLK";
    case 167: return "WM_NCMBUTTONDOWN";
    case 168: return "WM_NCMBUTTONUP";
    case 169: return "WM_NCMBUTTONDBLCLK";
    case 171: return "WM_NCXBUTTONDOWN";
    case 172: return "WM_NCXBUTTONUP";
    case 173: return "WM_NCXBUTTONDBLCLK";
    case 176: return "EM_GETSEL";
    case 177: return "EM_SETSEL";
    case 178: return "EM_GETRECT";
    case 179: return "EM_SETRECT";
    case 180: return "EM_SETRECTNP";
    case 181: return "EM_SCROLL";
    case 182: return "EM_LINESCROLL";
    case 183: return "EM_SCROLLCARET";
    case 185: return "EM_GETMODIFY";
    case 187: return "EM_SETMODIFY";
    case 188: return "EM_GETLINECOUNT";
    case 189: return "EM_LINEINDEX";
    case 190: return "EM_SETHANDLE";
    case 191: return "EM_GETHANDLE";
    case 192: return "EM_GETTHUMB";
    case 193: return "EM_LINELENGTH";
    case 194: return "EM_REPLACESEL";
    case 195: return "EM_SETFONT";
    case 196: return "EM_GETLINE";
    case 197: return "EM_LIMITTEXT / EM_SETLIMITTEXT";
    case 198: return "EM_CANUNDO";
    case 199: return "EM_UNDO";
    case 200: return "EM_FMTLINES";
    case 201: return "EM_LINEFROMCHAR";
    case 202: return "EM_SETWORDBREAK";
    case 203: return "EM_SETTABSTOPS";
    case 204: return "EM_SETPASSWORDCHAR";
    case 205: return "EM_EMPTYUNDOBUFFER";
    case 206: return "EM_GETFIRSTVISIBLELINE";
    case 207: return "EM_SETREADONLY";
    case 209: return "EM_SETWORDBREAKPROC / EM_GETWORDBREAKPROC";
    case 210: return "EM_GETPASSWORDCHAR";
    case 211: return "EM_SETMARGINS";
    case 212: return "EM_GETMARGINS";
    case 213: return "EM_GETLIMITTEXT";
    case 214: return "EM_POSFROMCHAR";
    case 215: return "EM_CHARFROMPOS";
    case 216: return "EM_SETIMESTATUS";
    case 217: return "EM_GETIMESTATUS";
    case 224: return "SBM_SETPOS";
    case 225: return "SBM_GETPOS";
    case 226: return "SBM_SETRANGE";
    case 227: return "SBM_GETRANGE";
    case 228: return "SBM_ENABLE_ARROWS";
    case 230: return "SBM_SETRANGEREDRAW";
    case 233: return "SBM_SETSCROLLINFO";
    case 234: return "SBM_GETSCROLLINFO";
    case 235: return "SBM_GETSCROLLBARINFO";
    case 240: return "BM_GETCHECK";
    case 241: return "BM_SETCHECK";
    case 242: return "BM_GETSTATE";
    case 243: return "BM_SETSTATE";
    case 244: return "BM_SETSTYLE";
    case 245: return "BM_CLICK";
    case 246: return "BM_GETIMAGE";
    case 247: return "BM_SETIMAGE";
    case 248: return "BM_SETDONTCLICK";
    case 255: return "WM_INPUT";
    case 256: return "WM_KEYDOWN";
    case 257: return "WM_KEYUP";
    case 258: return "WM_CHAR";
    case 259: return "WM_DEADCHAR";
    case 260: return "WM_SYSKEYDOWN";
    case 261: return "WM_SYSKEYUP";
    case 262: return "WM_SYSCHAR";
    case 263: return "WM_SYSDEADCHAR";
    case 265: return "WM_UNICHAR / WM_WNT_CONVERTREQUESTEX";
    case 266: return "WM_CONVERTREQUEST";
    case 267: return "WM_CONVERTRESULT";
    case 268: return "WM_INTERIM";
    case 269: return "WM_IME_STARTCOMPOSITION";
    case 270: return "WM_IME_ENDCOMPOSITION";
    case 272: return "WM_INITDIALOG";
    case 273: return "WM_COMMAND";
    case 274: return "WM_SYSCOMMAND";
    case 275: return "WM_TIMER";
    case 276: return "WM_HSCROLL";
    case 277: return "WM_VSCROLL";
    case 278: return "WM_INITMENU";
    case 279: return "WM_INITMENUPOPUP";
    case 280: return "WM_SYSTIMER";
    case 287: return "WM_MENUSELECT";
    case 288: return "WM_MENUCHAR";
    case 289: return "WM_ENTERIDLE";
    case 290: return "WM_MENURBUTTONUP";
    case 291: return "WM_MENUDRAG";
    case 292: return "WM_MENUGETOBJECT";
    case 293: return "WM_UNINITMENUPOPUP";
    case 294: return "WM_MENUCOMMAND";
    case 295: return "WM_CHANGEUISTATE";
    case 296: return "WM_UPDATEUISTATE";
    case 297: return "WM_QUERYUISTATE";
    case 306: return "WM_CTLCOLORMSGBOX";
    case 307: return "WM_CTLCOLOREDIT";
    case 308: return "WM_CTLCOLORLISTBOX";
    case 309: return "WM_CTLCOLORBTN";
    case 310: return "WM_CTLCOLORDLG";
    case 311: return "WM_CTLCOLORSCROLLBAR";
    case 312: return "WM_CTLCOLORSTATIC";
    case 512: return "WM_MOUSEMOVE";
    case 513: return "WM_LBUTTONDOWN";
    case 514: return "WM_LBUTTONUP";
    case 515: return "WM_LBUTTONDBLCLK";
    case 516: return "WM_RBUTTONDOWN";
    case 517: return "WM_RBUTTONUP";
    case 518: return "WM_RBUTTONDBLCLK";
    case 519: return "WM_MBUTTONDOWN";
    case 520: return "WM_MBUTTONUP";
    case 521: return "WM_MBUTTONDBLCLK";
    case 522: return "WM_MOUSEWHEEL";
    case 523: return "WM_XBUTTONDOWN";
    case 524: return "WM_XBUTTONUP";
    case 525: return "WM_XBUTTONDBLCLK";
    case 528: return "WM_PARENTNOTIFY";
    case 529: return "WM_ENTERMENULOOP";
    case 530: return "WM_EXITMENULOOP";
    case 531: return "WM_NEXTMENU";
    case 532: return "WM_SIZING";
    case 533: return "WM_CAPTURECHANGED";
    case 534: return "WM_MOVING";
    case 536: return "WM_POWERBROADCAST";
    case 537: return "WM_DEVICECHANGE";
    case 544: return "WM_MDICREATE";
    case 545: return "WM_MDIDESTROY";
    case 546: return "WM_MDIACTIVATE";
    case 547: return "WM_MDIRESTORE";
    case 548: return "WM_MDINEXT";
    case 549: return "WM_MDIMAXIMIZE";
    case 550: return "WM_MDITILE";
    case 551: return "WM_MDICASCADE";
    case 552: return "WM_MDIICONARRANGE";
    case 553: return "WM_MDIGETACTIVE";
    case 560: return "WM_MDISETMENU";
    case 561: return "WM_ENTERSIZEMOVE";
    case 562: return "WM_EXITSIZEMOVE";
    case 563: return "WM_DROPFILES";
    case 564: return "WM_MDIREFRESHMENU";
    case 640: return "WM_IME_REPORT";
    case 641: return "WM_IME_SETCONTEXT";
    case 642: return "WM_IME_NOTIFY";
    case 643: return "WM_IME_CONTROL";
    case 644: return "WM_IME_COMPOSITIONFULL";
    case 645: return "WM_IME_SELECT";
    case 646: return "WM_IME_CHAR";
    case 648: return "WM_IME_REQUEST";
    case 656: return "WM_IME_KEYDOWN";
    case 657: return "WM_IME_KEYUP";
    case 672: return "WM_NCMOUSEHOVER";
    case 673: return "WM_MOUSEHOVER";
    case 674: return "WM_NCMOUSELEAVE";
    case 675: return "WM_MOUSELEAVE";
    case 768: return "WM_CUT";
    case 769: return "WM_COPY";
    case 770: return "WM_PASTE";
    case 771: return "WM_CLEAR";
    case 772: return "WM_UNDO";
    case 773: return "WM_RENDERFORMAT";
    case 774: return "WM_RENDERALLFORMATS";
    case 775: return "WM_DESTROYCLIPBOARD";
    case 776: return "WM_DRAWCLIPBOARD";
    case 777: return "WM_PAINTCLIPBOARD";
    case 778: return "WM_VSCROLLCLIPBOARD";
    case 779: return "WM_SIZECLIPBOARD";
    case 780: return "WM_ASKCBFORMATNAME";
    case 781: return "WM_CHANGECBCHAIN";
    case 782: return "WM_HSCROLLCLIPBOARD";
    case 783: return "WM_QUERYNEWPALETTE";
    case 784: return "WM_PALETTEISCHANGING";
    case 785: return "WM_PALETTECHANGED";
    case 786: return "WM_HOTKEY";
    case 791: return "WM_PRINT";
    case 792: return "WM_PRINTCLIENT";
    case 793: return "WM_APPCOMMAND";
    case 856: return "WM_HANDHELDFIRST";
    case 863: return "WM_HANDHELDLAST";
    case 864: return "WM_AFXFIRST";
    case 895: return "WM_AFXLAST";
    case 896: return "WM_PENWINFIRST";
    case 897: return "WM_RCRESULT";
    case 898: return "WM_HOOKRCRESULT";
    case 899: return "WM_GLOBALRCCHANGE / WM_PENMISCINFO";
    case 900: return "WM_SKB";
    case 901: return "WM_HEDITCTL / WM_PENCTL";
    case 902: return "WM_PENMISC";
    case 903: return "WM_CTLINIT";
    case 904: return "WM_PENEVENT";
    case 911: return "WM_PENWINLAST";
    default:
    	return "unknown WM_ message";
	}

	return "unknown WM_ message";
}
	#endif
#endif

/**
 * @brief The z order counter floating.
 */
static unsigned int zOrderCounterFloating = 0;

/**
 * @brief Private data class of CFloatingDockContainer class (pimpl).
 */
struct FloatingDockContainerPrivate
{
		/// @brief The this.
		CFloatingDockContainer* _this;
		/// @brief The dock container.
		CDockContainerWidget* DockContainer;
		/// @brief The z order index.
		unsigned int zOrderIndex = ++zOrderCounterFloating;
		/// @brief The dock manager.
		QPointer<CDockManager> DockManager;
		/// @brief The dragging state.
		eDragState DraggingState = DraggingInactive;
		/// @brief The drag start mouse position.
		QPoint DragStartMousePosition;
		/// @brief The drop container.
		CDockContainerWidget* DropContainer = nullptr;
		/// @brief The single dock area.
		CDockAreaWidget* SingleDockArea = nullptr;
		/// @brief The drag start pos.
		QPoint DragStartPos;
		/// @brief The hiding.
		bool Hiding = false;
		/// @brief The auto hide children.
		bool AutoHideChildren = true;
		/// @brief The hide content on next hide.
		bool HideContentOnNextHide = false;
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
		QWidget* MouseEventHandler = nullptr;
		CFloatingWidgetTitleBar* TitleBar = nullptr;
		bool IsResizing = false;
		bool MousePressed = false;

		// Edge/corner resize support
		int _gripWidth = 6;
		bool _manualResizing = false;
		bool _edgeCursorActive = false;
		Qt::Edges _resizeEdges;
		QPointF _pressGlobalPos;
		QRect _startGeometry;
#endif

		/**
		 * @brief Private data constructor.
		 */
		FloatingDockContainerPrivate(CFloatingDockContainer* _public);

		/**
		 * @brief Handles the title mouse release event.
		 */
		void titleMouseReleaseEvent();
		/**
		 * @brief Updates drop overlays.
		 */
		void updateDropOverlays(const QPoint& GlobalPos);

		/**
		 * @brief Returns true if the given config flag is set.
		 */
		static bool testConfigFlag(CDockManager::eConfigFlag Flag)
		{
			return CDockManager::testConfigFlag(Flag);
		}

		/**
		 * @brief Tests is a certain state is active.
		 */
		bool isState(eDragState StateId) const
		{
			return StateId == DraggingState;
		}

		/**
		 * @brief Sets the dragging state and posts a FloatingWidgetDragStartEvent
		 * if dragging starts.
		 */
		void setState(eDragState StateId)
		{
			if (DraggingState == StateId)
			{
				return;
			}

			DraggingState = StateId;
			if (DraggingFloatingWidget == DraggingState)
			{
				qApp->postEvent(_this, new QEvent(static_cast<QEvent::Type>(internal::FloatingWidgetDragStartEvent)));
			}
		}

		/**
		 * @brief Sets the window title.
		 */
		void setWindowTitle(const QString& Text)
		{
#if (defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)) || defined(Q_OS_WIN)
			if (TitleBar)
			{
				TitleBar->setTitle(Text);
			}
#endif
			_this->setWindowTitle(Text);
		}

		/**
		 * @brief Reflect the current dock widget title in the floating widget windowTitle()
		 * depending on the CDockManager::FloatingContainerHasWidgetTitle flag.
		 */
		void reflectCurrentWidget(CDockWidget* CurrentWidget)
		{
			// reflect CurrentWidget's title if configured to do so, otherwise display
			// application name as window title
			if (testConfigFlag(CDockManager::FloatingContainerHasWidgetTitle))
			{
				setWindowTitle(CurrentWidget->windowTitle());
			}
			else
			{
				setWindowTitle(floatingContainersTitle());
			}

			// reflect CurrentWidget's icon if configured to do so, otherwise display
			// application icon as window icon
			QIcon CurrentWidgetIcon = CurrentWidget->icon();
			if (testConfigFlag(CDockManager::FloatingContainerHasWidgetIcon) && !CurrentWidgetIcon.isNull())
			{
				_this->setWindowIcon(CurrentWidget->icon());
			}
			else
			{
				_this->setWindowIcon(CurrentWidget->windowIcon());
			}
		}

		/**
		 * @brief Handles escape key press when dragging around the floating widget.
		 */
		void handleEscapeKey();

		/**
		 * @brief Returns the title used by all FloatingContainer that does not
		 * reflect the title of the current dock widget.
		 *
		 * If not title was set with CDockManager::setFloatingContainersTitle(),
		 * it returns QGuiApplication::applicationDisplayName().
		 */
		static QString floatingContainersTitle()
		{
			return CDockManager::floatingContainersTitle();
		}
};
}// namespace ads

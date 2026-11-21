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

#include "CTitleBarButton.h"
#include "CDockAreaTitleBar.h"
#include "CDockManager.h"

namespace ads
{
CTitleBarButton::CTitleBarButton(bool showInTitleBar, bool hideWhenDisabled, TitleBarButton ButtonId, QWidget* parent)
	: tTitleBarButton(parent)
	, ShowInTitleBar(showInTitleBar)
	, HideWhenDisabled(CDockManager::testConfigFlag(CDockManager::DockAreaHideDisabledButtons) && hideWhenDisabled)
	, TitleBarButtonId(ButtonId)
{
	setFocusPolicy(Qt::NoFocus);
}

void CTitleBarButton::setVisible(bool visible)
{
	// 'visible' can stay 'true' if and only if this button is configured to
	// generally visible:
	visible = visible && this->ShowInTitleBar;

	// 'visible' can stay 'true' unless: this button is configured to be invisible
	// when it is disabled and it is currently disabled:
	if (visible && HideWhenDisabled)
	{
		visible = isEnabled();
	}

	Super::setVisible(visible);
}

void CTitleBarButton::setShowInTitleBar(bool Show)
{
	this->ShowInTitleBar = Show;
	if (!Show)
	{
		setVisible(false);
	}
}

TitleBarButton CTitleBarButton::buttonId() const
{
	return TitleBarButtonId;
}

bool CTitleBarButton::event(QEvent* ev)
{
	if (QEvent::EnabledChange != ev->type() || !HideWhenDisabled || !ShowInTitleBar)
	{
		return Super::event(ev);
	}

	bool Show = true;
	if (isInAutoHideArea())
	{
		switch (TitleBarButtonId)
		{
			case TitleBarButtonClose:
				Show = CDockManager::testAutoHideConfigFlag(CDockManager::AutoHideHasCloseButton);
				break;
			case TitleBarButtonUndock:
				Show = false;
				break;
			default:
				break;
		}
	}

	// force setVisible() call - Calling setVisible() directly here doesn't
	// work well when button is expected to be shown first time
	QMetaObject::invokeMethod(this, "setVisible", Qt::QueuedConnection, Q_ARG(bool, isEnabledTo(this->parentWidget()) & Show));

	return Super::event(ev);
}

CDockAreaTitleBar* CTitleBarButton::titleBar() const
{
	return qobject_cast<CDockAreaTitleBar*>(parentWidget());
}

bool CTitleBarButton::isInAutoHideArea() const
{
	auto TitleBar = titleBar();
	return TitleBar && TitleBar->isAutoHide();
}

}// namespace ads

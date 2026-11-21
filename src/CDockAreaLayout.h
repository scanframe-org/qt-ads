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

#include <QBoxLayout>
#include <QDebug>
#include <QList>
#include <QPointer>
#include <QRect>
#include <QWidget>

namespace ads
{

/**
 * @brief Internal dock area layout mimics stack layout but only inserts the current
 * widget into the internal QLayout object.
 * \warning Only the current widget has a parent. All other widgets
 * do not have a parent. That means, a widget that is in this layout may
 * return nullptr for its parent() function if it is not the current widget.
 */
class CDockAreaLayout
{
	public:
		/**
		 * @brief Creates an instance with the given parent layout.
		 */
		CDockAreaLayout(QBoxLayout* ParentLayout)
			: m_ParentLayout(ParentLayout)
		{}

	private:
		/**
		 * @brief Detaches a widget without turning an existing native window into a
		 * top-level window.
		 */
		void detachWidget(QWidget* Widget)
		{
			CDockAreaWidget* DockArea = qobject_cast<CDockAreaWidget*>(m_ParentLayout->parentWidget());
			if (Widget->internalWinId() && DockArea && DockArea->dockManager())
			{
				Widget->setParent(DockArea->dockManager());
			}
			else
			{
				Widget->setParent(nullptr);
			}
		}

	public:
		/**
		 * @brief Returns the number of widgets in this layout.
		 */
		int count() const
		{
			return m_Widgets.count();
		}

		/**
		 * @brief Removes the current content widget from the box layout by widget identity.
		 * Never use hardcoded takeAt(1): with TabsAtBottom the TabBar shares this
		 * layout and may sit at index 1, so takeAt(1) would orphan it as a top-level
		 * window (blank 200x44 popup, workspace tabs disappear).
		 */
		void removeCurrentWidgetFromBoxLayout()
		{
			if (!m_CurrentWidget)
				return;

			int idx = m_ParentLayout->indexOf(m_CurrentWidget);
			if (idx < 0)
			{
				detachWidget(m_CurrentWidget);
				return;
			}

			QLayoutItem* LayoutItem = m_ParentLayout->takeAt(idx);
			if (!LayoutItem)
				return;

			QWidget* taken = LayoutItem->widget();
			if (taken && (qobject_cast<CDockAreaTabBar*>(taken) || qobject_cast<CDockAreaTitleBar*>(taken)))
			{
				m_ParentLayout->insertWidget(idx, taken);
				delete LayoutItem;
				detachWidget(m_CurrentWidget);
				return;
			}

			if (taken)
				detachWidget(taken);
			delete LayoutItem;
		}

		/**
		 * @brief Inserts the widget at the given index position into the internal widget
		 * list.
		 */
		void insertWidget(int index, QWidget* Widget)
		{
			detachWidget(Widget);
			if (index < 0)
			{
				index = m_Widgets.count();
			}
			m_Widgets.insert(index, Widget);
			if (m_CurrentIndex < 0)
			{
				setCurrentIndex(index);
			}
			else
			{
				if (index <= m_CurrentIndex)
				{
					++m_CurrentIndex;
				}
			}
		}

		/**
		 * @brief Removes the given widget from the layout.
		 */
		void removeWidget(QWidget* Widget)
		{
			if (currentWidget() == Widget)
			{
				removeCurrentWidgetFromBoxLayout();
				m_CurrentWidget = nullptr;
				m_CurrentIndex = -1;
			}
			else if (indexOf(Widget) < m_CurrentIndex)
			{
				--m_CurrentIndex;
			}
			m_Widgets.removeOne(Widget);
		}

		/**
		 * @brief Returns the current selected widget.
		 */
		QWidget* currentWidget() const
		{
			return m_CurrentWidget;
		}

		/**
		 * @brief Activates the widget with the give index.
		 */
		void setCurrentIndex(int index)
		{
			QWidget* prev = currentWidget();
			QWidget* next = widget(index);
			if (!next || (next == prev && !m_CurrentWidget))
			{
				return;
			}

			bool reenableUpdates = false;
			QWidget* parent = m_ParentLayout->parentWidget();

			if (parent && parent->updatesEnabled())
			{
				reenableUpdates = true;
				parent->setUpdatesEnabled(false);
			}

			removeCurrentWidgetFromBoxLayout();

			int insertAt = 1;
			if (m_ParentLayout->count() > 0)
			{
				for (int i = 0; i < m_ParentLayout->count(); ++i)
				{
					QLayoutItem* it = m_ParentLayout->itemAt(i);
					QWidget* w = it ? it->widget() : nullptr;
					if (qobject_cast<CDockAreaTabBar*>(w))
					{
						insertAt = i;
						break;
					}
				}
			}
			m_ParentLayout->insertWidget(insertAt, next);
			if (prev)
			{
				prev->hide();
			}
			m_CurrentIndex = index;
			m_CurrentWidget = next;

			if (reenableUpdates)
			{
				parent->setUpdatesEnabled(true);
			}
		}

		/**
		 * @brief Returns the index of the current active widget.
		 */
		int currentIndex() const
		{
			return m_CurrentIndex;
		}

		/**
		 * @brief Returns true if there are no widgets in the layout.
		 */
		bool isEmpty() const
		{
			return m_Widgets.empty();
		}

		/**
		 * @brief Returns the index of the given widget.
		 */
		int indexOf(QWidget* w) const
		{
			return m_Widgets.indexOf(w);
		}

		/**
		 * @brief Returns the widget for the given index.
		 */
		QWidget* widget(int index) const
		{
			return (index < m_Widgets.size()) ? m_Widgets.at(index) : nullptr;
		}

		/**
		 * @brief Returns the geometry of the current active widget.
		 */
		QRect geometry() const
		{
			return m_Widgets.empty() ? QRect() : currentWidget()->geometry();
		}

	private:
		/// @brief The parent layout.
		QBoxLayout* m_ParentLayout;
		/// @brief The widgets.
		QList<QPointer<QWidget>> m_Widgets;
		/// @brief The current index.
		int m_CurrentIndex = -1;
		/// @brief The current widget.
		QWidget* m_CurrentWidget = nullptr;
};

using DockAreaLayout = CDockAreaLayout;

}// namespace ads

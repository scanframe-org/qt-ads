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

#include "CDockOverlayCross.h"

#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QPainter>
#include <QPixmap>
#include <QWindow>

#include "CDockOverlay.h"
#include "CDockOverlayCross_p.h"

namespace ads
{
static int areaAlignment(const DockWidgetArea area)
{
	switch (area)
	{
		case TopDockWidgetArea:
			return (int) Qt::AlignHCenter | Qt::AlignBottom;
		case RightDockWidgetArea:
			return (int) Qt::AlignLeft | Qt::AlignVCenter;
		case BottomDockWidgetArea:
			return (int) Qt::AlignHCenter | Qt::AlignTop;
		case LeftDockWidgetArea:
			return (int) Qt::AlignRight | Qt::AlignVCenter;
		case CenterDockWidgetArea:
			return (int) Qt::AlignCenter;
		default:
			return Qt::AlignCenter;
	}
}

DockOverlayCrossPrivate::DockOverlayCrossPrivate(CDockOverlayCross* _public)
	: _this(_public)
	, DockOverlay(nullptr)
	, GridLayout(nullptr)
{}

QPoint DockOverlayCrossPrivate::areaGridPosition(const DockWidgetArea area)
{
	if (CDockOverlay::ModeDockAreaOverlay == Mode)
	{
		switch (area)
		{
			case TopDockWidgetArea:
				return QPoint(1, 2);
			case RightDockWidgetArea:
				return QPoint(2, 3);
			case BottomDockWidgetArea:
				return QPoint(3, 2);
			case LeftDockWidgetArea:
				return QPoint(2, 1);
			case CenterDockWidgetArea:
				return QPoint(2, 2);
			default:
				return QPoint();
		}
	}
	else
	{
		switch (area)
		{
			case TopDockWidgetArea:
				return QPoint(0, 2);
			case RightDockWidgetArea:
				return QPoint(2, 4);
			case BottomDockWidgetArea:
				return QPoint(4, 2);
			case LeftDockWidgetArea:
				return QPoint(2, 0);
			case CenterDockWidgetArea:
				return QPoint(2, 2);
			default:
				return QPoint();
		}
	}
}

QColor DockOverlayCrossPrivate::defaultIconColor(CDockOverlayCross::eIconColor ColorIndex)
{
	QPalette pal = _this->palette();
	switch (ColorIndex)
	{
		case CDockOverlayCross::FrameColor:
			return pal.color(QPalette::Active, QPalette::Highlight);
		case CDockOverlayCross::WindowBackgroundColor:
			return pal.color(QPalette::Active, QPalette::Base);
		case CDockOverlayCross::OverlayColor:
		{
			QColor Color = pal.color(QPalette::Active, QPalette::Highlight);
			Color.setAlpha(64);
			return Color;
		}

		case CDockOverlayCross::ArrowColor:
			return pal.color(QPalette::Active, QPalette::Base);
		case CDockOverlayCross::ShadowColor:
		{
			QColor Color = pal.color(QPalette::Active, QPalette::Text);
			Color.setAlpha(64);
			return Color;
		}
	}

	return {};
}

QColor DockOverlayCrossPrivate::iconColor(CDockOverlayCross::eIconColor ColorIndex)
{
	QColor Color = IconColors[ColorIndex];
	if (!Color.isValid())
	{
		Color = defaultIconColor(ColorIndex);
	}
	return Color;
}

qreal DockOverlayCrossPrivate::dropIndicatiorWidth(QLabel* l) const
{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
	Q_UNUSED(l)
	return 40;
#else
	return static_cast<qreal>(l->fontMetrics().height()) * 3.f;
#endif
}

QWidget* DockOverlayCrossPrivate::createDropIndicatorWidget(DockWidgetArea DockWidgetArea, CDockOverlay::eMode Mode)
{
	QLabel* l = new QLabel();
	l->setObjectName("DockWidgetAreaLabel");

	qreal metric = dropIndicatiorWidth(l);
	QSizeF size(metric, metric);
	if (internal::isSideBarArea(DockWidgetArea))
	{
		auto SideBarLocation = internal::toSideBarLocation(DockWidgetArea);
		if (internal::isHorizontalSideBarLocation(SideBarLocation))
		{
			size.setHeight(size.height() / 2);
		}
		else
		{
			size.setWidth(size.width() / 2);
		}
	}

	l->setPixmap(createHighDpiDropIndicatorPixmap(size, DockWidgetArea, Mode));
	l->setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
	l->setAttribute(Qt::WA_TranslucentBackground);
	l->setProperty("dockWidgetArea", DockWidgetArea);
	return l;
}

void DockOverlayCrossPrivate::updateDropIndicatorIcon(QWidget* DropIndicatorWidget)
{
	QLabel* l = qobject_cast<QLabel*>(DropIndicatorWidget);
	const qreal metric = dropIndicatiorWidth(l);
	const QSizeF size(metric, metric);

	int Area = l->property("dockWidgetArea").toInt();
	l->setPixmap(createHighDpiDropIndicatorPixmap(size, (DockWidgetArea) Area, Mode));
}

QPixmap DockOverlayCrossPrivate::createHighDpiDropIndicatorPixmap(const QSizeF& size, DockWidgetArea DockWidgetArea, CDockOverlay::eMode Mode)
{
	QColor borderColor = iconColor(CDockOverlayCross::FrameColor);
	QColor backgroundColor = iconColor(CDockOverlayCross::WindowBackgroundColor);
	QColor overlayColor = iconColor(CDockOverlayCross::OverlayColor);
	if (overlayColor.alpha() == 255)
	{
		overlayColor.setAlpha(64);
	}

#if QT_VERSION >= 0x050600
	double DevicePixelRatio = _this->window()->devicePixelRatioF();
#else
	double DevicePixelRatio = _this->window()->devicePixelRatio();
#endif
	QSizeF PixmapSize = size * DevicePixelRatio;
	QPixmap pm(PixmapSize.toSize());
	pm.fill(QColor(0, 0, 0, 0));

	QPainter p(&pm);
	QPen pen = p.pen();
	QRectF ShadowRect(pm.rect());
	QRectF baseRect;
	baseRect.setSize(ShadowRect.size() * 0.7);
	baseRect.moveCenter(ShadowRect.center());

	// Fill
	QColor ShadowColor = iconColor(CDockOverlayCross::ShadowColor);
	if (ShadowColor.alpha() == 255)
	{
		ShadowColor.setAlpha(64);
	}
	p.fillRect(ShadowRect, ShadowColor);

	// Drop area rect.
	p.save();
	QRectF areaRect;
	QLineF areaLine;
	QRectF nonAreaRect;
	switch (DockWidgetArea)
	{
		case TopDockWidgetArea:
			areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width(), baseRect.height() * .5f);
			nonAreaRect = QRectF(baseRect.x(), ShadowRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
			areaLine = QLineF(areaRect.bottomLeft(), areaRect.bottomRight());
			break;
		case RightDockWidgetArea:
			areaRect = QRectF(ShadowRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
			nonAreaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width() * .5f, baseRect.height());
			areaLine = QLineF(areaRect.topLeft(), areaRect.bottomLeft());
			break;
		case BottomDockWidgetArea:
			areaRect = QRectF(baseRect.x(), ShadowRect.height() * .5f, baseRect.width(), baseRect.height() * .5f);
			nonAreaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width(), baseRect.height() * .5f);
			areaLine = QLineF(areaRect.topLeft(), areaRect.topRight());
			break;
		case LeftDockWidgetArea:
			areaRect = QRectF(baseRect.x(), baseRect.y(), baseRect.width() * .5f, baseRect.height());
			nonAreaRect = QRectF(ShadowRect.width() * .5f, baseRect.y(), baseRect.width() * .5f, baseRect.height());
			areaLine = QLineF(areaRect.topRight(), areaRect.bottomRight());
			break;
		default:
			break;
	}

	QSizeF baseSize = baseRect.size();
	bool IsOuterContainerArea =
		(CDockOverlay::ModeContainerOverlay == Mode) && (DockWidgetArea != CenterDockWidgetArea) && !internal::isSideBarArea(DockWidgetArea);

	if (IsOuterContainerArea)
	{
		baseRect = areaRect;
	}

	p.fillRect(baseRect, backgroundColor);

	if (areaRect.isValid())
	{
		pen = p.pen();
		pen.setColor(borderColor);
		p.setBrush(overlayColor);
		p.setPen(Qt::NoPen);
		p.drawRect(areaRect);

		pen = p.pen();
		pen.setWidth(1);
		pen.setColor(borderColor);
		pen.setStyle(Qt::DashLine);
		p.setPen(pen);
		p.drawLine(areaLine);
	}
	p.restore();

	p.save();
	// Draw outer border
	pen = p.pen();
	pen.setColor(borderColor);
	pen.setWidth(1);
	p.setBrush(Qt::NoBrush);
	p.setPen(pen);
	p.drawRect(baseRect);

	// draw window title bar
	p.setBrush(borderColor);
	QRectF FrameRect(baseRect.topLeft(), QSizeF(baseRect.width(), baseSize.height() / 10));
	p.drawRect(FrameRect);
	p.restore();

	// Draw arrow for outer container drop indicators
	if (IsOuterContainerArea)
	{
		QRectF ArrowRect;
		ArrowRect.setSize(baseSize);
		ArrowRect.setWidth(ArrowRect.width() / 4.6);
		ArrowRect.setHeight(ArrowRect.height() / 2);
		ArrowRect.moveCenter(QPointF(0, 0));
		QPolygonF Arrow;
		Arrow << ArrowRect.topLeft() << QPointF(ArrowRect.right(), ArrowRect.center().y()) << ArrowRect.bottomLeft();
		p.setPen(Qt::NoPen);
		p.setBrush(iconColor(CDockOverlayCross::ArrowColor));
		p.setRenderHint(QPainter::Antialiasing, true);
		p.translate(nonAreaRect.center().x(), nonAreaRect.center().y());

		switch (DockWidgetArea)
		{
			case TopDockWidgetArea:
				p.rotate(-90);
				break;
			case RightDockWidgetArea:
				break;
			case BottomDockWidgetArea:
				p.rotate(90);
				break;
			case LeftDockWidgetArea:
				p.rotate(180);
				break;
			default:
				break;
		}

		p.drawPolygon(Arrow);
	}

	pm.setDevicePixelRatio(DevicePixelRatio);
	return pm;
}

CDockOverlayCross::CDockOverlayCross(CDockOverlay* overlay)
	:// Wayland: see CDockOverlay - the cross must be a child of the target
	// window for the same reason as the overlay
	QWidget(internal::isWayland() && overlay->parentWidget() ? overlay->parentWidget()->window() : overlay->parentWidget())
	, d(new DockOverlayCrossPrivate(this))
{
	d->DockOverlay = overlay;
	if (internal::isWayland())
	{
		setAttribute(Qt::WA_TransparentForMouseEvents);
	}
	else
	{
#if defined(Q_OS_UNIX) && !defined(Q_OS_MACOS)
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::X11BypassWindowManagerHint);
#else
		setWindowFlags(Qt::Tool | Qt::FramelessWindowHint);
#endif
		setAttribute(Qt::WA_TranslucentBackground);
	}
	setWindowTitle("DockOverlayCross");

	d->GridLayout = new QGridLayout();
	d->GridLayout->setSpacing(0);
	setLayout(d->GridLayout);
}

CDockOverlayCross::~CDockOverlayCross()
{
	delete d;
}

void CDockOverlayCross::setupOverlayCross(CDockOverlay::eMode Mode)
{
	d->Mode = Mode;

	QHash<DockWidgetArea, QWidget*> areaWidgets;
	areaWidgets.insert(TopDockWidgetArea, d->createDropIndicatorWidget(TopDockWidgetArea, Mode));
	areaWidgets.insert(RightDockWidgetArea, d->createDropIndicatorWidget(RightDockWidgetArea, Mode));
	areaWidgets.insert(BottomDockWidgetArea, d->createDropIndicatorWidget(BottomDockWidgetArea, Mode));
	areaWidgets.insert(LeftDockWidgetArea, d->createDropIndicatorWidget(LeftDockWidgetArea, Mode));
	areaWidgets.insert(CenterDockWidgetArea, d->createDropIndicatorWidget(CenterDockWidgetArea, Mode));

#if QT_VERSION >= 0x050600
	d->LastDevicePixelRatio = devicePixelRatioF();
#else
	d->LastDevicePixelRatio = devicePixelRatio();
#endif
	setAreaWidgets(areaWidgets);
	d->UpdateRequired = false;
}

void CDockOverlayCross::updateOverlayIcons()
{
	// Wayland: the cross is a child widget and has no window handle, so the
	// device pixel ratio is taken from the widget instead of the window
	const qreal DevicePixelRatio = internal::isWayland() ? devicePixelRatioF() : windowHandle()->devicePixelRatio();
	if (DevicePixelRatio == d->LastDevicePixelRatio)
	{
		return;
	}

	for (auto Widget: d->DropIndicatorWidgets)
	{
		d->updateDropIndicatorIcon(Widget);
	}
#if QT_VERSION >= 0x050600
	d->LastDevicePixelRatio = devicePixelRatioF();
#else
	d->LastDevicePixelRatio = devicePixelRatio();
#endif
}

void CDockOverlayCross::setIconColor(eIconColor ColorIndex, const QColor& Color)
{
	d->IconColors[ColorIndex] = Color;
	d->UpdateRequired = true;
}

QColor CDockOverlayCross::iconColor(eIconColor ColorIndex) const
{
	return d->IconColors[ColorIndex];
}

void CDockOverlayCross::setAreaWidgets(const QHash<DockWidgetArea, QWidget*>& widgets)
{
	// Delete old widgets.
	QMutableHashIterator<DockWidgetArea, QWidget*> i(d->DropIndicatorWidgets);
	while (i.hasNext())
	{
		i.next();
		QWidget* widget = i.value();
		d->GridLayout->removeWidget(widget);
		delete widget;
		i.remove();
	}

	// Insert new widgets into grid.
	d->DropIndicatorWidgets = widgets;
	QHashIterator<DockWidgetArea, QWidget*> i2(d->DropIndicatorWidgets);
	while (i2.hasNext())
	{
		i2.next();
		const DockWidgetArea area = i2.key();
		QWidget* widget = i2.value();
		QPoint p = d->areaGridPosition(area);
		d->GridLayout->addWidget(widget, p.x(), p.y(), (Qt::Alignment) areaAlignment(area));
	}

	if (CDockOverlay::ModeDockAreaOverlay == d->Mode)
	{
		d->GridLayout->setContentsMargins(0, 0, 0, 0);
		d->GridLayout->setRowStretch(0, 1);
		d->GridLayout->setRowStretch(1, 0);
		d->GridLayout->setRowStretch(2, 0);
		d->GridLayout->setRowStretch(3, 0);
		d->GridLayout->setRowStretch(4, 1);

		d->GridLayout->setColumnStretch(0, 1);
		d->GridLayout->setColumnStretch(1, 0);
		d->GridLayout->setColumnStretch(2, 0);
		d->GridLayout->setColumnStretch(3, 0);
		d->GridLayout->setColumnStretch(4, 1);
	}
	else
	{
		d->GridLayout->setContentsMargins(4, 4, 4, 4);
		d->GridLayout->setRowStretch(0, 0);
		d->GridLayout->setRowStretch(1, 1);
		d->GridLayout->setRowStretch(2, 1);
		d->GridLayout->setRowStretch(3, 1);
		d->GridLayout->setRowStretch(4, 0);

		d->GridLayout->setColumnStretch(0, 0);
		d->GridLayout->setColumnStretch(1, 1);
		d->GridLayout->setColumnStretch(2, 1);
		d->GridLayout->setColumnStretch(3, 1);
		d->GridLayout->setColumnStretch(4, 0);
	}
	reset();
}

DockWidgetArea CDockOverlayCross::cursorLocation() const
{
	return cursorLocation(QCursor::pos());
}

DockWidgetArea CDockOverlayCross::cursorLocation(const QPoint& GlobalPos) const
{
	const QPoint pos = mapFromGlobal(GlobalPos);
	QHashIterator<DockWidgetArea, QWidget*> i(d->DropIndicatorWidgets);
	while (i.hasNext())
	{
		i.next();
		if (d->DockOverlay->allowedAreas().testFlag(i.key()) && i.value() && i.value()->isVisible() && i.value()->geometry().contains(pos))
		{
			return i.key();
		}
	}
	return InvalidDockWidgetArea;
}

void CDockOverlayCross::showEvent(QShowEvent*)
{
	if (d->UpdateRequired)
	{
		setupOverlayCross(d->Mode);
	}
	this->updatePosition();
}

bool CDockOverlayCross::event(QEvent* e)
{
	bool Result = QWidget::event(e);

	if (e->type() == QEvent::ApplicationPaletteChange)
	{
		d->UpdateRequired = true;
	}

	return Result;
}

void CDockOverlayCross::updatePosition()
{
	resize(d->DockOverlay->size());
	QPoint TopLeft = d->DockOverlay->pos();
	QPoint Offest((this->width() - d->DockOverlay->width()) / 2, (this->height() - d->DockOverlay->height()) / 2);
	QPoint CrossTopLeft = TopLeft - Offest;
	move(CrossTopLeft);
	if (internal::isWayland())
	{
		// paint the child cross above its sibling dock widgets
		raise();
	}
}

void CDockOverlayCross::reset()
{
	QList<DockWidgetArea> allAreas;
	allAreas << TopDockWidgetArea << RightDockWidgetArea << BottomDockWidgetArea << LeftDockWidgetArea << CenterDockWidgetArea;
	const DockWidgetAreas allowedAreas = d->DockOverlay->allowedAreas();

	// Update visibility of area widgets based on allowedAreas.
	for (int i = 0; i < allAreas.count(); ++i)
	{
		QPoint p = d->areaGridPosition(allAreas.at(i));
		QLayoutItem* item = d->GridLayout->itemAtPosition(p.x(), p.y());
		QWidget* w = nullptr;
		if (item && (w = item->widget()) != nullptr)
		{
			w->setVisible(allowedAreas.testFlag(allAreas.at(i)));
		}
	}
}

void CDockOverlayCross::setIconColors(const QString& Colors)
{
	static const QMap<QString, int> ColorCompenentStringMap{
		{"Frame", CDockOverlayCross::FrameColor},
		{"Background", CDockOverlayCross::WindowBackgroundColor},
		{"Overlay", CDockOverlayCross::OverlayColor},
		{"Arrow", CDockOverlayCross::ArrowColor},
		{"Shadow", CDockOverlayCross::ShadowColor}
	};

#if (QT_VERSION < QT_VERSION_CHECK(5, 14, 0))
	auto SkipEmptyParts = QString::SkipEmptyParts;
#else
	auto SkipEmptyParts = Qt::SkipEmptyParts;
#endif
	auto ColorList = Colors.split(' ', SkipEmptyParts);
	for (const auto& ColorListEntry: ColorList)
	{
		auto ComponentColor = ColorListEntry.split('=', SkipEmptyParts);
		int Component = ColorCompenentStringMap.value(ComponentColor[0], -1);
		if (Component < 0)
		{
			continue;
		}
		d->IconColors[Component] = QColor(ComponentColor[1]);
	}

	d->UpdateRequired = true;
}

QString CDockOverlayCross::iconColors() const
{
	return QString();
}

}// namespace ads

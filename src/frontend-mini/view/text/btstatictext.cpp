/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the test suite of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtCore/qmath.h> 
#include <QtGui/qcolor.h>
#include <QtGui/qapplication.h>

#include <private/qtextengine_p.h>
#include <private/qfontengine_p.h>
#include <qabstracttextdocumentlayout.h>
#include <private/qstatictext_p.h>

#include "btstatictext.h"


extern int qt_defaultDpiX();
extern int qt_defaultDpiY();


class BtStaticText;
class BtStaticTextPrivate
{
public:
	BtStaticTextPrivate();
	BtStaticTextPrivate(const BtStaticTextPrivate &other);
	~BtStaticTextPrivate();

	void init();
	void paintText(const QPointF &pos, QPainter *p);

	void invalidate()
	{
		needsRelayout = true;
	}

	QAtomicInt ref;                      // 4 bytes per text

	QString text;                        // 4 bytes per text
	QFont font;                          // 8 bytes per text
	qreal textWidth;                     // 8 bytes per text
	QSizeF actualSize;                   // 16 bytes per text
	QPointF position;                    // 16 bytes per text

	QTransform matrix;                   // 80 bytes per text
	QStaticTextItem *items;              // 4 bytes per text
	int itemCount;                       // 4 bytes per text

	glyph_t *glyphPool;                  // 4 bytes per text
	QFixedPoint *positionPool;           // 4 bytes per text
	QChar *charPool;                     // 4 bytes per text

	QTextOption textOption;              // 28 bytes per text

	unsigned char needsRelayout            : 1; // 1 byte per text
	unsigned char useBackendOptimizations  : 1;
	unsigned char textFormat               : 2;
	unsigned char untransformedCoordinates : 1;
	// ================
	// 195 bytes per text

	static BtStaticTextPrivate *get(const BtStaticText *q);
};

/*!
    \class BtStaticText
    \brief The BtStaticText class enables optimized drawing of text when the text and its layout
    is updated rarely.
    \since 4.7

    \ingroup multimedia
    \ingroup text
    \mainclass

    BtStaticText provides a way to cache layout data for a block of text so that it can be drawn
    more efficiently than by using QPainter::drawText() in which the layout information is 
    recalculated with every call. 

    The class primarily provides an optimization for cases where the text, its font and the
    transformations on the painter are static over several paint events. If the text or its layout
    is changed for every iteration, QPainter::drawText() is the more efficient alternative, since
    the static text's layout would have to be recalculated to take the new state into consideration.

    Translating the painter will not cause the layout of the text to be recalculated, but will cause
    a very small performance impact on drawStaticText(). Altering any other parts of the painter's
    transformation or the painter's font will cause the layout of the static text to be
    recalculated. This should be avoided as often as possible to maximize the performance
    benefit of using BtStaticText.

    In addition, only affine transformations are supported by drawStaticText(). Calling
    drawStaticText() on a projected painter will perform slightly worse than using the regular
    drawText() call, so this should be avoided.

    \code
    class MyWidget: public QWidget
    {
    public:
        MyWidget(QWidget *parent = 0) : QWidget(parent), m_staticText("This is static text")

    protected:
        void paintEvent(QPaintEvent *)
        {
            QPainter painter(this);
            painter.drawStaticText(0, 0, m_staticText);
        }

    private:
        BtStaticText m_staticText;
    };
    \endcode

    The BtStaticText class can be used to mimic the behavior of QPainter::drawText() to a specific
    point with no boundaries, and also when QPainter::drawText() is called with a bounding 
    rectangle. 

    If a bounding rectangle is not required, create a BtStaticText object without setting a preferred
    text width. The text will then occupy a single line.

    If you set a text width on the BtStaticText object, this will bound the text. The text will
    be formatted so that no line exceeds the given width. The text width set for BtStaticText will
    not automatically be used for clipping. To achieve clipping in addition to line breaks, use
    QPainter::setClipRect(). The position of the text is decided by the argument passed to
    QPainter::drawStaticText() and can change from call to call with a minimal impact on
    performance.

    For extra convenience, it is possible to apply formatting to the text using the HTML subset
    supported by QTextDocument. BtStaticText will attempt to guess the format of the input text using
    Qt::mightBeRichText(), and interpret it as rich text if this function returns true. To force
    BtStaticText to display its contents as either plain text or rich text, use the function
    BtStaticText::setTextFormat() and pass in, respectively, Qt::PlainText and Qt::RichText.

    BtStaticText can only represent text, so only HTML tags which alter the layout or appearance of
    the text will be respected. Adding an image to the input HTML, for instance, will cause the
    image to be included as part of the layout, affecting the positions of the text glyphs, but it
    will not be displayed. The result will be an empty area the size of the image in the output.
    Similarly, using tables will cause the text to be laid out in table format, but the borders
    will not be drawn.

    If it's the first time the static text is drawn, or if the static text, or the painter's font
    has been altered since the last time it was drawn, the text's layout has to be
    recalculated. On some paint engines, changing the matrix of the painter will also cause the
    layout to be recalculated. In particular, this will happen for any engine except for the
    OpenGL2 paint engine. Recalculating the layout will impose an overhead on the
    QPainter::drawStaticText() call where it occurs. To avoid this overhead in the paint event, you
    can call prepare() ahead of time to ensure that the layout is calculated.

    \sa QPainter::drawText(), QPainter::drawStaticText(), QTextLayout, QTextDocument
*/

/*!
    \enum BtStaticText::PerformanceHint

    This enum the different performance hints that can be set on the BtStaticText. These hints
    can be used to indicate that the BtStaticText should use additional caches, if possible,
    to improve performance at the expense of memory. In particular, setting the performance hint
    AggressiveCaching on the BtStaticText will improve performance when using the OpenGL graphics
    system or when drawing to a QGLWidget.

    \value ModerateCaching Do basic caching for high performance at a low memory cost.
    \value AggressiveCaching Use additional caching when available. This may improve performance
           at a higher memory cost.
*/

/*!
    Constructs an empty BtStaticText
*/
BtStaticText::BtStaticText()    
    : data(new BtStaticTextPrivate)
{
}

/*!
    Constructs a BtStaticText object with the given \a text.
*/
BtStaticText::BtStaticText(const QString &text)
    : data(new BtStaticTextPrivate)
{    
    data->text = text;
    data->invalidate();
}

/*!
    Constructs a BtStaticText object which is a copy of \a other.
*/
BtStaticText::BtStaticText(const BtStaticText &other)    
{
    data = other.data;
}

/*!
    Destroys the BtStaticText.
*/
BtStaticText::~BtStaticText()
{
    Q_ASSERT(!data || data->ref >= 1);
}

/*!
    \internal
*/
void BtStaticText::detach()
{    
    if (data->ref != 1)
        data.detach();
}

/*!
  Prepares the BtStaticText object for being painted with the given \a matrix and the given \a font
  to avoid overhead when the actual drawStaticText() call is made.

  When drawStaticText() is called, the layout of the BtStaticText will be recalculated if any part
  of the BtStaticText object has changed since the last time it was drawn. It will also be
  recalculated if the painter's font is not the same as when the BtStaticText was last drawn, or,
  on any other paint engine than the OpenGL2 engine, if the painter's matrix has been altered
  since the static text was last drawn.

  To avoid the overhead of creating the layout the first time you draw the BtStaticText after
  making changes, you can use the prepare() function and pass in the \a matrix and \a font you
  expect to use when drawing the text.

  \sa QPainter::setFont(), QPainter::setMatrix()
*/
void BtStaticText::prepare(const QTransform &matrix, const QFont &font)
{
    data->matrix = matrix;
    data->font = font;
    data->init();
}


/*!
    Assigns \a other to this BtStaticText.
*/
BtStaticText &BtStaticText::operator=(const BtStaticText &other)
{    
    data = other.data;
    return *this;
}

/*!
    Compares \a other to this BtStaticText. Returns true if the texts, fonts and text widths
    are equal.
*/
bool BtStaticText::operator==(const BtStaticText &other) const
{
    return (data == other.data
            || (data->text == other.data->text
                && data->font == other.data->font
                && data->textWidth == other.data->textWidth));
}

/*!
    Compares \a other to this BtStaticText. Returns true if the texts, fonts or maximum sizes
    are different.
*/
bool BtStaticText::operator!=(const BtStaticText &other) const
{
    return !(*this == other);
}

/*!
    Sets the text of the BtStaticText to \a text.

    \note This function will cause the layout of the text to require recalculation.

    \sa text()
*/
void BtStaticText::setText(const QString &text)
{
    detach();
    data->text = text;
    data->invalidate();
}

/*!
   Sets the text format of the BtStaticText to \a textFormat. If \a textFormat is set to
   Qt::AutoText (the default), the format of the text will try to be determined using the
   function Qt::mightBeRichText(). If the text format is Qt::PlainText, then the text will be
   displayed as is, whereas it will be interpreted as HTML if the format is Qt::RichText. HTML tags
   that alter the font of the text, its color, or its layout are supported by BtStaticText.

   \note This function will cause the layout of the text to require recalculation.

   \sa textFormat(), setText(), text()
*/
void BtStaticText::setTextFormat(Qt::TextFormat textFormat)
{
    detach();
    data->textFormat = textFormat;
    data->invalidate();
}

/*!
  Returns the text format of the BtStaticText.

  \sa setTextFormat(), setText(), text()
*/
Qt::TextFormat BtStaticText::textFormat() const
{
    return Qt::TextFormat(data->textFormat);
}

/*!
    Returns the text of the BtStaticText.

    \sa setText()
*/
QString BtStaticText::text() const 
{
    return data->text;
}

/*!
  Sets the performance hint of the BtStaticText according to the \a
  performanceHint provided. The \a performanceHint is used to
  customize how much caching is done internally to improve
  performance.

  The default is BtStaticText::ModerateCaching.

  \note This function will cause the layout of the text to require recalculation.

  \sa performanceHint()
*/
void BtStaticText::setPerformanceHint(PerformanceHint performanceHint)
{
    if ((performanceHint == ModerateCaching && !data->useBackendOptimizations)
        || (performanceHint == AggressiveCaching && data->useBackendOptimizations)) {
        return;
    }
    detach();
    data->useBackendOptimizations = (performanceHint == AggressiveCaching);
    data->invalidate();
}

/*!
  Returns which performance hint is set for the BtStaticText.

  \sa setPerformanceHint()
*/
BtStaticText::PerformanceHint BtStaticText::performanceHint() const
{
    return data->useBackendOptimizations ? AggressiveCaching : ModerateCaching;
}

/*!
   Sets the text option structure that controls the layout process to the given \a textOption.

   \sa textOption()
*/
void BtStaticText::setTextOption(const QTextOption &textOption)
{
    detach();
    data->textOption = textOption;
    data->invalidate();
}

/*!
    Returns the current text option used to control the layout process.
*/
QTextOption BtStaticText::textOption() const
{
    return data->textOption;
}

/*!
    Sets the preferred width for this BtStaticText. If the text is wider than the specified width,
    it will be broken into multiple lines and grow vertically. If the text cannot be split into
    multiple lines, it will be larger than the specified \a textWidth.

    Setting the preferred text width to a negative number will cause the text to be unbounded.

    Use size() to get the actual size of the text.

    \note This function will cause the layout of the text to require recalculation.

    \sa textWidth(), size()
*/
void BtStaticText::setTextWidth(qreal textWidth)
{
    detach();
    data->textWidth = textWidth;
    data->invalidate();
}

/*!
    Returns the preferred width for this BtStaticText.

    \sa setTextWidth()
*/
qreal BtStaticText::textWidth() const
{
    return data->textWidth;
}

/*!
  Returns the size of the bounding rect for this BtStaticText.

  \sa textWidth()
*/
QSizeF BtStaticText::size() const
{
    if (data->needsRelayout)
        data->init();
    return data->actualSize;
}

/*!
  Returns the font used to layout text for BtStaticText.
*/
QFont BtStaticText::defaultFont()
{
	return data->font;
}

/*!
  Perform hit test at point \coordinates.
*/
int BtStaticText::hitTest(QPoint coordinates)
{
	QPointF p(coordinates + data->position - QPoint(0, data->font.pixelSize() / 4));

	float d = 0.0;
	int c = -1;
	int cc = 0;

	for (int item = 0; item < data->itemCount; ++item)
	{
		QStaticTextItem *textItem = data->items + item;
		for (int i = 0; i < textItem->numGlyphs; ++i, ++cc)
		{
			QPointF pp(textItem->glyphPositions[i].toPointF());
			if(pp.x() < p.x() && pp.y() > p.y())
			{
				QPointF ppp(p - pp);
				qreal l = qSqrt(qPow(ppp.x(), 2) + qPow(ppp.y(), 2));

				if(c == -1 || l < d)
				{
					c = cc;
					d = l;
				}
			}
		}
	}

	return c;
}

/*!
  Return string consists of only drawable characters.
*/
QString BtStaticText::plainText()
{
	QString t;

	for (int item = 0; item < data->itemCount; ++item)
	{
		QStaticTextItem *textItem = data->items + item;
		for (int i=0; i < textItem->numGlyphs; ++i)
			t += textItem->chars[i];
	}

	return t;
}

BtStaticTextPrivate::BtStaticTextPrivate()
        : textWidth(-1.0), items(0), itemCount(0), glyphPool(0), positionPool(0), charPool(0),
          needsRelayout(true), useBackendOptimizations(false), textFormat(Qt::AutoText),
          untransformedCoordinates(false)
{
}

BtStaticTextPrivate::BtStaticTextPrivate(const BtStaticTextPrivate &other)
    : text(other.text), font(other.font), textWidth(other.textWidth), matrix(other.matrix),
      items(0), itemCount(0), glyphPool(0), positionPool(0), charPool(0), textOption(other.textOption),
      needsRelayout(true), useBackendOptimizations(other.useBackendOptimizations),
      textFormat(other.textFormat), untransformedCoordinates(other.untransformedCoordinates)
{
}

BtStaticTextPrivate::~BtStaticTextPrivate()
{
    delete[] items;
    delete[] glyphPool;
    delete[] positionPool;
    delete[] charPool;
}

BtStaticTextPrivate *BtStaticTextPrivate::get(const BtStaticText *q)
{
    return q->data.data();
}

namespace {

    class DrawTextItemRecorder: public QPaintEngine
    {
    public:
        DrawTextItemRecorder(bool untransformedCoordinates, bool useBackendOptimizations)
                : m_dirtyPen(false), m_useBackendOptimizations(useBackendOptimizations),
                  m_untransformedCoordinates(untransformedCoordinates), m_currentColor(Qt::black)
        {
        }

        virtual void updateState(const QPaintEngineState &newState)
        {
            if (newState.state() & QPaintEngine::DirtyPen
                && newState.pen().color() != m_currentColor) {
                m_dirtyPen = true;
                m_currentColor = newState.pen().color();
            }
        }

        virtual void drawTextItem(const QPointF &position, const QTextItem &textItem)
        {
            const QTextItemInt &ti = static_cast<const QTextItemInt &>(textItem);

			QStaticTextItem currentItem;
			//currentItem.fontEngine = ti.fontEngine;
			currentItem.setFontEngine(ti.fontEngine);
            currentItem.font = ti.font();
            currentItem.charOffset = m_chars.size();
            currentItem.numChars = ti.num_chars;
            currentItem.glyphOffset = m_glyphs.size(); // Store offset into glyph pool
            currentItem.positionOffset = m_glyphs.size(); // Offset into position pool
            currentItem.useBackendOptimizations = m_useBackendOptimizations;
            if (m_dirtyPen)
                currentItem.color = m_currentColor;

            QTransform matrix = m_untransformedCoordinates ? QTransform() : state->transform();
            matrix.translate(position.x(), position.y());

            QVarLengthArray<glyph_t> glyphs;
            QVarLengthArray<QFixedPoint> positions;
            ti.fontEngine->getGlyphPositions(ti.glyphs, matrix, ti.flags, glyphs, positions);

            int size = glyphs.size();
            Q_ASSERT(size == positions.size());
            currentItem.numGlyphs = size;

            m_glyphs.resize(m_glyphs.size() + size);
            m_positions.resize(m_glyphs.size());
            m_chars.resize(m_chars.size() + ti.num_chars);

            glyph_t *glyphsDestination = m_glyphs.data() + currentItem.glyphOffset;
            memcpy(glyphsDestination, glyphs.constData(), sizeof(glyph_t) * currentItem.numGlyphs);

            QFixedPoint *positionsDestination = m_positions.data() + currentItem.positionOffset;
            memcpy(positionsDestination, positions.constData(), sizeof(QFixedPoint) * currentItem.numGlyphs);

            QChar *charsDestination = m_chars.data() + currentItem.charOffset;
            memcpy(charsDestination, ti.chars, sizeof(QChar) * currentItem.numChars);

            m_items.append(currentItem);
        }

        virtual void drawPolygon(const QPointF *, int , PolygonDrawMode )
        {
            /* intentionally empty */
        }

        virtual bool begin(QPaintDevice *)  { return true; }
        virtual bool end() { return true; }
        virtual void drawPixmap(const QRectF &, const QPixmap &, const QRectF &) {}
        virtual Type type() const
        {
            return User;
        }

        QVector<QStaticTextItem> items() const
        {
            return m_items;
        }

        QVector<QFixedPoint> positions() const
        {
            return m_positions;
        }

        QVector<glyph_t> glyphs() const
        {
            return m_glyphs;
        }

        QVector<QChar> chars() const
        {
            return m_chars;
        }

    private:
        QVector<QStaticTextItem> m_items;
        QVector<QFixedPoint> m_positions;
        QVector<glyph_t> m_glyphs;
        QVector<QChar> m_chars;

        bool m_dirtyPen;
        bool m_useBackendOptimizations;
        bool m_untransformedCoordinates;
        QColor m_currentColor;
    };

    class DrawTextItemDevice: public QPaintDevice
    {
    public:
        DrawTextItemDevice(bool untransformedCoordinates, bool useBackendOptimizations)
        {
            m_paintEngine = new DrawTextItemRecorder(untransformedCoordinates,
                                                     useBackendOptimizations);
        }

        ~DrawTextItemDevice()
        {
            delete m_paintEngine;
        }

        int metric(PaintDeviceMetric m) const
        {
            int val;
            switch (m) {
            case PdmWidth:
            case PdmHeight:
            case PdmWidthMM:
            case PdmHeightMM:
                val = 0;
                break;
            case PdmDpiX:
            case PdmPhysicalDpiX:
                val = qt_defaultDpiX();
                break;
            case PdmDpiY:
            case PdmPhysicalDpiY:
                val = qt_defaultDpiY();
                break;
            case PdmNumColors:
                val = 16777216;
                break;
            case PdmDepth:
                val = 24;
                break;
            default:
                val = 0;
                qWarning("DrawTextItemDevice::metric: Invalid metric command");
            }
            return val;
        }

        virtual QPaintEngine *paintEngine() const
        {
            return m_paintEngine;
        }

        QVector<glyph_t> glyphs() const
        {
            return m_paintEngine->glyphs();
        }

        QVector<QFixedPoint> positions() const
        {
            return m_paintEngine->positions();
        }

        QVector<QStaticTextItem> items() const
        {
            return m_paintEngine->items();
        }

        QVector<QChar> chars() const
        {
            return m_paintEngine->chars();
        }

    private:
        DrawTextItemRecorder *m_paintEngine;
    };
}

void BtStaticTextPrivate::paintText(const QPointF &topLeftPosition, QPainter *p)
{
    bool preferRichText = textFormat == Qt::RichText
                          || (textFormat == Qt::AutoText && Qt::mightBeRichText(text));

    if (!preferRichText) {
        QTextLayout textLayout;
        textLayout.setText(text);
        textLayout.setFont(font);
        textLayout.setTextOption(textOption);

        qreal leading = QFontMetricsF(font).leading();
        qreal height = -leading;

        textLayout.beginLayout();
        while (1) {
            QTextLine line = textLayout.createLine();
            if (!line.isValid())
                break;

            if (textWidth >= 0.0)
                line.setLineWidth(textWidth);
            height += leading;
            line.setPosition(QPointF(0.0, height));
            height += line.height();
        }
        textLayout.endLayout();

        actualSize = textLayout.boundingRect().size();
        textLayout.draw(p, topLeftPosition);
    } else {
        QTextDocument document;
//#ifndef QT_NO_CSSPARSER
//        QColor color = p->pen().color();
//        document.setDefaultStyleSheet(QString::fromLatin1("body { color: #%1%2%3 }")
//                                      .arg(QString::number(color.red(), 16), 2, QLatin1Char('0'))
//                                      .arg(QString::number(color.green(), 16), 2, QLatin1Char('0'))
//                                      .arg(QString::number(color.blue(), 16), 2, QLatin1Char('0')));
//#endif
        document.setDefaultFont(font);
        document.setDocumentMargin(0.0);        
#ifndef QT_NO_TEXTHTMLPARSER
        document.setHtml(text);
#else
        document.setPlainText(text);
#endif
        if (textWidth >= 0.0)
            document.setTextWidth(textWidth);
        else
            document.adjustSize();
        document.setDefaultTextOption(textOption);

        p->save();
        p->translate(topLeftPosition);
        QAbstractTextDocumentLayout::PaintContext ctx;
        //ctx.palette.setColor(QPalette::Text, p->pen().color());
        document.documentLayout()->draw(p, ctx);
        p->restore();

        //if (textWidth >= 0.0)
        //    document.adjustSize(); // Find optimal size

        actualSize = document.size();
    }
}

void BtStaticTextPrivate::init()
{
	delete[] items;
	delete[] glyphPool;
	delete[] positionPool;
	delete[] charPool;

	position = QPointF(0, 0);

	DrawTextItemDevice device(untransformedCoordinates, useBackendOptimizations);
	{
		QPainter painter(&device);
		painter.setFont(font);
		painter.setTransform(matrix);

		paintText(QPointF(0, 0), &painter);
	}

	QVector<QStaticTextItem> deviceItems = device.items();
	QVector<QFixedPoint> positions = device.positions();
	QVector<glyph_t> glyphs = device.glyphs();
	QVector<QChar> chars = device.chars();

	itemCount = deviceItems.size();
	items = new QStaticTextItem[itemCount];

	glyphPool = new glyph_t[glyphs.size()];
	memcpy(glyphPool, glyphs.constData(), glyphs.size() * sizeof(glyph_t));

	positionPool = new QFixedPoint[positions.size()];
	memcpy(positionPool, positions.constData(), positions.size() * sizeof(QFixedPoint));

	charPool = new QChar[chars.size()];
	memcpy(charPool, chars.constData(), chars.size() * sizeof(QChar));

	for (int i=0; i<itemCount; ++i) {
		items[i] = deviceItems.at(i);

		items[i].glyphs = glyphPool + items[i].glyphOffset;
		items[i].glyphPositions = positionPool + items[i].positionOffset;
		items[i].chars = charPool + items[i].charOffset;
	}

	//for(int i = 0; i < itemCount; ++i)
	//	qDebug() << charPool[i] << positionPool[i];

	needsRelayout = false;
}
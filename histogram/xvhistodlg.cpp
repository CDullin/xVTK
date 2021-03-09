#include "xvhistodlg.h"

#include <QResizeEvent>
#include <QLabel>
#include <QGraphicsSceneHoverEvent>
#include <QLineEdit>
#include <QDoubleValidator>
#include <QHeaderView>

#include <math.h>
#include <stdlib.h>

using namespace std;

xVHistoDlg::xVHistoDlg(QWidget *parent) :
    QDialog(parent)
{
    setWindowTitle("xVTK - histogram");
    if (QFileInfo("xVHistoSettings.dat").exists()) loadSettings();
    else
    {
        _paramMp["live mode"]._id=1;
        _paramMp["live mode"]._value=true;
        _paramMp["histogram color"]._id=2;
        _paramMp["histogram color"]._value=QVariant::fromValue(QColor(Qt::blue));
        _paramMp["background color"]._id=3;
        _paramMp["background color"]._value=QVariant::fromValue(QColor(Qt::black));
        _paramMp["axis color"]._id=4;
        _paramMp["axis color"]._value=QVariant::fromValue(QColor(Qt::white));
        _paramMp["show axis"]._id=5;
        _paramMp["show axis"]._value=true;
        _paramMp["grid color"]._id=6;
        _paramMp["grid color"]._value=QVariant::fromValue(QColor(Qt::darkGray));
        _paramMp["show grid"]._id=7;
        _paramMp["show grid"]._value=true;
        _paramMp["zoom active"]._id=8;
        _paramMp["zoom active"]._value=true;
        _paramMp["zoom selector color"]._id=9;
        _paramMp["zoom selector color"]._value=QVariant::fromValue(QColor(Qt::red));
        _paramMp["control node color"]._id=10;
        _paramMp["control node color"]._value=QVariant::fromValue(QColor(Qt::white));
        _paramMp["highlight color"]._id=11;
        _paramMp["highlight color"]._value=QVariant::fromValue(QColor(Qt::yellow));
        _paramMp["opacity function color"]._id=12;
        _paramMp["opacity function color"]._value=QVariant::fromValue(QColor(Qt::yellow));
        _paramMp["histogram mode"]._id=13;
        _paramMp["histogram mode"]._value="relative";
        _paramMp["histogram colorization"]._id=14;
        _paramMp["histogram colorization"]._value=true;
    }

    pVBLayout = new QVBoxLayout(this);
    pHBLayout = new QHBoxLayout(this);
    pCancelTB = new QToolButton(this);
    pCancelTB->setText("&cancel");
    connect(pCancelTB,SIGNAL(clicked()),this,SLOT(reject()));
    pApplyTB = new QToolButton(this);
    pApplyTB->setText("&apply");
    connect(pApplyTB,SIGNAL(clicked()),this,SLOT(accept()));
    pLEdit = new QLineEdit(this);
    pLEdit->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    pLEdit->setReadOnly(true);
    QLabel *pLabel=new QLabel("histogram mode",this);
    pHistoGV = new QGraphicsView(this);
    pHistoGV->setScene(new QGraphicsScene());
    pHistoGV->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
    pHistoGV->setRenderHints(QPainter::TextAntialiasing|QPainter::HighQualityAntialiasing|QPainter::Antialiasing);
    pStatusLab = new QLabel(this);
    pStatusLab->setMinimumWidth(50);
    pStatusLab->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Preferred);
    /*
    QLinearGradient gr(QPointF(0,0),QPointF(500,312));
    gr.setColorAt(0,QColor(255,255,255));
    gr.setColorAt(1,QColor(100,100,100));
    pHistoGV->setBackgroundBrush(QBrush(gr));
    */
    QPixmap pix("://images/seamless-circuit-board.png");
    pix=pix.scaledToWidth(200,Qt::SmoothTransformation);
    //pHistoGV->setBackgroundBrush(QBrush(pix));
    pHistoGV->setBackgroundBrush(QBrush(_paramMp["background color"]._value.value<QColor>()));

    pHBLayout->addWidget(pLabel);
    pHBLayout->addWidget(pLEdit);
    pHBLayout->addWidget(pStatusLab);
    pHBLayout->addWidget(pCancelTB);
    pHBLayout->addWidget(pApplyTB);
    pHBLayout->setMargin(0);
    pHBLayout->setSpacing(6);

    pPropTableWdgt=new xVPropTable();
    pPropTableWdgt->setMaximumHeight(75);
    pPropTableWdgt->setMinimumHeight(75);
    pPropTableWdgt->setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Fixed);
    pPropTableWdgt->updateTable(&_paramMp,nullptr);
    pPropTableWdgt->verticalHeader()->setVisible(false);
    pPropTableWdgt->horizontalHeader()->setVisible(false);
    pPropTableWdgt->horizontalHeader()->setStretchLastSection(true);
    connect(pPropTableWdgt,SIGNAL(paramModified(const QString&)),this,SLOT(paramModified_SLOT(const QString&)));

    pVBLayout->addWidget(pHistoGV);
    pVBLayout->addWidget(pPropTableWdgt);
    pVBLayout->addItem(pHBLayout);
    pVBLayout->setMargin(2);

    setMinimumSize(500,512);
    pVBLayout->setGeometry(QRect(3,3,500-6,512-6));
    updateView();
}

void xVHistoDlg::saveSettings()
{
    QFile f("xVHistoSettings.dat");
    if (f.open(QFile::WriteOnly))
    {
        QDataStream d(&f);
        d << (quint16)_paramMp.count();
        for (QMap<QString,xPROP_TYPE>::iterator it=_paramMp.begin();it!=_paramMp.end();++it)
        {
            d << it.key();
            it->save(d);
        }
        f.close();
    }
}

void xVHistoDlg::loadSettings()
{
    QFile f("xVHistoSettings.dat");
    if (f.open(QFile::ReadOnly))
    {
        QDataStream d(&f);
        quint16 count; d >> count;

        for (int i=0;i<count;++i)
        {
            QString k;d >> k;
            xPROP_TYPE prop(d);
            _paramMp[k]=prop;
        }
        f.close();
    }
}

void xVHistoDlg::paramModified_SLOT(const QString& name)
{
    const char* pattern[]={"live mode","histogram color","background color",
                           "axis color","show axis","grid color",
                           "show grid","zoom active","zoom selector color",
                           "control node color","highlight color","opacity function color",
                           "histogram mode","histogram colorization"};
    int nr=-1;
    for (int i=0;i<14;++i)
        if (name==pattern[i]) nr=i;

    switch (nr)
    {
    case 0 : // live mode
        break;
    case 13: // histogram colorization
    case 12: // mode (logarithmic etc.)
    case 7 : // zoom active
    case 8 : // zoom selector color
    case 1 : // histo color
    case 2 : // background color
        updateView();
        break;
    case 3 : // axis color
        if (pHorAllAxis) pHorAllAxis->setColor(_paramMp["axis color"]._value.value<QColor>());
        if (pVerAllAxis) pVerAllAxis->setColor(_paramMp["axis color"]._value.value<QColor>());
        if (pHorZoomAxis) pHorZoomAxis->setColor(_paramMp["axis color"]._value.value<QColor>());
        if (pVerZoomAxis) pVerZoomAxis->setColor(_paramMp["axis color"]._value.value<QColor>());
        break;
    case 4 : // axis visible
        if (pHorAllAxis) pHorAllAxis->setVisible(_paramMp["show axis"]._value.toBool());
        if (pVerAllAxis) pVerAllAxis->setVisible(_paramMp["show axis"]._value.toBool());
        if (pHorZoomAxis) pHorZoomAxis->setVisible(_paramMp["show axis"]._value.toBool());
        if (pVerZoomAxis) pVerZoomAxis->setVisible(_paramMp["show axis"]._value.toBool());
        break;
    case 5 : // grid color
        if (pHorAllAxis) pHorAllAxis->setGridColor(_paramMp["grid color"]._value.value<QColor>());
        if (pVerAllAxis) pVerAllAxis->setGridColor(_paramMp["grid color"]._value.value<QColor>());
        if (pHorZoomAxis) pHorZoomAxis->setGridColor(_paramMp["grid color"]._value.value<QColor>());
        if (pVerZoomAxis) pVerZoomAxis->setGridColor(_paramMp["grid color"]._value.value<QColor>());
        break;
    case 6 : // grid visible
        if (pHorAllAxis) pHorAllAxis->setGridVisible(_paramMp["show grid"]._value.toBool());
        if (pVerAllAxis) pVerAllAxis->setGridVisible(_paramMp["show grid"]._value.toBool());
        if (pHorZoomAxis) pHorZoomAxis->setGridVisible(_paramMp["show grid"]._value.toBool());
        if (pVerZoomAxis) pVerZoomAxis->setGridVisible(_paramMp["show grid"]._value.toBool());
        break;
    case 9 : // control node color
        for (int i=0;i<_opacityNodeLst.count();++i)
            _opacityNodeLst[i]->setShapeColor(_paramMp["control node color"]._value.value<QColor>());
        for (int i=0;i<_colorNodeLst.count();++i)
            _colorNodeLst[i]->setShapeColor(_paramMp["control node color"]._value.value<QColor>());
        break;
    case 10 : // highlight color
        if (pZoomLeft) pZoomLeft->setHighlightColor(_paramMp["highlight color"]._value.value<QColor>());
        if (pZoomRight) pZoomRight->setHighlightColor(_paramMp["highlight color"]._value.value<QColor>());
        for (int i=0;i<_opacityNodeLst.count();++i)
            _opacityNodeLst[i]->setHighlightColor(_paramMp["control node color"]._value.value<QColor>());
        for (int i=0;i<_colorNodeLst.count();++i)
            _colorNodeLst[i]->setHighlightColor(_paramMp["control node color"]._value.value<QColor>());
        break;
    case 11 : // opacity function color
        if (pOpacityPathItem)
        {
            QPen pen=pOpacityPathItem->pen();pen.setColor(_paramMp["opacity function color"]._value.value<QColor>());pOpacityPathItem->setPen(pen);
        }
        if (pZoomOpacityPathItem)
        {
            QPen pen=pZoomOpacityPathItem->pen();pen.setColor(_paramMp["opacity function color"]._value.value<QColor>());pZoomOpacityPathItem->setPen(pen);
        }
        break;
    default:
        emit KSignal(ST_WARN_MSG,new QString("unhandled parameter <"+name+"> in histogram dialog"));
        break;
    }
}

xVHistoDlg::~xVHistoDlg()
{
    // free space?
    saveSettings();
}

void xVHistoDlg::accept()
{
    return QDialog::accept();
}

void xVHistoDlg::reject()
{
    switch (_mode)
    {
    case HDM_LUT:
    {
        if (colorTransFuncRef && cpyColorTransFuncRef)
        {
            colorTransFuncRef->RemoveAllPoints();
            double v[6];
            for (int i=0;i<cpyColorTransFuncRef->GetSize();++i)
            {
                cpyColorTransFuncRef->GetNodeValue(i,v);
                colorTransFuncRef->AddRGBPoint(v[0],v[1],v[2],v[3]);
            }
            cpyColorTransFuncRef->Delete();
            emit modified("color function");
        }
    }
        break;
    case HDM_OPACITY:
    {
        if (opacityFuncRef && cpyOpacityFuncRef)
        {
            opacityFuncRef->RemoveAllPoints();
            double v[4];
            for (int i=0;i<cpyOpacityFuncRef->GetSize();++i)
            {
                cpyOpacityFuncRef->GetNodeValue(i,v);
                opacityFuncRef->AddPoint(v[0],v[1]);
            }
            cpyOpacityFuncRef->Delete();
            emit modified("opacity function");
        }
    }
        break;
    }

    return QDialog::reject();
}

void xVHistoDlg::resizeEvent(QResizeEvent *event)
{
    QDialog::resizeEvent(event);
    pVBLayout->setGeometry(QRect(3,3,width()-6,height()-6));
    /*
    QLinearGradient gr(QPointF(0,0),QPointF(width(),height()));
    gr.setColorAt(0,QColor(255,255,255));
    gr.setColorAt(1,QColor(100,100,100));
    pHistoGV->setBackgroundBrush(QBrush(gr));
    */
    updateView();
}

void xVHistoDlg::updateView()
{
    pHistoGV->setBackgroundBrush(QBrush(_paramMp["background color"]._value.value<QColor>()));
    // generate two pixmaps and the coordinate axis
    if (pData)                       
    {
        double w = pHistoGV->width()*.8;
        double h = pHistoGV->height()*.8;
        pHistoGV->setSceneRect(0,0,w,h);

        bool ok;
        // calculate meaningful mim, max
        _all = generatePixmap(pData->info()._dataTypeMin,pData->info()._dataTypeMax,w*0.95,_paramMp["zoom active"]._value.toBool() ? h*0.25 : h*0.8);
        if (!pAllHistoPixItem)
        {
            pAllHistoPixItem = new xVHistoPixmapItem(this);
            connect(pAllHistoPixItem,SIGNAL(cursorPos(QPointF)),this,SLOT(dispCursorPos(QPointF)));
            connect(pAllHistoPixItem,SIGNAL(dblClick(QPointF)),this,SLOT(dblClickInAll(QPointF)));
            pHistoGV->scene()->addItem(pAllHistoPixItem);
        }
        pAllHistoPixItem->setPixmap(_all.pix);
        pAllHistoPixItem->setZValue(-10);

        if (!pZoomLeft)
        {
            pZoomLeft = new xVHistoVLine(HLM_ZOOM_LEFT,this);
            connect(pZoomLeft,SIGNAL(moved(QPointF,const int&)),this,SLOT(interactiveElementMoved(QPointF,const int&)));
            connect(pZoomLeft,SIGNAL(dblClick(QPointF,const int&)),this,SLOT(interactiveElementDblClicked(QPointF,const int&)));
            pZoomLeft->setZValue(1);
            pZoomLeft->setParentItem(pAllHistoPixItem);
            pZoomLeft->setPos(mapGValToAllPixmap(pData->info()._minVal,ok),0);
        }
        else
            pZoomLeft->setPos(mapGValToAllPixmap(_zoom._dispMin,ok),0);

        pZoomLeft->setPen(QPen(_paramMp["zoom selector color"]._value.value<QColor>(),2));
        pZoomLeft->setLimits(0,pAllHistoPixItem->pixmap().width());
        pZoomLeft->setLine(QLine(0,0,0,pAllHistoPixItem->pixmap().height()));

        if (!pZoomRight)
        {
            pZoomRight = new xVHistoVLine(HLM_ZOOM_RIGHT,this);
            connect(pZoomRight,SIGNAL(moved(QPointF,const int&)),this,SLOT(interactiveElementMoved(QPointF,const int&)));
            connect(pZoomRight,SIGNAL(dblClick(QPointF,const int&)),this,SLOT(interactiveElementDblClicked(QPointF,const int&)));
            pZoomRight->setZValue(1);
            pZoomRight->setParentItem(pAllHistoPixItem);
            pZoomRight->setPos(mapGValToAllPixmap(pData->info()._maxVal,ok),0);
        }
        else
            pZoomRight->setPos(mapGValToAllPixmap(_zoom._dispMax,ok),0);
        pZoomRight->setPen(QPen(_paramMp["zoom selector color"]._value.value<QColor>(),2));
        pZoomRight->setLimits(0,pAllHistoPixItem->pixmap().width());
        pZoomRight->setLine(QLine(0,0,0,pAllHistoPixItem->pixmap().height()));

        if (!pZoomHistoPixItem)
        {
            _zoom = generatePixmap(pData->info()._minVal,pData->info()._maxVal,w*0.95,h*0.25);
            pZoomHistoPixItem = new xVHistoPixmapItem(this);
            connect(pZoomHistoPixItem,SIGNAL(cursorPos(QPointF)),this,SLOT(dispCursorPos(QPointF)));
            connect(pZoomHistoPixItem,SIGNAL(dblClick(QPointF)),this,SLOT(dblClickInZoom(QPointF)));
            pHistoGV->scene()->addItem(pZoomHistoPixItem);
        }
        else
            _zoom = generatePixmap(_zoom._dispMin,_zoom._dispMax,w*0.95,h*0.25);

        pZoomHistoPixItem->setPixmap(_zoom.pix);
        pZoomHistoPixItem->setZValue(-10);

        if (!pZoomConLeft)
        {
            pZoomConLeft = new QGraphicsLineItem();
            pZoomConLeft->setAcceptedMouseButtons(Qt::NoButton);
            pHistoGV->scene()->addItem(pZoomConLeft);
        }
        pZoomConLeft->setPen(QPen(_paramMp["zoom selector color"]._value.value<QColor>(),2));

        if (!pZoomConRight)
        {
            pZoomConRight = new QGraphicsLineItem();
            pZoomConRight->setAcceptedMouseButtons(Qt::NoButton);
            pHistoGV->scene()->addItem(pZoomConRight);
        }
        pZoomConRight->setPen(QPen(_paramMp["zoom selector color"]._value.value<QColor>(),2));

        if (!pHorAllAxis)
        {
            pHorAllAxis=new xVHistoHValueAxis(HLM_HOR_AXIS_ALL,this);
            pHorAllAxis->setReferences(&_all,pAllHistoPixItem,this);
            pHistoGV->scene()->addItem(pHorAllAxis);
        }

        pHorAllAxis->setZValue(-1);

        if (!pHorZoomAxis)
        {
            pHorZoomAxis=new xVHistoHValueAxis(HLM_HOR_AXIS_ZOOM,this);
            pHorZoomAxis->setReferences(&_zoom,pZoomHistoPixItem,this);
            pHistoGV->scene()->addItem(pHorZoomAxis);
        }

        pHorZoomAxis->setZValue(-1);

        if (!pVerAllAxis)
        {
            pVerAllAxis=new xVHistoVValueAxis(HLM_VER_AXIS_ALL,this);
            pVerAllAxis->setReferences(&_all,pAllHistoPixItem,this);
            pHistoGV->scene()->addItem(pVerAllAxis);
        }

        pVerAllAxis->setZValue(-1);

        if (!pVerZoomAxis)
        {
            pVerZoomAxis=new xVHistoVValueAxis(HLM_VER_AXIS_ZOOM,this);
            pVerZoomAxis->setReferences(&_zoom,pZoomHistoPixItem,this);
            pHistoGV->scene()->addItem(pVerZoomAxis);
        }

        pVerZoomAxis->setZValue(-1);

        if (!pColorBarPixItem)
        {
            pColorBarPixItem = new xVHistoPixmapItem(this);
            pHistoGV->scene()->addItem(pColorBarPixItem);
        }
        pColorBarPixItem->setPixmap(generateColorBar(w*0.95,h*.1));
        connect(pColorBarPixItem,SIGNAL(dblClick(QPointF)),this,SLOT(dblClickInColorBar(QPointF)));

        if (!pZoomColorBarPixItem)
        {
            pZoomColorBarPixItem = new xVHistoPixmapItem(this);
            pHistoGV->scene()->addItem(pZoomColorBarPixItem);
        }
        pZoomColorBarPixItem->setPixmap(generateColorBar(w*0.95,h*.1,true));
        connect(pZoomColorBarPixItem,SIGNAL(dblClick(QPointF)),this,SLOT(dblClickInZoomColorBar(QPointF)));

        switch (_mode)
        {
        case HDM_OPACITY: createOpacityFunc();break;
        case HDM_LUT: createColorFunc();break;
        }

        if (!_paramMp["zoom active"]._value.toBool())
        {
            if (pZoomColorBarPixItem) pZoomColorBarPixItem->setVisible(false);
            if (pZoomConLeft) pZoomConLeft->setVisible(false);
            if (pZoomConRight) pZoomConRight->setVisible(false);
            if (pZoomHistoPixItem) pZoomHistoPixItem->setVisible(false);
            if (pZoomLeft) pZoomLeft->setVisible(false);
            if (pZoomOpacityPathItem) pZoomOpacityPathItem->setVisible(false);
            if (pZoomRight) pZoomRight->setVisible(false);
            if (pVerZoomAxis) pVerZoomAxis->setVisible(false);
            if (pHorZoomAxis) pHorZoomAxis->setVisible(false);

            pAllHistoPixItem->setPos(10,0.05*h);
            pColorBarPixItem->setPos(10,0.95*h);
            pHorAllAxis->setPos(pAllHistoPixItem->pos().x(),pAllHistoPixItem->pos().y()+pAllHistoPixItem->pixmap().height()+5);
            pVerAllAxis->setPos(pAllHistoPixItem->pos().x()-5,pAllHistoPixItem->pos().y());

            updateColorFunc();
            updateOpacityFunc();
        }
        else
        {
            if (pZoomColorBarPixItem) pZoomColorBarPixItem->setVisible(true);
            if (pZoomConLeft) pZoomConLeft->setVisible(true);
            if (pZoomConRight) pZoomConRight->setVisible(true);
            if (pZoomHistoPixItem) pZoomHistoPixItem->setVisible(true);
            if (pZoomLeft) pZoomLeft->setVisible(true);
            if (pZoomOpacityPathItem) pZoomOpacityPathItem->setVisible(true);
            if (pZoomRight) pZoomRight->setVisible(true);
            if (pVerZoomAxis) pVerZoomAxis->setVisible(true);
            if (pHorZoomAxis) pHorZoomAxis->setVisible(true);

            pAllHistoPixItem->setPos(10,0.60*h);
            pZoomHistoPixItem->setPos(10,0.15*h);
            pColorBarPixItem->setPos(10,0.95*h);
            pZoomColorBarPixItem->setPos(10,-10);
            pHorAllAxis->setPos(pAllHistoPixItem->pos().x(),pAllHistoPixItem->pos().y()+pAllHistoPixItem->pixmap().height()+5);
            pVerAllAxis->setPos(pAllHistoPixItem->pos().x()-5,pAllHistoPixItem->pos().y());
            pVerZoomAxis->setPos(pZoomHistoPixItem->pos().x()-5,pZoomHistoPixItem->pos().y());
            pHorZoomAxis->setPos(pZoomHistoPixItem->pos().x(),pZoomHistoPixItem->pos().y()+pZoomHistoPixItem->pixmap().height()+5);

            QPointF zoomPixBottomLeft = pZoomHistoPixItem->pos()+QPointF(0,pZoomHistoPixItem->pixmap().height());
            QPointF zoomPixBottomRight = pZoomHistoPixItem->pos()+QPointF(pZoomHistoPixItem->pixmap().width(),pZoomHistoPixItem->pixmap().height());
            pZoomConLeft->setLine(QLineF(pZoomLeft->mapToScene(QPointF(0,0)),zoomPixBottomLeft));
            pZoomConRight->setLine(QLineF(pZoomRight->mapToScene(QPointF(0,0)),zoomPixBottomRight));

            updateColorFunc();
            updateOpacityFunc();
        }

        pHorAllAxis->updateAxis();
        pHorZoomAxis->updateAxis();
        pVerAllAxis->updateAxis();
        pVerZoomAxis->updateAxis();
    }
}

void xVHistoDlg::setMode(const HISTO_DLG_MODE& m)
{
    _mode = m;
    const char* pattern[]={"window & level","opacity transfer function","color transfer function","measurement"};
    pLEdit->setText(pattern[m]);
    updateView();
}


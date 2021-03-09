#include "xvtkdlg.h"
#include "ui_xvtkdlg.h"
#include "xVObjects.h"
#include "xVPolyObj.h"
#include "xVConnector.h"
#include "xVConnectorObj.h"
#include "xVTypes.h"
#include "xVObjectTypes.h"
#include "xvproptable.h"
#include <QGraphicsScene>
#include <QMessageBox>
#include <QResizeEvent>
#include <QWindow>
#include <QDockWidget>
#include <QAction>
#include <vtkAutoInit.h>
#include <vtkRenderingVolumeModule.h>
#include <vtkRenderingOpenGL2ObjectFactory.h>
#include <vtkRenderingVolumeOpenGL2ObjectFactory.h>
#include <vtkRenderingVolumeOpenGL2Module.h>
#include <QKeyEvent>
#include <sys/sysinfo.h>
#include <QStorageInfo>

QMap<QString,LIMITS> _limitMp;

xVTKDlg::xVTKDlg(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::xVTKDlg)
{

    vtkObjectFactory::RegisterFactory(vtkRenderingOpenGL2ObjectFactory::New());
    vtkObjectFactory::RegisterFactory(vtkRenderingVolumeOpenGL2ObjectFactory::New());

    ui->setupUi(this);
    //ui->toolBox->setCurrentIndex(0);
    xVGraphicsScene *pScene=new xVGraphicsScene();
    connect(pScene,SIGNAL(dblClicked()),this,SLOT(deselectAll()));
    ui->pDashBoardGV->setScene(pScene);
    ui->pDashBoardGV->scene()->setSceneRect(QRectF(0,0,ui->pDashBoardGV->width()-5,ui->pDashBoardGV->height()-5));
    ui->pDashBoardGV->setRenderHints(QPainter::Antialiasing|QPainter::TextAntialiasing|QPainter::HighQualityAntialiasing);
    ui->pTabWdgt->installEventFilter(this);
    QPixmap pix("://images/seamless-circuit-board.png");
    pix=pix.scaledToWidth(200,Qt::SmoothTransformation);
    ui->pDashBoardGV->scene()->setBackgroundBrush(QBrush(pix));

    QVBoxLayout *pVBox=new QVBoxLayout(ui->pDataTab);
    pVBox->addWidget(ui->pDashBoardGV);
    pVBox->setMargin(3);

    // main win
    pVisMainWin = new xVVisMainWin(ui->pMainVisTab);
    pVisMainWin->setWindowFlags(Qt::Widget);
    pVisMainWin->installEventFilter(this);
    pVBox=new QVBoxLayout(ui->pMainVisTab);
    pVBox->addWidget(pVisMainWin);
    pVBox->setMargin(3);

    connect(ui->pClearTB,SIGNAL(clicked()),this,SLOT(clear()));
    connect(ui->pSaveTB,SIGNAL(clicked()),this,SLOT(save()));
    connect(ui->pLoadTB,SIGNAL(clicked()),this,SLOT(load()));
    connect(ui->pVerifyTB,SIGNAL(clicked()),this,SLOT(verify()));
    connect(ui->pResetTB,SIGNAL(clicked()),this,SLOT(reset()));
    connect(ui->pRunTB,SIGNAL(clicked()),this,SLOT(run()));
    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void *)),ui->pPropWdgt,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(ui->pPropWdgt,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(ui->pSidePanelWdgt,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void *)),ui->pSidePanelWdgt,SLOT(KSlot(const SIG_TYPE&,void *)));

    ui->pTabWdgt->setCurrentIndex(0);
    show();
    ui->pPropGrpB->hide();
    ui->pDashBoardGV->setGeometry(3,3,ui->pDataTab->width()-6,ui->pDataTab->height()-7);
    pVisMainWin->setGeometry(3,3,ui->pDataTab->width()-6,ui->pDataTab->height()-7);

    installEventFilter(this);
    ui->pPropWdgt->installEventFilter(this);

    emit KSignal(ST_GENERATE_CONNECTION_WITH_MAIN_DLG,this);
    generateHooks();

    // memory survailance
    QTimer *pMemoryTimer = new QTimer(this);
    pMemoryTimer->setInterval(5000);
    connect(pMemoryTimer,SIGNAL(timeout()),this,SLOT(dispMemoryConsumption()));
    pMemoryTimer->start();
    dispMemoryConsumption();

    clear(false);
}

void xVTKDlg::dispMemoryConsumption()
{
    struct sysinfo info;
    sysinfo(&info);
    ui->pMemBar->setTextVisible(true);
    ui->pMemBar->setFormat("%p% taken");
    ui->pMemBar->setMaximum(info.totalram/1000000);
    ui->pMemBar->setValue((info.totalram-info.freeram)/1000000);

    float ratio = (double)info.freeram/(double)info.totalram;

    ui->pMemBar->setStyleSheet(this->styleSheet());
    if (ratio < 0.33)
    {
        if (ratio < 0.1)
            ui->pMemBar->setStyleSheet("background:red");
        else
            ui->pMemBar->setStyleSheet("background:yellow");
    }

    QStorageInfo sinfo= QStorageInfo::root();
    ui->pHDDBar->setRange(0,sinfo.bytesTotal()/1000000);
    ui->pHDDBar->setTextVisible(true);
    ui->pHDDBar->setFormat("%p% taken");
    ui->pHDDBar->setValue((sinfo.bytesTotal()-sinfo.bytesAvailable())/1000000);
}

void xVTKDlg::generateHooks()
{
    QList <HOOK> _hookLst;
    _hookLst.append(HOOK("objects","volume",ST_CREATE_OBJ_HOOK,xVOT_VOLUME));
    _hookLst.append(HOOK("objects","mesh",ST_CREATE_OBJ_HOOK,xVOT_MESH));
    _hookLst.append(HOOK("user interaction","parameter input",ST_CREATE_OBJ_HOOK,xVOT_USER_TABLE_DLG));
    _hookLst.append(HOOK("properties","volume",ST_CREATE_OBJ_HOOK,xVOT_VOLUME_VIS_PROP));
    _hookLst.append(HOOK("properties","mesh",ST_CREATE_OBJ_HOOK,xVOT_MESH_VIS_PROP));
    _hookLst.append(HOOK("properties","clip plane",ST_CREATE_OBJ_HOOK,xVOT_CLIPPING_PLANE));
    _hookLst.append(HOOK("properties","clip box",ST_CREATE_OBJ_HOOK,xVOT_CLIPPING_BOX));
    _hookLst.append(HOOK("properties","clip sphere",ST_CREATE_OBJ_HOOK,xVOT_CLIPPING_SPHERE));
    _hookLst.append(HOOK("properties","slicer",ST_CREATE_OBJ_HOOK,xVOT_SLICER_OBJ));
    _hookLst.append(HOOK("views","2D",ST_CREATE_OBJ_HOOK,xVOT_2D_VIEW));
    _hookLst.append(HOOK("views","3D",ST_CREATE_OBJ_HOOK,xVOT_3D_VIEW));
    _hookLst.append(HOOK("tools","call external",ST_CREATE_OBJ_HOOK,xVOT_CALL_EXTERNAL));    
    _hookLst.append(HOOK("logic","variable definition",ST_CREATE_OBJ_HOOK,xVOT_VAR_DEFINITION));
    _hookLst.append(HOOK("logic","3 state result",ST_CREATE_OBJ_HOOK,xVOT_TRAFFIC_LIGHT));
    _hookLst.append(HOOK("logic","if",ST_CREATE_OBJ_HOOK,xVOT_IF));
    _hookLst.append(HOOK("logic","mathematical equation",ST_CREATE_OBJ_HOOK,xVOT_MATH));
    _hookLst.append(HOOK("filter","filter",ST_CREATE_OBJ_HOOK,xVOT_FILTER));

    emit KSignal(ST_ADD_ACTION_HOOKS,&_hookLst);
}

bool xVTKDlg::eventFilter(QObject *obj, QEvent *event)
{
    if (event->type()==QEvent::KeyPress)
    {
        QKeyEvent *pKEvent=dynamic_cast<QKeyEvent*>(event);
        if (pKEvent)
        {
            switch (pKEvent->key())
            {
            case Qt::Key_Tab:
                if (pKEvent->modifiers() & Qt::AltModifier) activateNext();
                event->accept();
                break;
            }
        }
    }
    return QWidget::eventFilter(obj,event);
}


void xVTKDlg::activateNext()
{
    if (_objLst.count()==0) return;
    xAbstractBasisObj *pBaseObj = ui->pPropWdgt->activeObj();
    if (pBaseObj)
    {
        int i=_objLst.indexOf(pBaseObj);
        ui->pPropWdgt->objSelected(_objLst.at((i+1)%_objLst.count()));
    }
    else
        ui->pPropWdgt->objSelected(_objLst.at(0));
}

void xVTKDlg::clear(bool verbose)
{
    if (!verbose || QMessageBox::question(0,"Attention!","Do you really like to clear the dash board?")==QMessageBox::Yes)
    {
        reset(false);
        if (_objLst.count()>0)
        {
            xAbstractBasisObj* pItem=_objLst.first();
            while (pItem && _objLst.count()>0)
            {
                _objLst.removeOne(pItem);
                removeObject(pItem);
                _objLst.count()>0 ? pItem = _objLst.first() : pItem=nullptr;
            }
        }
        xVObj_Basics *pVObj = new xVStartObj("#start");
        ui->pDashBoardGV->scene()->addItem(pVObj->item());
        emit KSignal(ST_ADD_OBJECT,pVObj);
    }
}
void xVTKDlg::verify()
{}

void xVTKDlg::placeObjInScene(xVObj_Basics* pObj)
{
    if (!pObj) return;
    bool _placed = false;
    QGraphicsScene *pScene = ui->pDashBoardGV->scene();
    QRectF _sceneRect = ui->pDashBoardGV->sceneRect();
    QPolygonF _objRect = pObj->item()->mapToScene(pObj->item()->boundingRect());
    QPointF _scrPos = pObj->item()->mapToScene(pObj->item()->pos());

    while (!_placed) {
        _placed=true;
        for (QList <xAbstractBasisObj*>::iterator it=_objLst.begin();it!=_objLst.end();++it)
        {
            xVObj_Basics* pCurObj = dynamic_cast<xVObj_Basics*>(*it);
            if (pCurObj && pCurObj!=pObj)
            {
                QPolygonF _curObjRect = pCurObj->item()->mapToScene(pCurObj->item()->boundingRect());
                if (_curObjRect.intersects(_objRect))
                {
                    _placed = false;
                    pObj->item()->moveBy(0,20);
                    _objRect = pObj->item()->mapToScene(pObj->item()->boundingRect());
                    if (_objRect.boundingRect().bottom()>_sceneRect.bottom())
                    {
                        pObj->item()->setPos(pObj->item()->x()+50,20);
                    }
                    _objRect = pObj->item()->mapToScene(pObj->item()->boundingRect());
                    continue;
                }
            }
        }
    }

    snapToGrid(pObj);
}

QString xVTKDlg::uniqueName(QString s)
{
    QString res = s;
    bool _found = false;
    long _id = 2;
    do
    {
        _found = false;
        for (QList <xAbstractBasisObj*>::iterator it=_objLst.begin();it!=_objLst.end();++it)
        {
            xVObj_Basics* pVObj = dynamic_cast<xVObj_Basics*>(*it);
            if (pVObj && pVObj->paramMap()->contains("name") && (*pVObj->paramMap())["name"]._value.toString()==res)
                _found = true;
        }
        if (_found) res=s+QString("%1").arg(_id++);

    } while (_found);
    return res;
}

void xVTKDlg::KSlot(const SIG_TYPE& type,void *pData)
{
    switch (type)
    {
    case ST_ACTION_HOOK_TRIGGERED:
    {
        xVO_TYPE *pOType=(xVO_TYPE*)pData;
        xVObj_Basics *pVObj=nullptr;
        switch (*pOType)
        {
        case xVOT_START:                pVObj = new xVStartObj("#start");                                               break;
        case xVOT_VOLUME:               pVObj = new xVVolObj(uniqueName("#volume"));                                    break;
        case xVOT_MESH:                 pVObj = new xVPolyObj(uniqueName("#mesh"));                                     break;
        case xVOT_2D_VIEW:              pVObj = new xV2DVisObj(uniqueName("#2dView"));                                  break;
        case xVOT_3D_VIEW:              pVObj = new xV3DVisObj(uniqueName("#3dView"));                                  break;
        case xVOT_VOLUME_VIS_PROP:      pVObj = new xVVolumeVisPropObj(uniqueName("volume visualization properties"));  break;
        case xVOT_MESH_VIS_PROP:        pVObj = new xVMeshVisPropObj(uniqueName("mesh visualization properties"));      break;
        case xVOT_CALL_EXTERNAL:        pVObj = new xVCallExternalObj(uniqueName("#execute"));                          break;
        case xVOT_IF:                   pVObj = new xVIFObj(uniqueName("#if"));                                         break;
        case xVOT_FILTER:               pVObj = new xVGenFilterObj(uniqueName("#filter"));                              break;
        case xVOT_USER_TABLE_DLG:       pVObj = new xVUserTableImportDlgObj(uniqueName("#user"));                       break;
        case xVOT_VAR_DEFINITION:       pVObj = new xVVarDefinitionObj(uniqueName("#define"));                          break;
        case xVOT_TRAFFIC_LIGHT:        pVObj = new xVTrafficLightObj(uniqueName("#light"));                            break;
        case xVOT_MATH:                 pVObj = new xVMathObj(uniqueName("#Eq."));                            break;
        default:
            emit KSignal(ST_ERROR_MSG,new QString("unsupported object type for creating hook detected"));
            break;
        }
        if (pVObj)
        {
            ui->pDashBoardGV->scene()->addItem(pVObj->item());
            emit KSignal(ST_ADD_OBJECT,pVObj);
        }
        delete pOType;
    }
        break;
    case ST_ADD_VISUALIZATION_WDGT:
        addVisWidget((QWidget*)pData);
        break;
    case ST_REMOVE_VISUALIZATION_WDGT:
        removeVisWidget((QWidget*)pData);
        break;
    case ST_GLOBAL_NAMESPACE_MODIFIED:
        ui->pGlobalNamespaceTableWdgt->updateTable(&::_globalNameSpace);
        break;
    case ST_RESET_ALL_PROCESS:
        ui->pProg->setTextVisible(false);
    break;
    case ST_SET_ALL_PROCESS_RANGE:
    {
        QPoint *p=(QPoint*)pData;ui->pProg->setRange(p->x(),p->y());delete p;
    }
        break;
    case ST_SET_ALL_PROCESS:
    {
        int *p=(int*)pData;ui->pProg->setValue(*p);delete p;
    }
        break;
    case ST_INC_ALL_PROCESS:ui->pProg->setValue(ui->pProg->value()+1);
        break;
    case ST_SET_ALL_PROCESS_TXT:
    {
        ui->pProg->setTextVisible(true);
        QString *s=(QString*)pData;ui->pProg->setFormat(*s+" %p%");delete s;
    }
        break;
    case ST_SET_PROCESS_RANGE:
    {
        QPoint *p=(QPoint*)pData;ui->pSingleProg->setRange(p->x(),p->y());delete p;
    }
        break;
    case ST_RESET_PROCESS:
        ui->pSingleProg->setTextVisible(false);
        break;
    case ST_SET_PROCESS:
    {
        int *p=(int*)pData;ui->pSingleProg->setValue(*p);delete p;
    }
        break;
    case ST_INC_PROCESS:ui->pSingleProg->setValue(ui->pProg->value()+1);
        break;
    case ST_SET_PROCESS_TXT:
    {
        ui->pSingleProg->setTextVisible(true);
        QString *s=(QString*)pData;ui->pSingleProg->setFormat(*s+" %p%");delete s;
    }
        break;
    case ST_ERROR_MSG:
    {
        QString *pStr=(QString*)pData;
        ui->pMSGBrowser->setTextColor(QColor(Qt::red));
        ui->pMSGBrowser->append(*pStr);
        delete pStr;
    }
        break;
    case ST_WARN_MSG:
    {
        QString *pStr=(QString*)pData;
        ui->pMSGBrowser->setTextColor(QColor(Qt::yellow));
        ui->pMSGBrowser->append(*pStr);
        delete pStr;
    }
        break;
    case ST_MSG:
    {
        QString *pStr=(QString*)pData;
        ui->pMSGBrowser->setTextColor(QColor(Qt::white));
        ui->pMSGBrowser->append(*pStr);
        delete pStr;
    }
        break;

    case ST_ADD_OBJECT: _objLst.append((xAbstractBasisObj*)pData);
        connect((xAbstractBasisObj*)pData,SIGNAL(selected(xAbstractBasisObj*)),ui->pPropWdgt,SLOT(objSelected(xAbstractBasisObj*)));
        connect((xAbstractBasisObj*)pData,SIGNAL(parameterModified()),ui->pPropWdgt,SLOT(parameterModified()));
        connect((xAbstractBasisObj*)pData,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SLOT(KSlot(const SIG_TYPE&,void*)));
        if (dynamic_cast<xVObj_Basics*>((xAbstractBasisObj*)pData))
        {
            dynamic_cast<xVObj_Basics*>((xAbstractBasisObj*)pData)->item()->setPos(50,50);
            connect(dynamic_cast<xVObj_Basics*>((xAbstractBasisObj*)pData),SIGNAL(placed()),this,SLOT(snapToGrid()));
            connect(dynamic_cast<xVObj_Basics*>((xAbstractBasisObj*)pData),SIGNAL(connectorActivated(xVObj_Basics*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated(xVObj_Basics*,xCONNECTOR_TYPE)));
        }
        placeObjInScene(dynamic_cast<xVObj_Basics*>((xAbstractBasisObj*)pData));
        emit KSignal(ST_OBJECT_ADDED,pData);
        break;
    case ST_REMOVE_OBJECT: removeObject((xVObj_Basics*)pData);break;
    }
}

void xVTKDlg::removeObject(xAbstractBasisObj* pObj)
{
    QList <xAbstractBasisObj*> killLst;
    killLst.append(pObj);
    xVObj_Basics *pBaseObj = dynamic_cast<xVObj_Basics*>(pObj);
    xConnectorObj *pConBaseObj = dynamic_cast<xConnectorObj*>(pObj);
    if (pConBaseObj)
    {
        if (pConBaseObj->outputObj() && pConBaseObj->inputObj()) {
            pConBaseObj->outputObj()->removeConObj(pConBaseObj->inputObj()->baseObj());
            pConBaseObj->inputObj()->removeConObj(pConBaseObj->outputObj()->baseObj());
        }
    }
    if (pBaseObj)
    {
        for (QList <xAbstractBasisObj*>::iterator it=_objLst.begin();it!=_objLst.end();++it)
        {
            xConnectorObj *pCurrentConnectorObj = dynamic_cast<xConnectorObj*>((*it));
            if (pCurrentConnectorObj && (pCurrentConnectorObj->inputObj()->baseObj()==pBaseObj || pCurrentConnectorObj->outputObj()->baseObj()==pBaseObj))
                killLst.append(pCurrentConnectorObj);

            xVObj_Basics *pCurrentBaseObj = dynamic_cast<xVObj_Basics*>((*it));
            if (pCurrentBaseObj)
                for (QList <xConnector*>::iterator it2=pCurrentBaseObj->connectorLst()->begin();it2!=pCurrentBaseObj->connectorLst()->end();++it2)
                    (*it2)->removeConObj(pBaseObj);
        }
    }

    for (QList <xAbstractBasisObj*>::iterator it=killLst.begin();it!=killLst.end();++it)
    {
        _objLst.removeOne((*it));
        emit KSignal(ST_OBJECT_REMOVED,*it);
        delete (*it);
    }
}

xVTKDlg::~xVTKDlg()
{
    delete ui;
}

void xVTKDlg::accept()
{
    if (QMessageBox::question(0,"Warning!","Do you really like to close the application?")==QMessageBox::Yes)
        QDialog::accept();
}

void xVTKDlg::reject()
{
    accept();
}

void xVTKDlg::deselectAll()
{
    for (QList <xAbstractBasisObj*>::iterator it=_objLst.begin();it!=_objLst.end();++it)
        if ((*it)->isParamSelected()) {
            ui->pPropWdgt->objSelected(*it);
        }
}

void xVTKDlg::snapToGrid(xVObj_Basics *pVObj)
{
    if (pVObj==nullptr) pVObj = dynamic_cast<xVObj_Basics *>(sender());
    if (pVObj && ::_settings["snap to grid"]._value.toBool())
    {
        float grPosX = floor(pVObj->item()->pos().x()/::_settings["grid resolution"]._value.toDouble())*::_settings["grid resolution"]._value.toDouble();
        if (fabs(grPosX-pVObj->item()->pos().x())>fabs(grPosX+::_settings["grid resolution"]._value.toDouble()-pVObj->item()->pos().x()))
            grPosX+=::_settings["grid resolution"]._value.toDouble();
        float grPosY = floor(pVObj->item()->pos().y()/::_settings["grid resolution"]._value.toDouble())*::_settings["grid resolution"]._value.toDouble();
        if (fabs(grPosY-pVObj->item()->pos().y())>fabs(grPosY+::_settings["grid resolution"]._value.toDouble()-pVObj->item()->pos().y()))
            grPosY+=::_settings["grid resolution"]._value.toDouble();
        pVObj->item()->setPos(grPosX,grPosY);
        pVObj->positionChanged();
    }
}

void xVTKDlg::removeVisWidget(QWidget* wdgt)
{
    emit KSignal(ST_MSG,new QString("remove vis wdgt"));
    pVisMainWin->removeWindowAction(wdgt);
    QDockWidget* pDWdgt=nullptr;
    QList<QDockWidget *> dockWidgets = findChildren<QDockWidget *>();
    for (QList<QDockWidget *>::iterator it=dockWidgets.begin();it!=dockWidgets.end();++it)
        if ((*it)->widget()==wdgt) pDWdgt=*it;
    if (pDWdgt) {
        pVisMainWin->removeDockWidget(pDWdgt);
        delete pDWdgt;
    }
}

void xVTKDlg::addVisWidget(QWidget* wdgt)
{
    QString title="visualization";
    if (wdgt->property("title").isValid()) title=wdgt->property("title").toString();
    QDockWidget *pDockWdgt = new QDockWidget(title);
    wdgt->setWindowFlags(Qt::Widget);
    wdgt->setParent(this);
    wdgt->installEventFilter(this);
    pDockWdgt->setMinimumSize(410,410);
    pDockWdgt->setAllowedAreas(Qt::AllDockWidgetAreas);
    pDockWdgt->setWidget(wdgt);
    pDockWdgt->installEventFilter(this);
    wdgt->setGeometry(5,5,400,400);
    pVisMainWin->addDockWidget(Qt::LeftDockWidgetArea,pDockWdgt);
    pDockWdgt->show();
    pDockWdgt->setFeatures(QDockWidget::DockWidgetMovable|QDockWidget::DockWidgetFloatable);
    wdgt->show();
    QAction *pAction = new QAction(title);
    pAction->setCheckable(true);
    pAction->setChecked(true);
    pAction->setProperty("widget",QVariant::fromValue<void*>(wdgt));
    connect(pAction,SIGNAL(toggled(bool)),pDockWdgt,SLOT(setVisible(bool)));
    pVisMainWin->addWindowAction(pAction);
}

xVObj_Basics* xVTKDlg::baseObjFromId(const QString& id)
{
    xVObj_Basics* pObj=nullptr;
    for ( QList <xAbstractBasisObj*>::iterator it=_objLst.begin(); it!=_objLst.end(); ++it) {
        xVObj_Basics* pCurrentVObj=dynamic_cast<xVObj_Basics* >(*it);
        if (pCurrentVObj && pCurrentVObj->id()==id)
            pObj=pCurrentVObj;
    }
    return pObj;
}


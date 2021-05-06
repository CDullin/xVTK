#include "xvtkdlg.h"
#include "ui_xvtkdlg.h"
#include "xVSessionDlg.h"
#include "xVObjects.h"
#include "xVPolyObj.h"
#include "xVConnector.h"
#include "xVConnectorObj.h"
#include "xVTypes.h"
#include "xVObjectTypes.h"
#include "xvproptable.h"
#include "xVDashboardView.h"
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

    pErrorSoundEffect = new QSoundEffect();
    pErrorSoundEffect->setSource(QUrl::fromLocalFile(":/sounds/sounds/error.wav"));
    pWarnSoundEffect = new QSoundEffect();
    pWarnSoundEffect->setSource(QUrl::fromLocalFile(":/sounds/sounds/warn.wav"));

    //ui->toolBox->setCurrentIndex(0);
    ui->pTabWdgt->installEventFilter(this);
    ui->pReportBox->clear();

    // main win
    QVBoxLayout *pBoxLayout = new QVBoxLayout();
    pBoxLayout->setMargin(3);
    pVisMainWin = new xVVisMainWin(ui->pMainVisTab);
    pVisMainWin->setWindowFlags(Qt::Widget);
    pVisMainWin->installEventFilter(this);
    pBoxLayout->addWidget(pVisMainWin);
    ui->pMainVisTab->setLayout(pBoxLayout);

    connect(ui->pStepTB,SIGNAL(clicked()),this,SLOT(step()));
    connect(ui->pResetTB,SIGNAL(clicked()),this,SLOT(reset()));
    connect(ui->pRunTB,SIGNAL(clicked()),this,SLOT(run()));
    connect(ui->pVerifyTB,SIGNAL(clicked()),this,SLOT(verify()));
    connect(ui->pStopTB,SIGNAL(clicked()),this,SLOT(stop()));
    connect(ui->pCompileTB,SIGNAL(clicked()),this,SLOT(compile()));

    pSessionDlg = new xVSessionDlg();
    pSessionDlg->setPalette(palette());

    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void *)),ui->pPropWdgt,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(ui->pPropWdgt,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(ui->pSidePanelWdgt,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void *)),ui->pSidePanelWdgt,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(ui->pDashboardToolbar,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void *)),ui->pDashboardToolbar,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(ui->pReportBox,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void *)),ui->pReportBox,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(pSessionDlg,SIGNAL(KSignal(const SIG_TYPE&,void *)),this,SLOT(KSlot(const SIG_TYPE&,void *)));
    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void *)),pSessionDlg,SLOT(KSlot(const SIG_TYPE&,void *)));

    ui->pTabWdgt->setCurrentIndex(0);
    show();
    ui->pPropWdgt->hide();

    installEventFilter(this);
    ui->pPropWdgt->installEventFilter(this);

    emit KSignal(ST_GENERATE_CONNECTION_WITH_MAIN_DLG,this);

    // memory survailance
    QTimer *pMemoryTimer = new QTimer(this);
    pMemoryTimer->setInterval(5000);
    connect(pMemoryTimer,SIGNAL(timeout()),this,SLOT(dispMemoryConsumption()));
    pMemoryTimer->start();
    dispMemoryConsumption();

    ui->pBarTabWdgt->setCurrentIndex(2);
    createDashboard();
    //emit KSignal(ST_PTR_TO_SCENE,_dashboardLst[_currentDashBoard]->pDashBoardGV);

    generateHooks();
    pSessionDlg->exec();
}

void xVTKDlg::createDashboard()
{
    QString name = "untitled";
    int j=0;
    bool _found=false;
    do
    {
        _found=false;
        for (int i=0;i<_dashboardLst.count();++i)
            if (_dashboardLst[i]->_name==name)
            {
                _found = true;
            }
        if (_found)
            name = QString("untitled_%1").arg(j++);
    }
    while (_found);

    xVDashBoard *pDashboard = new xVDashBoard;
    pDashboard->_name = name;
    pDashboard->_lastSaved = QDateTime::currentDateTime();
    pDashboard->_lastAutoSaved = pDashboard->_lastSaved;
    pDashboard->pDashBoardGV = new xVDashboardView();
    _dashboardLst.append(pDashboard);
    ui->pDashboardsStackWdgt->addWidget(pDashboard->pDashBoardGV);
    _currentDashBoard=_dashboardLst.count()-1;
    _dashBoardCreateID++;
    ui->pDashboardsStackWdgt->setCurrentIndex(_currentDashBoard);
    clear(false);
    _dashboardLst[_currentDashBoard]->pDashBoardGV->updateMap();    

    connect(this,SIGNAL(KSignal(const SIG_TYPE&,void*)),pDashboard->pDashBoardGV,SLOT(KSlot(const SIG_TYPE&,void*)));
    connect(pDashboard->pDashBoardGV,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SLOT(KSlot(const SIG_TYPE&,void*)));

    emit KSignal(ST_DASHBOARD_CREATED,pDashboard);
}

void xVTKDlg::rDblClickInScene()
{
    if (ui->pPropWdgt->activeObj() && ui->pPropWdgt->activeObj()->type()==xVOT_CONNECTOR)
    {
        xConnectorObj *pConObj = dynamic_cast<xConnectorObj*>(ui->pPropWdgt->activeObj());
        pConObj->addNodeAtCursorPos();
    }
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
    _hookLst.append(HOOK("import","volume","imports a volume data set",ST_CREATE_OBJ_HOOK,xVOT_VOLUME));
    _hookLst.append(HOOK("import","mesh","imports STL,PLY or OBJ mesh files",ST_CREATE_OBJ_HOOK,xVOT_MESH));
    _hookLst.append(HOOK("import","cvs","imports a table in *.csv format",ST_CREATE_OBJ_HOOK,xVOT_CVS));
    _hookLst.append(HOOK("import","camera","connects to a pylon camera",ST_CREATE_OBJ_HOOK,xVOT_PYLON_CAMERA));
    _hookLst.append(HOOK("import","image","loads PNG, JPG, TIF and BMP files",ST_CREATE_OBJ_HOOK,xVOT_IMAGE));
    _hookLst.append(HOOK("import","image stack","",ST_CREATE_OBJ_HOOK,xVOT_IMAGE_STACK));
    _hookLst.append(HOOK("filter","LSI","",ST_CREATE_OBJ_HOOK,xVOT_LSI_FILTER));
    _hookLst.append(HOOK("filter","morphologic","",ST_CREATE_OBJ_HOOK,xVOT_MORPH_FILTER));
    _hookLst.append(HOOK("filter","polygonization","",ST_CREATE_OBJ_HOOK,xVOT_POLYGONIZATION));
    _hookLst.append(HOOK("user interaction","parameter input","",ST_CREATE_OBJ_HOOK,xVOT_USER_TABLE_DLG));
    _hookLst.append(HOOK("properties","volume","",ST_CREATE_OBJ_HOOK,xVOT_VOLUME_VIS_PROP));
    _hookLst.append(HOOK("properties","mesh","",ST_CREATE_OBJ_HOOK,xVOT_MESH_VIS_PROP));
    _hookLst.append(HOOK("properties","camera","",ST_CREATE_OBJ_HOOK,xVOT_PYLON_CAM_PROP));
    _hookLst.append(HOOK("properties","clip plane","",ST_CREATE_OBJ_HOOK,xVOT_CLIPPING_PLANE));
    _hookLst.append(HOOK("properties","clip box","",ST_CREATE_OBJ_HOOK,xVOT_CLIPPING_BOX));
    _hookLst.append(HOOK("properties","clip sphere","",ST_CREATE_OBJ_HOOK,xVOT_CLIPPING_SPHERE));
    _hookLst.append(HOOK("properties","slicer","",ST_CREATE_OBJ_HOOK,xVOT_SLICER_OBJ));
    _hookLst.append(HOOK("output","2D","",ST_CREATE_OBJ_HOOK,xVOT_2D_VIEW));
    _hookLst.append(HOOK("output","3D","",ST_CREATE_OBJ_HOOK,xVOT_3D_VIEW));
    _hookLst.append(HOOK("output","report","",ST_CREATE_OBJ_HOOK,xVOT_REPORT));
    _hookLst.append(HOOK("output","export","",ST_CREATE_OBJ_HOOK,xVOT_EXPORT));
    _hookLst.append(HOOK("output","graph","",ST_CREATE_OBJ_HOOK,xVOT_GRAPH));
    _hookLst.append(HOOK("tools","call external","",ST_CREATE_OBJ_HOOK,xVOT_CALL_EXTERNAL));
    _hookLst.append(HOOK("tools","call dashboard","",ST_CREATE_OBJ_HOOK,xVOT_CALL_DASHBOARD));
    _hookLst.append(HOOK("logic","definition","",ST_CREATE_OBJ_HOOK,xVOT_VAR_DEFINITION));
    _hookLst.append(HOOK("logic","3 state","",ST_CREATE_OBJ_HOOK,xVOT_TRAFFIC_LIGHT));
    _hookLst.append(HOOK("logic","if","",ST_CREATE_OBJ_HOOK,xVOT_IF));
    _hookLst.append(HOOK("logic","equation","",ST_CREATE_OBJ_HOOK,xVOT_MATH));
    _hookLst.append(HOOK("logic","wait","",ST_CREATE_OBJ_HOOK,xVOT_WAIT));
    _hookLst.append(HOOK("logic","fuzzification","",ST_CREATE_OBJ_HOOK,xVOT_FUZZIFICATION));
    _hookLst.append(HOOK("logic","fuzzy rule base","",ST_CREATE_OBJ_HOOK,xVOT_FUZZY_RULES));
    _hookLst.append(HOOK("logic","defuzzification","",ST_CREATE_OBJ_HOOK,xVOT_DEFUZZIFICATION));
    _hookLst.append(HOOK("arduino","connect","",ST_CREATE_OBJ_HOOK,xVOT_ARDUINO_CONNECT));
    _hookLst.append(HOOK("arduino","communication","",ST_CREATE_OBJ_HOOK,xVOT_ARDUINO_COM));

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
                return true;
                break;
            }
        }
    }
    return QWidget::eventFilter(obj,event);
}


void xVTKDlg::activateNext()
{
    if (_dashboardLst[_currentDashBoard]->_objLst.count()==0) return;
    xVAbstractBaseObj *pBaseObj = ui->pPropWdgt->activeObj();
    if (pBaseObj)
    {
        int i=_dashboardLst[_currentDashBoard]->_objLst.indexOf(pBaseObj);
        ui->pPropWdgt->objSelected(_dashboardLst[_currentDashBoard]->_objLst.at((i+1)%_dashboardLst[_currentDashBoard]->_objLst.count()));
    }
    else
        ui->pPropWdgt->objSelected(_dashboardLst[_currentDashBoard]->_objLst.at(0));
}

void xVTKDlg::activatePrev()
{
    if (_dashboardLst[_currentDashBoard]->_objLst.count()==0) return;
    xVAbstractBaseObj *pBaseObj = ui->pPropWdgt->activeObj();
    if (pBaseObj)
    {
        int i=_dashboardLst[_currentDashBoard]->_objLst.indexOf(pBaseObj);
        ui->pPropWdgt->objSelected(_dashboardLst[_currentDashBoard]->_objLst.at((i-1)%_dashboardLst[_currentDashBoard]->_objLst.count()));
    }
    else
        ui->pPropWdgt->objSelected(_dashboardLst[_currentDashBoard]->_objLst.at(0));
}

void xVTKDlg::clear(bool verbose)
{
    if (!verbose || QMessageBox::question(0,"Attention!","Do you really like to clear the dash board?")==QMessageBox::Yes)
    {
        reset(false);
        if (_dashboardLst[_currentDashBoard]->_objLst.count()>0)
        {
            xVAbstractBaseObj* pItem=_dashboardLst[_currentDashBoard]->_objLst.first();
            while (pItem && _dashboardLst[_currentDashBoard]->_objLst.count()>0)
            {
                _dashboardLst[_currentDashBoard]->_objLst.removeOne(pItem);
                removeObject(pItem);
                _dashboardLst[_currentDashBoard]->_objLst.count()>0 ? pItem = _dashboardLst[_currentDashBoard]->_objLst.first() : pItem=nullptr;
            }
        }
        _dashboardLst[_currentDashBoard]->pDashBoardGV->scene()->setSceneRect(QRectF(0,0,800,100));
        xVObj_Basics *pVObj = new xVStartObj("#start");
        _dashboardLst[_currentDashBoard]->pDashBoardGV->scene()->addItem(pVObj->item());
        emit KSignal(ST_ADD_OBJECT,pVObj);
        pVObj->item()->setPos(0,25);
        snapToGrid(pVObj);
        pVObj = new xVEndObj("#end");
        _dashboardLst[_currentDashBoard]->pDashBoardGV->scene()->addItem(pVObj->item());
        emit KSignal(ST_ADD_OBJECT,pVObj);
        pVObj->item()->setPos(700,25);
        snapToGrid(pVObj);
    }
}
void xVTKDlg::verify()
{}
void xVTKDlg::closeDashboard()
{
    if (QMessageBox::question(0,"Warning","Do you really like to close this dashboard?")==QMessageBox::Yes)
    {
        reset(false);
        clear(false);
        int i=_currentDashBoard;
        ui->pDashboardsStackWdgt->removeWidget(_dashboardLst[i]->pDashBoardGV);
        _dashboardLst.removeAt(i);
        if (_dashboardLst.count()==0) createDashboard();
        _currentDashBoard = std::min(_dashboardLst.count()-1,i);
        emit KSignal(ST_ACTIVATE_DASHBOARD,new int(_currentDashBoard));
    }
}

void xVTKDlg::placeObjInScene(xVObj_Basics* pObj)
{
    if (!pObj) return;
    bool _placed = false;
    QGraphicsScene *pScene = _dashboardLst[_currentDashBoard]->pDashBoardGV->scene();
    QRectF _sceneRect = _dashboardLst[_currentDashBoard]->pDashBoardGV->sceneRect();
    QPolygonF _objRect = pObj->item()->mapToScene(pObj->item()->boundingRect());
    QPointF _scrPos = pObj->item()->mapToScene(pObj->item()->pos());

    while (!_placed) {
        _placed=true;
        for (QList <xVAbstractBaseObj*>::iterator it=_dashboardLst[_currentDashBoard]->_objLst.begin();it!=_dashboardLst[_currentDashBoard]->_objLst.end();++it)
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
        for (QList <xVAbstractBaseObj*>::iterator it=_dashboardLst[_currentDashBoard]->_objLst.begin();it!=_dashboardLst[_currentDashBoard]->_objLst.end();++it)
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
    QObject *pSObj = sender();
    switch (type)
    {
    case ST_RDBL_CLICK_IN_SCENE: rDblClickInScene();break;
    case ST_DESELECT_ALL_OBJS: deselectAll();break;
    case ST_ACTIVATE_NEXT_OBJ: activateNext();
        break;
    case ST_ACTIVATE_PREV_OBJ: activatePrev();
        break;
    case ST_LOAD_REPORT:
        ui->pTabWdgt->setCurrentIndex(2);
        if (pSObj!=this) emit KSignal(type,pData);
        break;
    case ST_RUN:
        run();
        break;
    case ST_STEP:
        if (ui->pStepTB->isEnabled()) step();
        break;
    case ST_AUTOSAVE_DASHBOARD:
        autoSave((xVDashBoard*)pData);
        break;
    case ST_SAVEAS_DASHBOARD:
        save(true);
        break;
    case ST_SAVE_DASHBOARD:
        save();
        break;
    case ST_CLEAR_DASHBOARD:
        clear();
        break;
    case ST_CLOSE_DASHBOARD:
        closeDashboard();
        break;
    case ST_LOAD_DASHBOARD:
        {
            ui->pTabWdgt->setCurrentIndex(0);
            QString fname="";
            if (pData!=0) fname=*((QString*)pData);
            load(fname);
        }
        break;
    case ST_ACTIVATE_DASHBOARD:
    {
        _currentDashBoard = *((int*)pData);
        ui->pDashboardsStackWdgt->setCurrentIndex(_currentDashBoard);
        ui->pTabWdgt->setCurrentIndex(0);
    }
        break;
    case ST_CREATE_DASHBOARD: createDashboard();break;
    case ST_ACTION_HOOK_TRIGGERED:
    {
        xVO_TYPE *pOType=(xVO_TYPE*)pData;
        xVObj_Basics *pVObj=nullptr;
        switch (*pOType)
        {
        case xVOT_CLIPPING_PLANE:       pVObj = new xVPlaneObj(uniqueName("#plane"));                                   break;
        case xVOT_POLYGONIZATION:       pVObj = new xVPolygonizationFilterObj(uniqueName("#polygonization"));           break;
        case xVOT_MORPH_FILTER:         pVObj = new xVMorphFilterObj(uniqueName("#morph filter"));                      break;
        case xVOT_LSI_FILTER:           pVObj = new xVLSIFilterObj(uniqueName("#LSI filter"));                          break;
        case xVOT_FUZZIFICATION:        pVObj = new xVFuzzificationObj(uniqueName("#fuzzy"));                           break;
        case xVOT_FUZZY_RULES:          pVObj = new xVFuzzyRuleBaseObj(uniqueName("#rules"));                           break;
        case xVOT_DEFUZZIFICATION:      pVObj = new xVDefuzzificationObj(uniqueName("#defuzzy"));                       break;
        case xVOT_GRAPH:                pVObj = new xVGraphObj(uniqueName("#graph"));                                   break;
        case xVOT_EXPORT:               pVObj = new xVExportObj(uniqueName("#export"));                                 break;
        case xVOT_IMAGE:                pVObj = new xVImageObj(uniqueName("#image"));                                   break;
        case xVOT_IMAGE_STACK:          pVObj = new xVImageStackObj(uniqueName("#stack"));                              break;
        case xVOT_REPORT:               pVObj = new xVReportObj(uniqueName("#report"));                                 break;
        case xVOT_CALL_DASHBOARD:       pVObj = new xVCallDashboardObj(uniqueName("#dashboard"));                       break;
        case xVOT_PYLON_CAM_PROP:       pVObj = new xVCamPropObj(uniqueName("#cam prop"));                              break;
        case xVOT_PYLON_CAMERA:         pVObj = new xVCamObj(uniqueName("#camera"));                                    break;
        case xVOT_CVS:                  pVObj = new xVImportCVSObj(uniqueName("-> *.cvs"));                             break;
        case xVOT_ARDUINO_COM:          pVObj = new xVArduinoComObj(uniqueName("#arduino <->"));                        break;
        case xVOT_ARDUINO_CONNECT:      pVObj = new xVArduinoConnectObj(uniqueName("#arduino"));                        break;
        case xVOT_WAIT:                 pVObj = new xVWaitObj(uniqueName("#wait"));                                     break;
        case xVOT_END:                  pVObj = new xVEndObj("#end");                                                   break;
        case xVOT_START:                pVObj = new xVStartObj("#start");                                               break;
        case xVOT_VOLUME:               pVObj = new xVVolObj(uniqueName("#volume"));                                    break;
        case xVOT_MESH:                 pVObj = new xVPolyObj(uniqueName("#mesh"));                                     break;
        case xVOT_2D_VIEW:              pVObj = new xV2DVisObj(uniqueName("#2dView"));                                  break;
        case xVOT_3D_VIEW:              pVObj = new xV3DVisObj(uniqueName("#3dView"));                                  break;
        case xVOT_VOLUME_VIS_PROP:      pVObj = new xVVolumeVisPropObj(uniqueName("volume properties"));                break;
        case xVOT_MESH_VIS_PROP:        pVObj = new xVMeshVisPropObj(uniqueName("mesh properties"));                    break;
        case xVOT_CALL_EXTERNAL:        pVObj = new xVCallExternalObj(uniqueName("#execute"));                          break;
        case xVOT_IF:                   pVObj = new xVIFObj(uniqueName("#if"));                                         break;
        case xVOT_FILTER:               pVObj = new xVGenFilterObj(uniqueName("#filter"));                              break;
        case xVOT_USER_TABLE_DLG:       pVObj = new xVUserTableImportDlgObj(uniqueName("#user"));                       break;
        case xVOT_VAR_DEFINITION:       pVObj = new xVVarDefinitionObj(uniqueName("#define"));                          break;
        case xVOT_TRAFFIC_LIGHT:        pVObj = new xVTrafficLightObj(uniqueName("#light"));                            break;
        case xVOT_MATH:                 pVObj = new xVMathObj(uniqueName("#Eq."));                                      break;
        default:
            emit KSignal(ST_ERROR_MSG,new QString("unsupported object type for creating hook detected"));
            break;
        }
        if (pVObj)
        {
            _dashboardLst[_currentDashBoard]->pDashBoardGV->scene()->addItem(pVObj->item());
            emit KSignal(ST_ADD_OBJECT,pVObj);
        }
        delete pOType;
    }
        break;
    case ST_ADD_VISUALIZATION_WDGT:
        addVisWidget((QWidget*)pData);
        ui->pTabWdgt->setCurrentIndex(1);
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
        QPoint *p=(QPoint*)pData;
        ui->pSingleProg->setRange(p->x(),p->y());
        delete p;
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
        ui->pBarTabWdgt->setCurrentIndex(0);
        if (_settings["play sound"]._value.toBool())
        {
            pErrorSoundEffect->setVolume(_settings["master volume"]._value.value<xLimitedDouble>()._value);
            pErrorSoundEffect->play();
        }
        delete pStr;
    }
        break;
    case ST_WARN_MSG:
    {
        QString *pStr=(QString*)pData;
        ui->pMSGBrowser->setTextColor(QColor(Qt::yellow));
        ui->pMSGBrowser->append(*pStr);
        ui->pBarTabWdgt->setCurrentIndex(0);
        if (_settings["play sound"]._value.toBool())
        {
            pWarnSoundEffect->setVolume(_settings["master volume"]._value.value<xLimitedDouble>()._value);
            pWarnSoundEffect->play();
        }
        delete pStr;
    }
        break;
    case ST_MSG:
    {
        QString *pStr=(QString*)pData;
        ui->pMSGBrowser->setTextColor(QColor(Qt::white));
        ui->pMSGBrowser->append(*pStr);
        ui->pBarTabWdgt->setCurrentIndex(0);
        delete pStr;
    }
        break;
    case ST_ADD_OBJECT:
        _dashboardLst[_currentDashBoard]->_objLst.append((xVAbstractBaseObj*)pData);
        connect((xVAbstractBaseObj*)pData,SIGNAL(selected(xVAbstractBaseObj*)),ui->pPropWdgt,SLOT(objSelected(xVAbstractBaseObj*)));
        connect((xVAbstractBaseObj*)pData,SIGNAL(parameterModified()),ui->pPropWdgt,SLOT(parameterModified()));
        connect((xVAbstractBaseObj*)pData,SIGNAL(KSignal(const SIG_TYPE&,void*)),this,SLOT(KSlot(const SIG_TYPE&,void*)));
        connect(this,SIGNAL(KSignal(const SIG_TYPE&,void*)),(xVAbstractBaseObj*)pData,SLOT(KSlot(const SIG_TYPE&,void*)));
        if (dynamic_cast<xVObj_Basics*>((xVAbstractBaseObj*)pData))
        {
            dynamic_cast<xVObj_Basics*>((xVAbstractBaseObj*)pData)->item()->setPos(50,50);
            connect(dynamic_cast<xVObj_Basics*>((xVAbstractBaseObj*)pData),SIGNAL(placed()),this,SLOT(snapToGrid()));
            connect(dynamic_cast<xVObj_Basics*>((xVAbstractBaseObj*)pData),SIGNAL(connectorActivated(xVObj_Basics*,xCONNECTOR_TYPE)),this,SLOT(connectorActivated(xVObj_Basics*,xCONNECTOR_TYPE)));
        }
        placeObjInScene(dynamic_cast<xVObj_Basics*>((xVAbstractBaseObj*)pData));
        emit KSignal(ST_OBJECT_ADDED,pData);
        break;
    case ST_REMOVE_OBJECT: removeObject((xVObj_Basics*)pData);break;
    default:
    {
        if (pSObj!=this)
            emit KSignal(type,pData);
    }
        break;
    }
}

void xVTKDlg::removeObject(xVAbstractBaseObj* pObj)
{
    QList <xVAbstractBaseObj*> killLst;
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
        for (QList <xVAbstractBaseObj*>::iterator it=_dashboardLst[_currentDashBoard]->_objLst.begin();it!=_dashboardLst[_currentDashBoard]->_objLst.end();++it)
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

    for (QList <xVAbstractBaseObj*>::iterator it=killLst.begin();it!=killLst.end();++it)
    {
        _dashboardLst[_currentDashBoard]->_objLst.removeOne((*it));
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
    {
        if (pSessionDlg) delete pSessionDlg;
        QDialog::accept();
    }
}

void xVTKDlg::reject()
{
    accept();
}

void xVTKDlg::deselectAll()
{
    for (QList <xVAbstractBaseObj*>::iterator it=_dashboardLst[_currentDashBoard]->_objLst.begin();it!=_dashboardLst[_currentDashBoard]->_objLst.end();++it)
        if ((*it)->isParamSelected()) {
            ui->pPropWdgt->objSelected(*it);
        }
}

void xVTKDlg::snapToGrid(xVObj_Basics *pVObj)
{
    if (pVObj==nullptr) pVObj = dynamic_cast<xVObj_Basics *>(sender());
    if (pVObj && ::_settings["snap to grid"]._value.toBool())
    {
        float resolution = ::_settings["grid resolution"]._value.value<xLimitedInt>()._value;

        float grPosX = floor(pVObj->item()->pos().x()/resolution)*resolution;
        if (fabs(grPosX-pVObj->item()->pos().x())>fabs(grPosX+resolution-pVObj->item()->pos().x()))
            grPosX+=resolution;
        float grPosY = floor(pVObj->item()->pos().y()/resolution)*resolution;
        if (fabs(grPosY-pVObj->item()->pos().y())>fabs(grPosY+resolution-pVObj->item()->pos().y()))
            grPosY+=resolution;
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
    return dynamic_cast<xVObj_Basics* >(objId2objPtr(id));
}


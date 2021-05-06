#include "xVReportObj.h"
#include "xVObjects.h"

xVReportObj::xVReportObj(const QString& txt):xVGenVisObj(txt)
{
    _type=xVOT_REPORT;
    _paramMp["report name"]._id = 1;
    _paramMp["report name"]._value = "untitled";
    _paramMp["file name"]._id = 2;
    _paramMp["file name"]._value = QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,"untitled.html"));
    _paramMp["parameter"]._id = 3;
    _paramMp["parameter"]._value = QVariant::fromValue(xParamMap());
    _paramMp["auto save"]._id = 4;
    _paramMp["auto save"]._value = true;
}

xVReportObj::xVReportObj(QDataStream& d):xVGenVisObj(d)
{
    _type=xVOT_REPORT;
}

void xVReportObj::KSlot(const SIG_TYPE& type,void* pData)
{
    switch (type)
    {
    case ST_REPORT_ACTIVATED:
        pReportRef = (xVReport*)pData;
        break;
    default:
        // all handled
        break;
    }
}

void xVReportObj::run()
{
    if (_status==OS_UPDATE_NEEDED)
    {
        // create or activate report -> should be followed by the current pointer in KSlot
        emit KSignal(ST_CREATE_REPORT,new QString(_paramMp["report name"]._value.toString()));

        // walk over all connected inputs and outputs
        if (pReportRef)
        {
            for (QList <xConnector*>::iterator it2=_connectorLst.begin();it2!=_connectorLst.end();++it2)
            {
                // find all connected and enabled inputs
                if ((*it2)->isEnabled())
                {
                    for (QList <xVObj_Basics*>::iterator it=(*it2)->connectedObjects()->begin();it!=(*it2)->connectedObjects()->end();++it)
                    {
                        // parameter map
                        for (QMap<QString,xPROP_TYPE>::iterator it3=(*it)->paramMap()->begin();it3!=(*it)->paramMap()->end();++it3)
                        {
                            if (_paramMp["parameter"]._value.value<xParamMap>().contains(it3.key()) &&
                                _paramMp["parameter"]._value.value<xParamMap>()[it3.key()].pRefObj == (*it))
                                pReportRef->append(xVReport::xVReport_TRANSFER(it3.key(),*it3));
                        }
                        // output parameter map
                        for (QMap<QString,xPROP_TYPE>::iterator it3=(*it)->outputParamMap()->begin();it3!=(*it)->outputParamMap()->end();++it3)
                        {
                            if (_paramMp["parameter"]._value.value<xParamMap>().contains(it3.key()) &&
                                _paramMp["parameter"]._value.value<xParamMap>()[it3.key()].pRefObj == (*it))
                                pReportRef->append(xVReport::xVReport_TRANSFER(it3.key(),*it3));
                        }
                    }
                }
            }

            // global namespace
            for (QMap<QString,xPROP_TYPE>::iterator it3=_globalNameSpace.begin();it3!=_globalNameSpace.end();++it3)
            {
                if (_paramMp["parameter"]._value.value<xParamMap>().contains(it3.key()) &&
                    _paramMp["parameter"]._value.value<xParamMap>()[it3.key()].pRefObj == nullptr)
                    pReportRef->append(xVReport::xVReport_TRANSFER(it3.key(),*it3));
            }
        }
        setStatus(OS_VALID);
    }
}

void xVReportObj::reset()
{}

void xVReportObj::paramModified(const QString& txt)
{
    xVGenVisObj::paramModified(txt);
}

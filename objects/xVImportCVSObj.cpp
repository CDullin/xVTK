#include "xVImportCVSObj.h"

xVImportCVSObj::xVImportCVSObj(const QString& txt):xVGenImpObj(txt)
{
    _type = xVOT_CVS;
    _description="Imports a table in *.csv format";
    _paramMp["file name"]._id=1;
    //_paramMp["file name"]._value=QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,::_settings["last cvs file"]._value.toString()));
    _paramMp["file name"]._value=QVariant::fromValue(xFileName(xFileName::FN_INPUT_FILE,"/home/heimdall/development/sample_data/Madlaine.csv"));
    _paramMp["name in global namespace"]._id=2;
    _paramMp["name in global namespace"]._value="table1";
    _paramMp["separator"]._id=3;
    _paramMp["separator"]._value=QString(",");
    _paramMp["header skip [lines]"]._id=4;
    _paramMp["header skip [lines]"]._value=QVariant::fromValue<xLimitedInt>(xLimitedInt(0,0,100));
    _paramMp["skip every N [line]"]._id=5;
    _paramMp["skip every N [line]"]._value=QVariant::fromValue<xLimitedInt>(xLimitedInt(0,0,100));
    _paramMp["skip every N [column]"]._id=6;
    _paramMp["skip every N [column]"]._value=QVariant::fromValue<xLimitedInt>(xLimitedInt(0,0,100));
    _paramMp["data ordered in columns"]._id=7;
    _paramMp["data ordered in columns"]._value=true;
    _paramMp["autodetect data type"]._id=8;
    _paramMp["autodetect data type"]._value=true;
    _paramMp["use first N lines as label"]._id=9;
    _paramMp["use first N lines as label"]._value=QVariant::fromValue<xLimitedInt>(xLimitedInt(1,0,100));
    _paramMp["decimal point"]._id=10;
    _paramMp["decimal point"]._value=QString(".");

    _outputParamMp["table"]._id=0;
    _outputParamMp["table"]._value=QVariant::fromValue(new xVTable());
    _hasPreview = true;
}

xVImportCVSObj::xVImportCVSObj(QDataStream& d):xVGenImpObj(d)
{
    _type = xVOT_CVS;
    _hasPreview = true;

    // this needs to go
    _outputParamMp["table"]._id=0;
    _outputParamMp["table"]._value=QVariant::fromValue(new xVTable());
}

void xVImportCVSObj::run()
{
    if (status()==OS_UPDATE_NEEDED)
    {
        setStatus(OS_RUNNING);
        // import table into global namespace
        setStatus(OS_VALID);
    }
}

void xVImportCVSObj::reset()
{
    // remove data
    setStatus(OS_UPDATE_NEEDED);
}

void xVImportCVSObj::openPreview()
{
    if (!_inPreview)
    {
        _outputParamMp["table"]._value.value<xVTablePtr>()->setFile(_paramMp["file name"]._value.value<xFileName>()._fileName);
        _outputParamMp["table"]._value.value<xVTablePtr>()->_importParam._autoDetectDataType = _paramMp["autodetect data type"]._value.toBool();
        _outputParamMp["table"]._value.value<xVTablePtr>()->_importParam._dataOrderedInColumns = _paramMp["data ordered in columns"]._value.toBool();
        _outputParamMp["table"]._value.value<xVTablePtr>()->_importParam._decimalSeparator = _paramMp["decimal point"]._value.toString();
        _outputParamMp["table"]._value.value<xVTablePtr>()->_importParam._headerSkipLines = _paramMp["header skip [lines]"]._value.value<xLimitedInt>()._value;
        _outputParamMp["table"]._value.value<xVTablePtr>()->_importParam._separator = _paramMp["separator"]._value.toString();
        _outputParamMp["table"]._value.value<xVTablePtr>()->_importParam._skipEveryNColumn = _paramMp["skip every N [column]"]._value.value<xLimitedInt>()._value;
        _outputParamMp["table"]._value.value<xVTablePtr>()->_importParam._skipEveryNLines = _paramMp["skip every N [line]"]._value.value<xLimitedInt>()._value;
        _outputParamMp["table"]._value.value<xVTablePtr>()->_importParam._useFirstNLinesAsLabel = _paramMp["use first N lines as label"]._value.value<xLimitedInt>()._value;
        _outputParamMp["table"]._value.value<xVTablePtr>()->fromCVS();

        if (pPrevDlg!=nullptr) delete pPrevDlg;
        pPrevDlg = new xVCVSImportPreviewDlg(this,_outputParamMp["table"]._value.value<xVTablePtr>());
        connect(pPrevDlg,SIGNAL(closed()),this,SLOT(closePreview()));
        pPrevDlg->updateDialog();
        pPrevDlg->exec();
    }
}

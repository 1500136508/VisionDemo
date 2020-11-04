#ifndef SAVEDIALOG_H
#define SAVEDIALOG_H

#include <QDialog>
//#include "ui_savedialog.h"

namespace Ui {
class saveDialog;
}

class saveDialog : public QDialog
{
    Q_OBJECT

public:
    saveDialog(QWidget *parent = nullptr);
    ~saveDialog();

private:
    Ui::saveDialog *ui;
    bool readyToSave = true;

private slots:
    void savePathBtnSlot();
    void savePathCheckSlot();
    void saveBtnSlot();
    void stopBtnSlot();
    void showInfo(QString info);
    void chooseModeThree();
    void chooseModeFour();
signals:
    void sendSavePath(QString savePtah);
    void sendError();
    void sendSaveStart(int frameNumber);
    void sendSaveStop();
    void sendSaveMode(int mode);

};

#endif // SAVEDIALOG_H

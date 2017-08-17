#ifndef LIBURDICONNECTIONDIALOG_H
#define LIBURDICONNECTIONDIALOG_H

#include <QDialog>
#include <QUrl>

QT_USE_NAMESPACE

QT_BEGIN_NAMESPACE

namespace Ui {
class liburdiConnectionDialog;
}

QT_END_NAMESPACE

class liburdiConnectionDialog : public QDialog
{
    Q_OBJECT

public:
    explicit liburdiConnectionDialog(QWidget *parent = 0);
    ~liburdiConnectionDialog();
    struct Settings {
        QString ipAddress;
        QUrl liburdiURL;
    };

    Settings settings() const;

private slots:
    void apply();

private:
    Ui::liburdiConnectionDialog *ui;
    Settings currentSettings;

    void updateSettings();
};

#endif // LIBURDICONNECTIONDIALOG_H

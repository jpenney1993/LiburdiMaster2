#include "liburdiconnectiondialog.h"
#include "ui_liburdiconnectiondialog.h"

#include <QUrl>

liburdiConnectionDialog::liburdiConnectionDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::liburdiConnectionDialog)
{
    ui->setupUi(this);

    ui->ipAddressEdit->setText(QLatin1Literal("0.0.0.0:502"));
    connect(ui->applyButton,&QPushButton::clicked,
            this,&liburdiConnectionDialog::apply);
}

liburdiConnectionDialog::~liburdiConnectionDialog()
{
    delete ui;
}

liburdiConnectionDialog::Settings liburdiConnectionDialog::settings() const
{
    return currentSettings;
}

void liburdiConnectionDialog::apply()
{
    updateSettings();
    hide();
}

void liburdiConnectionDialog::updateSettings()
{
    currentSettings.ipAddress = ui->ipAddressEdit->text();
    QUrl liburdiUrl = QUrl::fromUserInput(ui->ipAddressEdit->text());
    currentSettings.liburdiURL = liburdiUrl;
}

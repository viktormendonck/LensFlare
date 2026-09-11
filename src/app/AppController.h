#pragma once
#include <qobject.h>

class AppController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(
        QString statusText
        READ GetStatusText
        WRITE SetStatusText
        NOTIFY StatusTextChanged
    )
public:
    AppController() = default;

    Q_INVOKABLE
    void OpenFileButton();
    Q_INVOKABLE
    void SaveButton();
    Q_INVOKABLE
    void UndoButton();
    Q_INVOKABLE
    void RedoButton();

    QString GetStatusText() const{return StatusText;};
    void SetStatusText(const QString& in)
    {
        StatusText = in;
        StatusTextChanged();
    };

signals:
    void StatusTextChanged();

private:
    QString StatusText{"idle"};
};



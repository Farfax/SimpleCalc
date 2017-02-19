#ifndef CALCULATOR_H
#define CALCULATOR_H

#include <QWidget>
#include <QLineEdit>
#include <QToolButton>

namespace Ui
{
    class Calculator;
    class Button;
}

class Button : public QToolButton
{
    Q_OBJECT

public:
    explicit Button(const QString &text, QWidget *parent = 0);

    QSize sizeHint() const override;
};

class Calculator : public QWidget
{
    Q_OBJECT

private slots:
    void DigitClicked();
    void UnaryOperatorClicked();
    void AdditiveOperatorClicked();
    void MultiplicativeOperatorClicked();
    void EqualClicked();
    void PointClicked();
    void ChangeSignClicked();
    void BackspaceClicked();
    void Clear();
    void ClearAll();
    void ClearMemory();
    void ReadMemory();
    void SetMemory();
    void AddToMemory();

public:
    explicit Calculator(QWidget *parent = 0);
    ~Calculator();

private:
    Ui::Calculator *ui;

    double sumInMemory;
    double sumSoFar;
    double FactorSoFar;
    QString PendingAdditiveOperator;
    QString PendingMultiplicativeOperator;
    bool WaitingForOperand;
    QLineEdit *display;

    enum { NumDigitButtons = 10} ;
    Button *digitButtons[NumDigitButtons];

    Button *CreateButton(const QString& text, const char *member);
    void AbortOperation();
    bool Calculate(double rightOperand, const QString &pendingOperator);

};

#endif // CALCULATOR_H

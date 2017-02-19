#include "calculator.h"
#include "ui_calculator.h"
#include <QGridLayout>
#include <algorithm>
#include<cmath>

Calculator::Calculator(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Calculator),
    sumInMemory(0.0),
    sumSoFar(0.0),
    FactorSoFar(0.0),
    WaitingForOperand(false)
{
    for(int i=0;i< NumDigitButtons;++i)
        digitButtons[i]= CreateButton(QString::number(i), SLOT(DigitClicked()));

    Button* MultipleButton= CreateButton(tr("\303\227"), SLOT(MultiplicativeOperatorClicked()));
    Button* DivisionButton= CreateButton(tr("\303\267"), SLOT(MultiplicativeOperatorClicked()));
    Button* AdditiveButton= CreateButton(tr("+"), SLOT(AdditiveOperatorClicked()));
    Button* MinusButton= CreateButton(tr("-"), SLOT(AdditiveOperatorClicked()));

    Button* SqrtButton= CreateButton(tr("Sqrt"), SLOT(UnaryOperatorClicked()));
    Button* EqualButton= CreateButton(tr("="), SLOT(EqualClicked()));

    Button* ChangeSignButton= CreateButton(tr("\302\261"), SLOT (ChangeSignClicked()));
    Button* PointButton= CreateButton(tr("."), SLOT(PointClicked()));

    Button* BackspaceButton= CreateButton(tr("Undo"), SLOT(BackspaceClicked()));
    Button* ClearButton= CreateButton(tr("C"), SLOT(Clear()));
    Button* ClearAllButton= CreateButton(tr("AC"), SLOT(ClearAll()));

    Button* ClearMemoryButton= CreateButton(tr("M-"), SLOT(ClearMemory()));
    Button* ReadMemoryButton= CreateButton(tr("MR"), SLOT(ReadMemory()));
    Button* SetMemoryButton= CreateButton(tr("SM"), SLOT(SetMemory()));
    Button* AddToMemoryButton= CreateButton(tr("M+"), SLOT(AddToMemory()));

    display = new QLineEdit("0");
    display->setReadOnly(true);
    display->setAlignment(Qt::AlignRight);
    display->setMaxLength(15);

    QFont font = display->font();
    font.setPointSize(font.pointSize() + 8);
    display->setFont(font);

    QGridLayout* MainLayout= new QGridLayout;
    MainLayout->setSizeConstraint(QLayout::SetFixedSize);
    MainLayout->addWidget(display,0,0,1,6);
    MainLayout->addWidget(BackspaceButton, 1, 0, 1, 2);
    MainLayout->addWidget(ClearButton, 1, 2, 1, 2);
    MainLayout->addWidget(ClearAllButton, 1, 4, 1, 2);

    MainLayout->addWidget(ClearMemoryButton, 2, 0);
    MainLayout->addWidget(ReadMemoryButton, 3, 0);
    MainLayout->addWidget(SetMemoryButton, 4, 0);
    MainLayout->addWidget(AddToMemoryButton, 5, 0);

    for(int i= 1; i< NumDigitButtons; ++i)
    {
        int row = ((9-i) / 3) +2;
        int column= ((i-1) % 3) +1;
        MainLayout->addWidget(digitButtons[i], row, column  );
    }

    MainLayout->addWidget(digitButtons[0], 5, 1);
    MainLayout->addWidget(PointButton, 5, 2);
    MainLayout->addWidget(ChangeSignButton, 5, 3);

    MainLayout->addWidget(DivisionButton, 2, 4);
    MainLayout->addWidget(MultipleButton, 3, 4);
    MainLayout->addWidget(MinusButton, 4, 4);
    MainLayout->addWidget(AdditiveButton, 5, 4);

    MainLayout->addWidget(SqrtButton, 2, 5,2,1);
    MainLayout->addWidget(EqualButton, 4, 5,2,1);

    setLayout(MainLayout);
    setWindowTitle(tr("Simple Calc"));
  //  ui->setupUi(this);
}

Calculator::~Calculator()
{
  //  delete ui;
}

Button::Button(const QString& text, QWidget *parent)
    : QToolButton(parent)
{
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    setText(text);
}

QSize Button::sizeHint() const
{
    QSize size = QToolButton::sizeHint();
    size.rheight() += 20;
    size.rwidth() = qMax(size.width(), size.height());
    return size;
}

Button *Calculator::CreateButton(const QString& text, const char *member)
{
    Button *button= new Button(text);
    connect(button,SIGNAL(clicked()), this, member);

    return button;

}

void Calculator::DigitClicked()
{
    Button *ClickedButton= qobject_cast<Button *>(sender());

   // int DigitValue= ClickedButton->text().toInt();
    QString digit=ClickedButton->text().remove(0,1);
    int DigitValue=digit.toInt();

    if (display->text() == "0" && DigitValue == 0.0)
        return;

    if (WaitingForOperand)
    {
        display->clear();
        WaitingForOperand=false;
    }

    if(display->text()== "0")
        display->clear();

    display->setText(display->text() + QString::number(DigitValue));
}

void Calculator::ClearAll()
{
    display->setText("0");
    WaitingForOperand=true;
    sumSoFar=0.0;
    FactorSoFar=0.0;
    PendingAdditiveOperator.clear();
    PendingMultiplicativeOperator.clear();
}

void Calculator::SetMemory()
{
    sumInMemory=0.0;
}

void Calculator::ClearMemory()
{
    sumInMemory = 0.0;
}

void Calculator::ReadMemory()
{
    display->setText(QString::number(sumInMemory));
    WaitingForOperand=true;
}

void Calculator::AddToMemory()
{
    sumInMemory+= display->text().toDouble();
}

void Calculator::AbortOperation()
{
    ClearAll();
    display->setText(tr("###"));
}

void Calculator::Clear()
{
    if(WaitingForOperand)
        return;

    display->setText("0");

    WaitingForOperand=false;
}

void Calculator::BackspaceClicked()
{
    QString Text=display->text();

    if(WaitingForOperand)
       return;

    Text.chop(1);

    if(Text.isEmpty())
    {
        Text="0";
        WaitingForOperand=true;
    }

    display->setText(Text);
}

void Calculator::ChangeSignClicked()
{
    if(display->text()=="0")
        return;

    double ActualSum = display->text().toDouble()*(-1);

    display->setText(QString::number(ActualSum));
}

void Calculator::PointClicked()
{
    if(WaitingForOperand)
        display->setText("0");

    if(!display->text().contains("."))
        display->setText(display->text()+ tr("."));

    WaitingForOperand=false;

}

void Calculator::EqualClicked()
{
    double operand = display->text().toDouble();

    if (!PendingMultiplicativeOperator.isEmpty()) {
        if (!Calculate(operand, PendingMultiplicativeOperator)) {
            AbortOperation();
            return;
        }
        operand = FactorSoFar;
        FactorSoFar = 0.0;
        PendingMultiplicativeOperator.clear();
    }
    if (!PendingAdditiveOperator.isEmpty()) {
        if (!Calculate(operand, PendingAdditiveOperator)) {
            AbortOperation();
            return;
        }
        PendingAdditiveOperator.clear();
    } else {
        sumSoFar = operand;
    }

    display->setText(QString::number(sumSoFar));
    sumSoFar = 0.0;
    WaitingForOperand = true;
}
void Calculator::AdditiveOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();
    if (!PendingMultiplicativeOperator.isEmpty()) {
        if (!Calculate(operand, PendingMultiplicativeOperator)) {
            AbortOperation();
            return;
        }
        display->setText(QString::number(FactorSoFar));
        operand = FactorSoFar;
        FactorSoFar = 0.0;
        PendingMultiplicativeOperator.clear();
    }
    if (!PendingAdditiveOperator.isEmpty()) {
        if (!Calculate(operand, PendingAdditiveOperator)) {
            AbortOperation();
            return;
        }
        display->setText(QString::number(sumSoFar));
    } else {
        sumSoFar = operand;
    }
    PendingAdditiveOperator = clickedOperator;
    WaitingForOperand = true;
}
void Calculator::UnaryOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    clickedOperator.remove(1,1); // need remove "&" I don't know why qt is adding this shit xD

    double operand = display->text().toDouble();
    double result = 0.0;

    if (clickedOperator == tr("Sqrt"))
    {
        if (operand < 0.0) {
            AbortOperation();
            return;
        }
        result = std::sqrt(operand);
    }

    display->setText(QString::number(result));
    WaitingForOperand = true;
}

void Calculator::MultiplicativeOperatorClicked()
{
    Button *clickedButton = qobject_cast<Button *>(sender());
    QString clickedOperator = clickedButton->text();
    double operand = display->text().toDouble();

    if (!PendingMultiplicativeOperator.isEmpty()) {
        if (!Calculate(operand, PendingMultiplicativeOperator)) {
            AbortOperation();
            return;
        }
        display->setText(QString::number(FactorSoFar));
    } else {
        FactorSoFar = operand;
    }

    PendingMultiplicativeOperator = clickedOperator;
    WaitingForOperand = true;
}

bool Calculator::Calculate(double rightOperand, const QString& pendingOperator)
{
        if (pendingOperator == tr("+"))
        {
            sumSoFar += rightOperand;
        }
        else if (pendingOperator == tr("-"))
        {
            sumSoFar -= rightOperand;
        }
        else if (pendingOperator == tr("\303\227"))
        {
            FactorSoFar *= rightOperand;
        }
        else if (pendingOperator == tr("\303\267"))
        {
            if (rightOperand == 0.0)
                return false;

            FactorSoFar /= rightOperand;
        }
        return true;
}


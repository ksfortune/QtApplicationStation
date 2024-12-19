#pragma once
#include <iostream>
#include <memory>
#include <QTextStream>
#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QSpinbox>
#include <QMessageBox>
#include <QInputDialog>
#include <QGraphicsTextItem>
#include <QApplication>
#include <QGradient>
#include <QLinearGradient>
#include <QFormLayout>
#include <QComboBox>
#include <QDateEdit>
#include <QPushButton>
#include <QPainter>
#include <QPaintEvent>
#include <QSize>
#include <QFileDialog>
#include "User.h"
#include "Train.h"
#include "Source.h"
#include "Strategy.h"


class BuyTicketWindow : public QWidget
{
    Q_OBJECT
private:
    QString station_file = "stations.txt";
    User* user = nullptr;
    Train* train = nullptr;
    int place = -1;
    int wagon = -1;
    const int H = 600;
    const int W = 800;
    QString DarkPinkButtonStyle =
        "QPushButton { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FFFFFF; "
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "stop: 0 #FF69B4, stop: 1 #000000); "
        "border: 2px solid #FF69B4; "
        "border-radius: 20px; "
        "padding: 10px; "
        "}"         "QPushButton:hover { background-color: #99004C; }"
        ;

    QString GreyButtonStyle =
        "QPushButton { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FFFFFF; "
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "stop: 0 #2D2D2D, stop: 1 #000000); "
        "border: 2px solid #FF69B4; "
        "border-radius: 20px; "
        "padding: 10px; "
        "}"         "QPushButton:hover { background-color: #99004C; }"
        ;

    const QString backgroundColor = "#000000";
    QString BaseFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 20px; color: #FFFFFF; }";
    QString HeaderFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 40px; color: #FFFFFF; }";
    QFont font;

    QString BaseEditFontStyle =
        "QLineEdit { "
        "font-family: 'Arkhip'; "
        "background-color: #2D2D2D; "
        "color: white; "
        "font-size: 20px; "
        "border: 2px solid #FF69B4; "
        "border-radius: 15px; "
        "padding: 8px; "
        "}";

    QString dateEditStyle =
        "QDateEdit {"
        "font-family: 'Arkhip'; "
        "    background-color: #2D2D2D;"
        "    color: white;"
        "    border: 2px solid #FF69B4;"
        "    border-radius: 15px;"
        "    padding: 5px;"
        "}";

    QString timeEditStyle =
        "QTimeEdit {"
        "font-family: 'Arkhip'; "
        "    background-color: #2D2D2D;"
        "    color: white;"
        "    border: 2px solid #FF69B4;"
        "    border-radius: 15px;"
        "    padding: 5px;"
        "}"
        "QTimeEdit::up-button, QTimeEdit::down-button { "
        "border: none; "
        "width: 20px; "
        "height: 20px; "
        "border-radius: 4px; "
        "}"
        "QTimeEdit::up-button:hover, QTimeEdit::down-button:hover { "
        "background-color: #e0e0e0; "
        "}"
        "QTimeEdit::up-button:pressed, QTimeEdit::down-button:pressed { "
        "background-color: #d0d0d0; "
        "}";

    QString SpinBoxEditStyle =
        "QSpinBox { "
        "font-family: 'Arkhip'; color: white; font - size: 20px; "
        "background-color: #2D2D2D; border: 2px solid #FF69B4; padding: 5px; border-radius: 15px; } "
        "QSpinBox::up-button, QSpinBox::down-button { "
        "border: none; "
        "width: 20px; "
        "height: 20px; "
        "border-radius: 4px; "
        "}"
        "QSpinBox::up-button:hover, QSpinBox::down-button:hover { "
        "background-color: #e0e0e0; "
        "}"
        "QSpinBox::up-button:pressed, QSpinBox::down-button:pressed { "
        "background-color: #d0d0d0; "
        "}";

    QString ListWidgetEditStyle =
        "QListWidget { "
        "    font-family: 'Arkhip'; "
        "    color: white; "
        "    font-size: 20px; "
        "    background-color: #2D2D2D; "
        "    border: 2px solid #FF69B4; "
        "    border-radius: 15px; "
        "    padding: 5px; "
        "} "

        "QListWidget::item { "
        "    padding: 5px; "
        "    border-bottom: 1px solid #FF69B4; "
        "} "

        "QListWidget::item:hover { "
        "background-color: #ffcccb; "
        "} "

        "QListWidget::item:selected { "
        "background-color: #ffcccb; "
        "} ";

    QLineEdit* fromInput;
    QLineEdit* toInput;
    QDateEdit* dateEdit;
    QStringList stations;
    QWidget* currentWindow = nullptr;
    QWidget* currentWindow1 = nullptr;
    void decreaseDate();
    void increaseDate();
    //void onTrainItemClicked();


public:
    BuyTicketWindow(User* user, QWidget* parent = nullptr);
    void showSearchResults();
    ~BuyTicketWindow();
};


class CustomTriangleButton : public QPushButton {
    Q_OBJECT

public:
    enum Direction {
        Left,
        Right
    };

    explicit CustomTriangleButton(Direction direction, QWidget* parent = nullptr)
        : QPushButton(parent), direction(direction) {
        setFixedSize(60, 60);  // Фиксированный размер кнопки
        QString GreyButtonStyle =
            "QPushButton { "
            "font-family: 'Arkhip'; "
            "font-size: 20px; "
            "color: #FFFFFF; "
            "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
            "stop: 0 #2D2D2D, stop: 1 #000000); "
            "border: 2px solid #FF69B4; "
            "border-radius: 20px; "
            "padding: 10px; "
            "}"         "QPushButton:hover { background-color: #99004C; }"
            ;
        setStyleSheet(GreyButtonStyle);  // Прозрачный фон

    }

protected:
    void paintEvent(QPaintEvent* event) override {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);

        painter.setBrush(QColor(255, 192, 203, 128)); 
        painter.setPen(Qt::NoPen);  

        QPolygon triangle;

        if (direction == Left) {
            triangle << QPoint(width(), 0)
                << QPoint(0, height() / 2)
                << QPoint(width(), height());
        }
        else if (direction == Right) {
            triangle << QPoint(0, 0)
                << QPoint(width(), height() / 2)
                << QPoint(0, height());
        }
        painter.drawPolygon(triangle);
    }

private:
    Direction direction;
};


class WagonSeatsWindow : public QWidget {
    Q_OBJECT

public:
    explicit WagonSeatsWindow(Train* train, QString item, User* user, QWidget* parent = nullptr);

private slots:
    void onSeatClicked(QPushButton* seatButton, int seatNumber, QString item);
    void decreaseWagon();
    void updateWagonSeats();
    void increaseWagon();
    //void loadSeats();



private:
    int m_wagonNumber = 1;
    QString item;
    QLabel* wagonLabel;      
    QGridLayout* seatGridLayout; 
    QWidget* currentWindow = nullptr;
    Train* train;
    User* user;
    const QString backgroundColor = "#000000";
    QString BaseFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 20px; color: #FFFFFF; }";
    QString HeaderFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 40px; color: #FFFFFF; }";
    QFont font;
};

class StartWindow : public QWidget
{
    Q_OBJECT
private:
    const int H = 600;
    const int W = 800;
    QString DarkPinkButtonStyle =
        "QPushButton { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FFFFFF; "
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "stop: 0 #FF69B4, stop: 1 #000000); "
        "border: 2px solid #FF69B4; "
        "border-radius: 20px; "
        "padding: 10px; "
        "}"         "QPushButton:hover { background-color: #99004C; }"
        ;

    QString LightPinkButtonStyle =
        "QPushButton { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FFFFFF; "
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "stop: 0 #FF69B4, stop: 1 #234e0a); "
        "border: 2px solid #FF69B4; "
        "border-radius: 20px; "
        "padding: 10px; "
        "}"         "QPushButton:hover { background-color: #99004C; }"
        ;

    const QString backgroundColor = "#000000";
    QString BaseFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 20px; color: #FFFFFF; }";
    QString BaseCFontStyle = "QLabel { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FF69B4; "
        "cursor: pointer;"
        "}"
        "QLabel:hover { "
        "color: #FFFFFF; "
        "}";

    QString HeaderFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 40px; color: #FFFFFF; }";
    QFont font;

    QString BaseEditFontStyle =
        "QLineEdit { "
        "font-family: 'Arkhip'; "
        "background-color: #2D2D2D; "
        "color: white; "
        "font-size: 20px; "
        "border: 2px solid #FF69B4; "
        "border-radius: 15px; "
        "padding: 8px; "
        "}";
    const QString stripeColor = "#FF69B4";
    const int stripeWidth = 10;

    QLineEdit* usernameInput;
    QLineEdit* passwordInput;

    void paintEvent(QPaintEvent* event);
    void sing_up_form();

public:
    StartWindow(QWidget* parent = nullptr);
    ~StartWindow();
};


class ClickableLabel : public QLabel {
    Q_OBJECT
public:
    explicit ClickableLabel(QWidget* parent = nullptr) : QLabel(parent) {}

signals:
    void clicked();

protected:
    void mousePressEvent(QMouseEvent* event) override {
        emit clicked();
        QLabel::mousePressEvent(event);
    }
};


class TicketImageWidget : public QWidget {
    Q_OBJECT

public:
    explicit TicketImageWidget(Train* train, User* user, int wagon, int place,
        QString price, QString from, QString to, QWidget* parent = nullptr)
        : QWidget(parent), train(train), user(user), wagon(wagon), place(place),
        price(price), from(from), to(to) {
        setFixedSize(600, 300);
    }

    bool saveImageToFile(const QString& filePath) {
        QPixmap pixmap(size());
        render(&pixmap); 
        return pixmap.save(filePath);
    }

protected:
    void paintEvent(QPaintEvent* event) override {
        QPainter painter(this);

        painter.setBrush(QBrush(Qt::black));
        painter.drawRect(0, 0, width(), height());

        QPen pen(Qt::white);
        painter.setPen(pen);
        QFont font("Arkhip", 6);
        painter.setFont(font);

        QString name = "Passenger: " + QString::fromStdString(user->get_name() + " " + user->get_surname());
        QString trainName = QString::fromStdString(train->get_name());
        QString wagon_ = "Wagon: " + QString::number(wagon);
        QString seat = "Seat: " + QString::number(place);

        painter.drawText(10, 30, name);
        painter.drawText(10, 60, "Train: " + trainName);
        painter.drawText(10, 90, wagon_);
        painter.drawText(10, 120, seat);
        painter.drawText(10, 180, to);
        painter.drawText(10, 150, from);
        painter.drawText(10, 210, "Price: " + price);
    }

private:
    Train* train;
    User* user;
    int wagon, place;
    QString price, from, to;
};

class Registration : public QWidget
{
    Q_OBJECT

private slots:
    void onLoginButtonClicked();

private:
    QLabel* statusLabel;
    const int H = 600;
    const int W = 600;

    QString DarkPinkButtonStyle =
        "QPushButton { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FFFFFF; "
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "stop: 0 #FF69B4, stop: 1 #000000); "
        "border: 2px solid #FF69B4; "
        "border-radius: 20px; "
        "padding: 10px; "
        "}"         "QPushButton:hover { background-color: #99004C; }"
        ;

    QString GreyButtonStyle =
        "QPushButton { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FFFFFF; "
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "stop: 0 #2D2D2D, stop: 1 #000000); "
        "border: 2px solid #FF69B4; "
        "border-radius: 20px; "
        "padding: 10px; "
        "}"         "QPushButton:hover { background-color: #99004C; }"
        ;

    const QString backgroundColor = "#000000";
    QString BaseFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 20px; color: #FFFFFF; }";
    QString BaseCFontStyle = "QLabel { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FF69B4; "
        "cursor: pointer;"
        "}"
        "QLabel:hover { "
        "color: #FFFFFF; "
        "}";

    QString dateEditStyle =
        "QDateEdit {"
        "font-family: 'Arkhip'; "
        "    background-color: #2D2D2D;"
        "    color: white;"
        "    border: 2px solid #FF69B4;"
        "    border-radius: 15px;"
        "    padding: 5px;"
        "}";


    QString HeaderFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 40px; color: #FFFFFF; }";
    QFont font;

    QString BaseEditFontStyle =
        "QLineEdit { "
        "font-family: 'Arkhip'; "
        "background-color: #2D2D2D; "
        "color: white; "
        "font-size: 20px; "
        "border: 2px solid #FF69B4; "
        "border-radius: 15px; "
        "padding: 8px; "
        "}";
    const QString stripeColor = "#FF69B4";
    const int stripeWidth = 10;
    QLineEdit* nameInput;
    QLineEdit* surnameInput;
    QDateEdit* dobInput;
    QLineEdit* userInput;
    QLineEdit* usernameInput;
    QLineEdit* passwordInput;
    QWidget* loginWidget;

public:
    Registration(QWidget* parent = nullptr);
    ~Registration();
};


class AdminMenuWindow : public QWidget
{
    Q_OBJECT
private:
    Admin* user = nullptr;
    const int H = 600;
    const int W = 1200;
    QString DarkPinkButtonStyle =
        "QPushButton { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FFFFFF; "
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "stop: 0 #FF69B4, stop: 1 #000000); "
        "border: 2px solid #FF69B4; "
        "border-radius: 20px; "
        "padding: 10px; "
        "}"         "QPushButton:hover { background-color: #99004C; }"
        ;

    QString GreyButtonStyle =
        "QPushButton { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FFFFFF; "
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "stop: 0 #2D2D2D, stop: 1 #000000); "
        "border: 2px solid #FF69B4; "
        "border-radius: 20px; "
        "padding: 10px; "
        "}"         "QPushButton:hover { background-color: #99004C; }"
        ;

    const QString backgroundColor = "#000000";
    QString BaseFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 20px; color: #FFFFFF; }";
    QString HeaderFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 40px; color: #FFFFFF; }";
    QFont font;
    const QString stripeColor = "#FF69B4";
    const int stripeWidth = 10;

public:
    AdminMenuWindow(Admin* user, QWidget* parent = nullptr);
    ~AdminMenuWindow();
};


class AddTrainWindow : public QWidget
{
    Q_OBJECT
private:
    QString station_file = "stations.txt";
    Admin* user = nullptr;
    const int H = 600;
    const int W = 1200;
    QString DarkPinkButtonStyle =
        "QPushButton { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FFFFFF; "
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "stop: 0 #FF69B4, stop: 1 #000000); "
        "border: 2px solid #FF69B4; "
        "border-radius: 20px; "
        "padding: 10px; "
        "}"         "QPushButton:hover { background-color: #99004C; }"
        ;

    QString GreyButtonStyle =
        "QPushButton { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FFFFFF; "
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "stop: 0 #2D2D2D, stop: 1 #000000); "
        "border: 2px solid #FF69B4; "
        "border-radius: 20px; "
        "padding: 10px; "
        "}"         "QPushButton:hover { background-color: #99004C; }"
        ;

    QString LightPinkButtonStyle =
        "QPushButton { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FFFFFF; "
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "stop: 0 #FF69B4, stop: 1 #234e0a); "
        "border: 2px solid #FF69B4; "
        "border-radius: 20px; "
        "padding: 10px; "
        "}"         "QPushButton:hover { background-color: #99004C; }"
        ;

    const QString backgroundColor = "#000000";
    QString BaseFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 20px; color: #FFFFFF; }";
    QString BaseCFontStyle = "QLabel { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FF69B4; "
        "cursor: pointer;"
        "}"
        "QLabel:hover { "
        "color: #FFFFFF; "
        "}";

    QString HeaderFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 40px; color: #FFFFFF; }";
    QFont font;

    QString BaseEditFontStyle =
        "QLineEdit { "
        "font-family: 'Arkhip'; "
        "background-color: #2D2D2D; "
        "color: white; "
        "font-size: 20px; "
        "border: 2px solid #FF69B4; "
        "border-radius: 15px; "
        "padding: 8px; "
        "}";

    QString dateEditStyle =
        "QDateEdit {"
        "font-family: 'Arkhip'; "
        "    background-color: #2D2D2D;"
        "    color: white;"
        "    border: 2px solid #FF69B4;"
        "    border-radius: 15px;"
        "    padding: 5px;"
        "}";

    QString timeEditStyle =
        "QTimeEdit {"
        "font-family: 'Arkhip'; "
        "    background-color: #2D2D2D;"
        "    color: white;"
        "    border: 2px solid #FF69B4;"
        "    border-radius: 15px;"
        "    padding: 5px;"
        "}"
        "QTimeEdit::up-button, QTimeEdit::down-button { "
        "border: none; "
        "width: 20px; "
        "height: 20px; "
        "border-radius: 4px; "
        "}"
        "QTimeEdit::up-button:hover, QTimeEdit::down-button:hover { "
        "background-color: #e0e0e0; "
        "}"
        "QTimeEdit::up-button:pressed, QTimeEdit::down-button:pressed { "
        "background-color: #d0d0d0; "
        "}";

    QString SpinBoxEditStyle =
        "QSpinBox { "
        "font-family: 'Arkhip'; color: white; font - size: 20px; "
        "background-color: #2D2D2D; border: 2px solid #FF69B4; padding: 5px; border-radius: 15px; } "
        "QSpinBox::up-button, QSpinBox::down-button { "
        "border: none; "
        "width: 20px; "
        "height: 20px; "
        "border-radius: 4px; "
        "}"
        "QSpinBox::up-button:hover, QSpinBox::down-button:hover { "
        "background-color: #e0e0e0; "
        "}"
        "QSpinBox::up-button:pressed, QSpinBox::down-button:pressed { "
        "background-color: #d0d0d0; "
        "}";

    QString ListWidgetEditStyle =
        "QListWidget { "
        "    font-family: 'Arkhip'; "
        "    color: white; "
        "    font-size: 20px; "
        "    background-color: #2D2D2D; "
        "    border: 2px solid #FF69B4; "
        "    border-radius: 15px; "
        "    padding: 5px; "
        "} "

        "QListWidget::item { "
        "    padding: 5px; "
        "    border-bottom: 1px solid #FF69B4; "
        "    color: black; "
        "} "

        "QListWidget::item:hover { "
        "background-color: #ffcccb; "
        "color: black; "
        "} "

        "QListWidget::item:selected { "
        "background-color: #ffcccb; "
        "color: black; "
        "} ";

    const QString stripeColor = "#FF69B4";
    const int stripeWidth = 10;
    QLineEdit* trainNameInput;
    QLineEdit* routeInput;
    QSpinBox* wagonsCountInput;
    QSpinBox* placePerWagInput;
    QLineEdit* searchEdit;
    QDateEdit* dateEdit;
    QListWidget* trainListWidget;
    QTimeEdit* timeEdit;
    QStringList stations;
public:
    AddTrainWindow(Admin* user, QWidget* parent = nullptr);
    QStringList loadStationsFromFile(const QString& fileName);
    ~AddTrainWindow();
};


class DeleteTrainWindow : public QWidget
{
    Q_OBJECT
private slots:
    void loadTrainsFromDatabase();
    int removeTrainFromDatabase(const QString& trainName);
private:
    Admin* user = nullptr;
    const int H = 600;
    const int W = 1200;
    QString DarkPinkButtonStyle =
        "QPushButton { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FFFFFF; "
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "stop: 0 #FF69B4, stop: 1 #000000); "
        "border: 2px solid #FF69B4; "
        "border-radius: 20px; "
        "padding: 10px; "
        "}"         "QPushButton:hover { background-color: #99004C; }"
        ;

    QString GreyButtonStyle =
        "QPushButton { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FFFFFF; "
        "background: qlineargradient(x1: 0, y1: 0, x2: 0, y2: 1, "
        "stop: 0 #2D2D2D, stop: 1 #000000); "
        "border: 2px solid #FF69B4; "
        "border-radius: 20px; "
        "padding: 10px; "
        "}"         "QPushButton:hover { background-color: #99004C; }"
        ;

    const QString backgroundColor = "#000000";
    QString BaseFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 20px; color: #FFFFFF; }";
    QString BaseCFontStyle = "QLabel { "
        "font-family: 'Arkhip'; "
        "font-size: 20px; "
        "color: #FF69B4; "
        "cursor: pointer;"
        "}"
        "QLabel:hover { "
        "color: #FFFFFF; "
        "}";

    QString HeaderFontStyle = "QLabel { font-family: 'Arkhip'; font-size: 40px; color: #FFFFFF; }";
    QFont font;

    QString BaseEditFontStyle =
        "QLineEdit { "
        "font-family: 'Arkhip'; "
        "background-color: #2D2D2D; "
        "color: white; "
        "font-size: 20px; "
        "border: 2px solid #FF69B4; "
        "border-radius: 15px; "
        "padding: 8px; "
        "}";

    QString ListWidgetEditStyle =
        "QListWidget { "
        "    font-family: 'Arkhip'; "
        "    color: white; "
        "    font-size: 20px; "
        "    background-color: #2D2D2D; "
        "    border: 2px solid #FF69B4; "
        "    border-radius: 15px; "
        "    padding: 5px; "
        "} "

        "QListWidget::item { "
        "    padding: 5px; "
        "    border-bottom: 1px solid #FF69B4; "
        "    color: white; "
        "} "

        "QListWidget::item:hover { "
        "background-color: #ffcccb; "
        "color: black; "
        "} "

        "QListWidget::item:selected { "
        "background-color: #ffcccb; "
        "color: black; "
        "} ";

    const QString stripeColor = "#FF69B4";
    const int stripeWidth = 10;
    QWidget* detailsWindow;
    QListWidget* trainListWidget;

    Train* train;
public:
    DeleteTrainWindow(Admin* user, QWidget* parent = nullptr);
    ~DeleteTrainWindow();
};

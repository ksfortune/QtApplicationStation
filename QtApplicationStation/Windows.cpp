#define _CRT_SECURE_NO_WARNINGS
#include <QSpinBox>
#include <QCompleter>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFontDatabase>
#include <QMouseEvent>
#include "Windows.h"
#include "Source.h"
#include "Train.h"
#include <fstream>



BuyTicketWindow::BuyTicketWindow(User* user, QWidget* parent) : QWidget(parent), user(user) {
    if (currentWindow1) {
        currentWindow1->close();
        delete currentWindow1;
        currentWindow1 = this;
    }
    this->setFixedSize(W, H);
    stations = loadStationsFromFile(station_file);
    this->setWindowTitle("Tickets Form");

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(backgroundColor));
    this->setAutoFillBackground(true);
    this->setPalette(palette);
    QWidget* formContainer = new QWidget(this);
    formContainer->setFixedHeight(H / 2);

    QLabel* welcomeLabel = new QLabel("Find a Ticket:", this);
    welcomeLabel->setStyleSheet(HeaderFontStyle);

    fromInput = new QLineEdit(this);
    fromInput->setPlaceholderText("From");
    fromInput->setStyleSheet(BaseEditFontStyle);

    toInput = new QLineEdit(this);
    toInput->setPlaceholderText("To");
    toInput->setStyleSheet(BaseEditFontStyle);

    dateEdit = new QDateEdit(QDate::currentDate(), this);
    dateEdit->setCalendarPopup(true);
    dateEdit->setStyleSheet(dateEditStyle);

    QPushButton* findButton = new QPushButton("Find ticket", this);
    QPushButton* getBackButton = new QPushButton("Cancel", this);
    getBackButton->setStyleSheet(GreyButtonStyle);
    findButton->setStyleSheet(DarkPinkButtonStyle);
    QBoxLayout* boxButton = new QBoxLayout(QBoxLayout::LeftToRight);
    boxButton->addWidget(getBackButton);
    boxButton->addWidget(findButton);

    QVBoxLayout* formLayout = new QVBoxLayout();
    formLayout->addWidget(welcomeLabel);
    formLayout->addWidget(fromInput);
    formLayout->addWidget(toInput);
    formLayout->addWidget(dateEdit);
    formLayout->addLayout(boxButton);

    formContainer->setLayout(formLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    QSpacerItem* topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    mainLayout->addSpacerItem(topSpacer);
    mainLayout->addWidget(formContainer);

    this->setLayout(mainLayout);

    QCompleter* completer = new QCompleter(stations, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    fromInput->setCompleter(completer);
    toInput->setCompleter(completer);

    connect(getBackButton, &QPushButton::clicked, [=]() {
        this->close();
        StartWindow* userWindow = new StartWindow(nullptr);
        userWindow->show();
        });

    connect(findButton, &QPushButton::clicked, this, &BuyTicketWindow::showSearchResults);
    
}

BuyTicketWindow::~BuyTicketWindow(){}

void BuyTicketWindow::showSearchResults() {
    bool foundMatch = false;
    QString fromStation = fromInput->text().trimmed();
    QString toStation = toInput->text().trimmed();
    QString date = dateEdit->date().toString("dd-MM-yyyy");
    
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y");
    QString today = QString::fromStdString(oss.str());
    int age = time_between_to_times_(today, QString::fromStdString(user->get_date_birth()), 0) / 365;
    std::cerr << (user->get_date_birth()) << " " << today.toStdString() << " " <<  age << std::endl;


    if (fromStation.isEmpty() || toStation.isEmpty()) {
        QMessageBox::warning(this, "Error", "Enter more information");
        return;
    }

    QMap<QString, std::vector<QPair<QString, QString>>> stations = getStationsData();
    std::vector<QPair<QString, QString>> train_list = checkTrainAtStations(fromStation, toStation, date, stations);

    if (currentWindow1) {
        currentWindow1->close();
        delete currentWindow1;
        currentWindow1 = nullptr;
    }

    QWidget* detailsWindow = new QWidget();
    currentWindow1 = detailsWindow;
    detailsWindow->setAttribute(Qt::WA_DeleteOnClose);
    detailsWindow->setFixedSize(W / 2, H);
    detailsWindow->setWindowTitle("Trains");

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(backgroundColor));
    detailsWindow->setAutoFillBackground(true);
    detailsWindow->setPalette(palette);

    connect(detailsWindow, &QWidget::destroyed, [this]() {
        currentWindow1 = nullptr;
        });

    QListWidget* resultListWidget = new QListWidget(detailsWindow);

    for (const auto& item : train_list) {
        Train* tr0 = get_train_from_db(item.first.toStdString());
        setScheduleToTrain(tr0);

        QString fromStationCoords = getCoordinatesForStation(fromStation);
        QString toStationCoords = getCoordinatesForStation(toStation);

        QMap<QString, QString> schedule = tr0->get_schedule();
        QString date1 = schedule[fromStation];
        QString date2 = schedule[toStation];

        int duration_min = time_between_to_times_(date2, date1, 1);
        double hours = duration_min / 60.0;

        double distance = calculateDistance(
            fromStationCoords.split(", ")[0].toDouble(),
            fromStationCoords.split(", ")[1].toDouble(),
            toStationCoords.split(", ")[0].toDouble(),
            toStationCoords.split(", ")[1].toDouble());

        TicketPriceStrategy* priceStrategy = nullptr;
        if (age < 18|| age > 60)
            priceStrategy = new DiscountTicketPrice();
        else
            priceStrategy = new StandardTicketPrice();

        double price = priceStrategy->calculatePrice(distance, hours);
        delete priceStrategy;

        QString result = QString("Train: %1\nRoute: %2\n\n%3: %4\n%5: %6\nPrice: %7$")
            .arg(QString::fromStdString(tr0->get_name()))
            .arg(QString::fromStdString(tr0->get_route()))
            .arg(fromStation.split(":")[1].trimmed()).arg(date1)
            .arg(toStation.split(":")[1].trimmed()).arg(date2)
            .arg(price);
        foundMatch = true;
        resultListWidget->addItem(result);
    }

    if (!foundMatch) {
        resultListWidget->addItem("No trains available for the selected criteria.");
    }
    else {
        connect(resultListWidget, &QListWidget::itemClicked, this, [this](QListWidgetItem* item) {
            QString selectedTrain = item->text();
            QString a = selectedTrain.split('\n')[0];
            a = a.split(": ")[1];
            Train* train_ = get_train_from_db(a.toStdString());

            WagonSeatsWindow* wagonWindow = new WagonSeatsWindow(train_, item->text(), user, nullptr);
            wagonWindow->show();
            });
    }

    resultListWidget->setStyleSheet(ListWidgetEditStyle);
    CustomTriangleButton* triangleButton1 = new CustomTriangleButton(CustomTriangleButton::Left);
    CustomTriangleButton* triangleButton2 = new CustomTriangleButton(CustomTriangleButton::Right);
    connect(triangleButton1, &QPushButton::clicked, this, &BuyTicketWindow::decreaseDate);
    connect(triangleButton2, &QPushButton::clicked, this, &BuyTicketWindow::increaseDate);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(triangleButton1);
    buttonLayout->addSpacing(20);
    buttonLayout->addWidget(triangleButton2);
    buttonLayout->addStretch();

    QWidget* buttonContainer = new QWidget(detailsWindow);
    buttonContainer->setLayout(buttonLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout(detailsWindow);
    mainLayout->addWidget(resultListWidget, 9);
    mainLayout->addWidget(buttonContainer, 1);

    detailsWindow->setLayout(mainLayout);
    detailsWindow->setWindowModality(Qt::NonModal);
    detailsWindow->show();
}

void BuyTicketWindow::increaseDate() {
    QDate currentDate = dateEdit->date();
    currentDate = currentDate.addDays(1);
    dateEdit->setDate(currentDate);
    update();
    showSearchResults();
}

void BuyTicketWindow::decreaseDate() {
    QDate currentDate = dateEdit->date();
    currentDate = currentDate.addDays(-1);
    dateEdit->setDate(currentDate);
    update();
    showSearchResults();
}


WagonSeatsWindow::WagonSeatsWindow(Train* train, QString item, User* user, QWidget* parent)
    : QWidget(parent), train(train), item(item), user(user) {

    if (train == nullptr) {
        return;
    }

    this->setAttribute(Qt::WA_DeleteOnClose);
    this->setFixedSize(600, 400);
    this->setWindowTitle("Wagon " + QString::number(m_wagonNumber));

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(backgroundColor));
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    CustomTriangleButton* triangleButton1_ = new CustomTriangleButton(CustomTriangleButton::Left);
    CustomTriangleButton* triangleButton2_ = new CustomTriangleButton(CustomTriangleButton::Right);

    wagonLabel = new QLabel("Wagon " + QString::number(m_wagonNumber), this);
    wagonLabel->setAlignment(Qt::AlignCenter);
    wagonLabel->setStyleSheet(HeaderFontStyle);

    QHBoxLayout* layout_ = new QHBoxLayout(this);
    layout_->addWidget(triangleButton1_);
    layout_->addWidget(wagonLabel);
    layout_->addWidget(triangleButton2_);

    mainLayout->addLayout(layout_);

    seatGridLayout = new QGridLayout();
    mainLayout->addLayout(seatGridLayout);

    connect(triangleButton1_, &QPushButton::clicked, this, &WagonSeatsWindow::decreaseWagon);
    connect(triangleButton2_, &QPushButton::clicked, this, &WagonSeatsWindow::increaseWagon);

    this->setLayout(mainLayout);

    updateWagonSeats();

    this->show();
}


void WagonSeatsWindow::updateWagonSeats() {
    QLayoutItem* child;
    while ((child = seatGridLayout->takeAt(0)) != nullptr) {
        delete child->widget(); 
        delete child;           
    }
    this->setWindowTitle("Wagon " + QString::number(m_wagonNumber));
    wagonLabel->setText("Wagon " + QString::number(m_wagonNumber));

    std::vector<std::pair<int, std::string>> seats = getSeatsInfo(train->get_name(), m_wagonNumber);

    for (const auto& seat : seats) {
        int seatNumber = seat.first;
        const std::string& ticketId = seat.second;

        QPushButton* seatButton = new QPushButton(QString::number(seatNumber), this);
        seatButton->setFixedSize(40, 40);

        if (ticketId != "Free") {
            seatButton->setStyleSheet("background-color: red; border-radius: 5px;");
        }
        else {
            seatButton->setStyleSheet("background-color: green; border-radius: 5px;");
        }
        connect(seatButton, &QPushButton::clicked, this, [this, seatButton, seatNumber, ticketId]() {
            if (ticketId == "Free") {
                onSeatClicked(seatButton, seatNumber, this->item);
                seatButton->setStyleSheet("background-color: red; border-radius: 5px;");
            }
            else {
                QMessageBox::critical(nullptr, "Error", "Seat is already occupied.");
            }
            });


        int row = (seatNumber - 1) / 4;
        int col = (seatNumber - 1) % 4;
        seatGridLayout->addWidget(seatButton, row, col);
    }
}

void WagonSeatsWindow::decreaseWagon() {
    m_wagonNumber--;
    if (m_wagonNumber == 0) m_wagonNumber = train->get_wagons_count();
    updateWagonSeats();
}

void WagonSeatsWindow::increaseWagon() {
    m_wagonNumber++;
    if (m_wagonNumber > train->get_wagons_count()) m_wagonNumber = 1;
    updateWagonSeats();
}



void WagonSeatsWindow::onSeatClicked(QPushButton* seatButton, int seatNumber, QString item) {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Buy Ticket", "Do you want to buy this seat?",
        QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        seatButton->setStyleSheet("background-color: grey; border-radius: 5px;");
        QStringList lines = item.split('\n');
        QString price = lines.at(lines.size() - 1);
        QString secondLastLine = lines.at(lines.size() - 2);
        QString thirdLastLine = lines.at(lines.size() - 3);

        TicketImageWidget* ticketWidget = new TicketImageWidget(
            train, user, m_wagonNumber, seatNumber, price, secondLastLine, thirdLastLine, nullptr);

        QString filePath = QFileDialog::getSaveFileName(
            this, "Save Ticket Image", QDir::homePath() + "/ticket_image.png", "Images (*.png *.jpg *.bmp)");
        if (!filePath.isEmpty()) {
            if (ticketWidget->saveImageToFile(filePath)) {
                QMessageBox::information(this, "Success", "Ticket saved successfully to " + filePath);
                occupy_place(QString::fromStdString(train->get_name()), m_wagonNumber, seatNumber, user);
                this->update();
            }
            else {
                QMessageBox::critical(this, "Error", "Failed to save the ticket.");
            }
        }
    }
}


StartWindow::StartWindow(QWidget* parent)
{
    this->setFixedSize(W, H);
    this->setWindowTitle("Login Form");

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(backgroundColor));
    this->setAutoFillBackground(true);
    this->setPalette(palette);
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    QWidget* formContainer = new QWidget(this);
    formContainer->setFixedWidth(W / 2);

    QVBoxLayout* formLayout = new QVBoxLayout(this);
    QLabel* label1 = new QLabel("Welcome!");
    label1->setStyleSheet(HeaderFontStyle);
    QLabel* label2 = new QLabel("Sing in to buy a train ticket:");
    label2->setStyleSheet(BaseFontStyle);
    QLabel* label3 = new QLabel("Don't have an account?");
    label3->setStyleSheet(BaseFontStyle);
    ClickableLabel* linkLabel = new ClickableLabel(this);
    linkLabel->setText("SING UP");
    linkLabel->setStyleSheet(BaseCFontStyle);

    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Username");
    usernameInput->setStyleSheet(BaseEditFontStyle);

    passwordInput = new QLineEdit(this);
    passwordInput->setPlaceholderText("Password");
    passwordInput->setStyleSheet(BaseEditFontStyle);
    passwordInput->setEchoMode(QLineEdit::Password);

    QPushButton* loginButton = new QPushButton("Login", this);
    loginButton->setStyleSheet(DarkPinkButtonStyle);

    QSpacerItem* topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QSpacerItem* bottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    formLayout->addSpacerItem(topSpacer);
    formLayout->addWidget(label1);
    formLayout->addWidget(label2);
    formLayout->addSpacing(20);
    formLayout->addWidget(usernameInput);
    formLayout->addWidget(passwordInput);
    formLayout->addWidget(loginButton);
    formLayout->addSpacing(20);
    formLayout->addWidget(label3);
    formLayout->addWidget(linkLabel);
    formLayout->addSpacerItem(bottomSpacer);

    formContainer->setLayout(formLayout);
    mainLayout->addWidget(formContainer);

    QWidget* rightSpacer = new QWidget(this);
    rightSpacer->setFixedWidth(W * 3 / 8);
    mainLayout->addWidget(rightSpacer);

    connect(linkLabel, &ClickableLabel::clicked, this, [this]() {
        Registration* regWindow = new Registration();
        regWindow->show();
        });

    connect(loginButton, &QPushButton::clicked, this, [this]() {
        QString userName = usernameInput->text();
        QString passw = passwordInput->text();
        int adm = CheckIfAdmin_(userName, passw);
        User* baseUser = GetUserByData_(usernameInput->text());
        if (adm == 0)
        {
            this->close();
            baseUser->set_role("Admin");
            Admin* admin = static_cast<Admin*>(baseUser);
            AdminMenuWindow* adminWindow = new AdminMenuWindow(admin, nullptr);
            adminWindow->show();
        }
        else {
            int res = FindUserData_(userName, passw);
            switch (res) {
            case -1:
                QMessageBox::warning(this, "Program Failed", "Please try again later or contact Developer.");
                break;
            case 1:
                QMessageBox::warning(this, "User not found", "This user is not registred in database.");
                break;
            case 2:
                QMessageBox::warning(this, "Wrong Password", "Password incorrect, please try again.");
                break;
            case 0:
                this->close();
                BuyTicketWindow* userWindow = new BuyTicketWindow(baseUser, nullptr);
                userWindow->show();
            }
        }});

    this->setLayout(mainLayout);
    this->show();
}

StartWindow::~StartWindow()
{}

void StartWindow::paintEvent(QPaintEvent* event)
{
    QWidget::paintEvent(event);

    QPainter painter(this);

    painter.setBrush(QBrush(QColor(stripeColor)));
    painter.setPen(Qt::NoPen);

    int stripeX = W - stripeWidth;
    painter.drawRect(stripeX, 0, stripeWidth, H);
    painter.drawRect(stripeX - 30, 0, stripeWidth, H);
    painter.drawRect(stripeX - 50, 0, stripeWidth, H);

}

Registration::Registration(QWidget* parent) {
    this->setFixedSize(W, H);
    this->setWindowTitle("Registration Form");

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(backgroundColor));
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    QWidget* formContainer = new QWidget(this);
    formContainer->setFixedWidth(W * 0.8);

    QVBoxLayout* formLayout = new QVBoxLayout(this);
    QLabel* label1 = new QLabel("Create an account:");
    label1->setStyleSheet(HeaderFontStyle);

    nameInput = new QLineEdit(this);
    nameInput->setPlaceholderText("Name");
    nameInput->setStyleSheet(BaseEditFontStyle);

    surnameInput = new QLineEdit(this);
    surnameInput->setPlaceholderText("Surname");
    surnameInput->setStyleSheet(BaseEditFontStyle);

    QLabel* label2 = new QLabel("Date of Birth:", this);
    label2->setStyleSheet(BaseFontStyle);
    dobInput = new QDateEdit(this);
    dobInput->setCalendarPopup(true);
    dobInput->setStyleSheet(dateEditStyle);
    dobInput->setDate(QDate::currentDate());
    QHBoxLayout* formLayout0 = new QHBoxLayout(this);
    formLayout0->addWidget(label2);
    formLayout0->addWidget(dobInput);

    usernameInput = new QLineEdit(this);
    usernameInput->setPlaceholderText("Username");
    usernameInput->setStyleSheet(BaseEditFontStyle);

    passwordInput = new QLineEdit(this);
    passwordInput->setPlaceholderText("Password");
    passwordInput->setStyleSheet(BaseEditFontStyle);
    passwordInput->setEchoMode(QLineEdit::Password);

    QPushButton* loginButton = new QPushButton("Login", this);
    loginButton->setStyleSheet(DarkPinkButtonStyle);
    QPushButton* cancelButton = new QPushButton("Back", this);
    cancelButton->setStyleSheet(GreyButtonStyle);
    QHBoxLayout* buttonLayout = new QHBoxLayout(this);
    buttonLayout->addWidget(cancelButton);
    buttonLayout->addWidget(loginButton);

    QSpacerItem* topSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);
    QSpacerItem* bottomSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

    formLayout->addSpacerItem(topSpacer);
    formLayout->addWidget(label1);
    formLayout->addSpacing(20);
    formLayout->addWidget(nameInput);
    formLayout->addWidget(surnameInput);
    formLayout->addLayout(formLayout0);
    formLayout->addWidget(usernameInput);
    formLayout->addWidget(passwordInput);
    formLayout->addLayout(buttonLayout);
    formLayout->addSpacing(20);
    formLayout->addSpacerItem(bottomSpacer);

    formContainer->setLayout(formLayout);
    mainLayout->addWidget(formContainer);

    this->setLayout(mainLayout);
    this->show();
    connect(loginButton, &QPushButton::clicked, this, &Registration::onLoginButtonClicked);

}

Registration::~Registration() {}

void Registration::onLoginButtonClicked() {
    if (nameInput->text().isEmpty() || surnameInput->text().isEmpty() || usernameInput->text().isEmpty() || passwordInput->text().isEmpty())
        QMessageBox::warning(this, "Registration Error", "Please fill all the gaps.");
    else {
        int res = appendUserData_(nameInput->text(), surnameInput->text(), dobInput->date().toString("dd-MM-yyyy"), usernameInput->text(), passwordInput->text());
        switch (res) {
        case -1:
            QMessageBox::warning(this, "Program Failed", "Please try again later or contact Admin.");
            break;
        case 1:
            QMessageBox::warning(this, "Username Error", "Your username is too short, please change it.");
            break;
        case 2:
            QMessageBox::warning(this, "Date Error", "The date of birthday is incorrect, please change it.");
            break;
        case 3:
            QMessageBox::warning(this, "Username Taken", "A user with same username already exists.");
            break;
        case 0:
            QMessageBox::information(this, "Success", "You are now in data base!");
            this->close();
        }
    }
}

AdminMenuWindow::AdminMenuWindow(Admin* u, QWidget* parent) : QWidget(parent), user(u) {
    this->setFixedSize(W, H);
    this->setWindowTitle("Admin Menu");
    user = u;

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(backgroundColor));
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setContentsMargins(10, 10, 10, 10);

    QString name = user ? QString::fromStdString(user->get_name()) : "Dear";
    QString surname = user ? QString::fromStdString(user->get_surname()) : "Guest";

    QLabel* welcomeLabel0 = new QLabel("Hello, " + name + " " + surname + " !", this);
    welcomeLabel0->setAlignment(Qt::AlignCenter);
    welcomeLabel0->setStyleSheet(HeaderFontStyle);
    welcomeLabel0->setFixedHeight(50);
    QLabel* welcomeLabel = new QLabel("Welcome to Admin Menu!", this);
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet(HeaderFontStyle);
    welcomeLabel->setFixedHeight(50);

    QPushButton* button1 = new QPushButton("Add a Train", this);
    QPushButton* button2 = new QPushButton("Delete a Train", this);
    QPushButton* button4 = new QPushButton("Logout", this);

    button1->setStyleSheet(DarkPinkButtonStyle);
    button2->setStyleSheet(DarkPinkButtonStyle);
    button4->setStyleSheet(GreyButtonStyle);

    mainLayout->addWidget(welcomeLabel0);
    mainLayout->addWidget(welcomeLabel);
    mainLayout->addSpacing(20);
    mainLayout->addWidget(button1);
    mainLayout->addWidget(button2);
    mainLayout->addWidget(button4);
    this->setLayout(mainLayout);

    connect(button1, &QPushButton::clicked, this, [this]() {
        this->close();
        AddTrainWindow* adminWindow = new AddTrainWindow(user, nullptr);
        adminWindow->show();
        });
       
    connect(button2, &QPushButton::clicked, this, [this]() {
        this->close();
        DeleteTrainWindow* adminWindow = new DeleteTrainWindow(user, nullptr);
        adminWindow->show();
        });
    connect(button4, &QPushButton::clicked, this, [this]() {
        this->close();
        StartWindow* adminWindow = new StartWindow(nullptr);
        adminWindow->show();
        });
}

AdminMenuWindow::~AdminMenuWindow() {};


AddTrainWindow::AddTrainWindow(Admin* user, QWidget* parent)
    : QWidget(parent), user(user) {

    this->setFixedSize(W, H);
    stations = loadStationsFromFile(station_file);
    this->setWindowTitle("Train adding Form");

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(backgroundColor));
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    trainNameInput = new QLineEdit(this);
    trainNameInput->setPlaceholderText("Train name");
    trainNameInput->setStyleSheet(BaseEditFontStyle);

    routeInput = new QLineEdit(this);
    routeInput->setPlaceholderText("Route");
    routeInput->setStyleSheet(BaseEditFontStyle);

    wagonsCountInput = new QSpinBox(this);
    wagonsCountInput->setMinimum(1);
    wagonsCountInput->setMaximum(9999);

    wagonsCountInput->setPrefix("Wagon count:   ");
    wagonsCountInput->setStyleSheet(SpinBoxEditStyle);

    placePerWagInput = new QSpinBox(this);
    placePerWagInput->setMinimum(1);
    placePerWagInput->setMaximum(9999);
    placePerWagInput->setStyleSheet(SpinBoxEditStyle);
    placePerWagInput->setPrefix("Places per wagon:  ");

    searchEdit = new QLineEdit(this);
    searchEdit->setPlaceholderText("Search station...");
    searchEdit->setStyleSheet(BaseEditFontStyle);

    dateEdit = new QDateEdit(QDate::currentDate(), this);
    dateEdit->setCalendarPopup(true);
    dateEdit->setStyleSheet(dateEditStyle);

    trainListWidget = new QListWidget(this);
    trainListWidget->setStyleSheet(ListWidgetEditStyle);
    timeEdit = new QTimeEdit(QTime::currentTime(), this);
    timeEdit->setDisplayFormat("HH:mm");
    timeEdit->setStyleSheet(timeEditStyle);

    QPushButton* dateButton = new QPushButton("Add Station", this);
    QPushButton* getBackButton = new QPushButton("Back", this);
    getBackButton->setStyleSheet(GreyButtonStyle);
    dateButton->setStyleSheet(DarkPinkButtonStyle);

    QHBoxLayout* stationDateLayout = new QHBoxLayout();
    stationDateLayout->addWidget(searchEdit);
    stationDateLayout->addWidget(dateEdit);
    stationDateLayout->addWidget(timeEdit);
    stationDateLayout->addWidget(dateButton);

    QVBoxLayout* formLayout = new QVBoxLayout();
    formLayout->addWidget(trainNameInput);
    formLayout->addWidget(routeInput);
    formLayout->addWidget(wagonsCountInput);
    formLayout->addWidget(placePerWagInput);
    formLayout->addLayout(stationDateLayout);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(formLayout);
    formLayout->addSpacing(20);
    mainLayout->addWidget(trainListWidget);
    QPushButton* createButton = new QPushButton("Add Train", this);
    createButton->setStyleSheet(DarkPinkButtonStyle);
    QBoxLayout* boxButton = new QBoxLayout(QBoxLayout::LeftToRight);
    boxButton->addWidget(getBackButton);
    boxButton->addWidget(createButton);
    mainLayout->addLayout(boxButton);
    this->setLayout(mainLayout);

    QCompleter* completer = new QCompleter(stations, this);
    completer->setCaseSensitivity(Qt::CaseInsensitive);
    searchEdit->setCompleter(completer);

    connect(getBackButton, &QPushButton::clicked, [=]() {
        this->close();
        AdminMenuWindow* adminWindow = new AdminMenuWindow(user, nullptr);
        adminWindow->show();
        });

    connect(createButton, &QPushButton::clicked, this,
        [this, user] {
            QString trainName = trainNameInput->text();
            QString route = routeInput->text();
            int wagonsCount = wagonsCountInput->value();
            int placePerWag = placePerWagInput->value();
            QStringList* stationsList = new QStringList();

            for (int i = 0; i < trainListWidget->count(); ++i) {
                QListWidgetItem* item = trainListWidget->item(i);
                *stationsList << item->text();
            }

            if (trainName.isEmpty() || route.isEmpty() || (*stationsList).isEmpty()) {
                QMessageBox::warning(this, "Error", "Enter more information");
            }
            else {
                int retVal = saveTrainData_(trainName, route, wagonsCount, placePerWag, *stationsList);
                switch (retVal) {
                case -1:
                    QMessageBox::warning(this, "Program Failed", "Please try again later or contact Developer.");
                    break;
                case 1:
                    QMessageBox::warning(this, "Date Passed", "You can't add a train if date has already passed.");
                    break;
                case 2:
                    QMessageBox::warning(this, "Train Already Exists", "The train with this name already exists, try other names.");
                    break;
                case 0:
                    QMessageBox::information(this, "Success", "Train was successfully added!");
                    this->close();
                    AdminMenuWindow* adminWindow = new AdminMenuWindow(user, nullptr);
                    adminWindow->show();
                }
            }
        });

    connect(dateButton, &QPushButton::clicked, [this]() {
        if (searchEdit->text().isEmpty()) {
            QMessageBox::warning(this, "Error", "Enter more information.");
        }
        else {
            QString trainInfo = QString("%1 %2 | %3 | %4")
                .arg(dateEdit->date().toString("dd-MM-yyyy"))
                .arg(timeEdit->time().toString("HH:mm"))
                .arg(searchEdit->text().split(" - ")[0])
                .arg(searchEdit->text().split(" - ")[1]);
            trainListWidget->addItem(trainInfo);
        }
        });

}

QStringList AddTrainWindow::loadStationsFromFile(const QString& fileName) {
    QStringList stations;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "File Error", "Could not open file: " + fileName);
        return stations;
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (!line.isEmpty()) {
            stations.append(line);
        }
    }

    file.close();
    return stations;
}

AddTrainWindow::~AddTrainWindow() {};


DeleteTrainWindow::DeleteTrainWindow(Admin* user, QWidget* parent)
    : QWidget(parent), user(user) {
    this->setFixedSize(W, H);
    this->setWindowTitle("Delete Train Form");

    QPalette palette;
    palette.setColor(QPalette::Background, QColor(backgroundColor));
    this->setAutoFillBackground(true);
    this->setPalette(palette);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* titleLabel = new QLabel("Delete Train", this);
    titleLabel->setStyleSheet(HeaderFontStyle);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    trainListWidget = new QListWidget(this);
    trainListWidget->setSelectionMode(QAbstractItemView::SingleSelection);
    trainListWidget->setStyleSheet(ListWidgetEditStyle);
    mainLayout->addWidget(trainListWidget);

    loadTrainsFromDatabase();

    QPushButton* backButton = new QPushButton("Back", this);
    QPushButton* deleteButton = new QPushButton("Delete", this);
    backButton->setStyleSheet(GreyButtonStyle);
    deleteButton->setStyleSheet(DarkPinkButtonStyle);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(backButton);
    buttonLayout->addWidget(deleteButton);
    mainLayout->addLayout(buttonLayout);

    connect(backButton, &QPushButton::clicked, [=]() {
        this->close();
        AdminMenuWindow* adminWindow = new AdminMenuWindow(user, nullptr);
        adminWindow->show();
        });

    connect(deleteButton, &QPushButton::clicked, [this]() {
        QListWidgetItem* selectedItem = trainListWidget->currentItem();
        if (!selectedItem) {
            QMessageBox::warning(this, "Error", "Please select a train to delete.");
        }
        else {
            QString trainName = selectedItem->text();
            int success = removeTrainFromDatabase(trainName);

            if (success == 0) {
                QMessageBox::information(this, "Success", "Train deleted successfully.");
                delete selectedItem;
            }
            else {
                QMessageBox::warning(this, "Error", "Failed to delete train. Please try again.");
            }
        }
        });
}

void DeleteTrainWindow::loadTrainsFromDatabase() {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    rc = sqlite3_open("trains.db", &db);
    if (rc != SQLITE_OK) {
        QMessageBox::critical(this, "Database Error", "Failed to open database.");
        return;
    }

    const char* query = "SELECT train_name FROM trains;";
    rc = sqlite3_prepare_v2(db, query, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        QMessageBox::critical(this, "Database Error", "Failed to prepare query.");
        sqlite3_close(db);
        return;
    }

    while ((rc = sqlite3_step(stmt)) == SQLITE_ROW) {
        QString trainName = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        trainListWidget->addItem(trainName);
    }

    if (rc != SQLITE_DONE) {
        QMessageBox::warning(this, "Database Error", "Error occurred while reading trains.");
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}

int DeleteTrainWindow::removeTrainFromDatabase(const QString& trainName) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    rc = sqlite3_open("trains.db", &db);
    if (rc != SQLITE_OK) {
        return -1; // Error opening database
    }

    const char* deleteTrainSQL = "DELETE FROM trains WHERE train_name = ?;";
    rc = sqlite3_prepare_v2(db, deleteTrainSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return -1; // Error preparing statement
    }

    sqlite3_bind_text(stmt, 1, trainName.toUtf8().constData(), -1, SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    if (rc != SQLITE_DONE) {
        sqlite3_close(db);
        return -1; // Error executing statement
    }

    const char* deleteStationsSQL = "DELETE FROM stations WHERE train_name = ?;";
    const char* deleteSeatsSQL = "DELETE FROM seats WHERE train_name = ?;";

    rc = sqlite3_prepare_v2(db, deleteStationsSQL, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, trainName.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    rc = sqlite3_prepare_v2(db, deleteSeatsSQL, -1, &stmt, nullptr);
    if (rc == SQLITE_OK) {
        sqlite3_bind_text(stmt, 1, trainName.toUtf8().constData(), -1, SQLITE_TRANSIENT);
        sqlite3_step(stmt);
        sqlite3_finalize(stmt);
    }

    sqlite3_close(db);
    return 0; // Success
}

DeleteTrainWindow::~DeleteTrainWindow() {}



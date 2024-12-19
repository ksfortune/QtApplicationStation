#define _CRT_SECURE_NO_WARNINGS
#include "Source.h"



QStringList loadStationsFromFile(const QString& fileName) {
    QStringList stations;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "File Error", "Could not open file: " + fileName);
        return stations;
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line;
        line = in.readLine().split(" - ")[0];
        if (!line.isEmpty()) {
            stations.append(line);
        }
    }
    file.close();
    return stations;
}

QString getCoordinatesForStation(const QString& stationName) {
    QFile file("stations.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(nullptr, "File Error", "Could not open file");
        return "";
    }
    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine().trimmed();
        QStringList parts = line.split(" - ");
        if (parts.size() == 2) {
            QString name = parts[0].trimmed();
            QString coords = parts[1].trimmed();

            if (name == stationName) {
                file.close();
                return coords;
            }
        }
    }
    file.close();
    return "";
}

int appendUserData_(const QString& name, const QString& surname, const QString& bd, const QString& usr, const QString& psw) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    rc = sqlite3_open("users.db", &db);
    if (rc) {
        return -1;
    }

    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%d-%m-%Y %H:%M:%S");
    QString today = QString::fromStdString(oss.str());

    if (usr.length() <= 2) {
        sqlite3_close(db);
        return 1; // Имя пользователя слишком короткое
    }

    if (time_between_to_times_(today.split(" ")[0], bd, 0) < 0) {
        sqlite3_close(db);
        return 2; // Некорректная дата рождения
    }

    const char* createTableSQL = R"(
        CREATE TABLE IF NOT EXISTS users (
            id INTEGER PRIMARY KEY AUTOINCREMENT,
            username TEXT UNIQUE NOT NULL,
            password_hash TEXT NOT NULL,
            name TEXT NOT NULL,
            surname TEXT NOT NULL,
            birthday TEXT NOT NULL,
            date_of_signup TEXT NOT NULL,
            role TEXT NOT NULL
        )
    )";
    rc = sqlite3_exec(db, createTableSQL, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return -1; // Ошибка создания таблицы
    }

    const char* checkUserSQL = "SELECT COUNT(*) FROM users WHERE username = ?;";
    rc = sqlite3_prepare_v2(db, checkUserSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return -1; // Ошибка подготовки запроса
    }

    sqlite3_bind_text(stmt, 1, usr.toUtf8().constData(), usr.toUtf8().size(), SQLITE_TRANSIENT);
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW && sqlite3_column_int(stmt, 0) > 0) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 3; // Пользователь с таким именем уже существует
    }
    sqlite3_finalize(stmt);
    std::string hashPassword = std::to_string(DJBHash_(psw));

    const char* insertUserSQL = R"(
        INSERT INTO users (username, password_hash, name, surname, birthday, date_of_signup, role)
        VALUES (?, ?, ?, ?, ?, ?, 'User');
    )";
    rc = sqlite3_prepare_v2(db, insertUserSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return -1; // Ошибка подготовки вставки
    }

    sqlite3_bind_text(stmt, 1, usr.toUtf8().constData(), usr.toUtf8().size(), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, hashPassword.c_str(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 3, name.toUtf8().constData(), name.toUtf8().size(), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 4, surname.toUtf8().constData(), surname.toUtf8().size(), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 5, bd.toUtf8().constData(), bd.toUtf8().size(), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 6, today.toUtf8().constData(), today.toUtf8().size(), SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return -1; // Ошибка вставки данных
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0; // Успешное добавление
}

int saveTrainData_(const QString& trainName, const QString& route, int wagonsCount, int placePerWag, const QStringList& stationsList) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    rc = sqlite3_open("trains.db", &db);
    if (rc != SQLITE_OK) {
        return -1; // Error opening database
    }

    const char* createTrainTableSQL = R"(
        CREATE TABLE IF NOT EXISTS trains (
            train_name TEXT PRIMARY KEY,
            route TEXT NOT NULL,
            wagons_count INTEGER NOT NULL,
            place_per_wag INTEGER NOT NULL
        );
    )";

    rc = sqlite3_exec(db, createTrainTableSQL, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }

    const char* insertTrainSQL = R"(
        INSERT INTO trains (train_name, route, wagons_count, place_per_wag)
        VALUES (?, ?, ?, ?);
    )";

    rc = sqlite3_prepare_v2(db, insertTrainSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }

    sqlite3_bind_text(stmt, 1, trainName.toUtf8().constData(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, route.toUtf8().constData(), -1, SQLITE_TRANSIENT);
    sqlite3_bind_int(stmt, 3, wagonsCount);
    sqlite3_bind_int(stmt, 4, placePerWag);

    rc = sqlite3_step(stmt);
    if (rc != SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return -1;
    }
    sqlite3_finalize(stmt);

    // Create stations table
    const char* createStationTableSQL = R"(
        CREATE TABLE IF NOT EXISTS stations (
            train_name TEXT,
            station_name TEXT,
            date TEXT,
            longitude REAL,
            latitude REAL,
            FOREIGN KEY (train_name) REFERENCES trains (train_name)
        );
    )";

    rc = sqlite3_exec(db, createStationTableSQL, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }

    const char* insertStationSQL = R"(
        INSERT INTO stations (train_name, station_name, date, longitude, latitude)
        VALUES (?, ?, ?, ?, ?);
    )";

    rc = sqlite3_prepare_v2(db, insertStationSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }

    for (const QString& entry : stationsList) {
        QStringList parts = entry.split(" | ");
        if (parts.size() == 3) {
            QString dateStr = parts[0].trimmed();
            QString stationName = parts[1].trimmed();
            double longitude = parts[2].split(", ")[0].trimmed().toDouble();
            double latitude = parts[2].split(", ")[1].trimmed().toDouble();

            sqlite3_bind_text(stmt, 1, trainName.toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 2, stationName.toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_text(stmt, 3, dateStr.toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_double(stmt, 4, longitude);
            sqlite3_bind_double(stmt, 5, latitude);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                return -1;
            }

            sqlite3_reset(stmt);
        }
    }
    sqlite3_finalize(stmt);

    // Create seats table
    const char* createSeatsTableSQL = R"(
        CREATE TABLE IF NOT EXISTS seats (
            seat_id INTEGER PRIMARY KEY AUTOINCREMENT,
            train_name TEXT NOT NULL,
            wagon_number INTEGER NOT NULL,
            seat_number INTEGER NOT NULL,
            user_name TEXT DEFAULT NULL,
            ticket_id TEXT DEFAULT NULL,
            FOREIGN KEY (train_name) REFERENCES trains (train_name)
        );
    )";

    rc = sqlite3_exec(db, createSeatsTableSQL, nullptr, nullptr, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }

    // Insert seat data
    const char* insertSeatSQL = R"(
        INSERT INTO seats (train_name, wagon_number, seat_number)
        VALUES (?, ?, ?);
    )";

    rc = sqlite3_prepare_v2(db, insertSeatSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return -1;
    }

    for (int wagon = 1; wagon <= wagonsCount; ++wagon) {
        for (int seat = 1; seat <= placePerWag; ++seat) {
            sqlite3_bind_text(stmt, 1, trainName.toUtf8().constData(), -1, SQLITE_TRANSIENT);
            sqlite3_bind_int(stmt, 2, wagon);
            sqlite3_bind_int(stmt, 3, seat);

            rc = sqlite3_step(stmt);
            if (rc != SQLITE_DONE) {
                sqlite3_finalize(stmt);
                sqlite3_close(db);
                return -1;
            }

            sqlite3_reset(stmt);
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 0; // Success
}


QMap<QString, std::vector<QPair<QString, QString>>> getStationsData() {
    QMap<QString, std::vector<QPair<QString, QString>>> stationsMap;

    sqlite3* db = nullptr;
    sqlite3_stmt* stmt = nullptr;
    const char* query = "SELECT station_name, train_name, date FROM stations";

    // Открыть базу данных
    if (sqlite3_open("trains.db", &db) != SQLITE_OK) {
        return stationsMap;
    }

    // Подготовить SQL-запрос
    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        sqlite3_close(db);
        return stationsMap;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        QString stationName = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0))); // Название станции
        QString trainName = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));   // Название поезда
        QString date = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));         // Дата

        // Добавить данные в карту
        stationsMap[stationName].push_back(QPair<QString, QString>(trainName, date));
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return stationsMap;
}

std::vector<QPair<QString, QString>> checkTrainAtStations(const QString& station1, const QString& station2, const QString& date, const QMap<QString, std::vector<QPair<QString, QString>>>& stationsMap)
{
    std::vector<QPair<QString, QString>> matchingTrains;
    if (!stationsMap.contains(station1) || !stationsMap.contains(station2)) {
        return matchingTrains;
    }
    const std::vector<QPair<QString, QString>>& station1Trains = stationsMap[station1];

    for (const QPair<QString, QString>& pair : station1Trains) {
        QString trainName = pair.first; 
        QString station1Date = pair.second.split(" ")[0];
      
        if (station1Date == date) {
            const std::vector<QPair<QString, QString>>& station2Trains = stationsMap[station2];
            for (const QPair<QString, QString>& pair2 : station2Trains) {
                if (pair2.first == trainName) { 
                    matchingTrains.push_back(QPair<QString, QString>(trainName, station1Date));
                    break;
                }
            }
        }
    }
    return matchingTrains;  
}

int time_between_to_times_(const QString& date1, const QString& date2, int type) {
    QStringList parts = date1.split("-");
    QStringList parts0 = date2.split("-");
    int months[] = { 31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    int day = parts[0].toInt() - parts0[0].toInt();
    int month = parts[1].toInt() - parts0[1].toInt();
    QStringList partsT1 = parts[2].split(" ");
    QStringList partsT2 = parts0[2].split(" ");
    int year = partsT1[0].toInt() - partsT2[0].toInt();
    if (type == 0) return year * 365 + month * months[month] + day;

    QStringList parts2 = partsT1[1].split(":");
    QStringList parts02 = partsT2[1].split(":");
    int hour = parts2[0].toInt() - parts02[0].toInt();
    int minutes = parts2[1].toInt() - parts02[1].toInt();
    return year * 365 * 24 * 60 + month * months[month] * 24 * 60 + day * 24 * 60 + hour * 60 + minutes;

}

Train* createTrainByName(const QString& name) {
    sqlite3* db = nullptr;
    sqlite3_stmt* stmt = nullptr;
    const char* query = "SELECT * FROM trains WHERE train_name = ? LIMIT 1;";
    Train* train = nullptr;

    if (sqlite3_open("trains.db", &db) != SQLITE_OK) {
        std::cerr << "Failed to open database: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to prepare statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return nullptr;
    }
    if (sqlite3_bind_text(stmt, 1, name.toStdString().c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Failed to bind the train name: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return nullptr;
    }

    if (sqlite3_step(stmt) == SQLITE_ROW) {
        // Create the Train object and retrieve its data from the database
        train = new Train();

        // Get the train name
        const unsigned char* retrieved_name = sqlite3_column_text(stmt, 0);
        if (retrieved_name) {
            train->set_new_name(reinterpret_cast<const char*>(retrieved_name));
        }

        // Get the train route
        const unsigned char* route = sqlite3_column_text(stmt, 1);
        if (route) {
            train->set_new_route(reinterpret_cast<const char*>(route));
        }

        // Get the number of wagons and places per wagon
        int wagons_count = sqlite3_column_int(stmt, 2);
        int place_per_wag = sqlite3_column_int(stmt, 3);

        // Set the wagons and places information
        train->set_new_wag_count(wagons_count, place_per_wag);
    }
    else {
        std::cerr << "No matching train found for: " << name.toStdString() << std::endl;
    }

    // Finalize the statement and close the database connection
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return train;
}

Train* get_train_from_db(const std::string& train_name) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    const char* sql = "SELECT train_name, route, wagons_count, place_per_wag FROM trains WHERE train_name = ?";

    if (sqlite3_open("trains.db", &db)) {
        std::cerr << "Can't open database: " << sqlite3_errmsg(db) << std::endl;
        return nullptr;
    }

    if (sqlite3_prepare_v2(db, sql, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Failed to fetch train: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return nullptr;
    }
    sqlite3_bind_text(stmt, 1, train_name.c_str(), -1, SQLITE_STATIC);

    Train* train = nullptr;
    if (sqlite3_step(stmt) == SQLITE_ROW) {
        train = new Train();
        train->set_new_name(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        train->set_new_route(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        train->set_new_wag_count(sqlite3_column_int(stmt, 2), sqlite3_column_int(stmt, 3));
    }

    // Очистка и закрытие базы данных
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return train;
}

void setScheduleToTrain(Train* train) {
    if (!train) {
        std::cerr << "Invalid train pointer." << std::endl;
        return;
    }

    sqlite3* db;
    sqlite3_stmt* stmt;
    const char* query = "SELECT date, station_name FROM stations WHERE train_name = ?";

    if (sqlite3_open("trains.db", &db) != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return;
    }
    const std::string& trainName = train->get_name();
    if (sqlite3_bind_text(stmt, 1, trainName.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Error binding parameter: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return;
    }
    
    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int columnCount = sqlite3_column_count(stmt);
        for (int i = 0; i < columnCount; ++i) {
            const char* dateValue = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0));
            const char* stationValue = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1));
            train->add_to_schedule(stationValue, dateValue);
        }
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);
}


std::vector<std::pair<int, std::string>> getSeatsInfo(const std::string& trainName, int wagonNumber) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    std::vector<std::pair<int, std::string>> seats; 

    if (sqlite3_open("trains.db", &db) != SQLITE_OK) {
        std::cerr << "Error opening database: " << sqlite3_errmsg(db) << std::endl;
        return seats;
    }

    const char* query = "SELECT seat_number, ticket_id FROM seats WHERE train_name = ? AND wagon_number = ?";

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) != SQLITE_OK) {
        std::cerr << "Error preparing statement: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_close(db);
        return seats;
    }

    if (sqlite3_bind_text(stmt, 1, trainName.c_str(), -1, SQLITE_STATIC) != SQLITE_OK) {
        std::cerr << "Error binding train name: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return seats;
    }

    if (sqlite3_bind_int(stmt, 2, wagonNumber) != SQLITE_OK) {
        std::cerr << "Error binding wagon number: " << sqlite3_errmsg(db) << std::endl;
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return seats;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        int seatNumber = sqlite3_column_int(stmt, 0);
        const char* ticketId = reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)); 

        if (ticketId != nullptr) {
            seats.push_back({ seatNumber, ticketId });
        }
        else {
            seats.push_back({ seatNumber, "Free" });
        }
    }
    
    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return seats; 
}


User* GetUserByData_(const QString& usr) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    // Открываем базу данных
    rc = sqlite3_open("users.db", &db);
    if (rc != SQLITE_OK) {
        return nullptr; // Ошибка открытия базы данных
    }

    // SQL-запрос для получения данных пользователя
    const char* getUserSQL = R"(
        SELECT name, surname, username, password_hash, birthday, role
        FROM users
        WHERE username = ?;
    )";

    rc = sqlite3_prepare_v2(db, getUserSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return nullptr; // Ошибка подготовки SQL-запроса
    }

    // Привязываем имя пользователя
    sqlite3_bind_text(stmt, 1, usr.toUtf8().constData(), usr.toUtf8().size(), SQLITE_TRANSIENT);

    // Выполняем запрос
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // Извлекаем данные пользователя из результата запроса
        QString name = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        QString surname = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 1)));
        QString username = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 2)));
        QString hashedPassword = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 3)));
        QString dateBirth = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 4)));
        QString role = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 5)));

        // Создаем пользователя
        User* user = nullptr;
        if (role == "Admin") {
            user = new Admin();
        }
        else {
            user = new User();
        }

        user->set_name(name.toStdString());
        user->set_surname(surname.toStdString());
        user->set_username(username.toStdString());
        user->set_hashed_password(hashedPassword.toStdString());
        user->set_date_birth(dateBirth.toStdString());

        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return user;
    }

    // Если пользователь не найден
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return nullptr;
}

int CheckIfAdmin_(const QString& usr, const QString& psw) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    rc = sqlite3_open("users.db", &db);
    if (rc != SQLITE_OK) {
        return -1; // Ошибка открытия базы данных
    }

    std::string hashPassword = std::to_string(DJBHash_(psw));

    // SQL-запрос для проверки пользователя
    const char* checkUserSQL = R"(
        SELECT role FROM users WHERE username = ? AND password_hash = ?;
    )";

    rc = sqlite3_prepare_v2(db, checkUserSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return -1; // Ошибка подготовки SQL-запроса
    }

    // Связывание параметров
    sqlite3_bind_text(stmt, 1, usr.toUtf8().constData(), usr.toUtf8().size(), SQLITE_TRANSIENT);
    sqlite3_bind_text(stmt, 2, hashPassword.c_str(), hashPassword.size(), SQLITE_TRANSIENT);

    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        QString role = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        sqlite3_finalize(stmt);
        sqlite3_close(db);

        if (role == "Admin") {
            return 0; // Пользователь является администратором
        }
        else {
            return 1; // Пользователь существует, но не является администратором
        }
    }

    // Если пользователь не найден
    sqlite3_finalize(stmt);
    sqlite3_close(db);
    return 1; // Пользователь не найден или данные неверны
}

int FindUserData_(const QString& usr, const QString& psw) {
    sqlite3* db;
    sqlite3_stmt* stmt;
    int rc;

    // Открываем базу данных
    rc = sqlite3_open("users.db", &db);
    if (rc != SQLITE_OK) {
        return -1; // Ошибка открытия базы данных
    }

    // SQL-запрос для поиска пользователя
    const char* findUserSQL = R"(
        SELECT password_hash FROM users WHERE username = ?;
    )";

    rc = sqlite3_prepare_v2(db, findUserSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        sqlite3_close(db);
        return -1; // Ошибка подготовки SQL-запроса
    }

    // Привязываем имя пользователя
    sqlite3_bind_text(stmt, 1, usr.toUtf8().constData(), usr.toUtf8().size(), SQLITE_TRANSIENT);

    // Выполняем запрос
    rc = sqlite3_step(stmt);
    if (rc == SQLITE_ROW) {
        // Пользователь найден, получаем хеш пароля из базы
        QString storedPassword = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));

        // Хешируем введённый пароль
        QString hashPassword = QString::number(DJBHash_(psw));

        sqlite3_finalize(stmt);
        sqlite3_close(db);

        // Сравниваем хеши
        if (storedPassword == hashPassword) {
            return 0; // Успешная аутентификация
        }
        else {
            return 2; // Неверный пароль
        }
    }
    else if (rc == SQLITE_DONE) {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return 1; // Пользователь не найден
    }
    else {
        sqlite3_finalize(stmt);
        sqlite3_close(db);
        return -1; // Ошибка выполнения SQL-запроса
    }
}

unsigned int DJBHash_(const QString& str)
{
    unsigned int hash = 5371;
    unsigned int i = 0;

    for (i = 0; i < str.length(); ++i)
        hash = ((hash << 8) + hash) + str[i].unicode();
    return hash;
}


void occupy_place(QString train_name, int wagonNumber, int seatNumber, User* user){
sqlite3_stmt* stmt;
    int rc;
    sqlite3* db;

    rc = sqlite3_open("trains.db", &db);
    if (rc != SQLITE_OK) {
        return; 
    }

    std::string user_id = user->get_username();
    const char* updateSQL = R"(
        UPDATE seats
        SET ticket_id = ?
        WHERE train_name = ?
        AND wagon_number = ?
        AND seat_number = ?
        AND ticket_id IS NULL;
    )";

    rc = sqlite3_prepare_v2(db, updateSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        std::cerr << "Ошибка подготовки запроса: " << sqlite3_errmsg(db) << std::endl;
        return;
    }

    rc = sqlite3_bind_text(stmt, 1, user_id.c_str(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_bind_text(stmt, 2, train_name.toUtf8().constData(), -1, SQLITE_TRANSIENT);
    rc = sqlite3_bind_int(stmt, 3, wagonNumber);
    rc = sqlite3_bind_int(stmt, 4, seatNumber);

    rc = sqlite3_step(stmt);
    sqlite3_finalize(stmt);

    return;
}

QStringList loadTrainsFromFile() {
    QStringList trainNames;
    sqlite3* db;
    sqlite3_stmt* stmt;

    int rc = sqlite3_open("trains.db", &db);
    if (rc != SQLITE_OK) {
        qWarning("Failed to open database: %s", sqlite3_errmsg(db));
        return trainNames;
    }

    const char* selectTrainNamesSQL = R"(
        SELECT train_name FROM trains;
    )";

    rc = sqlite3_prepare_v2(db, selectTrainNamesSQL, -1, &stmt, nullptr);
    if (rc != SQLITE_OK) {
        qWarning("Failed to prepare query: %s", sqlite3_errmsg(db));
        sqlite3_close(db);
        return trainNames;
    }

    while (sqlite3_step(stmt) == SQLITE_ROW) {
        QString trainName = QString::fromUtf8(reinterpret_cast<const char*>(sqlite3_column_text(stmt, 0)));
        trainNames.append(trainName);
    }

    sqlite3_finalize(stmt);
    sqlite3_close(db);

    return trainNames;
}



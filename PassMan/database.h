/*
 * Description: Definition of the Database class.
 *              Manages internal representation and manipulation of user data.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#ifndef DATABASE_H
#define DATABASE_H

//#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QList>
#include "entry.h"
#include <QDebug> //TESTING

class Database : public QObject
{
    Q_OBJECT

    public:
        Database(const QString& version);
        ~Database();

        void read(const QJsonObject& json); // Extracts entry information from JSON object
        void write(QJsonObject& json);  // Serialize entry information to JSON object
        QString name(int e);    // Retrieve information:
        QString username(int e);
        QString password(int e);
        QString notes(int e);
        void setName(const QString& n, int e);  // Set information:
        void setUsername(const QString& un, int e);
        void setPassword(const QString& pw, int e);
        void setNotes(const QString& nt, int e);
        void addNew();  // Append new entry
        void remove(int e); // Remove entry
        void clear();   // Clear all entries
        int size(); // Return number of entries held

    signals:
        void readNewData();
        void writeNewData();

    private:
        static const QString NEW_ENTRY_NAME, NAME_KEY, USERNAME_KEY, PASSWORD_KEY, NOTES_KEY, ENTRIES_KEY, VERSION_KEY;  // Common values
        QString version;
        QList<Entry*> entries;
        int newEntryCount;
};

#endif // DATABASE_H

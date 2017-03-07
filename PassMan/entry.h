/*
 * Description: Definition of the Entry class.  Holds user data for a single entry from the database.
 *              Provides JSON interpretation methods for loading from storage.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#ifndef ENTRY_H
#define ENTRY_H

#include <QString>
#include <QJsonObject>

class Entry
{
    public:
        Entry(const QString& name, const QString& username, const QString& password, const QString& notes);
        ~Entry();

        void read(const QJsonObject& json); // Read data into representation from JSON
        void write(QJsonObject& json) const;    // Store user data in JSON
        QString name() const;   // Retrieve information:
        QString username() const;
        QString password() const;
        QString notes() const;
        void setName(const QString& name);    // Set information:
        void setUsername(const QString& username);
        void setPassword(const QString& password);
        void setNotes(const QString& notes);

    private:
        QString entryName;
        QString entryUsername;
        QString entryPassword;
        QString entryNotes;
};

#endif // ENTRY_H

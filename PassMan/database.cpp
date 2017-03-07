/*
 * Description: Implementation of the Database class.
 *              Manages internal representation and manipulation of user data.
 *              Export functionality is provided for JSON storage.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#include "database.h"

const QString Database::NEW_ENTRY_NAME = "New Entry "; // Common values
const QString Database::NAME_KEY = "name";
const QString Database::USERNAME_KEY = "username";
const QString Database::PASSWORD_KEY = "password";
const QString Database::NOTES_KEY = "notes";
const QString Database::ENTRIES_KEY = "entries";
const QString Database::VERSION_KEY = "version";

Database::Database(const QString& version)
{
    this->version = version;
    newEntryCount = 1;
}

Database::~Database() { }

void Database::read(const QJsonObject &json) // Extracts entry information from JSON object
{
    entries.clear();
    QJsonArray entryArray = json.value(ENTRIES_KEY).toArray();
    for (int i = 0; i < entryArray.size(); i++)
    {
        QJsonObject entryObj = entryArray.at(i).toObject();
        entries.append(new Entry(entryObj.value(NAME_KEY).toString(), entryObj.value(USERNAME_KEY).toString(),
                             entryObj.value(PASSWORD_KEY).toString(), entryObj.value(NOTES_KEY).toString()));
    }
    version = json.value(VERSION_KEY).toString();
    emit readNewData(); // Notify watchers that database is loaded
}

void Database::write(QJsonObject& json) // Serialize entry information to JSON object
{
    QJsonArray entryArray;
    foreach (Entry* e, entries)
    {
        QJsonObject entryObj;
        e->write(entryObj);
        entryArray.append(entryObj);
    }
    json.insert(ENTRIES_KEY, entryArray);
    json.insert(VERSION_KEY, version);
    emit writeNewData();    // Notify watchers that database saved
}

QString Database::name(int e) { return (entries.size() > e && e >= 0) ? entries.at(e)->name() : ""; } // Retrieve information:

QString Database::username(int e) { return (entries.size() > e && e >= 0) ? entries.at(e)->username() : ""; }

QString Database::password(int e) { return (entries.size() > e && e >= 0) ? entries.at(e)->password() : ""; }

QString Database::notes(int e) { return (entries.size() > e && e >= 0) ? entries.at(e)->notes() : ""; }

void Database::setName(const QString &n, int e) { if (entries.size() > e && e >= 0) entries.at(e)->setName(n); } // Set information:

void Database::setUsername(const QString &un, int e) { if (entries.size() > e && e >= 0) entries.at(e)->setUsername(un); }

void Database::setPassword(const QString &pw, int e) { if (entries.size() > e && e >= 0) entries.at(e)->setPassword(pw); }

void Database::setNotes(const QString &nt, int e) { if (entries.size() > e && e >= 0) entries.at(e)->setNotes(nt); }

void Database::addNew() // Append new entry
{
    entries.append(new Entry(QString(NEW_ENTRY_NAME).append(QString::number(newEntryCount)), "", "", ""));
    newEntryCount++;
}

void Database::remove(int e)    // Remove entry
{
    if (entries.size() > e && e >= 0)
    {
        delete entries.at(e);
        entries.removeAt(e);
    }
}

void Database::clear()  // Clear all entries
{
    newEntryCount = 1;
    entries.clear();
}

int Database::size() { return entries.size(); } // Return number of entries held

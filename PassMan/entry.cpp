/*
 * Description: Implementation of the Entry class.  Holds user data for a single entry from the database.
 *              Provides JSON interpretation methods for loading from storage.
 * Author:      Adam Coffee
 * Licensed under the three-clause BSD license, found in the LICENSE file.
 */

#include "entry.h"

Entry::Entry(const QString& name, const QString& username, const QString& password, const QString& notes)
{
    entryName = name;
    entryUsername = username;
    entryPassword = password;
    entryNotes = notes;
}

Entry::~Entry() { }

void Entry::read(const QJsonObject& json)
{
    entryName = json.value("name").toString();
    entryUsername = json.value("username").toString();
    entryPassword = json.value("password").toString();
    entryNotes = json.value("notes").toString();
}

void Entry::write(QJsonObject& json) const
{
    json.insert("name", entryName);
    json.insert("username", entryUsername);
    json.insert("password", entryPassword);
    json.insert("notes", entryNotes);

}

QString Entry::name() const { return entryName; }   // Retrieve information:

QString Entry::username() const { return entryUsername; }

QString Entry::password() const { return entryPassword; }

QString Entry::notes() const { return entryNotes; }

void Entry::setName(const QString& name) { entryName = name; }  // Set information:

void Entry::setUsername(const QString& username) { entryUsername = username; }

void Entry::setPassword(const QString& password) { entryPassword = password; }

void Entry::setNotes(const QString& notes) { entryNotes = notes; }

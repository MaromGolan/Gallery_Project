#include "DatabaseAccess.h"
#include "ItemNotFoundException.h"
#include "SqlException.h"
#include <stdio.h>
#include <iostream>
#include <sstream>

std::list<Album> DatabaseAccess::albums;
std::list<User> DatabaseAccess::users;
std::list<Picture> DatabaseAccess::pics;


#define ALBUMS_TABLE "ALBUMS"
#define PICS_TABLE "PICTURES"
#define USERS_TABLE "USERS"
#define TAGS_TABLE "TAGS"
#define NAME_COLUMN  "NAME"
#define ID_COLUMN "ID"
#define ALBUM_ID_COLUMN "ALBUM_ID"
#define USER_ID_COLUMN "USER_ID"
#define PICTURE_ID_COLUMN "PICTURE_ID"
#define LOCATION_COLUMN "LOCATION"
#define CREATION_DATE_COLUMN "CREATION_DATE"



int DatabaseAccess::albumsSql(void* data, int argc, char** argv, char** colname)
{
	auto sqlalbums = static_cast<std::list<Album>*>(data);
	Album album;
	int i = 0;
	for (i = 0; i < argc; ++i)
	{
		if (std::string(colname[i]) == ID_COLUMN)
		{
			album.setOwner(atoi(argv[i]));
		}
		else if (std::string(colname[i]) == NAME_COLUMN)
		{
			album.setName(argv[i]);
		}
		else if (std::string(colname[i]) == CREATION_DATE_COLUMN)
		{
			album.setCreationDate(argv[i]);
		}
		else if (std::string(colname[i]) == USER_ID_COLUMN)
		{
			album.setOwner(atoi(argv[i]));
		}
		sqlalbums->push_back(album);
	}
	return 0;
}


int DatabaseAccess::idbyname(void* data, int argc, char** argv, char** colname)
{
	int* id = static_cast<int*>(data);
	for (int i = 0; i < argc; i++)
	{
		if (std::string(colname[i]) == ID_COLUMN)
			*id = atoi(argv[i]);
	}
	return 0;
}

int DatabaseAccess::picsSql(void* data, int argc, char** argv, char** colname)
{
	auto pic = static_cast<Picture*>(data);
	for (int i = 0; i < argc; i++)
	{
		if (colname[i] == "NAME")
			pic->setName(argv[i]);
		else if (colname[i] == "LOCATION")
			pic->setPath(argv[i]);
		else if (colname[i] == "CREATION_DATE")
			pic->setCreationDate(argv[i]);
	}
	return 0;
}

int DatabaseAccess::sqlusers(void* data, int argc, char** argv, char** colname)
{
	auto sqlusers = static_cast<std::list<User>*>(data);
	std::string name;
	int i = 0, id = -1;
	for (i = 0; i < argc; i++)
	{
		if (std::string(colname[i]) == ID_COLUMN)
		{
			id = atoi(argv[i]);
		}
		else if (std::string(colname[i]) == NAME_COLUMN)
		{
			name = argv[i];
		}
		User u(id, name);
		sqlusers->push_back(u);
	}
	return 0;
}








bool DatabaseAccess::open()
{
	if (sqlite3_open(DBName, &Db) == SQLITE_OK)
	{
		return true;
	}
	return false;
}

void DatabaseAccess::close()
{
	sqlite3_close(Db);
	Db = nullptr;
}

void DatabaseAccess::tagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	char* errMessage = nullptr;
	try
	{
		int res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		int* id;
		std::stringstream q;
		q << "SELECT " << NAME_COLUMN << " FROM " << PICS_TABLE << " WHERE " << NAME_COLUMN << " = '" << pictureName << "';";
		res = sqlite3_exec(Db, q.str().c_str(), idbyname, &id, &errMessage);
		if (res != SQLITE_OK)
			throw ItemNotFoundException(pictureName, userId);

		q << "INSERT INTO" << TAGS_TABLE << "(" << USER_ID_COLUMN << "," << PICTURE_ID_COLUMN << ") VALUES( '" << userId << "' , '" << *id << "');";
		res = sqlite3_exec(Db, q.str().c_str(), nullptr, nullptr, &errMessage);
		if (res != SQLITE_OK)
			throw MyException("CANNOT INSERT INTO");

		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);

	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

void DatabaseAccess::unTagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId)
{
	char* errMessage = nullptr;
	try
	{
		int res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		int *id;
		std::stringstream q;
		q << "SELECT " << NAME_COLUMN << " FROM " << PICS_TABLE << " WHERE " << NAME_COLUMN << " = '" << pictureName << "';";
		res = sqlite3_exec(Db, q.str().c_str(), idbyname, &id, &errMessage);
		if (res != SQLITE_OK)
			throw ItemNotFoundException(pictureName, userId);

		q << "DELETE FROM " << TAGS_TABLE << " WHERE " << USER_ID_COLUMN << " = '" << userId << "' AND " << PICTURE_ID_COLUMN << " = '"<< *id << "';";
		res = sqlite3_exec(Db, q.str().c_str(), nullptr, nullptr, &errMessage);
		if (res != SQLITE_OK)
			throw MyException("CANNOT DELETE TAG ");

		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);

	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}

}

void DatabaseAccess::clear()
{
	albums.clear();
	users.clear();
	pics.clear();
}

void DatabaseAccess::createUser(User& user)
{
	User u = user;
	try
	{
		int res = sqlite3_exec(Db, "BEGIN; ", nullptr, nullptr, nullptr);
		std::stringstream q;
		q << "INSERT INTO" << USERS_TABLE << "VALUES('" << u.getId() << "', '" << u.getName() << "');";
		res = sqlite3_exec(Db, q.str().c_str(), nullptr, nullptr, nullptr);
		if (res != SQLITE_OK)
			throw MyException("CANNOT ADD USER");
		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

void DatabaseAccess::deleteUser(const User& user)
{
	User u = user;
	try
	{
		int res = sqlite3_exec(Db, "BEGIN; ", nullptr, nullptr, nullptr);
		std::stringstream q;
		q << "DELETE FROM" << USERS_TABLE << "WHERE" << ID_COLUMN << " = '"<<  u.getId() << "' AND "<< NAME_COLUMN << " = '" << u.getName() << "';";
		res = sqlite3_exec(Db, q.str().c_str(), nullptr, nullptr, nullptr);
		if (res != SQLITE_OK)
			throw MyException("CANNOT DELETE USER");
		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

void DatabaseAccess::closeAlbum(Album& album)
{
//this was left empty in MemoryAccess
}

void DatabaseAccess::deleteAlbum(const std::string& albumName, int userId)
{
	std::stringstream q;
	try
	{
		q << "DELETE FROM " << ALBUMS_TABLE << " WHERE " <<NAME_COLUMN << " = '" << albumName << "' AND " << USER_ID_COLUMN << " = '" << userId << "';";
		std::string sql = q.str();
		int res = sqlite3_exec(Db, sql.c_str(), nullptr, nullptr, nullptr);
		if (res != SQLITE_OK)
			throw ItemNotFoundException(albumName,userId);
		eraseFunc(albumName);
		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

void DatabaseAccess::eraseFunc(std::string albumName, int userId)
{
	if(userId == -12)
		for (auto it = albums.begin(); it != albums.end(); it++)
		{
			if (it->getName() == albumName)
				albums.erase(it);
		}
	else
		for (auto it = users.begin(); it != users.end(); it++)
		{
			if (it->getId() == userId)
				users.erase(it);
		}
}


const std::list<Album> DatabaseAccess::getAlbums()
{
	char* errMessage = nullptr;
	std::list<Album> sqla;
	try
	{
		int res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		std::stringstream q;
		q << "SELECT * FROM" << ALBUMS_TABLE << ";";
		res = sqlite3_exec(Db, q.str().c_str(), albumsSql, &sqla, &errMessage);
		if (res != SQLITE_OK)
			throw MyException("CANNOT GET ALBUMS");

		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		for (Album& it : sqla)
		{
			albums.push_back(it);
		}
		return sqla;
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}


const std::list<Album> DatabaseAccess::getAlbumsOfUser(const User& user)
{
	char* errMessage = nullptr;
	try
	{
		std::list<Album> sqla;
		int res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		std::stringstream q;
		q << "SELECT * FROM " << ALBUMS_TABLE << " WHERE " << USER_ID_COLUMN << " = '" << user.getId() << "';";
		res = sqlite3_exec(Db, q.str().c_str(), albumsSql, &sqla, &errMessage);
		if (res != SQLITE_OK)
			throw MyException("CANNOT GET THE ALBUMS");
		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		for (Album& it : sqla)
		{
			albums.push_back(it);
		}
		return sqla;
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

void DatabaseAccess::createAlbum(Album& album)
{
	try
	{
		int res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		std::stringstream q;
		q << "INSERT INTO " << ALBUMS_TABLE << "(NAME, CREATION_DATE, USER_ID) VALUES('" << album.getName() << "' , '" << album.getCreationDate() << "' , '" << album.getOwnerId() << "');";
		res = sqlite3_exec(Db, q.str().c_str(), nullptr, nullptr, nullptr);
		if (res != SQLITE_OK)
			throw MyException("CANNOT CREATE ALBUM");
		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}


bool DatabaseAccess::doesAlbumExists(const std::string& albumName, int userId)
{
	try
	{
		int res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		std::stringstream q;
		q << "SELECT NAME FROM ALBUMS WHERE EXISTS(SELECT ID FROM ALBUMS WHERE ID = '" << userId << "');";
		res = sqlite3_exec(Db, q.str().c_str(), nullptr, nullptr, nullptr);
		if (res != SQLITE_OK)
			throw MyException("CANNOT ACCESS ALBUMS");
		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		return bool(res);
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
	
}

bool DatabaseAccess::doesUserExists(int userId)
{
	try
	{
		char* errMessage = nullptr;
		int *id;
		int res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		std::stringstream q;
		q << "SELECT ID FROM USERS WHERE EXISTS(SELECT ID FROM USERS WHERE ID = '" << userId << "');";
		res = sqlite3_exec(Db, q.str().c_str(), nullptr, nullptr, nullptr);
		if (res != SQLITE_OK)
			throw MyException("CANNOT ACCESS USERS");
		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		return bool(res);
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

Album DatabaseAccess::openAlbum(const std::string& albumName)
{	
	std::list<Album> sqla = getAlbums();
	for (Album& it : sqla)
	{
		if (albumName == it.getName())
		{
			return it;
		}
	}
}

void DatabaseAccess::printAlbums()
{
	std::list<Album> sqla = getAlbums();
	for (Album& it : sqla)
	{
		std::cout << "NAME: " << it.getName() << " CREATION DATE: " << it.getCreationDate() << " OWNER ID: " << it.getOwnerId() << std::endl;
	}
}

void DatabaseAccess::addPictureToAlbumByName(const std::string& albumName, const Picture& picture)
{
	try
	{
		char* errMessage = nullptr;
		int id = -1 , res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		std::stringstream q;
		/*
		q << "SELECT " << NAME_COLUMN << " FROM " << ALBUMS_TABLE << " WHERE " << NAME_COLUMN << " = '" << albumName << "';";
		res = sqlite3_exec(Db, q.str().c_str(), idbyname, &id, &errMessage);
		if (res != SQLITE_OK)
			throw MyException("CANNOT GET ALBUM ID FROM NAME");
			*/
		q << "INSERT INTO PICTURES(ALBUM_ID) SELECT ID FROM ALBUMS WHERE NAME = '" << albumName << "';";
		res = sqlite3_exec(Db, q.str().c_str(), nullptr, nullptr, nullptr);
		if (res != SQLITE_OK)
			throw MyException("CANNOT ADD ALBUM ID TO PIC");
		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

void DatabaseAccess::removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName)
{
	try
	{
		char* errMessage = nullptr;
		int *id, res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		std::stringstream q;
		
		q << "SELECT " << NAME_COLUMN << " FROM " << ALBUMS_TABLE << " WHERE " << NAME_COLUMN << " = '" << albumName << "';";
		res = sqlite3_exec(Db, q.str().c_str(), idbyname, &id, &errMessage);
		if (res != SQLITE_OK)
			throw MyException("CANNOT GET ALBUM ID FROM NAME");
			
		q << "DELETE FROM PICTURES WHERE ALBUM_ID = '" << *id << "' AND NAME = '" << pictureName << ";";
		res = sqlite3_exec(Db, q.str().c_str(), nullptr, nullptr, nullptr);
		if (res != SQLITE_OK)
			throw MyException("CANNOT DELETE ALBUM ID FROM PICTURE ");
		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

void DatabaseAccess::printUsers()
{
	try
	{
		std::list<User> u;
		char* errMessage = nullptr;
		int res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		std::stringstream q;
		q << "SELECT * FROM USERS;";
		res = sqlite3_exec(Db, q.str().c_str(),sqlusers,&u, &errMessage);
		if (res != SQLITE_OK)
			throw MyException("CANNOT ADD ALBUM ID TO PIC");
		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		for (User& it : u)
		{
			std::cout << "ID:" << it.getId() << " Name: " << it.getName() << std::endl;
		}
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

User DatabaseAccess::getUser(int userId)
{
	try
	{
		char* errMessage = nullptr;
		std::list<User> u;
		int res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		std::stringstream q;
		q << "SELECT * FROM USERS WHERE ID = '" << userId << "';";
		res = sqlite3_exec(Db, q.str().c_str(), sqlusers, &u, &errMessage);
		if (res != SQLITE_OK)
			throw MyException("CANNOT GET USER");
		res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		for (User& it : u)
			return it;
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

int DatabaseAccess::countAlbumsOwnedOfUser(const User& user)
{
	try
	{
		int count = 0, res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		std::stringstream q;
		q << "SELECT COUNT(USER_ID) FROM ALBUMS WHERE USER_ID = '" << user.getId() << "';";
		res = sqlite3_exec(Db, q.str().c_str(), nullptr, nullptr, nullptr);
		if (res != SQLITE_OK)
			throw MyException("CANNOT ACCESS ALBUMS");
		count = res;
		res = sqlite3_exec(Db, "END;", nullptr, nullptr, nullptr);
		return count;
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

int DatabaseAccess::countAlbumsTaggedOfUser(const User& user)
{
	try
	{
		int count = 0, res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		std::stringstream q;
		q << "SELECT COUNT(USER_ID) FROM TAGS WHERE USER_ID = '" << user.getId() << "';";
		res = sqlite3_exec(Db, q.str().c_str(), nullptr, nullptr, nullptr);
		if (res != SQLITE_OK)
			throw MyException("CANNOT ACCESS ALBUMS");
		count = res;
		res = sqlite3_exec(Db, "END;", nullptr, nullptr, nullptr);
		return count;
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

int DatabaseAccess::countTagsOfUser(const User& user)
{
//THIS IS LITERALLY THE SAME AS countAlbumsTaggedOfUser BECAUSE I DONT HAVE THE ALBUM YOU WANT ME TO COUNT
	return 0;
}

float DatabaseAccess::averageTagsPerAlbumOfUser(const User& user)
{
	try
	{
		int count = 0, amountAlbums = 0;
		std::stringstream q;
		std::list<Album> sqla = getAlbumsOfUser(user);
		amountAlbums = sqla.size();
		count = countAlbumsTaggedOfUser(user);
		return count/amountAlbums;
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

User DatabaseAccess::getTopTaggedUser()
{
	try
	{
		int res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		std::stringstream q;
		q << " SELECT MAX() FROM table_name WHERE condition;";
		//NOT DONE

	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}
}

Picture DatabaseAccess::getTopTaggedPicture()
{
	//return Picture();
	return Picture(-1, "IDONTKNOWWHATTHISDOWHYHOW");
}

std::list<Picture> DatabaseAccess::getTaggedPicturesOfUser(const User& user)
{
	try
	{
		std::list<int> i;
		char* errMessage = nullptr;
		std::list<Picture> sqlp;
		Picture* pic;
		int res = sqlite3_exec(Db, "BEGIN;", nullptr, nullptr, nullptr);
		std::stringstream q;
		q << "SELECT PICTURE_ID FROM TAGS WHERE USER_ID = '" << user.getId() << "';";
		res = sqlite3_exec(Db, q.str().c_str(), nullptr, &i, &errMessage);
		if (res != SQLITE_OK)
			throw MyException("CANNOT ACCESS TAGS");
		for (int& j : i)
		{
			q << "SELECT * FROM PICTURES WHERE PICTURE_ID = '" << j << "';";
			res = sqlite3_exec(Db, q.str().c_str(), picsSql, &pic, &errMessage);
			sqlp.push_back(*pic);
			pics.push_back(*pic);
		}
		res = sqlite3_exec(Db, "END;", nullptr, nullptr, nullptr);
		return sqlp;
	}
	catch (const SqlException& e)
	{
		int res = sqlite3_exec(Db, "END; ", nullptr, nullptr, nullptr);
		throw e;
	}

}

#pragma once

#include "Album.h"
#include "IDataAccess.h"
#include "sqlite3.h"
#include <vector>

class DatabaseAccess : public IDataAccess
{

public:
	static std::list<Album> albums;
	static std::list<User> users;
	static std::list<Picture> pics;
	static int albumsSql(void* data, int argc, char** argv, char** columnName);
	static int idbyname(void* data, int argc, char** argv, char** colname);
	static int picsSql(void* data, int argc, char** argv, char** colname);
	static int sqlusers(void* data, int argc, char** argv, char** colname);
	static int Counter(void* data, int argc, char** argv, char** colname);

	bool open() override;
	void close() override;
	void tagUserInPicture(const std::string& albumName,const std::string& pictureName,int userId);
	void unTagUserInPicture(const std::string& albumName, const std::string& pictureName, int userId);
	void clear() override;
	void createUser(User& user);
	void deleteUser(const User& user);
	void closeAlbum(Album& album);
	void deleteAlbum(const std::string& albumName, int userId);
	void eraseFunc(std::string albumName = "DEFAULT", int userId = -12);
	const std::list<Album> getAlbums() override;
	const std::list<Album> getAlbumsOfUser(const User& user) override;
	void createAlbum(Album& album);
	bool doesAlbumExists(const std::string& albumName,int userId) override;
	bool doesUserExists(int userId) override;
	Album openAlbum(const std::string& albumName)override;
	void printAlbums()override;
	void addPictureToAlbumByName(const std::string& albumName, const Picture& picture) override;
	void removePictureFromAlbumByName(const std::string& albumName, const std::string& pictureName) override;
	void printUsers();
	User getUser(int userId) override;
	int countAlbumsOwnedOfUser(const User& user) override;
	int countAlbumsTaggedOfUser(const User& user) override;
	int countTagsOfUser(const User& user)override;
	float averageTagsPerAlbumOfUser(const User& user) override;
	User getTopTaggedUser() override;
	Picture getTopTaggedPicture() override;
	std::list<Picture> getTaggedPicturesOfUser(const User& user) override;

	bool uExists(std::list<User> u,int userId);
	bool aExists(std::list<Album> a,std::string albumName);


private:
	const char* DBName = "galleryDB.sqlite";
	sqlite3* Db;

		

};


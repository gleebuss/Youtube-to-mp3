#ifndef SASHAPROJECT_CONVERTER_H
#define SASHAPROJECT_CONVERTER_H

#include <string>
#include <cpr/session.h>

/// \class Converter
/// \brief Класс, отвечающий за конвертацию файлов в формат MP3.
///
/// Данный класс предоставляет функционал для загрузки ролика по ссылке и его
/// последующей конвертации в формат MP3.
class Converter {
private:
    ///Ссылка на видео
    /// \var urlMusic
    std::string urlMusic;

    ///Сессия HTTP для выполнения запросов.
    /// \var session
    cpr::Session session;

    ///Название файла
    /// \var nameFile
    std::string nameFile;

    ///Токен для отправки запроса
    /// \var token
    std::string token;

    std::string get_time();
    std::string get_video_id(const std::string& url);

public:
    Converter(const std::string &urlMusic);
    std::string get_url_mp3();
    std::string get_file();
    std::string get_nameFile();
};

std::string trim(const std::string& str);

std::string unescapeString(const std::string& str);


#endif //SASHAPROJECT_CONVERTER_H

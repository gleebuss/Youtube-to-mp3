#include "Converter.h"

#include <string>
#include <cpr/session.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

/// Создает экземпляр класса Converter и инициализирует необходимые данные.
/// \param urlMusic - ссылка на видео.
Converter::Converter(const std::string &urlMusic) {
    this->urlMusic = urlMusic;
    this->session.SetHeader(cpr::Header{
            {"User-Agent",       "Mozilla/5.0 (Macintosh; Intel Mac OS X 10.15; rv:109.0) Gecko/20100101 Firefox/113.0"},
            {"Accept",           "*/*"},
            {"Accept-Language",  "ru-RU,ru;q=0.8,en-US;q=0.5,en;q=0.3"},
            {"X-Requested-With", "XMLHttpRequest"},
            {"Connection",       "keep-alive"},
            {"Referer",          "https://mp3-convert.org/youtube-to-mp3/"},
    });
    this->session.SetUrl(cpr::Url("https://mp3-convert.org/youtube-to-mp3/"));
    this->session.Get();

    auto tmp = session.Get().text;
    std::string searchString = "https://mp3-convert.org/get.php?r=";
    size_t foundIndex = tmp.find(searchString);
    if (foundIndex != std::string::npos) {
        size_t indexAfterString = foundIndex + searchString.length();
        size_t ampersandIndex = tmp.find('&', indexAfterString);
        if (ampersandIndex != std::string::npos)
            this->token = tmp.substr(indexAfterString, ampersandIndex - indexAfterString);
    }
};

///
/// \return Функия возвращает строку url на mp3 файл.
std::string Converter::get_url_mp3() {
    do {
        std::string temp = "https://mp3-convert.org/get.php?r=" + token + "&id=" + get_video_id(this->urlMusic) + "&t=" +
                           this->get_time();
        session.SetUrl(cpr::Url(temp));
        cpr::Response response = session.Get();

        if (response.status_code == 200) {
            try {
                json response_json = json::parse(response.text);

                if (response_json.contains("download_url") && response_json["download_url"] != "") {
                    this->nameFile = trim(response_json["title"]);
                    return response_json["download_url"];
                }
            } catch (const json::exception &e) {
                // Handle JSON parsing error
                std::cerr << "JSON parsing error: " << e.what() << std::endl;
            }
        }
    } while (true);
};

///
/// \param url - ссылка на видео.
/// \return Функция возвращает строку с id видео.
std::string Converter::get_video_id(const std::string &url) {
    std::string videoID;
    size_t lastSlashPos = url.find_last_of("/");
    size_t questPos = url.find_last_of("?");
    if (lastSlashPos != std::string::npos) {
        videoID = url.substr(lastSlashPos + 1, questPos - lastSlashPos - 1);
    }

    return videoID;
};

///
/// \return Функция возвращает количество количество миллисекунд, прошедших с начала эпохи.
std::string Converter::get_time() {
    auto now = std::chrono::system_clock::now();
    auto timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    std::string timestamp_str = std::to_string(timestamp);
    return timestamp_str;
}

///
/// \return Функция возвращает содержимое файла в виде строки.
std::string Converter::get_file() {
    this->session.SetUrl(cpr::Url(this->get_url_mp3()));
    cpr::Response response = session.Get();
    if (response.status_code == 200) {
        std::string fileData(response.text.begin(), response.text.end());
        return fileData;
    };
};

///
/// \return Функуия возвращает название файла.
std::string Converter::get_nameFile() {
    return this->nameFile;
}

///
/// \param str - строка в которой есть лишние пробелы в начале и в конце.
/// \return Функция возвращает строку без лишних пробелов.
std::string trim(const std::string &str) {
    size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos)
        return "";

    size_t last = str.find_last_not_of(' ');
    return str.substr(first, last - first + 1);
}


///
/// \param str - строка в которой есть '\\n', которые не преобразуются в перенос строки.
/// \return Функуия возвращает строку с переносами строки.
std::string unescapeString(const std::string &str) {
    std::string result;
    for (std::size_t i = 0; i < str.length(); ++i) {
        if (str[i] == '\\' && i + 1 < str.length()) {
            switch (str[i + 1]) {
                case 'n':
                    result += '\n';
                    break;
                default:
                    result += str[i];
                    break;
            }
            ++i;
        } else {
            result += str[i];
        }
    }
    return result;
}
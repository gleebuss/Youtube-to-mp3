#include <iostream>
#include <tgbot/tgbot.h>
#include "../include/dotenv.h"
#include "Converter.h"

/// @mainpage Описание
/// Телеграм-бот, который позволяет конвертировать ролик с YouTube в mp3 формат.



///
/// @brief Функция main, запускающая бота и обрабатывающая команды.
/// @return Код возврата программы.
///
/// Данная функция является точкой входа в программу. Она инициализирует бота,
/// устанавливает соединение с необходимыми сервисами и запускает бесконечный цикл
/// для обработки команд от пользователей.
///
int main() {

    //получаем переменные окружения из файла
    dotenv::init("../res/.env");

    TgBot::Bot bot(dotenv::getenv("TOKEN"));

    //обработка команды /start
    bot.getEvents().onCommand("start", [&bot](TgBot::Message::Ptr message) {

        //создание клавиатуры
        TgBot::ReplyKeyboardMarkup::Ptr keyboard(new TgBot::ReplyKeyboardMarkup);
        keyboard->resizeKeyboard = true;

        std::vector<TgBot::KeyboardButton::Ptr> buttonsRow;

        TgBot::KeyboardButton::Ptr button1(new TgBot::KeyboardButton);
        TgBot::KeyboardButton::Ptr button2(new TgBot::KeyboardButton);

        button1->text = "/about";
        button2->text = "/help";

        buttonsRow.push_back(button1);
        buttonsRow.push_back(button2);

        keyboard->keyboard.push_back(buttonsRow);

        //ответ на команду /start
        bot.getApi().sendMessage(message->chat->id, unescapeString(std::getenv("START")), false, 0, keyboard);

    });

    //обработка команды /about
    bot.getEvents().onCommand("about", [&bot](TgBot::Message::Ptr message) {

        //ответ на команду /about
        bot.getApi().sendMessage(message->chat->id, dotenv::getenv("ABOUT"));
    });

    //обработка команды /help
    bot.getEvents().onCommand("help", [&bot](TgBot::Message::Ptr message) {

        //ответ на команду /help
        bot.getApi().sendMessage(message->chat->id,  dotenv::getenv("HELP"));
    });

    //обработка сообщений от клиента, бот ожидает ссылку на видео
    bot.getEvents().onAnyMessage([&bot](TgBot::Message::Ptr message) {
        if (StringTools::startsWith(message->text, "/start") || StringTools::startsWith(message->text, "/about") || StringTools::startsWith(message->text, "/help")) {
            return;
        }
        else if (message->text.find("youtu.be") != std::string::npos) {
            //ответ, если пришла ссылка
            bot.getApi().sendMessage(message->chat->id, dotenv::getenv("GENERATE"));


            //получаем mp3 файл
            std::string youtubeLink = message->text;
            Converter conv(youtubeLink);
            std::string tmp = conv.get_url_mp3();


            //отправка mp3 файла
            try {
                auto input_file(std::make_shared<TgBot::InputFile>());
                input_file->data = conv.get_file();
                input_file->mimeType = "audio/mpeg";
                input_file->fileName = conv.get_nameFile();
                bot.getApi().sendAudio(message->chat->id, input_file);
            } catch (const TgBot::TgException& e) {
                bot.getApi().sendMessage(message->chat->id, dotenv::getenv("ERROR"));
                std::cerr << "Ошибка при отправке аудиофайла: " << e.what() << std::endl;
            }

        } else {
            //ответ на если пришла невалидная ссылка
            bot.getApi().sendMessage(message->chat->id, dotenv::getenv("VALID"));
        }
    });

    //запуск бота
    try {
        TgBot::TgLongPoll longPoll(bot);
        while (true) {
            bot.getApi().getUpdates();
            longPoll.start();
        }
    } catch (const TgBot::TgException& e) {
        std::cerr << "Ошибка в работе бота: " << e.what() << std::endl;
    }

    return 0;
}
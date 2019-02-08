# LessonFlow
Web style lesson app

В папке с исполняемым файлом должны также быть папки:
/api/ (в ней должны находиться файлы secret_google.json - ключ google для использования
API GoogleSheets, Google Cloud Text-to-Speech;
subandclas.json - файл с Id листа GoogleSheets, в котором расположены названия классов и предметов, напримерЖ
{"dataSheetId" : "YOUR_DATASHEET_ID"})

/avatar/ (в поддиректориях папки с названиями классов хранятся аватарки учеников,
также в ней должны быть 2 каталога /avatar/F/  и /avatar/M/ со стандартными наборами аватаров для
девушек и юношей с именами 0.png, 1.png ...)

/sounds/ (в этой папке располагаются файлы plus0.wav и minus0.wav)

Программа работает в качестве сервиса (службы) Windows
(версии 7- 10). Программа осуществляет резервное копирование данных
из указанной директории.
− Программа не интерактивна, все настройки задаются с
помощью конфигурационного файла;
− Программа создает архив с резервной копией данных из
указанной директории;
− Файлы, включаемые в резервную копию, задаются по маске (с
использованием символов *, ?) или непосредственно по именам;
− Резервная копия создаётся в виде архива формата ZIP;
− При изменении существующих или появлении новых файлов в
исходной директории (соответствующих заданной маске) архив с резервной
копией автоматически дополняется новыми данными (или заменяются
существующие);
− Инсталляция, деинсталляция, запуск, останов из командной строки
(реализация своего SCP);
− Корректный останов и перезапуск службы по запросу от SCM
(обработка команд, передаваемых через оснастку "Службы");
− Имена файлов и директорий задаются в формате UNC.
Конфигурационный файл содержит:
− Имя директории, из которой выполняется резервное копирование;
− Имя файла-архива и директории, в которой он должен находиться;
− Маски/имена файлов, помещаемых в резервную копию.

Пример конфигурационного файла:
C:\test1\
C:\Arhivdata.zip
*.txt
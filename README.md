MPStuBS - Multiprozessor Studenten Betriebssystem
=================================================

In diesem Git Quellcodeverzeichnis ist die Vorlage für die
Übungsaufgaben der Veranstaltung Betriebssysteme für das Wintersemester
2018/19 enthalten.

Updaten der Vorlage
-------------------

Bei jeder neuen Aufgabe, bzw. einem Bugfix in der Vorlage, solltet ihr
die Änderungen aus dem Vorlagenrepository in euer Repository übernehmen.

Zunächst müsst ihr sicherstellen, dass das Vorlagen Repository als Git
Remote eingetragen ist. Mit diesem Befehl wird ein neuer Verweis auf
ein entferntes Repository angelegt.

    git remote add vorlage https://gitlab.cs.fau.de/i4/bs/mpstubs.git

Das hinzufügen des entfernten Repositories ist nur einmal
erforderlich. Danach könnt ihr die Änderungen aus der Vorlage mittels

    git pull vorlage master

in euer Repository übernehmen. Es wird dabei ein *merge* Commit
erstellt, der eure Änderungen und die geänderte Vorlage zusammenführt.

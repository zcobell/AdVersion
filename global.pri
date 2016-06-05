#...QADCModules
INCLUDEPATH += $$QADCMODULES_PATH/include

#...LibGit2
INCLUDEPATH += $$LIBGIT2_PATH/include

#...Metis
INCLUDEPATH += $$METIS_PATH/include

#...Add libraries
win32{
LIBS += -L$$QADCMODULES_PATH/bin -lQADCModules -L$$LIBGIT2_PATH/bin -lgit2 -L$$METIS_PATH/bin -lmetis
}
unix{
LIBS += -L$$QADCMODULES_PATH/lib -lQADCModules -L$$LIBGIT2_PATH/lib -lgit2 -L$$METIS_PATH/lib -lmetis
}


#...Get the version information directly from Git
#   If git is not available, comment the next line and uncomment line line after
GIT_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)
#GIT_VERSION = unknown
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

DEFINES += EBUG

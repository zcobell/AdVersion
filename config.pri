win32{
    QADCMODULES_PATH = C:/libraries/QADCModules
    LIBGIT2_PATH     = C:/libraries/libgit2
}
unix{
    QADCMODULES_PATH = /opt/QADCModules
    LIBGIT2_PATH     = /usr
}


#...Get the version information directly from Git
#   If git is not available, comment the next line and uncomment line line after
GIT_VERSION = $$system(git --git-dir $$PWD/.git --work-tree $$PWD describe --always --tags)
#GIT_VERSION = unknown
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

DEFINES += EBUG

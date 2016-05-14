win32{
    QADCMODULES_PATH = C:/libraries/QADCModules
    LIBGIT2_PATH     = C:/libraries/libgit2
}
unix{
    QADCMODULES_PATH = /opt/QADCModules
    LIBGIT2_PATH     = /usr
    QMAKE_CXXFLAGS   = -I$LIBGIT2_PATH/include
}

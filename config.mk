PRJTYPE = Executable
ifeq ($(TARGET_OS), Windows_NT)
	DEFINES += _CRT_SECURE_NO_WARNINGS _CRT_SECURE_NO_DEPRECATE
endif

LIBS = nuklear macu glfw glad
ifeq ($(TARGET_OS), Windows_NT)
	LIBS += glu32 opengl32 gdi32 ole32 user32 shell32
else
    	LIBS += GL X11 Xrandr Xinerama Xcursor pthread dl
endif

EXTDEPS = macu::0.0.2dev

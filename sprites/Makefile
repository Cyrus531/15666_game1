
OS := $(shell uname)

ifeq ($(OS), Linux)
	GIMP=gimp
else
	GIMP=/Applications/GIMP.app/Contents/MacOS/gimp
endif


../dist/starry.png ../dist/starry.atlas : starry.xcf starry.list trade.xcf trade-font.list extract-sprites.py pack-sprites
	rm -rf starry
	./extract-sprites.py starry.list starry --gimp='$(GIMP)'
	./extract-sprites.py trade-font.list starry --gimp='$(GIMP)'
	./pack-sprites ../dist/starry starry/*

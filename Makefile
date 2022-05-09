#    Chemarium
#    Copyright (C) 2022  Andy Frank Schoknecht
#
#    This program is free software: you can redistribute it and/or modify
#    it under the terms of the GNU General Public License as published by
#    the Free Software Foundation, either version 3 of the License, or
#    (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program.  If not, see <https://www.gnu.org/licenses/>.

CC = cc
APP_NAME = chemarium
CFLAGS = -std=c99 -Wall -Wextra -O3
INCLUDE = -I /usr/include/SDL2 -I /usr/include/schoki_misc -I /usr/include/schoki_gui -I /usr/include/schoki_game
LIBS = -l SDL2 -l SDL2_image -l SDL2_ttf -l schoki_misc -l schoki_gui -l schoki_game

INSTALL_BIN_DIR = /usr/bin
INSTALL_ASSETS_DIR = /usr/share/${APP_NAME}
INSTALL_TEXTURES_DIR = ${INSTALL_ASSETS_DIR}/textures
INSTALL_DESKTOP_DIR = /usr/share/applications
INSTALL_ICONS_DIR = /usr/share/icons/hicolor

DEFINES = -D PATH_ASSETS="\"${INSTALL_ASSETS_DIR}/\"" -D PATH_TEXTURES="\"${INSTALL_TEXTURES_DIR}/\""

options:
	@echo ${APP_NAME} build options:
	@echo "CFLAGS   = ${CFLAGS}"
	@echo "CC       = ${CC}"
	@echo "LIBS     = ${LIBS}"

clean:
	rm -f ${APP_NAME} *.o

install:
	#compile
	${CC} src/*.c ${CFLAGS} ${INCLUDE} ${LIBS} -o ${APP_NAME} ${DEFINES}

	#move bin to install dir
	mkdir -p ${INSTALL_BIN_DIR}
	mv -f ${APP_NAME} ${INSTALL_BIN_DIR}
	chmod 755 ${INSTALL_BIN_DIR}/${APP_NAME}

	#move assets
	mkdir -p ${INSTALL_ASSETS_DIR}
	mkdir -p ${INSTALL_TEXTURES_DIR}
	cp -r assets/textures/* ${INSTALL_TEXTURES_DIR}
	cp data/${APP_NAME}_1024.png ${INSTALL_TEXTURES_DIR}/icon.png

	#move data
	mkdir -p ${INSTALL_DESKTOP_DIR}
	mkdir -p ${INSTALL_ICONS_DIR}/16x16/apps
	mkdir -p ${INSTALL_ICONS_DIR}/32x32/apps
	mkdir -p ${INSTALL_ICONS_DIR}/48x48/apps
	mkdir -p ${INSTALL_ICONS_DIR}/64x64/apps
	mkdir -p ${INSTALL_ICONS_DIR}/128x128/apps
	mkdir -p ${INSTALL_ICONS_DIR}/256x256/apps
	mkdir -p ${INSTALL_ICONS_DIR}/512x512/apps
	mkdir -p ${INSTALL_ICONS_DIR}/1024x1024/apps
	cp data/${APP_NAME}.desktop ${INSTALL_DESKTOP_DIR}
	cp data/${APP_NAME}_16.png ${INSTALL_ICONS_DIR}/16x16/apps/${APP_NAME}.png
	cp data/${APP_NAME}_32.png ${INSTALL_ICONS_DIR}/32x32/apps/${APP_NAME}.png
	cp data/${APP_NAME}_48.png ${INSTALL_ICONS_DIR}/48x48/apps/${APP_NAME}.png
	cp data/${APP_NAME}_64.png ${INSTALL_ICONS_DIR}/64x64/apps/${APP_NAME}.png
	cp data/${APP_NAME}_128.png ${INSTALL_ICONS_DIR}/128x128/apps/${APP_NAME}.png
	cp data/${APP_NAME}_256.png ${INSTALL_ICONS_DIR}/256x256/apps/${APP_NAME}.png
	cp data/${APP_NAME}_512.png ${INSTALL_ICONS_DIR}/512x512/apps/${APP_NAME}.png
	cp data/${APP_NAME}_1024.png ${INSTALL_ICONS_DIR}/1024x1024/apps/${APP_NAME}.png

uninstall:
	#remove bin
	rm -f ${INSTALL_BIN_DIR}/${APP_NAME}

	#remove assets
	rm -f -r ${INSTALL_ASSETS_DIR}

	#remove data
	rm -f ${INSTALL_DESKTOP_DIR}/${APP_NAME}.desktop
	rm -f ${INSTALL_ICONS_DIR}/16x16/apps/${APP_NAME}.png
	rm -f ${INSTALL_ICONS_DIR}/32x32/apps/${APP_NAME}.png
	rm -f ${INSTALL_ICONS_DIR}/48x48/apps/${APP_NAME}.png
	rm -f ${INSTALL_ICONS_DIR}/64x64/apps/${APP_NAME}.png
	rm -f ${INSTALL_ICONS_DIR}/128x128/apps/${APP_NAME}.png
	rm -f ${INSTALL_ICONS_DIR}/256x256/apps/${APP_NAME}.png
	rm -f ${INSTALL_ICONS_DIR}/512x512/apps/${APP_NAME}.png
	rm -f ${INSTALL_ICONS_DIR}/1024x1024/apps/${APP_NAME}.png

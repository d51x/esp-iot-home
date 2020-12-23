#
# Main component makefile.
#
# This Makefile can be left empty. By default, it will take the sources in the 
# src/ directory, compile them and link them into lib(subdirectory_name).a 
# in the build directory. This behaviour is entirely configurable,
# please read the ESP-IDF documents if you need to do this.
#
COMPONENT_SRCDIRS := 	.
EXTRA_COMPONENT_DIRS := components
COMPONENT_EMBED_FILES := 	${PROJECT_PATH}/${EXTRA_COMPONENT_DIRS}/web/favicon.ico \
							${PROJECT_PATH}/${EXTRA_COMPONENT_DIRS}/web/main.min.css \
							${PROJECT_PATH}/${EXTRA_COMPONENT_DIRS}/web/ajax.min.js	\
							${PROJECT_PATH}/${EXTRA_COMPONENT_DIRS}/web/menu.png	\
							${PROJECT_PATH}/${EXTRA_COMPONENT_DIRS}/web/menu2.png

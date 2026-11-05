.RECIPEPREFIX = >
GBDK_HOME ?= $(HOME)/gbdk
all:
> GBDK_HOME=$(GBDK_HOME) ./build.sh
clean:
> rm -rf build

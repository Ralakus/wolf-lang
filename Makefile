
ifndef config
  config=debug
endif

ifeq ($(config),debug)
else ifeq ($(config),release)
else
  $(error "invalid configuration $(config)")
endif

all:
	premake5 gmake2 && make -C ./build config=$(config) && nimble build -d:$(config)

clean: build
	rm -r build
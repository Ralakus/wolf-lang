ifndef config
  config=debug
endif

ifeq ($(config),debug)
else ifeq ($(config),release)
else
  $(error "invalid configuration $(config)")
endif

build/wolf: src/**.nim src/util/**.nim wolf.nimble src/wolf.nims
	nimble build -d:$(config)

build:
	mkdir build

clean: build
	rm -r build

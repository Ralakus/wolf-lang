ifndef config
  config=debug
endif

ifeq ($(config),debug)
else ifeq ($(config),release)
else
  $(error "invalid configuration $(config)")
endif

build/wolf: src/**.nim src/util/**.nim wolf.nimble
	nimble build -d:$(config) --verbose

clean:
	rm -r build

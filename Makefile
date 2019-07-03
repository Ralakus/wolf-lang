ifndef config
  config=debug
endif

ifeq ($(config),debug)
build/wolf: src/**.nim src/util/**.nim wolf.nimble
	nimble build -d:debug --verbosity:2 --checks:on --opt:none --debugger:native --stackTrace:on --lineTrace:on
else ifeq ($(config),release)
build/wolf: src/**.nim src/util/**.nim wolf.nimble
	nimble build -d:release --verbosity:2 --checks:off --opt:speed --debugger:native --stackTrace:off --lineTrace:off
else
  $(error "invalid configuration $(config)")
endif

clean:
	rm -r build

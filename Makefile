.PHONY: all
all: build

.PHONY: build
build: build/bin/zsh-prompt

build/bin/zsh-prompt:
	mkdir -p build
	cd build && \
	cmake .. && \
	cmake --build .

.PHONY: run
run: build/bin/zsh-prompt
	@build/bin/zsh-prompt

.PHONY: clean
clean:
	rm -rf build/bin
	rm -rf build/zsh-prompt-prefix

.PHONY: clean-all
clean-all:
	rm -rf build

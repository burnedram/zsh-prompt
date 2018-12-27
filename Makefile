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

.PHONY: docker-builder
docker-builder:
	docker build --build-arg http_proxy --build-arg https_proxy -t zsh-prompt-builder:latest .

.PHONY: docker-run
docker-run:
	docker run -e http_proxy -e https_proxy --name zsh-prompt-builder -d zsh-prompt-builder:latest bash -c "trap : TERM INT; sleep infinity & wait"

.PHONY: docker-build
docker-build: docker-update docker-make docker-commit docker-get

.PHONY: docker-update
docker-update:
	docker cp Makefile zsh-prompt-builder:/root/Makefile
	docker cp CMakeLists.txt zsh-prompt-builder:/root/CMakeLists.txt
	docker cp src/. zsh-prompt-builder:/root/src

.PHONY: docker-make
docker-make:
	docker exec -t zsh-prompt-builder make clean build

.PHONY: docker-commit
docker-commit:
	docker commit zsh-prompt-builder zsh-prompt-builder:latest

.PHONY: docker-get
docker-get:
	docker cp zsh-prompt-builder:/root/build/bin/zsh-prompt zsh-prompt

.PHONY: docker-cleanup
docker-cleanup: docker-stop docker-rm

.PHONY: docker-stop
docker-stop:
	docker stop zsh-prompt-builder

.PHONY: docker-rm
docker-rm:
	docker rm zsh-prompt-builder

all: build

.PHONY: build
build: deps
	mkdir -p build && \
	cd build && \
	cmake .. && \
	cmake --build .

.PHONY: deps
deps:
	cd deps && \
	mkdir -p build && \
	cd build && \
	cmake .. && \
	cmake --build .

.PHONY: clean-deps
clean-bin:
	rm -rf deps/build

.PHONY: clean
clean:
	rm -rf build

.PHONY: docker-build
docker-build:
	docker build --build-arg http_proxy --build-arg https_proxy -t zsh-prompt-builder:latest .

.PHONY: docker-get
docker-get:
	CONTAINER_ID=$$(docker create zsh-prompt-builder:latest) && \
	docker cp $${CONTAINER_ID}:/root/bin/zsh-prompt . && \
	docker rm $${CONTAINER_ID}

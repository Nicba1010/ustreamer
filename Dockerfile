FROM alpine:latest as builder

RUN apk add --update --no-cache make gcc musl-dev libevent-dev libbsd-dev libjpeg-turbo-dev libgpiod-dev

WORKDIR /usr/src/ustreamer
COPY . .

RUN make WITH_GPIO=1 WITH_PTHREAD_NP=0

FROM alpine:latest as release

RUN apk add --update --no-cache libevent libbsd libjpeg-turbo libgpiod

ARG user=ustreamer
ARG group=ustreamer
ARG uid=2000

RUN adduser --uid=${uid} --disabled-password --gecos="" ${user}
RUN adduser -m -g video ${user}

USER ${uid}:${uid}
WORKDIR $HOME

COPY --from=builder /usr/src/ustreamer/ustreamer /usr/local/bin/ustreamer

EXPOSE 8080
ENTRYPOINT ["ustreamer"]
CMD ["--host=0.0.0.0", "--port=8080"]
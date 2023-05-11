FROM ubuntu:20.04

RUN mkdir /temp
ADD . /temp
WORKDIR /temp
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get -y update && apt-get install -y libpqxx-dev postgresql libxerces-c-dev

#ENTRYPOINT [ "./run.sh" ]